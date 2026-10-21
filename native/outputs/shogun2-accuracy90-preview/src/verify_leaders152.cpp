#define main retained_leader_commands152
#include "verify_native_commands100.cpp"
#undef main
static uintptr_t task_projectile152;static unsigned native_task152;
static void __fastcall leader_task152(void*,void*,const uintptr_t* context){task_projectile152=context[0];++native_task152;}
int main(){SetUnhandledExceptionFilter(fault100);try{
    NativeFixture100 f;using P=NativeFixture100;state->heartbeat=GetTickCount64();auto a=possessed,v=P::ptr(f.view);
    std::vector<unsigned char> alternate(0x100),actions(0x20),root(0x40),definitions(0x1100),personal(0x100),normal(0x100);
    field<uintptr_t>(P::ptr(actions),0)=P::ptr(root);field<uintptr_t>(P::ptr(root),0x28)=P::ptr(definitions);field<unsigned>(P::ptr(definitions),0xc44)=1;
    field<uintptr_t>(a,0x23c)=P::ptr(actions);field<uintptr_t>(a,0x6cc)=P::ptr(personal);field<uintptr_t>(a,0x20c)=P::ptr(alternate);
    field<unsigned>(P::ptr(personal),0)=3;field<unsigned>(P::ptr(personal),0x60)=150;field<float>(P::ptr(personal),0x6c)=50;
    field<unsigned>(unit,0xf7c)=1;field<uintptr_t>(unit,0xf1c)=P::ptr(normal);field<unsigned>(P::ptr(normal),0)=60;field<unsigned>(P::ptr(normal),0x60)=100;field<float>(P::ptr(normal),0x6c)=200;
    DWORD old{};VirtualProtect(f.image,0x1580000,PAGE_EXECUTE_READWRITE,&old);f.jump(0x3d95a0,reinterpret_cast<void*>(leader_task152));
    for(unsigned role:{0x15dbd68u,0x15dbfb4u}){
        field<uintptr_t>(a,0)=base+role;possession_lease.release();require(followed(a,true),"Exact leader cannot acquire native lease");state->lease_generation=possession_lease.generation;
        NativeSubject subject;require(native_subject(a,unit,v,false,subject)&&!subject.actor&&followed(a,false),"Leader actual foot config rejected");
        Choice97 choice{environment,P::ptr(f.phase),P::ptr(f.manager),v,unit,P::ptr(f.holder),a,field<unsigned>(a,0x1c8),field<unsigned>(a,0x218)};
        field<uintptr_t>(P::ptr(f.manager),0x8f8)=v;require(choice_valid97(v,choice),"Exact cursor leader rejected by production camera choice");
        require(bow_record(unit)==P::ptr(personal),"Leader inherited rank-and-file firearm rather than own bow");
        uintptr_t context[17]{};context[0]=P::ptr(normal);reinterpret_cast<void(__thiscall*)(void*,const void*)>(base+0x391b50)(reinterpret_cast<void*>(a),context);
        require(task_projectile152==P::ptr(personal),"Native leader task did not select same personal record");
        field<unsigned>(P::ptr(definitions),0xc44)=0;require(!bow_record(unit),"Sword-only leader inherited unit projectile");field<unsigned>(P::ptr(definitions),0xc44)=1;
        field<uintptr_t>(a,0x6cc)=0;require(bow_record(unit)==unit_projectile152(unit),"Native leader no-override fallback changed");field<uintptr_t>(a,0x6cc)=P::ptr(personal);
        field<unsigned char>(a,0xf0)=0;require(!native_subject(a,unit,v,false,subject),"Dead leader accepted");field<unsigned char>(a,0xf0)=1;
        field<uintptr_t>(a,0x214)=0;require(!native_subject(a,unit,v,false,subject),"Foreign leader accepted");field<uintptr_t>(a,0x214)=unit;
    }
    field<uintptr_t>(a,0)=base+0x15dfec8;field<uintptr_t>(a,0x20c)=P::ptr(f.config);possession_lease.release();require(followed(a,true),"Morale retention baseline");state->lease_generation=possession_lease.generation;
    const unsigned char routed_native152[]={0xb0,1,0xc3};memcpy(reinterpret_cast<void*>(base+0x1821e0),routed_native152,sizeof routed_native152);require(native_routing(unit),"Routing negative control provider");
    for(unsigned route_state152:{2u,3u}){field<unsigned>(unit,0xcec)=route_state152;require(followed(a,false),"Broken/shattered ejected living player");}
    field<uintptr_t>(a,0)=base+0x15e65f4;NativeSubject subject;require(!native_subject(a,unit,v,false,subject),"Vehicle admitted as soldier");
    require(native_task152==2,"Original leader task boundary not executed");std::cout<<"PASS: exact native officer/general acquisition, current camera choice and actual individual config; original391b50 projectile override, no-ranged action rejection/fallback and dead/foreign/vehicle guards. No native game process opened.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}

