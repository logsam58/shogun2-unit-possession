// Full adapter ownership proof, original native record/aggregate/predicate.
// Existing native fixture substitutes environment providers, not identity rules.
#define main retained_commands128
#include "verify_native_commands100.cpp"
#undef main
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;auto actor=P::ptr(f.actor),peer=P::ptr(f.peer);unsigned char projectile[0x90]{};
 field<unsigned>(reinterpret_cast<uintptr_t>(projectile),0)=3;field<unsigned>(reinterpret_cast<uintptr_t>(projectile),0x60)=150;field<float>(reinterpret_cast<uintptr_t>(projectile),0x6c)=50;
 field<unsigned>(unit,0xf7c)=1;field<uintptr_t>(unit,0xf1c)=reinterpret_cast<uintptr_t>(projectile);state->heartbeat=GetTickCount64();
 require(weapon_owned(actor)&&ranged_position_owned128(actor),"Full production owned ranged proof");
 unsigned char roster[0x40]{},aggregate[0x170]{};uintptr_t actors[]={actor,peer};auto rp=reinterpret_cast<uintptr_t>(roster),ap=reinterpret_cast<uintptr_t>(aggregate);
 field<uintptr_t>(ap,4)=rp;field<unsigned>(rp,0x18)=2;field<uintptr_t>(rp,0x1c)=reinterpret_cast<uintptr_t>(actors);field<unsigned char>(actor,0x11f)=0;field<unsigned char>(peer,0x11f)=1;
 auto blocked=reinterpret_cast<bool(__thiscall*)(void*)>(base+0x1aef50);auto direct=reinterpret_cast<RangedSettled128>(base+0x3cfa60);
 require(blocked(aggregate)&&!direct(actor),"Native pre-correction wait");require(MH_Initialize()==MH_OK,"MinHook init");create_ranged_position128();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable position hooks");
 require(!blocked(aggregate)&&!direct(actor),"Full lease exception missing or leaked into other callers");
 unsigned char task[0x90]{};field<uintptr_t>(reinterpret_cast<uintptr_t>(task),4)=actor;
 require(ranged_position128(task,nullptr),"Proved personal task wait still blocks");
 // Individual guard failures must return to exact native behavior immediately.
 unsigned cases=0;for(unsigned failure=0;failure<10;++failure){
  auto serial=field<unsigned>(actor,0x1c8),unit_id=field<unsigned>(unit,4);auto holder_actor=field<uintptr_t>(P::ptr(f.holder),8),cam_actor=field<uintptr_t>(P::ptr(f.camera),0x188),config=field<uintptr_t>(actor,0x20c);
  state->heartbeat=GetTickCount64();state->stop=state->release=0;state->mode=POSSESSED;
  switch(failure){case 0:++field<unsigned>(actor,0x1c8);break;case 1:++field<unsigned>(unit,4);break;case 2:field<uintptr_t>(P::ptr(f.holder),8)=peer;break;case 3:field<uintptr_t>(P::ptr(f.camera),0x188)=peer;break;case 4:state->stop=1;break;case 5:state->release=1;break;case 6:state->mode=IDLE;break;case 7:state->heartbeat-=501;break;case 8:field<uintptr_t>(actor,0x20c)=0;break;case 9:enabled=0;break;}
  require(blocked(aggregate)&&!ranged_position128(task,nullptr),"Invalid full production ownership admitted readiness exception");
  field<unsigned>(actor,0x1c8)=serial;field<unsigned>(unit,4)=unit_id;field<uintptr_t>(P::ptr(f.holder),8)=holder_actor;field<uintptr_t>(P::ptr(f.camera),0x188)=cam_actor;field<uintptr_t>(actor,0x20c)=config;
  enabled=1;state->stop=state->release=0;state->mode=POSSESSED;state->heartbeat=GetTickCount64();++cases;
  require(!blocked(aggregate)&&ranged_position128(task,nullptr),"Restored proved lease did not recover");
 }
 // In mounted mode, peers must not enter the full rider proof at all. Make
 // that proof invalid and verify its failure observer is untouched by peers.
 auto saved_root=field<uintptr_t>(base,0x1bc8180);auto saved_mount=possession_lease.identity.mount;
 possession_lease.identity.mount=peer;field<uintptr_t>(base,0x1bc8180)=0;follow_failure93=0x128;
 for(unsigned i=0;i<100000;++i)require(!ranged_position_owned128(peer)&&follow_failure93==0x128,"Unrelated mounted subject entered full possession proof");
 possession_lease.identity.mount=saved_mount;field<uintptr_t>(base,0x1bc8180)=saved_root;
 field<unsigned char>(peer,0x11f)=0;require(blocked(aggregate),"Unsettled AI peer ignored with full ownership");
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&blocked(aggregate),"Native aggregate restoration");MH_Uninitialize();
 std::cout<<"PASS: full adapter possession proof and native record/aggregate; "<<cases<<" serial, unit, holder, camera, release, heartbeat, config and enable failures; peer preserved.\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
