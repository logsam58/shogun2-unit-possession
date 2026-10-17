#include "identity.h"
#include "steering.h"
#include "possession_lease.h"
#include "../vendor/include/MinHook.h"
static uintptr_t base,possessed,environment,unit;
static Status storage{},*state=&storage;
static volatile LONG enabled=1;
static PossessionLease possession_lease;
template<class T>static T& field(uintptr_t p,unsigned off){return *reinterpret_cast<T*>(p+off);}
#include "native_subject.h"
static bool followed(uintptr_t actor,bool){return possession_lease.active&&actor==possessed&&
    field<uintptr_t>(actor,0x214)==unit&&field<unsigned>(actor,0x1c8)==possession_lease.identity.actor_id;}
using UnitSpeed=float(__thiscall*)(void*,unsigned);
static float fixture_grade=.25f;static bool grade_valid=true,release_during_update=false;
static bool sample_native_grade(uintptr_t,Intent,float,float& grade,float& factor,float& distance){
    grade=fixture_grade;factor=1;distance=.35f;return grade_valid;
}
#include "native_fatigue.h"
static unsigned native_updates;
static void __fastcall synthetic_native_update(void* object,void*){
    uintptr_t actor=reinterpret_cast<uintptr_t>(object);
    ++native_updates;field<float>(actor,0x408)=field<float>(actor,0x1b8);
    int cost=0;
    reinterpret_cast<NativeFatigueAction>(base+0x3c73e0)(object,&cost);
    field<int>(actor,0x404)=cost;field<int>(actor,0x39c)+=cost;
    if(release_during_update)state->release=1;
}
static std::vector<unsigned char> code_at(const std::vector<unsigned char>& image,unsigned rva,unsigned size){
    auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(image.data());
    auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
    auto sections=IMAGE_FIRST_SECTION(nt);
    for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];
        if(rva>=s.VirtualAddress&&rva+size<=s.VirtualAddress+s.SizeOfRawData){unsigned off=s.PointerToRawData+rva-s.VirtualAddress;
            require(off+size<=image.size(),"Native fixture bounds");return {image.begin()+off,image.begin()+off+size};}}
    throw std::runtime_error("Native fixture section absent");
}
int main(){void* memory=nullptr;bool initialized=false;try{
    std::wstring path=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Total War SHOGUN 2\\empire.retail.dll";
    require(hash_file(path)==EXPECTED_HASH,"Fatigue fixture requires verified engine");
    std::ifstream stream(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(stream)),{});
    memory=VirtualAlloc(nullptr,0x1bc9000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory!=nullptr,"Fixture allocation");
    base=reinterpret_cast<uintptr_t>(memory);
    auto update=code_at(image,0x3f06e0,8),action=code_at(image,0x3c73e0,10),value=code_at(image,0x1be3a0,4);
    auto update_code=reinterpret_cast<unsigned char*>(base+0x3f06e0);
    auto action_code=reinterpret_cast<unsigned char*>(base+0x3c73e0);
    memcpy(update_code,update.data(),update.size());update_code[8]=0xe8;
    int32_t delta=int32_t(reinterpret_cast<uintptr_t>(&synthetic_native_update)-(base+0x3f06e0+13));memcpy(update_code+9,&delta,4);
    const unsigned char update_tail[]={0x5e,0x5d,0x5b,0x83,0xc4,0x10,0xc3};memcpy(update_code+13,update_tail,sizeof update_tail);
    memcpy(action_code,action.data(),action.size());
    const unsigned char action_tail[]={0x8b,0x44,0x24,0x0c,0x8b,0x96,0,4,0,0,0x89,0x10,0x5f,0x5e,0xc2,4,0};
    memcpy(action_code+10,action_tail,sizeof action_tail);
    memcpy(reinterpret_cast<void*>(base+0x1be3a0),value.data(),value.size());
    const unsigned char speed[]={0x80,0x7c,0x24,4,0,0x75,6,0xd9,0x41,0x10,0xc2,4,0,0xd9,0x41,0x14,0xc2,4,0};
    memcpy(reinterpret_cast<void*>(base+0x18b470),speed,sizeof speed);
    for(unsigned rva:{0x3f06e0u,0x3c73e0u,0x1be3a0u,0x18b470u}){
        DWORD old=0;require(VirtualProtect(reinterpret_cast<void*>((base+rva)&~uintptr_t(4095)),4096,PAGE_EXECUTE_READ,&old)!=0,"Fixture code protection");}
    FlushInstructionCache(GetCurrentProcess(),memory,0x1bc9000);
    std::array<unsigned char,0x600> actor{},other{};
    std::array<unsigned char,0x1000> kv{};std::array<unsigned char,0x40> env{},common{},root{};std::array<unsigned char,0x40> unit_bytes{};
    possessed=reinterpret_cast<uintptr_t>(actor.data());environment=reinterpret_cast<uintptr_t>(env.data());unit=reinterpret_cast<uintptr_t>(unit_bytes.data());
    field<uintptr_t>(base,0x1bc8180)=reinterpret_cast<uintptr_t>(root.data());
    // Root+110 is needed by the exact restoration guard; use a sufficiently sized root.
    std::array<unsigned char,0x120> battle_root{};
    field<uintptr_t>(base,0x1bc8180)=reinterpret_cast<uintptr_t>(battle_root.data());
    field<uintptr_t>(reinterpret_cast<uintptr_t>(battle_root.data()),0x110)=environment;
    field<float>(base,0x15b2448)=.1f;
    field<uintptr_t>(environment,8)=reinterpret_cast<uintptr_t>(common.data());
    field<uintptr_t>(possessed,0)=base+0x15dfec8;field<uintptr_t>(possessed,0x28)=environment;field<unsigned char>(possessed,0xf0)=1;
    field<uintptr_t>(possessed,0x214)=unit;field<unsigned>(possessed,0x1c8)=23;
    field<unsigned>(unit,4)=7;field<float>(unit,0x10)=2.1f;field<float>(unit,0x14)=4.25f;
    field<uintptr_t>(possessed,0x55c)=reinterpret_cast<uintptr_t>(kv.data());
    field<int>(reinterpret_cast<uintptr_t>(kv.data()),0xc+0x5c)=-8;
    field<int>(reinterpret_cast<uintptr_t>(kv.data()),0x3cc+0x5c)=0;
    // Deliberately distinguish shooting from running: stock values are both 6.
    field<int>(reinterpret_cast<uintptr_t>(kv.data()),0x24c+0x5c)=99;
    field<int>(reinterpret_cast<uintptr_t>(kv.data()),0x4ec+0x5c)=6;
    possession_lease.active=true;possession_lease.generation=1;possession_lease.identity.actor=possessed;
    possession_lease.identity.environment=environment;possession_lease.identity.unit=unit;
    possession_lease.identity.actor_id=23;possession_lease.identity.unit_id=7;
    state->mode=POSSESSED;
    unsigned char before_update[16],before_action[16];memcpy(before_update,update_code,16);memcpy(before_action,action_code,16);
    require(MH_Initialize()==MH_OK,"Fatigue hook initialization");initialized=true;
    create_fatigue_hooks();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Fatigue hooks enable");
    auto call=reinterpret_cast<NativeFatigueUpdate>(update_code);
    auto check=[&](unsigned input,int native_cost,int expected,bool weapon=false){
        state->input=input;state->weapon_phase=weapon?4:0;state->release=0;
        field<int>(possessed,0x400)=native_cost;field<int>(possessed,0x39c)=20;
        field<float>(possessed,0x1b8)=-.3f;
        unsigned old=native_updates;call(reinterpret_cast<void*>(possessed));
        require(native_updates==old+1,"Native fatigue callback must run exactly once");
        require(field<int>(possessed,0x404)==expected&&field<int>(possessed,0x39c)==20+expected,"Personal action cost mismatch");
        require(field<float>(possessed,0x408)==fixture_grade&&field<float>(possessed,0x1b8)==-.3f,"Directional grade injection/restoration");
        require(!fatigue_owner,"Fatigue ownership must remain scoped");
    };
    for(unsigned i=0;i<1000;++i){check(0,3,-8);check(FORWARD,-8,0);check(FORWARD|RUN,-8,6);check(FORWARD|RUN,3,6,true);check(FORWARD,8,8,true);}
    field<float>(unit,0x14)=2.1f;check(FORWARD|RUN,-8,0);field<float>(unit,0x14)=4.25f;
    release_during_update=true;check(FORWARD|RUN,-8,6);release_during_update=false;state->release=0;
    field<unsigned>(possessed,0x1f0)=18;field<int>(possessed,0x400)=8;field<float>(possessed,0x1b8)=-.3f;
    call(reinterpret_cast<void*>(possessed));require(field<int>(possessed,0x404)==8&&field<float>(possessed,0x408)==-.3f,"Stock melee forwarding");field<unsigned>(possessed,0x1f0)=0;
    uintptr_t other_actor=reinterpret_cast<uintptr_t>(other.data());field<uintptr_t>(other_actor,0x28)=environment;
    field<int>(other_actor,0x400)=15;call(reinterpret_cast<void*>(other_actor));require(field<int>(other_actor,0x404)==15,"Other actor fatigue changed");
    // Native motion capture showed movement fatigue charged to the horse,
    // while the linked rider retains native non-locomotion fatigue handling.
    memcpy(other.data(),actor.data(),actor.size());field<uintptr_t>(other_actor,0)=base+0x15e8524;field<unsigned>(other_actor,0x1c8)=32;
    possession_lease.identity.mount=other_actor;possession_lease.identity.mount_id=32;
    state->input=FORWARD|RUN;state->weapon_phase=0;
    field<int>(other_actor,0x400)=-8;field<int>(other_actor,0x39c)=20;field<float>(other_actor,0x1b8)=-.3f;
    call(reinterpret_cast<void*>(other_actor));
    require(field<int>(other_actor,0x404)==6&&field<int>(other_actor,0x39c)==26&&field<float>(other_actor,0x1b8)==-.3f&&!fatigue_owner,"Horse movement fatigue/restoration");
    field<int>(possessed,0x400)=15;field<float>(possessed,0x1b8)=-.3f;call(reinterpret_cast<void*>(possessed));
    require(field<int>(possessed,0x404)==15&&field<float>(possessed,0x408)==-.3f,"Rider cannot receive duplicate horse movement costs");
    possession_lease.identity.mount=0;possession_lease.identity.mount_id=0;
    grade_valid=false;field<int>(possessed,0x400)=3;call(reinterpret_cast<void*>(possessed));
    require(field<int>(possessed,0x404)==3&&!fatigue_owner&&state->fatigue_errors==1,"Invalid grade must retain native fatigue");
    field<unsigned char>(possessed,0xf0)=0;field<int>(possessed,0x400)=11;call(reinterpret_cast<void*>(possessed));
    require(field<int>(possessed,0x404)==11&&!fatigue_owner,"Corpse fatigue must forward unchanged");
    require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Fatigue disable");
    require(!memcmp(before_update,update_code,16)&&!memcmp(before_action,action_code,16),"Fatigue native bytes restoration");
    require(native_fatigue_signatures(base),"Fatigue shutdown signature validation");
    require(MH_Uninitialize()==MH_OK,"Fatigue cleanup");initialized=false;
    VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS:5000 scoped fatigue callbacks using original native entry bytes; thiscall/RET4 ABI, one native update, idle/walk/run/reload costs, denied run, release restoration, stock melee, other actor and invalid-grade forwarding; hook restoration. Real native fatigue/weather/aggregation remains a live check.\n";return 0;
}catch(const std::exception& e){if(initialized)MH_Uninitialize();if(memory)VirtualFree(memory,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
