// Real native squad removal/insertion, both formation compaction policies,
// native arrival/fire aggregates, native bulk transfer and the original native
// regroup candidate collector. Unit update body/pathfinding/target selection
// are outside this bounded fixture, not replaced into a live-success claim.
#define main retained_commands130
#include "verify_native_commands100.cpp"
#undef main
static bool ordered130;
static void __fastcall task_destructor130(void*,void*,unsigned){}
static bool __fastcall order_policy130(void*,void*){return ordered130;}
static int __fastcall next_slot130(void*,void*,unsigned,float,unsigned){return -1;}
static uintptr_t collector130;
static void __declspec(naked) __stdcall collect130(uintptr_t){__asm{
 push ebx
 push esi
 push edi
 sub esp,0x80
 mov edi,[esp+0x90]
 mov esi,[edi+0x388]
 cmp dword ptr [edi+0x384],0
 je done_collect
 call dword ptr [collector130]
 done_collect:
 add esp,0x80
 pop edi
 pop esi
 pop ebx
 ret 4
}}
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;auto actor=P::ptr(f.actor),peer=P::ptr(f.peer);
 std::vector<unsigned char> squad(0x2800),other(0x2800),policy(8),width_context(0x40),fire_state(0x170);
 auto sq=P::ptr(squad),sq2=P::ptr(other),pol=P::ptr(policy),fs=P::ptr(fire_state);
 std::vector<std::array<unsigned char,0x700>> peers(1000);for(unsigned i=0;i<peers.size();++i){memcpy(peers[i].data(),f.peer.data(),f.peer.size());field<unsigned>(reinterpret_cast<uintptr_t>(peers[i].data()),0x1c8)=100+i;}
 uintptr_t task_vt[]={reinterpret_cast<uintptr_t>(&task_destructor130)};field<uintptr_t>(actor,0x2f8)=reinterpret_cast<uintptr_t>(task_vt);
 uintptr_t physical[1000]{},members[1000]{},other_members[1000]{},squads[]={sq,sq2};uintptr_t policy_vt[16]{};
 policy_vt[0x30/4]=reinterpret_cast<uintptr_t>(&order_policy130);policy_vt[0x34/4]=reinterpret_cast<uintptr_t>(&next_slot130);field<uintptr_t>(pol,0)=reinterpret_cast<uintptr_t>(policy_vt);
 field<uintptr_t>(unit,0x14)=environment;field<unsigned>(unit,0xb70)=2;field<uintptr_t>(unit,0xb74)=reinterpret_cast<uintptr_t>(squads);field<uintptr_t>(unit,0xb9c)=sq;
 field<uintptr_t>(unit,0x440)=P::ptr(width_context);field<uintptr_t>(unit,0x388)=reinterpret_cast<uintptr_t>(physical);
 for(auto group:{sq,sq2}){field<uintptr_t>(group,0)=base+0x15bb21c;field<uintptr_t>(group,4)=environment;field<uintptr_t>(group,0x510)=unit;field<uintptr_t>(group,0x624)=pol;field<unsigned>(group,0x14)=1000;field<unsigned char>(group,0x470)=1;}
 field<uintptr_t>(sq,0x1c)=reinterpret_cast<uintptr_t>(members);field<uintptr_t>(sq2,0x1c)=reinterpret_cast<uintptr_t>(other_members);field<uintptr_t>(fs,4)=sq;
 auto add=reinterpret_cast<MemberInsert130>(base+0x14bc50),transfer=reinterpret_cast<MemberInsert130>(base+0x1b33f0);
 auto arrival=reinterpret_cast<FormationArrival129>(base+0x14ef80),moving=reinterpret_cast<FormationArrival129>(base+0x1aef50);
 field<unsigned char>(actor,0x384)=1;field<unsigned char>(actor,0x308)=0;field<unsigned char>(actor,0x11f)=0;field<unsigned>(actor,0x1f0)=0;
 field<uintptr_t>(sq,0x18)=0;add(reinterpret_cast<void*>(sq),actor,0);add(reinterpret_cast<void*>(sq),peer,0);
 require(field<uintptr_t>(actor,0x3b4)==sq&&squad_matches130(sq,actor)==1,"Original native membership insertion");
 require(MH_Initialize()==MH_OK,"MinHook init");create_unit_detach130();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable native detachment hooks");
 unsigned cases=0;
 for(bool ordered:{false,true})for(unsigned count:{1u,2u,90u,240u,1000u})for(unsigned index:{0u,count-1}){
  ordered130=ordered;field<unsigned char>(actor,0x384)=1;field<unsigned char>(actor,0x308)=0;field<unsigned>(sq,0x18)=count;field<unsigned>(sq2,0x18)=0;field<unsigned>(unit,0x384)=count;
  for(unsigned i=0;i<count;++i){auto p=reinterpret_cast<uintptr_t>(peers[i].data());physical[i]=members[i]=p;field<uintptr_t>(p,0x3b4)=sq;field<unsigned char>(p,0x384)=0;field<unsigned char>(p,0x11f)=1;}
  physical[index]=members[index]=actor;field<uintptr_t>(actor,0x3b4)=sq;state->heartbeat=GetTickCount64();state->mode=POSSESSED;state->release=state->stop=0;enabled=1;
  require(!arrival(squad.data())&&moving(fire_state.data()),"Original native membership blocks arrival/fire");
  auto original_physical=std::vector<uintptr_t>(physical,physical+count);auto original_unit=f.actor_unit;auto original_actor=f.actor;
  sync_members130(unit);
  require(!field<uintptr_t>(actor,0x3b4)&&squad_matches130(sq,actor)==0&&field<unsigned>(sq,0x18)==count-1,"Player not physically removed from native AI squad");
  require(arrival(squad.data())&&!moving(fire_state.data()),"Unmodified native arrival/fire still includes detached player");
  require(!memcmp(physical,original_physical.data(),count*4)&&!memcmp(f.actor_unit.data(),original_unit.data(),f.actor_unit.size()),"Master unit ownership/roster/stat bytes changed");
  original_actor[0x3b4]=original_actor[0x3b5]=original_actor[0x3b6]=original_actor[0x3b7]=0;
  require(!memcmp(f.actor.data(),original_actor.data(),f.actor.size()),"Unrelated actor, mount or animation bytes changed");
  require(followed(actor,false,false),"Detachment invalidated actual possession identity");
  transfer(reinterpret_cast<void*>(sq2),actor,1);add(reinterpret_cast<void*>(sq),actor,1);
  require(!field<uintptr_t>(actor,0x3b4)&&field<unsigned>(sq2,0x18)==0&&squad_matches130(sq,actor)==0,"Unit order reattached the player");
  if(count>1){auto p=members[0];field<unsigned char>(p,0x384)=1;field<unsigned char>(p,0x308)=0;field<unsigned char>(p,0x11f)=0;
   require(!arrival(squad.data())&&moving(fire_state.data()),"AI peer readiness was waived");field<unsigned char>(p,0x384)=0;field<unsigned char>(p,0x11f)=1;
   transfer(reinterpret_cast<void*>(sq2),p,0);require(field<uintptr_t>(p,0x3b4)==sq2&&squad_matches130(sq2,p)==1,"Native peer transfer stopped working");transfer(reinterpret_cast<void*>(sq),p,0);}
  state->mode=IDLE;sync_members130(unit);
  require(field<uintptr_t>(actor,0x3b4)==sq&&squad_matches130(sq,actor)==1&&field<unsigned>(sq,0x18)==count&&!detached_count130,"Exit failed to rejoin native squad exactly once");
  state->mode=POSSESSED;++cases;
 }
 // Execute the exact original native collection loop with only its terminal
 // continuation replaced by RET. Its real candidate/vector code is unchanged.
 DWORD old{};require(VirtualProtect(reinterpret_cast<void*>(base+0x1affa8),1,PAGE_EXECUTE_READWRITE,&old)!=0,"Collector fixture protection");auto endbyte=field<unsigned char>(base,0x1affa8);field<unsigned char>(base,0x1affa8)=0xc3;VirtualProtect(reinterpret_cast<void*>(base+0x1affa8),1,old,&old);FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(base+0x1affa8),1);
 uintptr_t candidates[2000]{};field<unsigned>(base,0x18d33f0)=1000;field<uintptr_t>(base,0x18d33f8)=reinterpret_cast<uintptr_t>(candidates);collector130=base+0x1afed0;
 for(unsigned count:{1u,2u,90u,240u,1000u})for(unsigned index:{0u,count-1}){
  field<unsigned>(unit,0x384)=count;for(unsigned i=0;i<count;++i){physical[i]=reinterpret_cast<uintptr_t>(peers[i].data());field<uintptr_t>(physical[i],0x3b4)=0;}physical[index]=actor;field<uintptr_t>(actor,0x3b4)=0;
  field<unsigned>(base,0x18d33f4)=0;state->heartbeat=GetTickCount64();collect130(unit);
  require(field<unsigned>(base,0x18d33f4)==count-1,"Automatic regroup collector still counts owned soldier");for(unsigned i=0;i<count-1;++i)require(candidates[i*2]!=actor,"Player leaked into native regroup candidates");
  state->mode=IDLE;field<unsigned>(base,0x18d33f4)=0;collect130(unit);require(field<unsigned>(base,0x18d33f4)==count,"Released actor not eligible for ordinary native regroup");state->mode=POSSESSED;
 }
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Disable detachment hooks");MH_Uninitialize();
 require(VirtualProtect(reinterpret_cast<void*>(base+0x1affa8),1,PAGE_EXECUTE_READWRITE,&old)!=0,"Collector restore protection");field<unsigned char>(base,0x1affa8)=endbyte;VirtualProtect(reinterpret_cast<void*>(base+0x1affa8),1,old,&old);
 std::cout<<"PASS: actual native squad detach/rejoin, "<<cases<<" ordered/unordered roster cases up to1000, unchanged master unit/stat/actor bytes, original arrival/fire predicates without Test128/129 hooks, native peer transfer and original regroup candidate loop. No live target selection/pathfinding claim.\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
