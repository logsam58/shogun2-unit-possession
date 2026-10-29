#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "../../shogun2-possession-cleanup-v46/vendor/include/MinHook.h"
#include "native_observer.h"
#include "native_battle_clock.h"
#include "observer_protocol.h"
#include "possession_observation.h"
#include <filesystem>
static_assert(sizeof(void*)==4,"Build x86");
static HMODULE self_module;
static Status* state;
static uintptr_t engine_base;
static volatile LONG collecting,last_environment;
static SRWLOCK queue_lock=SRWLOCK_INIT;
static constexpr unsigned QUEUE_CAPACITY=8192;
static qte::observation::Event queue[QUEUE_CAPACITY];
static uint64_t head,tail;
#ifndef OBSERVER_DETAILS
#define OBSERVER_DETAILS 0
#endif
using qte::observation::PossessionPrefix;
static const PossessionPrefix* possession_status;
static bool read(void*,uint32_t address,void* output,size_t size){
    if(!address||size>UINT32_MAX-address)return false;
    __try {memcpy(output,reinterpret_cast<void*>(address),size);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool snapshot(void*,uint32_t witness,qte::observation::Event& e){
    qte::Reader r{read,nullptr};uint32_t environment{},common{},root{},current{};uint8_t active{},phase{};
    if(witness>UINT32_MAX-0x100||!r.get(witness+4,environment)||!environment||environment>UINT32_MAX-0x28210||
       !r.get(environment+8,common)||!common||common>UINT32_MAX-0x54||!r.get(common+0x50,e.tick)||
       !r.get(uint32_t(engine_base)+0x1bc8180,root)||!root||root>UINT32_MAX-0x114||
       !r.get(root+0x110,current)||current!=environment||!r.get(witness+0xc,active)||
       !r.get(witness+0xd0,phase)||!r.get(witness+0x24,e.count)||e.count>5||
       !r.get(witness+0x38,e.principals)||e.principals>2)return false;
    e.environment=environment;e.active=active;e.phase=phase;
    for(unsigned i=0;i<e.count;++i)if(!r.get(witness+0x10+i*4,e.participants[i]))return false;
#if OBSERVER_DETAILS
    for(auto& actor:e.actors)actor={};e.animation={};
    e.owner_valid=e.owner_actor=e.owner_serial=e.owner_unit_serial=e.owner_lease=0;
    for(unsigned i=0;i<e.count;++i)qte::observation::actor_snapshot(r,e.participants[i],uint32_t(engine_base),e.actors[i]);
    qte::observation::animation_snapshot(r,witness,e.animation);
    if(possession_status){
        PossessionPrefix owned;LONG sequence=possession_status->sequence;
        if(!(sequence&1)&&read(nullptr,uint32_t(reinterpret_cast<uintptr_t>(possession_status)),&owned,sizeof owned)&&
           owned.sequence==sequence&&possession_status->sequence==sequence&&
           owned.magic==0x31504353&&owned.version==46&&owned.pid==GetCurrentProcessId()&&owned.size>=sizeof owned&&
           owned.ready&&owned.mode==2&&!owned.stop&&!owned.release&&owned.lease_generation&&
           owned.heartbeat<=GetTickCount64()&&GetTickCount64()-owned.heartbeat<1500){
            uint32_t manager{},camera{},actor{};qte::observation::ActorSnapshot identity;
            if(r.get(environment+0x281fc,manager)&&manager&&manager<=UINT32_MAX-0x900&&
               r.get(manager+0x8e0,camera)&&camera&&camera<=UINT32_MAX-0x18c&&r.get(camera+0x188,actor)&&
               qte::observation::actor_snapshot(r,actor,uint32_t(engine_base),identity)&&identity.alive&&
               identity.serial==owned.actor_id&&identity.unit_serial==owned.unit_id){
                e.owner_valid=1;e.owner_actor=actor;e.owner_serial=identity.serial;
                e.owner_unit_serial=identity.unit_serial;e.owner_lease=owned.lease_generation;
            }
        }
    }
#endif
    InterlockedExchange(&last_environment,LONG(environment));return true;
}
static void emit(void*,const qte::observation::Event& event){
    if(!collecting)return;
    if(!event.exit)InterlockedIncrement(state->calls+event.kind);
    AcquireSRWLockExclusive(&queue_lock);
    if(head-tail<QUEUE_CAPACITY){queue[head++%QUEUE_CAPACITY]=event;}
    else {InterlockedIncrement(&state->dropped);InterlockedExchange(&state->stop_reason,5);InterlockedExchange(&state->stop,1);}
    ReleaseSRWLockExclusive(&queue_lock);
}
static void flush(std::ofstream& log){
    // Take short batches. Disk writes never occur under a hook's queue lock.
    qte::observation::Event batch[128];
    for(;;){unsigned count=0;AcquireSRWLockExclusive(&queue_lock);
        while(count<128&&tail!=head)batch[count++]=queue[tail++%QUEUE_CAPACITY];ReleaseSRWLockExclusive(&queue_lock);
        if(!count)break;
        for(unsigned n=0;n<count;++n){const auto& e=batch[n];
            log<<"{\"qpc\":"<<e.qpc<<",\"generation\":"<<e.generation<<",\"kind\":"<<e.kind<<",\"exit\":"<<e.exit
                <<",\"thread\":"<<e.thread<<",\"caller_rva\":"<<e.caller<<",\"witness\":"<<e.witness
                <<",\"environment\":"<<e.environment<<",\"tick\":"<<e.tick<<",\"native_valid\":"<<e.native_valid
                <<",\"active\":"<<e.active<<",\"phase\":"<<e.phase<<",\"count\":"<<e.count<<",\"principals\":"<<e.principals<<",\"participants\":[";
            for(unsigned i=0;i<e.count&&i<5;++i){if(i)log<<',';log<<e.participants[i];}
            log<<"],\"arguments\":[";for(unsigned i=0;i<6;++i){if(i)log<<',';log<<e.arguments[i];}
            log<<"],\"returned\":"<<e.returned<<",\"completed\":"<<e.completed<<",\"verdict_valid\":"<<e.verdict_valid
                <<",\"verdict\":["<<e.verdict.outcome<<','<<e.verdict.attacker_address<<','<<e.verdict.defender_address<<"]";
#if OBSERVER_DETAILS
            log<<",\"owner\":{\"valid\":"<<e.owner_valid<<",\"actor\":"<<e.owner_actor<<",\"serial\":"<<e.owner_serial
                <<",\"unit_serial\":"<<e.owner_unit_serial<<",\"lease\":"<<e.owner_lease<<"},\"actors\":[";
            for(unsigned i=0;i<e.count&&i<5;++i){if(i)log<<',';const auto& a=e.actors[i];
                log<<"{\"valid\":"<<a.valid<<",\"address\":"<<a.address<<",\"serial\":"<<a.serial<<",\"unit\":"<<a.unit
                    <<",\"unit_serial\":"<<a.unit_serial<<",\"side\":"<<a.side<<",\"alive\":"<<a.alive
                    <<",\"witness\":"<<a.witness<<",\"state\":"<<a.state<<",\"mount\":"<<a.mount<<'}';}
            const auto& a=e.animation;
            log<<"],\"animation\":{\"valid\":"<<a.valid<<",\"outcome\":"<<a.outcome<<",\"attacker\":"<<a.attacker
                <<",\"defender\":"<<a.defender<<",\"matched\":"<<a.matched<<",\"fallback\":"<<a.fallback
                <<",\"rotation\":"<<a.rotation<<",\"requests\":[";
            for(unsigned i=0;i<a.request_count&&i<2;++i){if(i)log<<',';const auto& item=a.requests[i];
                log<<"{\"actor\":"<<item.actor<<",\"start\":"<<item.start<<",\"end\":"<<item.end<<",\"attacks\":"<<item.attacks<<'}';}
            log<<"],\"matches\":[";
            for(unsigned i=0;i<a.match_count&&i<2;++i){if(i)log<<',';const auto& item=a.matches[i];
                log<<"{\"start\":"<<item.start<<",\"end\":"<<item.end<<",\"alliance\":"<<item.alliance
                    <<",\"record\":"<<item.record<<",\"record_end\":"<<item.record_end<<",\"record_type\":"<<item.record_type<<'}';}
            log<<"]}";
#endif
            log<<"}\n";
        }
    }log.flush();
}
static DWORD WINAPI worker(void*){
    const uint32_t rvas[4]={0x3e5d40,0x3eaac0,0x3cea10,0x433b80};
    const unsigned char signatures[4][8]={{0x55,0x8b,0xec,0x83,0xe4,0xf0,0x81,0xec},
        {0x83,0xec,0x08,0x53,0x55,0x56,0x8b,0xf1},{0x56,0x8b,0x74,0x24,0x08,0x57,0x8b,0xf9},
        {0x55,0x8b,0xec,0x83,0xe4,0xf0,0x81,0xec}};
    void* targets[4]{};bool initialized=false;std::ofstream log;
    try {
        wchar_t name[80];mapping_name(name,GetCurrentProcessId());
        HANDLE mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Status),name);
        require(mapping&&GetLastError()!=ERROR_ALREADY_EXISTS,"Diagnostic already resident");
        state=static_cast<Status*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Status)));require(state!=nullptr,"Mapping failed");
        memset(state,0,sizeof(Status));state->magic=POC_MAGIC;state->version=POC_VERSION;state->pid=GetCurrentProcessId();state->size=sizeof(Status);
        Module engine=find_module(GetCurrentProcessId());const auto engine_sha170=hash_file(engine.path);require(engine.size==0x1d88000,"Unsupported engine address layout");(void)engine172::admit(engine.path,engine_sha170);engine_base=engine.base;
        auto& ports=qte::observation::ports;ports.engine=uint32_t(engine.base);ports.emit=emit;ports.read=read;ports.snapshot=snapshot;
