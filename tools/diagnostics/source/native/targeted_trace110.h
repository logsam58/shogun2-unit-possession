#pragma once
#include <map>
#include <chrono>
#include "trace_ownership111.h"
// Hardware debug registers only: no code patches, injected DLLs or game-data writes.
// Debug events stop all target threads; ownership changes are applied before continue.
class TargetedTrace110 {
    struct Saved {HANDLE handle{};CONTEXT original{},programmed{};unsigned kind[4]{};uintptr_t owner[4]{};bool owned{},dirty{};TraceOwnership111 history;};
    HANDLE process{};DWORD pid{};bool fixture{},disabled{},finished{};std::wstring folder;
    std::map<DWORD,Saved> threads;
    std::ofstream log;
    uintptr_t engine{},destructor{},audio_entry{},audio_wrapper{},audio_object{},effects_word{};
    uintptr_t fixture_global{};
    unsigned binding{},word_writes{},traps{},records{},dropped{},epoch_hits{};
    uint64_t epoch{},last_health{},pause_us{},max_pause_us{},bytes{};
    bool budget_exhausted{};
    static constexpr uint64_t max_bytes=32ull*1024*1024;
    static bool read(HANDLE p,uintptr_t a,void*dest,size_t n){SIZE_T got=0;return a>=0x10000&&ReadProcessMemory(p,reinterpret_cast<void*>(a),dest,n,&got)&&got==n;}
    bool word(uintptr_t a,DWORD& v)const{return read(process,a,&v,4);}
    static std::string address(uintptr_t a){std::ostringstream s;s<<'"'<<hex(a)<<'"';return s.str();}
    void event(const char*name,const std::string&extra={}){
        std::string line="{\"event\":\""+std::string(name)+"\",\"tick_ms\":"+std::to_string(GetTickCount64())+",\"pid\":"+std::to_string(pid)+(extra.empty()?"":","+extra)+"}\n";
        if(bytes+line.size()>max_bytes){++dropped;return;}log<<line;log.flush();bytes+=line.size();++records;
    }
    std::string evidence(const CONTEXT& c){
        std::ostringstream s;s<<"\"eip\":"<<address(c.Eip)<<",\"esp\":"<<address(c.Esp)<<",\"ebp\":"<<address(c.Ebp)
          <<",\"eax\":"<<address(c.Eax)<<",\"ecx\":"<<address(c.Ecx)<<",\"edx\":"<<address(c.Edx)<<",\"esi\":"<<address(c.Esi)<<",\"edi\":"<<address(c.Edi)
          <<",\"stack_words_not_unwound\":[";
        for(unsigned i=0;i<24;++i){DWORD x=0;if(!word(c.Esp+i*4,x))break;if(i)s<<',';s<<address(x);}s<<"],\"frame_pointer_chain\":[";
        uintptr_t fp=c.Ebp;bool comma=false;
        for(unsigned i=0;i<16;++i){DWORD pair[2]{};if(!read(process,fp,pair,8))break;if(comma)s<<',';comma=true;s<<address(pair[1]);if(pair[0]<=fp||pair[0]-fp>1024*1024)break;fp=pair[0];}
        s<<"],\"instruction_window_begin\":"<<address(c.Eip>=16?c.Eip-16:c.Eip)<<",\"instruction_window_hex\":\"";
        unsigned char raw[48]{};if(read(process,c.Eip>=16?c.Eip-16:c.Eip,raw,sizeof raw))for(auto v:raw)s<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(v);
        s<<"\"";return s.str();
    }
    bool executable(uintptr_t a){MEMORY_BASIC_INFORMATION m{};return VirtualQueryEx(process,reinterpret_cast<void*>(a),&m,sizeof m)==sizeof m&&m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&(m.Protect&0xf0);}
    std::string owner_snapshot(uintptr_t a){
        unsigned char raw[0x78]{};std::ostringstream s;
        if(!read(process,a,raw,sizeof raw))return "\"owner_prefix_readable\":false";
        DWORD vt=0,table=0,count=0;memcpy(&vt,raw,4);memcpy(&table,raw+0x28,4);memcpy(&count,raw+0x2c,4);
        s<<"\"owner_vtable\":"<<address(vt)<<",\"owner_table\":"<<address(table)<<",\"owner_table_count\":"<<count<<",\"owner_prefix_hex\":\"";
        for(auto v:raw)s<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(v);s<<'"';return s.str();
    }
    bool effects_matches(uintptr_t owner,std::string& result){
        DWORD h[2]{};if(!read(process,fixture?fixture_global:engine+0x1ca3464,h,8)||h[1]>256)return false;
        bool matched=false;std::ostringstream entries;
        for(unsigned i=0;i<h[1];++i){DWORD e[9]{};if(!read(process,h[0]+i*36,e,36))break;
            if((e[6]&255)&&e[8]==owner){if(matched)entries<<',';matched=true;entries<<"{\"index\":"<<i<<",\"element\":"<<e[7]<<'}';}}
        if(matched)result="\"owner\":"+address(owner)+",\"active_references\":["+entries.str()+"]";return matched;
    }
    void save_thread(DWORD tid){
        if(threads.count(tid))return;
        Saved saved;saved.handle=OpenThread(THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_SUSPEND_RESUME|THREAD_QUERY_INFORMATION,FALSE,tid);
        if(!saved.handle){event("thread_trace_unavailable","\"thread\":"+std::to_string(tid));return;}
        saved.original.ContextFlags=CONTEXT_DEBUG_REGISTERS;
        if(!GetThreadContext(saved.handle,&saved.original)){CloseHandle(saved.handle);event("thread_context_unavailable","\"thread\":"+std::to_string(tid));return;}
        if(saved.original.Dr7&255){CloseHandle(saved.handle);event("foreign_debug_registers_preserved","\"thread\":"+std::to_string(tid));return;}
        threads.emplace(tid,saved);
    }
    void enumerate_threads(){
        Handle snap(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0));THREADENTRY32 t{};t.dwSize=sizeof t;
        if(snap.h!=INVALID_HANDLE_VALUE&&Thread32First(snap.h,&t))do{if(t.th32OwnerProcessID==pid)save_thread(t.th32ThreadID);}while(Thread32Next(snap.h,&t));
    }
    void arm_all(){
        for(auto& pair:threads){auto& saved=pair.second;
            DWORD desired[]={DWORD(disabled?0:destructor),DWORD(disabled?0:(audio_wrapper?effects_word:audio_entry)),
                DWORD(disabled||!audio_wrapper?0:audio_wrapper+0x18),DWORD(disabled?0:audio_object)};
            if(saved.owned&&!saved.dirty&&saved.programmed.Dr0==desired[0]&&saved.programmed.Dr1==desired[1]&&
               saved.programmed.Dr2==desired[2]&&saved.programmed.Dr3==desired[3])continue;
            CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS;
            if(!GetThreadContext(saved.handle,&c))continue;
            // Another thread may already have a queued hardware exception. Keep
            // its old bindings intact until that exact event is handled.
            if(c.Dr6&15)continue;
            unsigned kinds[]={1u,audio_wrapper?3u:2u,4u,5u};
            uintptr_t owners[]={0,effects_word,audio_wrapper,audio_object};
            for(unsigned i=0;i<4;++i)if(desired[i]&&!saved.history.can_remember(i,{desired[i],kinds[i],DWORD(owners[i])})){
                // Never evict ownership that a saved context may restore later.
                disabled=true;budget_exhausted=true;event("ownership_history_full","\"hardware_observations_disabled\":true");
                for(auto& d:desired)d=0;break;
            }
            c.Dr0=disabled?0:destructor;
            c.Dr1=disabled?0:(audio_wrapper?effects_word:audio_entry);
            c.Dr2=disabled||!audio_wrapper?0:audio_wrapper+0x18;
            c.Dr3=disabled?0:audio_object;
            c.Dr7=0;
            DWORD values[]={c.Dr0,c.Dr1,c.Dr2,c.Dr3};
            for(unsigned i=0;i<4;++i)if(values[i]){c.Dr7|=1u<<(2*i);bool data=i>=2||(i==1&&audio_wrapper);if(data)c.Dr7|=13u<<(16+4*i);}
            if(SetThreadContext(saved.handle,&c)){
                saved.owned=true;saved.programmed=c;
                saved.dirty=false;
                saved.kind[0]=1;saved.kind[1]=audio_wrapper?3:2;saved.kind[2]=4;saved.kind[3]=5;
                saved.owner[1]=effects_word;saved.owner[2]=audio_wrapper;saved.owner[3]=audio_object;
                for(unsigned i=0;i<4;++i)if(values[i])require(saved.history.remember(i,{values[i],saved.kind[i],DWORD(saved.owner[i])}),"Ownership history admission failed");
            }
            else event("thread_arm_failed","\"thread\":"+std::to_string(pair.first)+",\"winerror\":"+std::to_string(GetLastError()));
        }
    }
    void bind_audio(uintptr_t wrapper,const CONTEXT& c,DWORD tid){
        DWORD object=0,vt=0,target=0;
        bool readable=word(wrapper+0x18,object)&&word(object,vt)&&word(vt+0x18,target);
        event("audio_interface_binding","\"thread\":"+std::to_string(tid)+",\"wrapper\":"+address(wrapper)+",\"interface\":"+address(object)+",\"vtable\":"+address(vt)+",\"slot18\":"+address(target)+",\"readable\":"+(readable?"true":"false")+",\"target_executable\":"+(readable&&executable(target)?"true":"false")+","+evidence(c));
        if(wrapper>=0x10000&&!(wrapper&3))audio_wrapper=wrapper;
        audio_object=readable&&!(object&3)?object:0;++binding;
    }
