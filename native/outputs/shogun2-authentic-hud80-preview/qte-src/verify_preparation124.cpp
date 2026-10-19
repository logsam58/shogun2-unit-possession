// Full production dispatch with an unlinked player: the initial encounter
// path must reject foreign participants before computing all their positions.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cstdint>
static uint64_t reads124,bytes124;static bool fail_prefix124;
static uint32_t mutate_at124,mutate_to124;static bool mutated124;
static BOOL WINAPI read124(HANDLE process,LPCVOID address,LPVOID out,SIZE_T size,SIZE_T* got){
    ++reads124;bytes124+=size;
    if(fail_prefix124&&size==0xef){if(got)*got=0;return FALSE;}
    auto result=ReadProcessMemory(process,address,out,size,got);
    if(result&&size==0xef&&mutate_at124){
        *reinterpret_cast<uint32_t*>(mutate_at124)=mutate_to124;mutate_at124=0;mutated124=true;
    }return result;
}
#define ReadProcessMemory read124
#include "playable_runtime.cpp"
#undef ReadProcessMemory
static bool original_participant124(const Reader& r,uint32_t witness,uint32_t actor){
    SelectionInput input;if(!actor||!selection_input(r,witness,engine,input))return false;
    for(uint32_t i=0;i<input.count;++i){uint32_t member{};if(!r.get(input.begin+i*4,member))return false;if(member==actor)return true;}
    return false;
}
static bool original_encounter124(const Reader& r,uint32_t actor,uint32_t witness,bool updating){
    uint32_t backlink{};if(!actor||!r.get(actor+0x688,backlink))return false;
    if(backlink==witness)return true;
    return !backlink&&!updating&&original_participant124(r,witness,actor);
}
static unsigned forwards124,selects124;static void* roles124[5];static unsigned count124;
static NativeVerdict* __fastcall select124(void*,void*,NativeVerdict* out,void*const* begin,void*const* end,uint32_t a,uint32_t b){
    ++selects124;require(begin==roles124&&end==roles124+count124&&a==7&&b==9,"Native selector arguments changed");
    *out={5,0xabc000,0xabc100};return out;
}
static uint32_t __fastcall prepare124(void*,void*){
    ++forwards124;NativeVerdict verdict{};
    require(select_hook(nullptr,nullptr,&verdict,roles124,roles124+count124,7,9)==&verdict&&verdict.outcome==5&&verdict.attacker_address==0xabc000&&verdict.defender_address==0xabc100,"Native selector output changed");
    return 0xcafe006f;
}
int main(int argc,char** argv){void* memory=nullptr;try{
    const bool baseline=argc>1&&!strcmp(argv[1],"--baseline");
    memory=VirtualAlloc(nullptr,0x1bc9000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory,"Engine fixture");engine=uint32_t(memory);
    std::vector<uint8_t> root(0x120),env(0x28300),common(0xc0),manager(0x900),camera(0x200),p(0x700),other(0x700),horse(0x700),pu(0x1000),ou(0x1000),w(0x110),riders(4);
    auto ptr=[](auto& x){return uint32_t(x.data());};auto put=[](uint32_t at,auto value){memcpy(reinterpret_cast<void*>(at),&value,sizeof value);};
    put(engine+0x1bc8180,ptr(root));put(ptr(root)+0x110,ptr(env));put(ptr(env)+0x281fc,ptr(manager));put(ptr(manager)+0x8e0,ptr(camera));put(ptr(camera)+0x188,ptr(p));
    put(ptr(env)+8,ptr(common));put(ptr(common)+0x9c,0x123000u);
    for(auto a:{ptr(p),ptr(other),ptr(horse)}){
        put(a,actor_table(engine,a==ptr(horse)?Horse:Human));put(a+0xf0,uint8_t(1));put(a+0x28,ptr(env));
        put(a+0x214,a==ptr(other)?ptr(ou):ptr(pu));put(a+0x1c8,a==ptr(p)?11u:a==ptr(other)?12u:13u);
        put(a+0x48,1.f);put(a+0x4c,2.f);put(a+0x50,3.f);
    }
    put(ptr(pu)+4,7u);put(ptr(ou)+4,8u);put(ptr(horse)+0x6cc,1u);put(ptr(horse)+0x6d0,ptr(riders));put(ptr(riders),ptr(p));
    auto valid=[&](unsigned count=5,bool alternate=false){
        std::fill(w.begin(),w.end(),uint8_t(0));put(ptr(w)+4,ptr(env));put(ptr(w)+0xee,uint8_t(alternate));
        put(ptr(w)+(alternate?0x38:0x24),count);
        for(unsigned i=0;i<count;++i)put(ptr(w)+(alternate?0x30:0x10)+i*4,ptr(other));
        put(ptr(p)+0x688,0u);put(ptr(horse)+0x688,0u);put(ptr(p)+0xf0,uint8_t(1));put(ptr(other)+0xf0,uint8_t(1));
        put(ptr(p)+0x48,1.f);put(ptr(other)+0x48,1.f);put(ptr(env)+8,ptr(common));
    };
    observation::PossessionPrefix lease{};lease.magic=0x31504353;lease.version=46;lease.pid=GetCurrentProcessId();lease.ready=1;lease.mode=2;lease.actor_id=11;lease.unit_id=7;lease.lease_generation=1;lease.heartbeat=GetTickCount64();possession=&lease;
    Status telemetry{};state=&telemetry;enabled=1;player_address=LONG(ptr(p));
    LARGE_INTEGER frequency124;QueryPerformanceFrequency(&frequency124);frequency=double(frequency124.QuadPart);
    native_prepare=reinterpret_cast<NativeWitnessCall>(prepare124);selection_ports.select=reinterpret_cast<NativeSelector>(select124);
    session=new Session(reader(),selection_ports.calls(),{nullptr,prepare_port,update_port,routing,facing_ready,retarget_port,effective_pair,retain_selected_pair},engine,{0,5,true},true,true,true,true,true);
    uint32_t random=0x124abcde;auto next=[&](){random^=random<<13;random^=random>>17;return random^=random<<5;};
    unsigned checks=0;
    for(unsigned n=0;n<16000;++n){
        valid();unsigned alternate=next()%3,count=next()%7;
        put(ptr(w)+0xee,uint8_t(alternate));put(ptr(w)+(alternate?0x38:0x24),count);
        for(unsigned i=0;i<5;++i){auto v=next()%5;put(ptr(w)+0x10+i*4,v==0?ptr(p):v==1?ptr(horse):v==2?ptr(other):v==3?0u:7u);}
        for(unsigned i=0;i<2;++i){auto v=next()%4;put(ptr(w)+0x30+i*4,v==0?ptr(p):v==1?ptr(horse):v==2?ptr(other):0u);}
        put(ptr(w)+0xc,uint8_t(n%13==0));put(ptr(w)+0xd0,uint8_t(n%17==0));
        if(n%19==0)put(ptr(w)+4,0u);if(n%23==0)put(ptr(env)+8,0u);
        if(n%29==0)put(ptr(other)+0x48,std::numeric_limits<float>::quiet_NaN());
        if(n%31==0)put(ptr(p)+0xf0,uint8_t(0));
        fail_prefix124=n%7==0;
        auto actor=n%3==0?ptr(horse):ptr(p);auto r=reader();
        require(preparation_participant(r,ptr(w),engine,actor)==original_participant124(r,ptr(w),actor),"Initial participant decision changed");
        put(actor+0x688,n%5==0?ptr(w):n%11==0?7u:0u);
        for(bool updating:{false,true})require(player_encounter(r,actor,ptr(w),engine,updating)==original_encounter124(r,actor,ptr(w),updating),"Backlink/update routing changed");
        ++checks;
    }
    fail_prefix124=false;
    // Matching candidates still repeat all native input checks after the filter.
    if(!baseline){
        for(unsigned mutation=0;mutation<3;++mutation){
            valid(2);put(ptr(w)+0x10,ptr(p));mutated124=false;
            mutate_at124=mutation==0?ptr(w)+0x10:mutation==1?ptr(p)+0xf0:ptr(w)+0xd0;
            mutate_to124=mutation==0?ptr(other):mutation==1?0u:1u;
            require(!preparation_participant(reader(),ptr(w),engine,ptr(p))&&mutated124,"Filter granted stale participant/state");
        }
    }
    // Current participants are reread on every call, including changing roles.
    for(unsigned n=0;n<1000;++n){
        valid(2,n%2);auto begin=ptr(w)+(n%2?0x30:0x10);
        require(!preparation_participant(reader(),ptr(w),engine,ptr(p)),"Foreign initial encounter admitted");
        put(begin,ptr(p));require(preparation_participant(reader(),ptr(w),engine,ptr(p)),"New player encounter missed");
        put(begin,ptr(other));require(!preparation_participant(reader(),ptr(w),engine,ptr(p)),"Stale membership retained");
    }
    SYSTEM_INFO si;GetSystemInfo(&si);auto pages=static_cast<uint8_t*>(VirtualAlloc(nullptr,si.dwPageSize*2,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(pages,"Page fixture");DWORD old{};
    require(VirtualProtect(pages+si.dwPageSize,si.dwPageSize,PAGE_NOACCESS,&old),"Page protection");auto edge=uint32_t(pages+si.dwPageSize-0x28);put(edge+0x10,ptr(p));put(edge+0x24,2u);
    require(!preparation_participant(reader(),edge,engine,ptr(p))&&!original_participant124(reader(),edge,ptr(p)),"Unreadable tail admitted");
    MEMORY_BASIC_INFORMATION mbi{};require(VirtualQuery(pages+si.dwPageSize,&mbi,sizeof mbi)&&mbi.Protect==PAGE_NOACCESS,"Protection changed");VirtualFree(pages,0,MEM_RELEASE);
    valid(5);fail_prefix124=true;put(ptr(w)+0x10,ptr(p));require(preparation_participant(reader(),ptr(w),engine,ptr(p)),"Individual-read fallback lost valid player");fail_prefix124=false;
    for(bool mounted:{false,true})for(unsigned count:{2u,5u}){
        valid(count);put(ptr(p)+0x210,mounted?ptr(horse):0u);player_address=LONG(mounted?ptr(horse):ptr(p));lease.heartbeat=GetTickCount64();
        require(fps_actor()==uint32_t(player_address),"Physical owner fixture invalid");
        count124=count;for(auto& role:roles124)role=other.data();forwards124=selects124=0;reads124=bytes124=0;
        require(dispatch(ptr(w),1,false)==0xcafe006f&&forwards124==1&&selects124==1,"Foreign initial native replay differs");
        auto reads=reads124,bytes=bytes124;const unsigned loops=20000;LARGE_INTEGER begin,end;QueryPerformanceCounter(&begin);reads124=0;forwards124=selects124=0;
        for(unsigned n=0;n<loops;++n){if(!(n%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),1,false)==0xcafe006f,"Repeated foreign result differs");}
        QueryPerformanceCounter(&end);
        require(forwards124==loops&&selects124==loops&&!coordinator&&!owns_coordinator&&!replay_parent&&!selector_missed&&!pending_address&&!delivery_address&&!session->occupied(),"Unrelated work changed ownership/native calls");
        std::cout<<"PREPARATION124 baseline="<<baseline<<" mounted="<<mounted<<" participants="<<count<<" callback_reads="<<reads<<" callback_bytes="<<bytes<<" callbacks="<<loops<<" reads="<<reads124<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
        if(!baseline)require(reads<=30,"Foreign initial preparation still evaluates all participant positions");
    }
    delete session;session=nullptr;VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: "<<checks<<" frozen participant/backlink cases, 1000 membership changes, stale filter rejection, partial-read fallback and full native forwarding. Synthetic CPU measurements, not game FPS.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';if(memory)VirtualFree(memory,0,MEM_RELEASE);return 1;}}