#if OBSERVER_DETAILS
        wchar_t possession_name[80];swprintf_s(possession_name,L"Local\\Shogun2PossessionPOC46-%lu",GetCurrentProcessId());
        HANDLE possession_mapping=OpenFileMappingW(FILE_MAP_READ,FALSE,possession_name);
        if(possession_mapping){possession_status=static_cast<const PossessionPrefix*>(MapViewOfFile(possession_mapping,FILE_MAP_READ,0,0,sizeof(PossessionPrefix)));CloseHandle(possession_mapping);}
#endif
        wchar_t module_path[MAX_PATH];require(GetModuleFileNameW(self_module,module_path,MAX_PATH)>0,"Diagnostic path absent");
        auto private_dir=std::filesystem::path(module_path).parent_path()/L"private";require(std::filesystem::is_directory(private_dir),"Diagnostic log directory absent");
        const wchar_t* trace_prefix=OBSERVER_DETAILS?L"qte-animation-observer-":L"qte-observer-";
        log.open(private_dir/(trace_prefix+std::to_wstring(GetCurrentProcessId())+L".jsonl"),std::ios::app);require(bool(log),"Diagnostic log unavailable");
        for(unsigned i=0;i<4;++i){targets[i]=reinterpret_cast<void*>(engine.base+rvas[i]);require(!memcmp(targets[i],signatures[i],8),"Hook entry differs or is already hooked");memcpy(state->original_code+i*16,targets[i],16);}
        require(MH_Initialize()==MH_OK,"Hook initialization failed");initialized=true;
        void* hooks[4]={reinterpret_cast<void*>(&qte::observation::prepare_hook),reinterpret_cast<void*>(&qte::observation::update_hook),
            reinterpret_cast<void*>(&qte::observation::remove_hook),reinterpret_cast<void*>(&qte::observation::select_hook)};
        void** originals[4]={reinterpret_cast<void**>(&ports.prepare),reinterpret_cast<void**>(&ports.update),reinterpret_cast<void**>(&ports.remove),reinterpret_cast<void**>(&ports.select)};
        for(unsigned i=0;i<4;++i)require(MH_CreateHook(targets[i],hooks[i],originals[i])==MH_OK,"Hook creation failed");
        InterlockedExchange(&collecting,1);require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Hook enable failed");
        LARGE_INTEGER frequency;QueryPerformanceFrequency(&frequency);state->ready=1;
        log<<"{\"event\":\"ready\",\"engine_sha256\":\""<<engine_sha170<<"\",\"qpc_frequency\":"<<frequency.QuadPart
            <<",\"changes_combat_outcomes\":false,\"animation_details\":"<<(OBSERVER_DETAILS?"true":"false")<<"}\n";log.flush();
        uint64_t expires=GetTickCount64()+180000;
        while(!state->stop&&GetTickCount64()<expires){state->heartbeat=GetTickCount64();flush(log);
            uint32_t environment=uint32_t(InterlockedCompareExchange(&last_environment,0,0));qte::NativeClock clock;
            if(environment&&qte::battle_clock({read,nullptr},environment,uint32_t(engine_base),clock)){
                LARGE_INTEGER now;QueryPerformanceCounter(&now);log<<"{\"event\":\"clock\",\"qpc\":"<<now.QuadPart<<",\"environment\":"<<clock.environment<<",\"phase\":"<<clock.phase<<",\"tick\":"<<clock.tick<<",\"step\":"<<clock.step<<",\"speed\":"<<clock.speed<<"}\n";}
            Sleep(16);
        }
        if(!state->stop_reason)state->stop_reason=GetTickCount64()>=expires?1:2;
        require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Hook disable failed");
        uint64_t drain_deadline=GetTickCount64()+2000;
        while(InterlockedCompareExchange(&qte::observation::active_callbacks,0,0)&&GetTickCount64()<drain_deadline){flush(log);Sleep(1);}
        require(!InterlockedCompareExchange(&qte::observation::active_callbacks,0,0),"Diagnostic callbacks still draining; DLL remains resident");
        InterlockedExchange(&collecting,0);
        for(unsigned i=0;i<4;++i){memcpy(state->restored_code+i*16,targets[i],16);require(!memcmp(state->restored_code+i*16,state->original_code+i*16,16),"Original entry restoration failed");}
        state->ready=0;state->mode=DISABLED;flush(log);
        log<<"{\"event\":\"disabled\",\"dropped\":"<<state->dropped<<",\"stop_reason\":"<<state->stop_reason<<",\"error\":0}\n";log.flush();
        // DLL and trampolines stay resident until process exit. Combat was
        // always forwarded; this probe never holds an unprepared witness.
    }catch(const std::exception& e){
        InterlockedExchange(&collecting,0);if(initialized)MH_DisableHook(MH_ALL_HOOKS);
        if(state){state->ready=0;state->mode=DISABLED;state->error=1;}
        if(log){flush(log);log<<"{\"event\":\"failed\",\"reason\":\""<<e.what()<<"\"}\n";log.flush();}
    }
    return 0;
}
BOOL WINAPI DllMain(HINSTANCE module,DWORD reason,LPVOID){
    if(reason==DLL_PROCESS_ATTACH){self_module=module;HANDLE worker_thread=CreateThread(nullptr,0,worker,nullptr,0,nullptr);if(!worker_thread)return FALSE;CloseHandle(worker_thread);}return TRUE;
}