public:
    TargetedTrace110(HANDLE target,DWORD target_pid,const std::wstring&dir,bool synthetic):process(target),pid(target_pid),fixture(synthetic),folder(dir),log(dir+L"\\targeted110.jsonl",std::ios::app){
        // Withdrawn after two real startup failures: a queued DR3 trap outlived
        // its recorded binding. Keep research available only to toy fixtures.
        disabled=!fixture;
        if(disabled)event("hardware_trace_withdrawn","\"reason\":\"real startup single-step ownership regression; exception-only capture\"");
        event("targeted110_started","\"fixture\":"+std::string(fixture?"true":"false")+",\"no_game_code_or_data_writes\":true,\"scope\":\"effects destructor plus first audio wrapper/interface; four processor slots, not whole-process allocation tracing\"");
    }
    ~TargetedTrace110(){for(auto& pair:threads)if(pair.second.handle)CloseHandle(pair.second.handle);}
    void fixture_init(){
        if(!fixture)return;std::ifstream f(folder+L"\\targeted110-fixture.txt");uintptr_t d=0,a=0,g=0;
        if(f>>std::hex>>d>>a>>g){destructor=d;audio_entry=a;fixture_global=g;event("fixture_targets_verified");enumerate_threads();arm_all();}
    }
    void module(uintptr_t base,const std::wstring&path){
        if(fixture||disabled)return;
        auto name=path.substr(path.find_last_of(L"\\/")+1);
        try{
            if(_wcsicmp(name.c_str(),L"Empire.Retail.dll")==0){
                if(hash_file(path)!=EXPECTED_HASH){event("engine_trace_rejected");return;}
                const unsigned char expected[]={0x56,0x8b,0xf1,0x8b,0x46,0x70};unsigned char actual[6]{};
                if(!read(process,base+0x1448700,actual,6)||memcmp(actual,expected,6)){event("destructor_signature_rejected");return;}
                engine=base;destructor=base+0x1448700;event("effects_destructor_armed","\"module_base\":"+address(base)+",\"entry\":"+address(destructor));
            }else if(_wcsicmp(name.c_str(),L"dsound.dll")==0){
                if(hash_file(path)!="e6519f8f41c4a2b64f0768488df4c3796a3adb65d3f8c7996a4872af6505ff09"){event("audio_trace_rejected_backing_hash");return;}
                const unsigned char expected[]={0x8b,0xff,0x55,0x8b,0xec,0x8b,0x49,0x18};unsigned char actual[8]{};
                if(!read(process,base+0x5dee0,actual,8)||memcmp(actual,expected,8)){event("audio_signature_rejected");return;}
                audio_entry=base+0x5dee0;event("audio_scout_armed","\"module_base\":"+address(base)+",\"entry\":"+address(audio_entry));
            }else return;
            enumerate_threads();arm_all();
        }catch(const std::exception&){event("trace_module_unavailable","\"winerror\":"+std::to_string(GetLastError()));}
    }
    void new_thread(DWORD tid){if(destructor||audio_entry){save_thread(tid);arm_all();}}
    void exit_thread(DWORD tid){auto p=threads.find(tid);if(p!=threads.end()){CloseHandle(p->second.handle);threads.erase(p);}}
    void unload(uintptr_t base){
        if(engine==base){destructor=effects_word=engine=0;arm_all();event("effects_module_unloaded");}
        if(base+0x5dee0==audio_entry){audio_entry=audio_wrapper=audio_object=0;arm_all();event("audio_module_unloaded");}
    }
    void finish(){finished=true;last_health=0;event("targeted110_finished");heartbeat();}
    bool exception(DWORD tid,DWORD code);
    void heartbeat();
    void restore();
};

