// Exact original arrival -> deployment work -> native child stack retirement
// -> formation task -> unit work -> queued-order retirement -> fire gate.
// Synthetic native records start after path planning; only destructors are
// substituted. No arrival/completion/fire decision is replaced by a stub.
#define main retained_commands129
#include "verify_native_commands100.cpp"
#undef main
static unsigned destroyed129;
static unsigned __fastcall fire_advance129(void*,void*){return 0x129;}
static void __fastcall destroy129(void*,void*,unsigned){++destroyed129;}
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;auto actor=P::ptr(f.actor),peer=P::ptr(f.peer);
 using Tick=void(__thiscall*)(void*);using Pop=void(__thiscall*)(void*,unsigned);
 auto arrived=reinterpret_cast<FormationArrival129>(base+0x14ef80),direct=reinterpret_cast<FormationArrival129>(base+0x3cbe40),fire=reinterpret_cast<FormationArrival129>(base+0x15c300);
 auto child_tick=reinterpret_cast<Tick>(base+0x1b7e20),view_tick=reinterpret_cast<Tick>(base+0x3efdd0),unit_tick=reinterpret_cast<Tick>(base+0x1b9490),queue_tick=reinterpret_cast<Tick>(base+0x1b8c80);
 auto unit_pop=reinterpret_cast<Pop>(base+0x19e400);
 std::vector<unsigned char> view(0x700),child(0x600),outer(0x200),job(0x60),selector(0x20);
 auto v=P::ptr(view),c=P::ptr(child),o=P::ptr(outer),j=P::ptr(job),q=unit+0x444;
 uintptr_t actors[1000]{},views[]={v},child_vt[]={reinterpret_cast<uintptr_t>(&destroy129),base+0x1b6670,base+0x8c580};
 uintptr_t outer_vt[]={reinterpret_cast<uintptr_t>(&destroy129),base+0x1b9490,base+0x8c580},order_vt[]={reinterpret_cast<uintptr_t>(&destroy129),base+0x172480,base+0x1b8b40};
 field<uintptr_t>(v,0x1c)=reinterpret_cast<uintptr_t>(actors);field<uintptr_t>(P::ptr(selector),0x10)=unit;
 field<unsigned>(unit,0xb70)=1;field<uintptr_t>(unit,0xb74)=reinterpret_cast<uintptr_t>(views);
 field<unsigned>(unit,0xb8c)=0;field<uintptr_t>(unit,0xb90)=reinterpret_cast<uintptr_t>(views);
 field<unsigned char>(actor,0x384)=1;field<unsigned char>(actor,0x308)=0;field<uintptr_t>(actor,0x248)=0;
 field<unsigned char>(peer,0x384)=1;field<unsigned char>(peer,0x308)=1;
 auto reset=[&](){
   std::fill(child.begin(),child.end(),0);std::fill(outer.begin(),outer.end(),0);std::fill(job.begin(),job.end(),0);
   field<uintptr_t>(j,0)=reinterpret_cast<uintptr_t>(child_vt);field<uintptr_t>(j,4)=v;
   field<uintptr_t>(c,0x400)=j;field<unsigned>(c,0x404)=0x18;field<unsigned>(c,0x440)=1;field<unsigned>(c,0x444)=0x500;
   field<unsigned char>(v,0x508)=1;field<unsigned char>(v,0x498)=0;field<uintptr_t>(v+0x488,0xc)=c;field<unsigned char>(v+0x488,0x44)=1;
   field<uintptr_t>(o,0)=reinterpret_cast<uintptr_t>(outer_vt);field<uintptr_t>(o,4)=unit;field<unsigned char>(o,0x54)=1;
   field<unsigned>(q,0x5a8)=1;field<unsigned>(q,0x5ac)=0;field<uintptr_t>(q+0x10,0)=reinterpret_cast<uintptr_t>(order_vt);field<uintptr_t>(q+0x10,0xc)=o;
   field<unsigned char>(q,0x90)=field<unsigned char>(q,0x91)=1;field<unsigned char>(q+0x10,0x10)=0;field<unsigned char>(q,0x93)=0;
   field<uintptr_t>(o,0x100)=o;field<unsigned>(o,0x104)=0x5c;field<unsigned>(o,0x120)=1;field<unsigned char>(o,0x130)=1;
   field<unsigned char>(unit,0xeb0)=1;field<unsigned char>(unit,0x198)=0;state->heartbeat=GetTickCount64();
 };
 auto tick=[&](){child_tick(reinterpret_cast<void*>(c));view_tick(reinterpret_cast<void*>(v+0x488));unit_tick(reinterpret_cast<void*>(o));
   // Same completion-byte condition used by the original outer stack driver.
   if(field<unsigned char>(o,0xc)==1)unit_pop(reinterpret_cast<void*>(o),0);
   queue_tick(reinterpret_cast<void*>(q));};
 field<unsigned>(v,0x18)=2;actors[0]=actor;actors[1]=peer;reset();tick();
 require(!arrived(view.data())&&!direct(reinterpret_cast<void*>(actor))&&field<unsigned>(c,0x440)==1&&field<unsigned>(q,0x5a8)==1&&!fire(selector.data()),"Original displaced-player deployment/fire stall not reproduced");
 unsigned char old[16];memcpy(old,reinterpret_cast<void*>(base+0x3cbe40),16);
 require(MH_Initialize()==MH_OK,"MinHook init");create_ranged_position128();create_formation_arrival129();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable arrival hook");
 unsigned cases=0;
 for(unsigned count:{1u,2u,90u,240u,1000u})for(unsigned index:{0u,count-1})for(unsigned pose:{0u,3u,17u,18u,29u,35u,99u}){
   field<unsigned>(v,0x18)=count;for(unsigned i=0;i<count;++i)actors[i]=peer;actors[index]=actor;field<unsigned>(actor,0x1f0)=pose;
   auto before=f.actor;auto peer_before=f.peer;reset();tick();
   require(field<unsigned>(c,0x440)==0&&field<unsigned char>(v,0x498)==1&&field<unsigned char>(o,0xc)==1&&field<unsigned>(o,0x120)==0&&field<unsigned>(q,0x5a8)==0&&fire(selector.data()),"Native deployment/order failed to retire after AI arrived");
   require(!direct(reinterpret_cast<void*>(actor))&&!memcmp(f.actor.data(),before.data(),before.size())&&!memcmp(f.peer.data(),peer_before.data(),peer_before.size()),"Caller isolation or actor-byte preservation failed");
   field<unsigned char>(unit,0xeb0)=0;require(!fire(selector.data()),"Hold Fire bypassed");field<unsigned char>(unit,0xeb0)=1;
   if(count>1){field<unsigned char>(peer,0x308)=0;reset();tick();require(field<unsigned>(c,0x440)==1&&field<unsigned>(q,0x5a8)==1&&!fire(selector.data()),"AI soldier arrival requirement bypassed");field<unsigned char>(peer,0x308)=1;}
   ++cases;
 }
 // Both deployment arrival AND the later firing waits must succeed. The
 // two Test128 exceptions remain complementary; they were insufficient alone.
 unsigned char projectile[0x90]{},fire_state[0x170]{},fire_task[0x90]{};
 auto pr=reinterpret_cast<uintptr_t>(projectile),fs=reinterpret_cast<uintptr_t>(fire_state),ft=reinterpret_cast<uintptr_t>(fire_task);
 uintptr_t fire_vt[32]{};fire_vt[0x40/4]=reinterpret_cast<uintptr_t>(&fire_advance129);
 field<unsigned>(pr,0)=3;field<unsigned>(pr,0x60)=150;field<float>(pr,0x6c)=50;
 field<unsigned>(unit,0xf7c)=1;field<uintptr_t>(unit,0xf1c)=pr;field<uintptr_t>(fs,4)=v;
 field<uintptr_t>(ft,0)=reinterpret_cast<uintptr_t>(fire_vt);field<uintptr_t>(ft,4)=actor;
 field<unsigned>(actor,0x1f0)=0;field<unsigned char>(actor,0x11f)=0;field<unsigned char>(peer,0x11f)=1;
 field<unsigned>(v,0x18)=2;actors[0]=actor;actors[1]=peer;
 auto moving=reinterpret_cast<FormationArrival129>(base+0x1aef50);auto personal=reinterpret_cast<unsigned(__stdcall*)(void*)>(base+0x3b2d90);
 reset();tick();require(field<unsigned>(q,0x5a8)==0&&fire(selector.data())&&!moving(fire_state)&&personal(fire_task+0x1c)==0x129,"Joint arrival/unit-fire/personal-fire readiness failed");
 field<unsigned char>(peer,0x11f)=0;require(moving(fire_state),"AI peer firing wait ignored");field<unsigned char>(peer,0x11f)=1;
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&!memcmp(old,reinterpret_cast<void*>(base+0x3cbe40),16),"Exact predicate restoration");
 field<unsigned>(v,0x18)=2;actors[0]=actor;actors[1]=peer;reset();tick();require(field<unsigned>(q,0x5a8)==1&&!fire(selector.data()),"Original deployment wait not restored");MH_Uninitialize();
 std::cout<<"PASS: original native formation arrival, child work/scheduler retirement, formation/unit completion, queued move retirement, Fire at Will gate and complementary firing waits; "<<cases<<" roster/pose cases, AI waits, Hold Fire, unchanged actors, caller isolation/restoration. Planning and projectile dispatch excluded.\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
