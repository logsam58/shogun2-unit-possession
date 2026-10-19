// Exercise the production callback, not just the reservation prefilter.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cstdint>
static uint64_t reads118,bytes118;static bool reject_bulk118;
static BOOL WINAPI counted_read118(HANDLE process,LPCVOID address,LPVOID out,SIZE_T size,SIZE_T* got){
    ++reads118;bytes118+=size;
    if(reject_bulk118&&size==0x50){if(got)*got=0;return FALSE;}
    return ReadProcessMemory(process,address,out,size,got);
}
#define ReadProcessMemory counted_read118
#include "playable_runtime.cpp"
#undef ReadProcessMemory
static unsigned forwards118,selects118;
static uint32_t __fastcall foreign_update118(void*,void*){++forwards118;return 0xcafe006f;}
static NativeVerdict* __fastcall foreign_select118(void*,void*,NativeVerdict* out,void*const* begin,void*const* end,uint32_t a,uint32_t b){
    ++selects118;require(end-begin==2&&a==7&&b==9,"Selector arguments changed");
    *out={5,uint32_t(begin[0]),uint32_t(begin[1])};return out;
}
// Frozen pre-Test118 predicate, including individual field reads and fallback.
static bool includes_before118(const Reader& r,uint32_t actor,uint32_t witness){
    if(!actor||!witness||witness>UINT32_MAX-0x100)return false;
    uint32_t backlink{};if(r.get(actor+0x688,backlink)&&backlink==witness)return true;
    for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
        uint32_t count{};if(!r.get(witness+offsets.second,count)||count>5)continue;
        for(uint32_t i=0;i<count;++i){uint32_t member{};if(!r.get(witness+offsets.first+i*4,member))break;if(member==actor)return true;}
    }return false;
}
static bool scoped_before118(uint32_t witness){
    return includes_before118(reader(),fps_actor(),witness)||includes_before118(reader(),fps_camera_actor(),witness);
}
int main(int argc,char** argv){void* memory=nullptr;try{
    bool baseline=argc>1&&!strcmp(argv[1],"--baseline");
    memory=VirtualAlloc(nullptr,0x1bc9000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory,"Fixture engine allocation");engine=uint32_t(memory);
    std::vector<uint8_t> root(0x120),env(0x28300),manager(0x900),camera(0x200),p(0x700),other(0x700),pu(0x1000),ou(0x1000),held(0x110),w(0x110),horse(0x700),riders(4);
    auto ptr=[](auto& x){return uint32_t(x.data());};auto put=[](uint32_t address,auto value){memcpy(reinterpret_cast<void*>(address),&value,sizeof value);};
    put(engine+0x1bc8180,ptr(root));put(ptr(root)+0x110,ptr(env));put(ptr(env)+0x281fc,ptr(manager));put(ptr(manager)+0x8e0,ptr(camera));put(ptr(camera)+0x188,ptr(p));
    for(auto a:{ptr(p),ptr(other),ptr(horse)}){put(a,engine+(a==ptr(horse)?0x15e8524:0x15dfec8));put(a+0xf0,uint8_t(1));put(a+0x28,ptr(env));put(a+0x688,ptr(held));put(a+0x4c,1.f);}
    put(ptr(p)+0x214,ptr(pu));put(ptr(other)+0x214,ptr(ou));put(ptr(horse)+0x214,ptr(pu));put(ptr(pu)+4,7u);put(ptr(ou)+4,8u);
    put(ptr(p)+0x1c8,11u);put(ptr(other)+0x1c8,12u);put(ptr(horse)+0x1c8,13u);put(ptr(horse)+0x6cc,1u);put(ptr(horse)+0x6d0,ptr(riders));put(ptr(riders),ptr(p));
    observation::PossessionPrefix lease{};lease.magic=0x31504353;lease.version=46;lease.pid=GetCurrentProcessId();lease.ready=1;lease.mode=2;lease.actor_id=11;lease.unit_id=7;lease.lease_generation=1;lease.heartbeat=GetTickCount64();possession=&lease;
    Status telemetry{};state=&telemetry;enabled=1;native_update=reinterpret_cast<NativeWitnessCall>(foreign_update118);native_prepare=reinterpret_cast<NativeWitnessCall>(foreign_update118);
    LARGE_INTEGER freq;QueryPerformanceFrequency(&freq);frequency=double(freq.QuadPart);
    player_address=LONG(ptr(p));selection_ports.select=reinterpret_cast<NativeSelector>(foreign_select118);
    session=new Session(reader(),selection_ports.calls(),{nullptr,prepare_port,update_port,routing,facing_ready,retarget_port,effective_pair,retain_selected_pair},engine,{0,5,true},true,true,true,true,true);
    unsigned checks=0;uint32_t random=0x31415926;auto next=[&](){random^=random<<13;random^=random>>17;return random^=random<<5;};
    for(unsigned n=0;n<6000;++n){
        std::fill(w.begin(),w.end(),uint8_t(0));
        for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
            for(unsigned i=0;i<5;++i){auto k=next()%5;put(ptr(w)+offsets.first+i*4,k==0?ptr(p):k==1?ptr(horse):k==2?ptr(other):k==3?0u:7u);}
            put(ptr(w)+offsets.second,next()%8);
        }
        bool mounted=n%3==0;put(ptr(p)+0x210,mounted?ptr(horse):0u);
        put(ptr(p)+0x688,n%11==0?ptr(w):ptr(held));put(ptr(horse)+0x688,n%13==0?ptr(w):ptr(held));
        put(ptr(p)+0x1c8,n%17==0?99u:11u);put(ptr(pu)+4,n%19==0?99u:7u);put(ptr(p)+0xf0,uint8_t(n%23==0?0:1));
        put(ptr(horse)+0x6cc,n%29==0?2u:1u);lease.release=n%31==0;lease.heartbeat=GetTickCount64();reject_bulk118=n%7==0;
        require(scoped(ptr(w))==scoped_before118(ptr(w)),"Scope differs from original on identity, death, mounted alias, backlink, counts or fallback");++checks;
    }
    reject_bulk118=false;lease.release=0;lease.heartbeat=GetTickCount64();put(ptr(p)+0x1c8,11u);put(ptr(pu)+4,7u);put(ptr(p)+0xf0,uint8_t(1));put(ptr(p)+0x210,0u);put(ptr(p)+0x688,ptr(held));
    // Native reads can straddle pages. Failed bulk reads must retain the exact
    // individual-read behavior and leave PAGE_GUARD/NOACCESS protections alone.
    SYSTEM_INFO si;GetSystemInfo(&si);auto pages=static_cast<uint8_t*>(VirtualAlloc(nullptr,si.dwPageSize*2,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(pages,"Page fixture");
    DWORD old;require(VirtualProtect(pages+si.dwPageSize,si.dwPageSize,PAGE_NOACCESS,&old),"Page protection");auto edge=uint32_t(pages+si.dwPageSize-0x28);
    put(edge+0x24,1u);put(edge+0x10,ptr(p));require(scoped(edge)&&scoped_before118(edge),"Readable first group lost at a page boundary");
    put(edge+0x10,ptr(other));require(!scoped(edge)&&!scoped_before118(edge),"Unreadable groups invented ownership");VirtualFree(pages,0,MEM_RELEASE);checks+=2;
    std::fill(w.begin(),w.end(),uint8_t(0));put(ptr(w)+0x24,5u);for(unsigned i=0;i<5;++i)put(ptr(w)+0x10+i*4,ptr(other));
    reads118=bytes118=0;forwards118=0;coordinator=0;
    require(dispatch(ptr(w),1,true)==0xcafe006f&&forwards118==1,"Foreign full callback did not forward exactly once");
    auto callback_reads=reads118,callback_bytes=bytes118;
    // A selector outside an owned replay cannot change its stock verdict. It
    // must not perform any possession lookup solely to discard the result.
    void* participants[2]={other.data(),horse.data()};NativeVerdict verdict{};owns_coordinator=false;replay_parent=0;reads118=bytes118=0;
    require(select_hook(nullptr,nullptr,&verdict,participants,participants+2,7,9)==&verdict&&selects118==1&&verdict.outcome==5,"Unrelated selector changed native output");auto selector_reads=reads118;
    const unsigned iterations=20000;forwards118=0;reads118=bytes118=0;LARGE_INTEGER begin,end;QueryPerformanceCounter(&begin);
    for(unsigned i=0;i<iterations;++i){if(!(i%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),1,true)==0xcafe006f,"Crowd callback changed native result");}
    QueryPerformanceCounter(&end);require(forwards118==iterations&&coordinator==0&&!owns_coordinator&&!pending_address&&!delivery_address,"Crowd changed player/session ownership");
    std::cout<<"SCOPE118 baseline="<<baseline<<" checks="<<checks<<" callback_reads="<<callback_reads<<" callback_bytes="<<callback_bytes<<" selector_reads="<<selector_reads<<" callbacks="<<iterations<<" reads="<<reads118<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
    if(!baseline){require(callback_reads<=32,"Unrelated full callback still repeats expensive ownership proof");require(selector_reads==0,"Foreign selector still queries possession");}
    delete session;session=nullptr;VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: full production dispatch and selection wrappers, fresh-identity differential cases and unreadable-page fallback. Offline CPU benchmark, not game FPS.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';if(memory)VirtualFree(memory,0,MEM_RELEASE);return 1;}}