inline bool TargetedTrace110::exception(DWORD tid,DWORD code){
    if(!fixture)return false;
    if(code!=EXCEPTION_SINGLE_STEP)return false;
    auto p=threads.find(tid);if(p==threads.end()||!p->second.owned){event("single_step_thread_not_owned","\"thread\":"+std::to_string(tid)+",\"present\":"+(p!=threads.end()?"true":"false"));return false;}
    CONTEXT c{};c.ContextFlags=CONTEXT_FULL|CONTEXT_DEBUG_REGISTERS;
    if(!GetThreadContext(p->second.handle,&c)){event("single_step_context_failed","\"thread\":"+std::to_string(tid));return false;}
    unsigned hit=unsigned(c.Dr6)&15,owned=0;
    auto previous=p->second;
    DWORD actual[]={c.Dr0,c.Dr1,c.Dr2,c.Dr3};
    DWORD programmed[]={previous.programmed.Dr0,previous.programmed.Dr1,previous.programmed.Dr2,previous.programmed.Dr3};
    bool restored_context=false;
    for(unsigned i=0;i<4;++i)if(hit&(1u<<i)){
        if(const auto* known_binding=previous.history.match(i,actual[i],c.Dr7,c.Eip)){
            owned|=1u<<i;previous.kind[i]=known_binding->kind;previous.owner[i]=known_binding->owner;
            if(actual[i]!=programmed[i])restored_context=true;
        }
    }
    if(!owned){
        event("unowned_single_step","\"thread\":"+std::to_string(tid)+",\"dr6\":"+address(c.Dr6)+",\"dr7\":"+address(c.Dr7)+",\"dr0\":"+address(c.Dr0)+",\"dr1\":"+address(c.Dr1)+",\"dr2\":"+address(c.Dr2)+",\"dr3\":"+address(c.Dr3)+",\"programmed3\":"+address(previous.programmed.Dr3)+","+evidence(c));
        return false;
    }
    // Do not swallow an unrelated trap-flag single step or an unowned data trap.
    if((c.Dr6&(7u<<13))||(hit&~owned)){event("mixed_single_step_not_consumed","\"thread\":"+std::to_string(tid)+",\"dr6\":"+address(c.Dr6));return false;}
    if(restored_context)event("retired_binding_restored","\"thread\":"+std::to_string(tid)+",\"actual_dr3\":"+address(c.Dr3)+",\"desired_dr3\":"+address(programmed[3])+",\"reprogram_current_event_thread\":true");
    auto began=std::chrono::steady_clock::now();auto now=GetTickCount64();
    if(now-epoch>=1000){epoch=now;epoch_hits=0;pause_us=0;}
    ++traps;++epoch_hits;
    bool execute=(owned&1)||((owned&2)&&previous.kind[1]==2);
    bool change=false;
    if(!disabled){
        if(owned&1){
            std::string matches;
            if(effects_matches(c.Ecx,matches)){
                event("effects_destructor_with_active_reference",matches+","+owner_snapshot(c.Ecx)+",\"thread\":"+std::to_string(tid)+",\"retirement_after_destructor_not_yet_known\":true,"+evidence(c));
                if(!(c.Ecx&3)){effects_word=c.Ecx;word_writes=0;change=true;}
            }
        }
        if(owned&2){
            if(previous.kind[1]==2){
                if(!audio_wrapper||audio_wrapper==c.Ecx){bind_audio(c.Ecx,c,tid);change=true;}
                else event("additional_audio_wrapper_untracked","\"wrapper\":"+address(c.Ecx));
            }
            else{
                DWORD v=0;auto watched=previous.owner[1];bool ok=word(watched,v);
                event("effects_owner_word_write","\"thread\":"+std::to_string(tid)+",\"owner\":"+address(watched)+",\"word\":"+address(v)+",\"readable\":"+(ok?"true":"false")+",\"eip_is_after_write\":true,"+evidence(c));
                if(watched==effects_word&&++word_writes>=16){effects_word=0;change=true;event("effects_word_watch_retired","\"reason\":\"sixteen writes captured; destructor scout remains\"");}
            }
        }
        if(owned&4){
            event("audio_wrapper_pointer_write","\"thread\":"+std::to_string(tid)+",\"wrapper\":"+address(audio_wrapper)+",\"eip_is_after_write\":true,"+evidence(c));
            bind_audio(audio_wrapper,c,tid);change=true;
        }
        if(owned&8){
            DWORD vt=0,method=0;auto watched=previous.owner[3];bool ok=word(watched,vt)&&word(vt+0x18,method);
            event("audio_interface_word_write","\"thread\":"+std::to_string(tid)+",\"interface\":"+address(watched)+",\"vtable\":"+address(vt)+",\"slot18\":"+address(method)+",\"readable\":"+(ok?"true":"false")+",\"target_executable\":"+(ok&&executable(method)?"true":"false")+",\"eip_is_after_write\":true,"+evidence(c));
        }
    }
    auto elapsed=uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-began).count());
    pause_us+=elapsed;max_pause_us=std::max(max_pause_us,elapsed);
    if(!disabled&&(epoch_hits>400||pause_us>25000||dropped)){
        disabled=true;budget_exhausted=true;change=true;
        event("targeted_trace_budget_reached","\"hits_in_second\":"+std::to_string(epoch_hits)+",\"handler_us_in_second\":"+std::to_string(pause_us)+",\"read_only_sampling_and_crash_capture_continue\":true");
    }
    // Resume flag skips exactly this owned execution breakpoint once. No TF step,
    // no temporary unpatch window, and no swallowing native AV/SEH exceptions.
    c.ContextFlags=CONTEXT_CONTROL|CONTEXT_DEBUG_REGISTERS;c.Dr6=0;
    if(execute)c.EFlags|=0x10000;
    if(!SetThreadContext(p->second.handle,&c))throw std::runtime_error("Cannot continue owned processor trap");
    // SEH/APC continuation may restore old registers after another thread's
    // event. Force this event thread to the current binding; cache equality
    // alone cannot prove its actual resumed register state.
    p->second.dirty=true;(void)change;arm_all();
    return true;
}

