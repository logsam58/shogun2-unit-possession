// Production dispatch/target/selector boundaries. Native witness preparation
// and clip playback below are explicit providers; no live kill/renderer claim.
#define NOMINMAX
#include <windows.h>
// Fixture focus is an explicit provider; no real foreground window is changed.
static DWORD WINAPI focus_owner140(HWND,LPDWORD pid){if(pid)*pid=GetCurrentProcessId();return GetCurrentThreadId();}
#define GetWindowThreadProcessId focus_owner140
#include "playable_runtime.cpp"
#undef GetWindowThreadProcessId
static unsigned prepares140,updates140,selects140,targets140;static uint32_t p140,e140,w140;static bool fail140;
static NativeVerdict verdict140;
static NativeVerdict* __fastcall selector140(void*,void*,NativeVerdict* out,void*const*,void*const*,uint32_t,uint32_t){++selects140;*out=verdict140;return out;}
static uint32_t __fastcall target140(void*,void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t){++targets140;return 0x99;}
template<class T>static void put140(uint32_t p,T v){memcpy(reinterpret_cast<void*>(p),&v,sizeof v);}
static uint32_t __fastcall prepare140(void* self,void*){
 ++prepares140;void* actors[]={reinterpret_cast<void*>(p140),reinterpret_cast<void*>(e140)};NativeVerdict v;
 require(select_hook(nullptr,nullptr,&v,actors,actors+2,0,0)==&v&&!memcmp(&v,&verdict140,sizeof v),"Native charge verdict was modified or reoriented");
 require(target_hook(self,nullptr,p140,0,0,0,0,0,0,0,0,0,0,0,0)==0x99,"Stock charge target setter was blocked");
 put140(w140+0xc,uint8_t(1));put140(w140+0x38,2u);put140(w140+0x30,p140);put140(w140+0x34,e140);
 put140(p140+0x688,w140);put140(e140+0x688,w140);put140(p140+0x1d0,13u);put140(p140+0x119,uint8_t(0));
 return fail140?1u:0u;
}
static uint32_t __fastcall update140(void*,void*){++updates140;return fail140?1u:0u;}
int main(){void* memory=nullptr;try{
 memory=VirtualAlloc(nullptr,0x1bc9000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory,"Engine fixture allocation");engine=uint32_t(memory);
 std::vector<uint8_t> root(0x600),env(0x28300),common(0x300),context(0x100),manager(0x900),camera(0x200),human(0x700),horse(0x700),enemy(0x700),pu(0x1800),eu(0x1800),w(0x110),links(4);
 auto ptr=[](auto& x){return uint32_t(x.data());};
 put140(engine+0x1bc8180,ptr(root));put140(ptr(root)+0x110,ptr(env));put140(ptr(env)+0x281fc,ptr(manager));put140(ptr(manager)+0x8e0,ptr(camera));put140(ptr(camera)+0x188,ptr(human));
 put140(ptr(env)+8,ptr(common));put140(ptr(common)+0x9c,ptr(context));
 put140(ptr(pu)+4,7u);put140(ptr(eu)+4,8u);
 for(auto a:{ptr(human),ptr(horse),ptr(enemy)}){
  put140(a,engine+(a==ptr(horse)?0x15e8524:0x15dfec8));put140(a+0xf0,uint8_t(1));put140(a+0x1c8,a==ptr(human)?11u:a==ptr(horse)?12u:13u);
  put140(a+0x214,a==ptr(enemy)?ptr(eu):ptr(pu));put140(a+0x28,ptr(env));put140(a+0x44,a==ptr(enemy)?1u:0u);put140(a+0x4c,1.f);
 }
 put140(ptr(horse)+0x6cc,1u);put140(ptr(horse)+0x6d0,ptr(links));put140(ptr(links),ptr(human));
 observation::PossessionPrefix lease{};lease.magic=0x31504353;lease.version=46;lease.pid=GetCurrentProcessId();lease.ready=1;lease.mode=2;lease.actor_id=11;lease.unit_id=7;lease.lease_generation=1;lease.heartbeat=GetTickCount64();possession=&lease;
 Status status{};state=&status;enabled=1;LARGE_INTEGER freq;QueryPerformanceFrequency(&freq);frequency=double(freq.QuadPart);
 native_prepare=reinterpret_cast<NativeWitnessCall>(&prepare140);native_update=reinterpret_cast<NativeWitnessCall>(&update140);native_target=reinterpret_cast<NativeTarget>(&target140);selection_ports.select=reinterpret_cast<NativeSelector>(&selector140);
 session=new Session(reader(),selection_ports.calls(),{},engine,{0,5,true},true,true,true,true,true);
 wchar_t mapping_name146[96];charge146::name(mapping_name146,GetCurrentProcessId());
 HANDLE publisher146=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(charge146::Shared),mapping_name146);
 require(publisher146,"Charge observation fixture mapping");
 auto publication146=static_cast<charge146::Shared*>(MapViewOfFile(publisher146,FILE_MAP_ALL_ACCESS,0,0,sizeof(charge146::Shared)));
 require(publication146,"Charge observation fixture view");new(publication146)charge146::Shared;
 w140=ptr(w);e140=ptr(enemy);unsigned cases=0;
 for(bool mounted:{false,true})for(uint32_t result:{0u,5u,2u})for(bool counter:{false,true})for(unsigned evidence146:{0u,1u,2u}){
  stock_charge_clear140();std::fill(w.begin(),w.end(),uint8_t(0));lease.heartbeat=GetTickCount64();lease.lease_generation++;
  put140(ptr(human)+0x210,mounted?ptr(horse):0u);p140=mounted?ptr(horse):ptr(human);player_address=LONG(p140);
  put140(p140+0x688,0u);put140(e140+0x688,0u);put140(w140,engine+0x15e1000);put140(w140+4,ptr(env));put140(w140+0x24,2u);put140(w140+0x10,p140);put140(w140+0x14,e140);
  put140(p140+0x1d0,16u);put140(p140+0x119,uint8_t(2));put140(p140+0x2e8,uint8_t(1));put140(p140+0x250,engine+0x15e0a5c);put140(p140+0x254,p140);put140(p140+0x258,uint8_t(counter)); // queued native contact may already have completed the charge task
  charge146::publish(publication146,{});
  // Contact can precede the visual charge pose, or replace the charge task
  // after the adapter has yielded the soldier to the native squad planner.
  if(evidence146) {put140(p140+0x1d0,0u);put140(p140+0x119,uint8_t(0));}
  if(evidence146==2){put140(p140+0x250,engine+0x15e0a20);charge146::publish(publication146,{GetCurrentProcessId(),lease.lease_generation,lease.actor_id,lease.unit_id,mounted?12u:11u,unsigned(mounted),1,GetTickCount64()});}
  verdict140={result,counter?e140:p140,counter?p140:e140};auto generation=lifetimes.get(w140,true);auto before=prepares140;
  auto got140=dispatch(w140,generation,false);
  if(got140||prepares140!=before+1||selects140!=prepares140||targets140!=prepares140)std::cerr<<"debug mounted="<<mounted<<" result="<<result<<" got="<<got140<<" prepares="<<prepares140<<" selects="<<selects140<<" targets="<<targets140<<" owned="<<scoped(w140)<<" actor="<<fps_actor()<<" task="<<stock_charge_task140(reader(),p140)<<" record="<<stock_charge140.address<<" current="<<stock_charge_current140(w140,generation)<<" session="<<session->occupied()<<"\n";
  require(got140==0&&prepares140==before+1&&selects140==prepares140&&targets140==prepares140,"Charge was not forwarded exactly once");
  require(!session->occupied()&&!pending_address&&!delivery_address&&stock_charge140.address==w140,"Charge acquired QTE state or lost native lifetime");
  require(commitment.phase==melee60::Aligning&&commitment.player_kind==unsigned(mounted),"Stock native pair did not publish existing alignment handoff");
  const auto serial=commitment.serial;auto updates=updates140;
  for(unsigned i=0;i<1000;++i){lease.heartbeat=GetTickCount64();require(dispatch(w140,generation,true)==0,"Native charge update blocked after charge pose cleared");}
  require(updates140==updates+1000&&prepares140==before+1&&commitment.serial==serial,"Charge update rerolled/restarted native selection");
  put140(w140+0xd1,uint8_t(1));stock_charge_heartbeat140();require(commitment.phase==melee60::NativeCombat,"Stock playback failed to release native root motion");
  stock_charge_removed140(w140,generation+1);require(stock_charge140.address==w140,"Stale removal cleared current charge");
  stock_charge_removed140(w140,generation);require(!stock_charge140.address&&commitment.phase==melee60::Off,"Native removal did not clear charge handoff");
  charge146::publish(publication146,{});
  // Speed/pose flags without the actual live charge task cannot grant bypass.
  put140(p140+0x250,engine+0x15e0a20);put140(p140+0x1d0,16u);put140(p140+0x119,uint8_t(2));require(!stock_charge_task140(reader(),p140),"Pose alone bypassed QTE");
  NativeActor identity;require(combat_identity(reader(),p140,engine,identity),"Actor proof");
  if(mounted){put140(p140+0x248,16u);put140(p140+0x250,engine+0x15e0cec);put140(p140+0x258,uint8_t(0));require(stock_charge_jump140(reader(),identity),"Native horse jump not recognized");put140(p140+0x258,uint8_t(1));require(!stock_charge_jump140(reader(),identity),"Completed jump retained charge bypass");}
  ++cases;
 }
 // A numeric handoff is usable only beside separately proved current actors.
 NativeActor observed146;require(combat_identity(reader(),p140,engine,observed146),"Observed charge actor");
 charge146::Value valid146{GetCurrentProcessId(),lease.lease_generation,lease.actor_id,lease.unit_id,observed146.serial,unsigned(observed146.kind),1,GetTickCount64()};
 charge146::publish(publication146,valid146);require(stock_charge_owner146(observed146,lease.lease_generation),"Fresh native handoff rejected");
 for(unsigned field146=0;field146<10;++field146){auto bad=valid146;
  switch(field146){case 0:++bad.pid;break;case 1:++bad.lease;break;case 2:++bad.human;break;case 3:++bad.unit;break;case 4:++bad.physical;break;case 5:bad.kind^=1;break;case 6:bad.active=0;break;case 7:bad.tick-=501;break;case 8:bad.tick+=10000;break;case 9:bad.lease=0;break;}
  charge146::publish(publication146,bad);require(!stock_charge_owner146(observed146,lease.lease_generation),"Stale/foreign charge handoff bypassed QTE");
 }
 charge146::publish(publication146,valid146);InterlockedIncrement(&publication146->sequence);charge146::Value torn146;
 require(!charge146::read(publication146,torn146),"Torn charge handoff accepted");InterlockedIncrement(&publication146->sequence);
 charge146::publish(publication146,{});
 // Lifetime/lease/serial proof is repeated; stale pointers never grant replay.
 stock_charge140={w140,engine+0x15e1000,ptr(env),lifetimes.get(w140),lease.lease_generation,99,{p140,12,ptr(pu),7,0,1.f,Horse},false};
 lease.lease_generation++;require(!stock_charge_current140(w140,stock_charge140.generation),"Reused possession lease retained bypass");stock_charge_clear140();
 // A removal racing the coordinator must not recreate a missing registry
 // entry merely to discover that its generation no longer matches.
 auto removed_generation140=lifetimes.get(w140);
 stock_charge140={w140,engine+0x15e1000,ptr(env),removed_generation140,lease.lease_generation,99,{p140,12,ptr(pu),7,0,1.f,Horse},false};
 require(stock_charge_current140(w140,removed_generation140),"Current native lifetime fixture");
 lifetimes.removed(w140,removed_generation140);auto removed_live140=lifetimes.metrics94().live;
 for(unsigned i=0;i<1000;++i)require(!stock_charge_current140(w140,removed_generation140),"Removed lifetime retained charge bypass");
 require(lifetimes.metrics94().live==removed_live140,"Removed charge recreated a ghost lifetime record");stock_charge_clear140();
 candidate_witness=w140;candidate_generation=removed_generation140;
 for(unsigned n152=0;n152<1000;++n152)require(!routed_generation152(w140),"Removed encounter gained automatic routing authority");
 require(lifetimes.metrics94().live==removed_live140,"Routing lookup created a ghost native lifetime");candidate_witness=0;candidate_generation=0;

 require(!stock_charge_target_scope140(w140)&&!stock_charge_scope140,"Native target scope leaked");
 if(charge_read146)UnmapViewOfFile(charge_read146);if(charge_read_map146)CloseHandle(charge_read_map146);charge_read146=nullptr;charge_read_map146=nullptr;
 UnmapViewOfFile(publication146);CloseHandle(publisher146);
 delete session;session=nullptr;VirtualFree(memory,0,MEM_RELEASE);
 std::cout<<"PASS: "<<cases<<" production infantry/horse stock-charge dispatch variants, untouched success/failed/countercharge verdicts,36000 original-port updates after pose exit; early contact before pose and fresh native handoff after task replacement; stale/foreign/torn handoff rejection, native target forwarding, alignment/root-motion handoff and exact removal. Native selection/preparation/clip outputs are explicit providers; live animations remain unproved.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';if(memory)VirtualFree(memory,0,MEM_RELEASE);return 1;}}
