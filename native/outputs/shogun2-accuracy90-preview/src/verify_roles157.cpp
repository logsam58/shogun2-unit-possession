#define main retained_role_commands157
#include "verify_native_commands100.cpp"
#undef main
static uintptr_t projectile157;static unsigned tasks157;
static void __fastcall task157(void*,void*,const uintptr_t* context){projectile157=context[0];++tasks157;}
int main(){SetUnhandledExceptionFilter(fault100);try{
    NativeFixture100 f;using P=NativeFixture100;auto a=possessed,v=P::ptr(f.view);
    std::vector<unsigned char> config(0x100),actions(0x20),root(0x40),defs(0x1100),personal(0x100),normal(0x100);
    field<uintptr_t>(P::ptr(actions),0)=P::ptr(root);field<uintptr_t>(P::ptr(root),0x28)=P::ptr(defs);
    field<uintptr_t>(a,0x23c)=P::ptr(actions);field<uintptr_t>(a,0x6cc)=P::ptr(personal);
    field<unsigned>(P::ptr(personal),0)=3;field<unsigned>(P::ptr(personal),0x60)=150;field<float>(P::ptr(personal),0x6c)=50;
    field<unsigned>(unit,0xf7c)=1;field<uintptr_t>(unit,0xf1c)=P::ptr(normal);field<unsigned>(P::ptr(normal),0)=60;
    field<unsigned>(P::ptr(normal),0x60)=100;field<float>(P::ptr(normal),0x6c)=200;
    DWORD old{};VirtualProtect(f.image,0x1580000,PAGE_EXECUTE_READWRITE,&old);f.jump(0x3d95a0,reinterpret_cast<void*>(task157));
    unsigned poses=0;
    for(unsigned role:{0x15dbd68u,0x15dbfb4u,0x15dc590u,0x15dc1a8u,0x15dc39cu,0x15dcf54u,0x15dc978u,0x15dcb6cu,0x15dcd60u}){
        field<uintptr_t>(a,0)=base+role;field<uintptr_t>(a,0x20c)=P::ptr(config);state->heartbeat=GetTickCount64();
        possession_lease.release();require(followed(a,true),"Native special soldier failed acquisition");state->lease_generation=possession_lease.generation;
        NativeSubject subject;Choice97 choice{environment,P::ptr(f.phase),P::ptr(f.manager),v,unit,P::ptr(f.holder),a,field<unsigned>(a,0x1c8),field<unsigned>(a,0x218)};
        field<uintptr_t>(P::ptr(f.manager),0x8f8)=v;
        // Exercise production acquisition/camera guards across the native posture
        // domain, including the old special-config >=18 rejection. Root flags
        // remain authoritative; this does not make each posture player-movable.
        for(unsigned posture=0;posture<=0x5c;++posture){
            field<unsigned>(a,0x1f0)=posture;
            require(native_subject(a,unit,v,false,subject)&&followed(a,false)&&choice_valid97(v,choice),"Special config posture incorrectly rejects camera/lease");++poses;
        }
        field<unsigned>(P::ptr(defs),0xc44)=1;require(bow_record(unit)==P::ptr(personal),"Special soldier inherited formation projectile");
        uintptr_t context[17]{};context[0]=P::ptr(normal);
        reinterpret_cast<void(__thiscall*)(void*,const void*)>(base+0x391b50)(reinterpret_cast<void*>(a),context);
        require(projectile157==P::ptr(personal),"Original391b50 disagrees with actual actor projectile");
        field<unsigned>(P::ptr(defs),0xc44)=0;require(!bow_record(unit),"Non-shooting special role inherited peer weapon");
        field<unsigned>(P::ptr(defs),0xc44)=1;field<uintptr_t>(a,0x6cc)=0;
        require(bow_record(unit)==unit_projectile152(unit),"Native no-override fallback changed");field<uintptr_t>(a,0x6cc)=P::ptr(personal);
        for(unsigned offset:{0xa3u,0xa4u}){field<unsigned char>(a,offset)=1;require(!native_subject(a,unit,v,false,subject),"Root ownership flag bypassed");field<unsigned char>(a,offset)=0;}
        field<unsigned char>(a,0xa2)=1;require(native_subject(a,unit,v,false,subject),"Retained native airborne role rejected");field<unsigned char>(a,0xa2)=0;
        field<uintptr_t>(a,0x18)=unit;require(!native_subject(a,unit,v,false,subject),"Attached actor admitted as free locomotion");field<uintptr_t>(a,0x18)=0;
        field<unsigned char>(a,0xf0)=0;require(!native_subject(a,unit,v,false,subject),"Dead role acquired");field<unsigned char>(a,0xf0)=1;
        field<uintptr_t>(a,0x214)=0;require(!native_subject(a,unit,v,false,subject),"Foreign role acquired");field<uintptr_t>(a,0x214)=unit;
        field<unsigned>(a,0x1c8)=1;require(!followed(a,false),"Role serial reuse accepted");field<unsigned>(a,0x1c8)=0;
    }
    NativeSubject subject;
    for(unsigned role:{0x15e65f4u,0x15e8524u,0x15d1124u,0x15d0ed8u,0x15d1370u,0x15d04c8u,0x15dc784u,0x15dbb1cu,0x15dcf58u}){
        field<uintptr_t>(a,0)=base+role;require(!native_subject(a,unit,v,false,subject),"Non-soldier/intermediate/unknown role acquired");
    }
    field<uintptr_t>(a,0)=base+0x15dfec8;require(!native_subject(a,unit,v,false,subject),"Regular soldier accepted foreign config");
    require(tasks157==9&&poses==837,"Native special-role coverage incomplete");
    std::cout<<"PASS: nine native special soldier roles through acquisition/current camera choice, 837 posture cases, original391b50 actual projectile/no-shoot/fallback; strict root/life/unit/serial/config/non-soldier guards. Native scene and animation providers remain offline boundaries.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