inline void TargetedTrace110::heartbeat(){
    auto now=GetTickCount64();if(now-last_health<1000)return;last_health=now;
    unsigned armed=0;for(const auto& p:threads)if(p.second.owned)++armed;
    std::wstring path=folder+L"\\targeted110-health.json",temp=path+L".tmp";
    {std::ofstream f(temp);f<<"{\"version\":110,\"pid\":"<<pid<<",\"tick_ms\":"<<now
       <<",\"state\":\""<<(finished?"finished":!fixture?"withdrawn":disabled?"budget_disabled":(destructor||audio_entry)?"armed":"waiting_for_supported_modules")
       <<"\",\"effects_destructor\":"<<address(destructor)<<",\"audio_scout\":"<<address(audio_entry)
       <<",\"audio_wrapper\":"<<address(audio_wrapper)<<",\"audio_interface\":"<<address(audio_object)
       <<",\"effects_word_watch\":"<<address(effects_word)<<",\"tracked_threads\":"<<threads.size()
       <<",\"armed_threads\":"<<armed<<",\"traps\":"<<traps<<",\"records\":"<<records
       <<",\"dropped_records\":"<<dropped<<",\"trace_bytes\":"<<bytes<<",\"maximum_handler_us\":"<<max_pause_us
       <<",\"budget_exhausted\":"<<(budget_exhausted?"true":"false")
       <<",\"audio_binding_generations\":"<<binding<<",\"audio_coverage\":\"first observed wrapper; future pointer writes followed\"}\n";}
    MoveFileExW(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING);
}

inline void TargetedTrace110::restore(){
    unsigned restored=0,failed=0;
    for(auto& p:threads){auto& s=p.second;if(!s.owned)continue;
        DWORD exit=0;if(!GetExitCodeThread(s.handle,&exit)||exit!=STILL_ACTIVE){s.owned=false;continue;}
        if(SuspendThread(s.handle)==DWORD(-1)){++failed;continue;}
        s.original.ContextFlags=CONTEXT_DEBUG_REGISTERS;bool ok=SetThreadContext(s.handle,&s.original)!=FALSE;
        ResumeThread(s.handle);if(ok){s.owned=false;++restored;}else ++failed;
    }
    event("debug_registers_restored","\"threads\":"+std::to_string(restored)+",\"failures\":"+std::to_string(failed));
    require(failed==0,"Cannot restore diagnostic debug registers");
}
