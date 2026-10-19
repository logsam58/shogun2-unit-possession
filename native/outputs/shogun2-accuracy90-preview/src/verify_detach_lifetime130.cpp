#define main retained_commands_lifetime130
#include "verify_native_commands100.cpp"
#undef main
static bool __fastcall unordered_lifetime130(void*,void*){return false;}
static unsigned cancellations130,unit_updates130;
static void __fastcall cancelled_move130(void*,void*,unsigned){++cancellations130;}
static void __fastcall unit_body130(void*,void*){++unit_updates130;require(!field<uintptr_t>(possessed,0x3b4),"Native unit update received possessed AI member");}
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;auto actor=P::ptr(f.actor),peer=P::ptr(f.peer);
 std::vector<unsigned char> squad(0x2800),other(0x2800),policy(8);auto sq=P::ptr(squad),sq2=P::ptr(other),pol=P::ptr(policy);
 uintptr_t phys[]={actor,peer},members[8]{},others[8]{},groups[]={sq,sq2},policy_vt[16]{};
 policy_vt[0x30/4]=reinterpret_cast<uintptr_t>(&unordered_lifetime130);field<uintptr_t>(pol,0)=reinterpret_cast<uintptr_t>(policy_vt);
 field<uintptr_t>(unit,0x14)=environment;field<unsigned>(unit,0x384)=2;field<uintptr_t>(unit,0x388)=reinterpret_cast<uintptr_t>(phys);
 field<unsigned>(unit,0xb70)=2;field<uintptr_t>(unit,0xb74)=reinterpret_cast<uintptr_t>(groups);field<uintptr_t>(unit,0xb9c)=sq;
 for(auto group:{sq,sq2}){field<uintptr_t>(group,0)=base+0x15bb21c;field<uintptr_t>(group,4)=environment;field<uintptr_t>(group,0x510)=unit;field<uintptr_t>(group,0x624)=pol;field<unsigned>(group,0x14)=8;field<unsigned char>(group,0x470)=1;}
 field<uintptr_t>(sq,0x1c)=reinterpret_cast<uintptr_t>(members);field<uintptr_t>(sq2,0x1c)=reinterpret_cast<uintptr_t>(others);
 auto add=reinterpret_cast<MemberInsert130>(base+0x14bc50);add(reinterpret_cast<void*>(sq),actor,0);add(reinterpret_cast<void*>(sq),peer,0);
 require(MH_Initialize()==MH_OK,"MinHook init");create_unit_detach130();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable lifetime hooks");
 auto actual_tick=original_unit_tick130;original_unit_tick130=reinterpret_cast<UnitTick130>(&unit_body130);
 state->heartbeat=GetTickCount64();unit_tick130(reinterpret_cast<void*>(unit),nullptr);
 require(unit_updates130==1&&detached_count130==1&&!field<uintptr_t>(actor,0x3b4)&&followed(actor,false,false),"Unit callback did not bracket original body with native detachment");
 state->mode=IDLE;sync_members130(unit);state->mode=POSSESSED;
 unsigned cases=0;
 for(unsigned failure=0;failure<13;++failure){
  state->heartbeat=GetTickCount64();sync_members130(unit);require(!field<uintptr_t>(actor,0x3b4)&&detached_count130==1,"Prepare detached actor");
  auto serial=field<unsigned>(actor,0x1c8),uid=field<unsigned>(unit,4),config=field<uintptr_t>(actor,0x20c),holder=field<uintptr_t>(P::ptr(f.holder),8),cam=field<uintptr_t>(P::ptr(f.camera),0x188);
  bool identity_reused=failure==0||failure==1||failure==8||failure==10;
  switch(failure){case 0:++field<unsigned>(actor,0x1c8);break;case 1:++field<unsigned>(unit,4);break;case 2:field<uintptr_t>(P::ptr(f.holder),8)=peer;break;case 3:field<uintptr_t>(P::ptr(f.camera),0x188)=peer;break;case 4:state->stop=1;break;case 5:state->release=1;break;case 6:state->mode=IDLE;break;case 7:state->heartbeat-=501;break;case 8:field<uintptr_t>(actor,0x20c)=0;break;case 9:enabled=0;break;case 10:field<unsigned char>(actor,0xf0)=0;break;case 11:possession_lease.active=false;break;case 12:field<unsigned>(fixture_group100,0x14)=2;break;}
  sync_members130(unit);require(!detached_count130,"Invalid control left a stale pending record");
  require(identity_reused?!field<uintptr_t>(actor,0x3b4):field<uintptr_t>(actor,0x3b4)==sq,"Invalid identity was written or surviving actor was not rejoined");
  field<unsigned>(actor,0x1c8)=serial;field<unsigned>(unit,4)=uid;field<uintptr_t>(actor,0x20c)=config;field<uintptr_t>(P::ptr(f.holder),8)=holder;field<uintptr_t>(P::ptr(f.camera),0x188)=cam;
  enabled=1;field<unsigned char>(actor,0xf0)=1;possession_lease.active=true;state->stop=state->release=0;state->mode=IDLE;state->heartbeat=GetTickCount64();field<unsigned>(fixture_group100,0x14)=1;
  if(identity_reused)original_member_insert130(reinterpret_cast<void*>(sq),actor,0);
  state->mode=POSSESSED;++cases;
 }
 // Prefer a current live squad; a former controller may have been destroyed.
 sync_members130(unit);groups[0]=sq2;field<unsigned>(unit,0xb70)=1;field<uintptr_t>(unit,0xb9c)=sq2;state->mode=IDLE;sync_members130(unit);
 require(field<uintptr_t>(actor,0x3b4)==sq2&&!detached_count130,"Stale former squad was dereferenced or live replacement not used");
 groups[0]=sq;groups[1]=sq2;field<unsigned>(unit,0xb70)=2;field<uintptr_t>(unit,0xb9c)=sq;original_member_transfer130(reinterpret_cast<void*>(sq),actor,0);state->mode=POSSESSED;
 // Real native cancellation routine, exact known formation task only. Its
 // resource destructor is an isolated provider; flags/dispatch remain native.
 DWORD old{};require(VirtualProtect(reinterpret_cast<void*>(base+0x3627b0),8,PAGE_EXECUTE_READWRITE,&old)!=0,"Move destructor fixture protection");unsigned char dtor[8];memcpy(dtor,reinterpret_cast<void*>(base+0x3627b0),8);f.jump(0x3627b0,reinterpret_cast<void*>(&cancelled_move130));VirtualProtect(reinterpret_cast<void*>(base+0x3627b0),8,old,&old);
 field<uintptr_t>(actor,0x2f8)=base+0x15dd7ac;field<unsigned char>(actor,0x384)=1;field<unsigned char>(actor,0x385)=1;field<uintptr_t>(actor,0x3b8)=0x130130;
 state->heartbeat=GetTickCount64();sync_members130(unit);
 require(cancellations130==1&&!field<unsigned char>(actor,0x384)&&!field<unsigned char>(actor,0x385)&&field<uintptr_t>(actor,0x3b8)==0x130130,"Native formation-move cancellation or secondary group isolation failed");
 state->mode=IDLE;sync_members130(unit);state->mode=POSSESSED;
 // A committed native combat witness prevents formation-task cancellation.
 // Avoid placing invalid combat pointers into the real proof; use a real
 // wall-pipe preservation case in existing siege fixtures instead. Task type
 // exclusion itself is exercised below with a different valid destructor.
 field<uintptr_t>(actor,0x2f8)=base+0x15dd800;field<unsigned char>(actor,0x384)=1;state->heartbeat=GetTickCount64();sync_members130(unit);
 require(cancellations130==1&&field<unsigned char>(actor,0x384)==1,"Unrelated personal task cancelled on detachment");
 field<unsigned char>(actor,0x384)=0;state->mode=IDLE;sync_members130(unit);state->mode=POSSESSED;
 // Battle generation changed: discard only numeric records, no old-world write.
 sync_members130(unit);auto phase=field<uintptr_t>(P::ptr(f.common),0xb4);field<uintptr_t>(P::ptr(f.common),0xb4)=0;sync_members130(unit);require(!detached_count130&&!field<uintptr_t>(actor,0x3b4),"Old battle memory written on phase invalidation");field<uintptr_t>(P::ptr(f.common),0xb4)=phase;
 state->mode=IDLE;original_member_insert130(reinterpret_cast<void*>(sq),actor,0);state->mode=POSSESSED;
 auto root=field<uintptr_t>(base,0x1bc8180);field<uintptr_t>(base,0x1bc8180)=0;follow_failure93=0x130;
 for(unsigned i=0;i<100000;++i)require(!member_owned130(peer)&&follow_failure93==0x130,"Peer entered full ownership proof");field<uintptr_t>(base,0x1bc8180)=root;
 // Repeated possession/exit leaves no records, duplicate members or allocations.
 for(unsigned i=0;i<1000;++i){state->heartbeat=GetTickCount64();sync_members130(unit);state->mode=IDLE;sync_members130(unit);require(!detached_count130&&squad_matches130(sq,actor)==1,"Possession lifecycle buildup");state->mode=POSSESSED;}
 // Unit-owned speed modifiers remain the original native getters while
 // squad control is detached; no copied/ad-hoc stat values replace them.
 field<float>(P::ptr(f.config),0x1c)=4;field<float>(P::ptr(f.config),0x20)=8;field<float>(P::ptr(f.config),0x6c)=2;field<float>(unit,0x22c)=.75f;
 auto speed=reinterpret_cast<float(__thiscall*)(void*,unsigned)>(base+0x18b470);
 float walk=speed(reinterpret_cast<void*>(unit),0),run=speed(reinterpret_cast<void*>(unit),1);require(walk==3&&run==6,"Original unit live speed scaling baseline");
 state->heartbeat=GetTickCount64();sync_members130(unit);float observed=0,turn=0;require(read_native_movement(RUN,observed,turn)&&observed==run&&turn==2&&field<uintptr_t>(actor,0x214)==unit,"Detachment severed inherited native unit movement stats");
 // Switch to another proved soldier in this same unit: restore the former
 // player and detach the new player; no cached authority for the old actor.
 field<uintptr_t>(P::ptr(f.camera),0x188)=peer;field<uintptr_t>(P::ptr(f.manager),0x8fc)=peer;possessed=peer;require(followed(peer,true),"Acquire switched soldier");state->heartbeat=GetTickCount64();sync_members130(unit);
 require(field<uintptr_t>(actor,0x3b4)==sq&&!field<uintptr_t>(peer,0x3b4)&&detached_count130==1,"Soldier switch left old actor detached or new actor controlled by unit");
 state->mode=IDLE;sync_members130(unit);field<uintptr_t>(P::ptr(f.camera),0x188)=actor;field<uintptr_t>(P::ptr(f.manager),0x8fc)=actor;possessed=actor;require(followed(actor,true),"Restore original soldier lease");state->mode=POSSESSED;
 // Mounted primary squad participation detaches; reciprocal passenger and
 // camera-holder ownership remain exact. Horse is sourced through its rider.
 std::vector<unsigned char> horse(0x700),horse_holder(0x210),mount_config(0x100),record(0x100);auto h=P::ptr(horse);uintptr_t passenger=actor;
 uintptr_t mounted_holders[]={P::ptr(f.holder),P::ptr(f.peer_holder),P::ptr(horse_holder)};
 field<uintptr_t>(h,0)=base+0x15e8524;field<uintptr_t>(h,0x214)=unit;field<uintptr_t>(h,0x20c)=P::ptr(mount_config);field<uintptr_t>(h,0x28)=environment;
 field<unsigned char>(h,0xf0)=field<unsigned char>(h,0xa1)=1;field<unsigned>(h,0x1c8)=44;field<unsigned>(h,0x6cc)=1;field<uintptr_t>(h,0x6d0)=reinterpret_cast<uintptr_t>(&passenger);field<uintptr_t>(h,0x3b8)=0x130beef;
 field<uintptr_t>(actor,0x210)=h;field<uintptr_t>(unit,0xf9c)=P::ptr(mount_config);field<uintptr_t>(unit,0x2c)=P::ptr(record);
 field<uintptr_t>(P::ptr(horse_holder),8)=h;field<unsigned>(P::ptr(f.view),0x18)=3;field<uintptr_t>(P::ptr(f.view),0x1c)=reinterpret_cast<uintptr_t>(mounted_holders);
 original_member_insert130(reinterpret_cast<void*>(sq),h,0);require(followed(actor,true),"Acquire native reciprocal mounted lease");state->heartbeat=GetTickCount64();sync_members130(unit);
 require(detached_count130==2&&!field<uintptr_t>(actor,0x3b4)&&!field<uintptr_t>(h,0x3b4)&&followed(actor,false,false),"Rider/horse primary squad detachment invalid");
 require(field<uintptr_t>(actor,0x210)==h&&passenger==actor&&field<uintptr_t>(h,0x3b8)==0x130beef&&field<uintptr_t>(actor,0x3b8)==0x130130,"Physical mount/passenger links altered");
 state->mode=IDLE;sync_members130(unit);require(!detached_count130&&field<uintptr_t>(actor,0x3b4)==sq&&field<uintptr_t>(h,0x3b4)==sq,"Mounted pair did not rejoin primary control");
 reinterpret_cast<MemberRemove130>(base+0x1a80a0)(reinterpret_cast<void*>(sq),h);
 field<uintptr_t>(actor,0x210)=0;field<uintptr_t>(unit,0xf9c)=P::ptr(f.config);field<unsigned>(P::ptr(f.view),0x18)=2;field<uintptr_t>(P::ptr(f.view),0x1c)=reinterpret_cast<uintptr_t>(f.holders.data());require(followed(actor,true),"Return to infantry lease");
 original_unit_tick130=actual_tick;require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Disable lifecycle hooks");MH_Uninitialize();
 std::cout<<"PASS: original native removal/rejoin lifecycle, "<<cases<<" full identity/exit failures, live squad replacement, exact formation-task cancellation, unrelated task/secondary group preservation, battle invalidation, 100000 cheap peers and 1000 detach/rejoin cycles without record accumulation, native inherited speed stats, soldier switching and reciprocal mounted detach/rejoin. Unit update body is a fixture provider.\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
