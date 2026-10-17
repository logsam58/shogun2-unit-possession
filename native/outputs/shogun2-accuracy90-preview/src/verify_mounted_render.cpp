// Native-call fixture: real x86 detours, synthetic holders/assets, no game access.
#include "identity.h"
#include "steering.h"
#include "../vendor/include/MinHook.h"
#include <limits>
static uintptr_t base,possessed;
static Status storage{},*state=&storage;
template<class T>static T& field(uintptr_t p,unsigned off){return *reinterpret_cast<T*>(p+off);}
struct Draw {uintptr_t animation;unsigned hold_frame;float release;};
static Draw mounted_draw{};
static bool held=true,mounted=true,live=true;
static bool mounted_clock_held(uintptr_t actor){return held&&mounted&&live&&actor==possessed&&state->mode==2&&!state->release&&!state->stop&&!state->weapon_phase;}
#include "mounted_render_clock.h"
static uintptr_t seen_asset,seen_args[4];static float seen_time;static unsigned calls;
static uintptr_t other_holder;
static SamplePose70 sample_entry;
static HolderRefresh70 holder_entry;
struct Asset {
    unsigned char bytes[0xa0]{};
    __declspec(noinline) uintptr_t sample(uintptr_t a,float time,uintptr_t b,uintptr_t c,uintptr_t d){
        seen_asset=reinterpret_cast<uintptr_t>(this);seen_args[0]=a;seen_args[1]=b;seen_args[2]=c;seen_args[3]=d;seen_time=time;++calls;return 0x12345678;
    }
};
struct Holder {
    unsigned char bytes[0x200]{};
    __declspec(noinline) void refresh(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d,uintptr_t e,uintptr_t f,float g,uintptr_t h){
        require(b==22&&c==33&&d==44&&e==55&&f==66&&g==7.25f&&h==88,"Holder detour corrupted one of eight native arguments");
        if(a==99){RaiseException(0xe0420070,0,0,nullptr);return;}
        if(a==11){
            require(render_hold70.actor==possessed,"Owned holder missing render scope");
            holder_entry(reinterpret_cast<void*>(other_holder),12,b,c,d,e,f,g,h);
            require(render_hold70.actor==possessed,"Nested holder failed to restore owned scope");
        }
        if(a==12)require(!render_hold70.actor,"Other holder inherited owned time override");
        require(sample_entry(reinterpret_cast<void*>(seen_asset),101,2.27f,202,303,404)==0x12345678,"Pose detour changed native result");
    }
};
template<class T>static void* address(T member){static_assert(sizeof(member)==4,"x86 thiscall fixture required");void* p;memcpy(&p,&member,4);return p;}
static void refresh(Holder& holder,uintptr_t a=10){holder_entry(&holder,a,22,33,44,55,66,7.25f,88);}
static bool fault(Holder* holder){__try{holder_entry(holder,99,22,33,44,55,66,7.25f,88);}__except(EXCEPTION_EXECUTE_HANDLER){return true;}return false;}
static DWORD WINAPI unrelated_thread(void*){sample_entry(reinterpret_cast<void*>(seen_asset),101,2.27f,202,303,404);return 0;}
int main(){try{
    base=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,0x15b3000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));require(base!=0,"Synthetic engine allocation failed");
    field<float>(base,0x15b2448)=.1f;
    Asset asset,other_asset;Holder holder,other;unsigned char actor[0x500]{},descriptor[0x100]{},clip[16]{};
    possessed=reinterpret_cast<uintptr_t>(actor);other_holder=reinterpret_cast<uintptr_t>(&other);
    field<uintptr_t>(reinterpret_cast<uintptr_t>(&holder),8)=possessed;field<uintptr_t>(other_holder,8)=possessed+4;
    field<uintptr_t>(reinterpret_cast<uintptr_t>(clip),4)=reinterpret_cast<uintptr_t>(descriptor);
    field<uintptr_t>(reinterpret_cast<uintptr_t>(descriptor),0x50)=reinterpret_cast<uintptr_t>(&asset);
    field<unsigned>(reinterpret_cast<uintptr_t>(descriptor),0x64)=57;
    field<float>(reinterpret_cast<uintptr_t>(&asset),0x48)=20;field<float>(reinterpret_cast<uintptr_t>(&asset),0x4c)=5.7f;
    mounted_draw={reinterpret_cast<uintptr_t>(clip),22,2.35f};storage.mode=2;storage.lease_generation=7;storage.draws_started=3;storage.actor_id=278;
    require(MH_Initialize()==MH_OK,"Fixture hook initialization failed");
    require(MH_CreateHook(address(&Asset::sample),reinterpret_cast<void*>(&sample_pose_hook70),reinterpret_cast<void**>(&original_sample_pose70))==MH_OK,"Fixture sample detour failed");
    require(MH_CreateHook(address(&Holder::refresh),reinterpret_cast<void*>(&holder_refresh_hook70),reinterpret_cast<void**>(&original_holder_refresh70))==MH_OK,"Fixture holder detour failed");
    sample_entry=reinterpret_cast<SamplePose70>(address(&Asset::sample));holder_entry=reinterpret_cast<HolderRefresh70>(address(&Holder::refresh));
    require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Fixture detours not enabled");seen_asset=reinterpret_cast<uintptr_t>(&asset);
    for(unsigned i=0;i<100;++i){
        refresh(holder);require(std::abs(seen_time-2.2f)<1e-6f,"Held rider retained native sub-tick oscillation");
        require(seen_asset==reinterpret_cast<uintptr_t>(&asset)&&seen_args[0]==101&&seen_args[1]==202&&seen_args[2]==303&&seen_args[3]==404,"Pose detour corrupted native arguments");
        require(!render_hold70.actor,"Holder scope escaped callback");
    }
    refresh(holder,11);require(std::abs(seen_time-2.2f)<1e-6f,"Nested unrelated holder broke owned hold");
    refresh(other,12);require(seen_time==2.27f,"Other actor sharing asset had its clock frozen");
    auto scope=render_hold_for70(reinterpret_cast<uintptr_t>(&holder));render_hold70=scope;
    auto previous_count=render_clamps70;HANDLE thread=CreateThread(nullptr,0,unrelated_thread,nullptr,0,nullptr);require(thread!=nullptr,"Fixture thread creation failed");
    WaitForSingleObject(thread,INFINITE);CloseHandle(thread);require(render_clamps70==previous_count&&seen_time==2.27f,"Thread-local scope leaked across render threads");
    sample_entry(&other_asset,101,2.27f,202,303,404);require(seen_time==2.27f,"Different blend asset was frozen");seen_asset=reinterpret_cast<uintptr_t>(&asset);
    ++state->draws_started;sample_entry(&asset,101,2.27f,202,303,404);require(seen_time==2.27f,"Retired draw remained scoped");--state->draws_started;
    ++state->lease_generation;sample_entry(&asset,101,2.27f,202,303,404);require(seen_time==2.27f,"Retired possession lease remained scoped");--state->lease_generation;
    render_hold70={};require(fault(&holder)&&!render_hold70.actor,"Native exception leaked owned scope");
    for(unsigned condition=0;condition<7;++condition){
        held=condition!=0;mounted=condition!=1;live=condition!=2;state->release=condition==3;state->stop=condition==4;state->weapon_phase=condition==5?4:condition==6?3:0;
        refresh(holder);require(seen_time==2.27f,"Release, infantry, death, fired or reload path froze native animation");
    }
    held=mounted=live=true;state->release=state->stop=state->weapon_phase=0;
    field<float>(base,0x15b2448)=std::numeric_limits<float>::quiet_NaN();refresh(holder);require(seen_time==2.27f,"Invalid native timing overrode render time");
    field<float>(base,0x15b2448)=.1f;auto before_asset=asset;auto before_holder=holder;
    refresh(holder);require(!memcmp(&asset,&before_asset,sizeof asset)&&!memcmp(&holder,&before_holder,sizeof holder),"Time override mutated native holder/shared asset memory");
    require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&MH_Uninitialize()==MH_OK,"Fixture hooks did not retire");VirtualFree(reinterpret_cast<void*>(base),0,MEM_RELEASE);
    std::cout<<"PASS: mounted render-time native x86 detours preserve all arguments/results, hold current rider sampling time, isolate shared assets/other holders/threads, restore nested and exceptional scopes, retire on draw/lease/release/death/fire/reload, reject invalid timing; native objects unchanged.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
