#define main retained_commands_ownership129
#include "verify_native_commands100.cpp"
#undef main
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;auto actor=P::ptr(f.actor),peer=P::ptr(f.peer);
 unsigned char view[0x40]{};auto v=reinterpret_cast<uintptr_t>(view);uintptr_t actors[]={actor,peer};
 field<unsigned>(v,0x18)=2;field<uintptr_t>(v,0x1c)=reinterpret_cast<uintptr_t>(actors);
 field<unsigned char>(actor,0x384)=1;field<unsigned char>(actor,0x308)=0;field<uintptr_t>(actor,0x248)=0;
 auto arrived=reinterpret_cast<FormationArrival129>(base+0x14ef80);state->heartbeat=GetTickCount64();
 require(formation_owned129(actor)&&!arrived(view),"Production proof or original arrival baseline");
 require(MH_Initialize()==MH_OK,"MinHook init");create_formation_arrival129();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK&&arrived(view),"Enable fully proved exception");
 unsigned cases=0;for(unsigned failure=0;failure<13;++failure){
  auto serial=field<unsigned>(actor,0x1c8),uid=field<unsigned>(unit,4),config=field<uintptr_t>(actor,0x20c);
  auto holder=field<uintptr_t>(P::ptr(f.holder),8),cam=field<uintptr_t>(P::ptr(f.camera),0x188);
  state->heartbeat=GetTickCount64();
  switch(failure){case 0:++field<unsigned>(actor,0x1c8);break;case 1:++field<unsigned>(unit,4);break;case 2:field<uintptr_t>(P::ptr(f.holder),8)=peer;break;case 3:field<uintptr_t>(P::ptr(f.camera),0x188)=peer;break;case 4:state->stop=1;break;case 5:state->release=1;break;case 6:state->mode=IDLE;break;case 7:state->heartbeat-=501;break;case 8:field<uintptr_t>(actor,0x20c)=0;break;case 9:enabled=0;break;case 10:field<unsigned char>(actor,0xf0)=0;break;case 11:possession_lease.active=false;break;case 12:field<unsigned>(fixture_group100,0x14)=2;break;}
  if(arrived(view)){std::cerr<<"ownership failure case="<<failure<<"\n";require(false,"Invalid ownership admitted formation exception");}
  field<unsigned>(actor,0x1c8)=serial;field<unsigned>(unit,4)=uid;field<uintptr_t>(actor,0x20c)=config;field<uintptr_t>(P::ptr(f.holder),8)=holder;field<uintptr_t>(P::ptr(f.camera),0x188)=cam;
  enabled=1;field<unsigned char>(actor,0xf0)=1;possession_lease.active=true;state->stop=state->release=0;state->mode=POSSESSED;state->heartbeat=GetTickCount64();field<unsigned>(fixture_group100,0x14)=1;
  require(arrived(view),"Restored full proof did not recover");++cases;
 }
 auto root=field<uintptr_t>(base,0x1bc8180);field<uintptr_t>(base,0x1bc8180)=0;follow_failure93=0x129;
 for(unsigned i=0;i<100000;++i)require(!formation_owned129(peer)&&follow_failure93==0x129,"Peer entered full proof");
 field<uintptr_t>(base,0x1bc8180)=root;
 // A stale/forged mount candidate still requires full reciprocal lease proof.
 auto mount=possession_lease.identity.mount;possession_lease.identity.mount=peer;require(!formation_owned129(peer),"Unproved principal accepted");possession_lease.identity.mount=mount;
 // Prove an ordinary reciprocal mounted principal using the full production
 // camera roster and lease. Rider and horse may both occur in native rosters.
 std::vector<unsigned char> horse(0x700),horse_holder(0x210),mount_config(0x100),record(0x100);
 auto h=P::ptr(horse);uintptr_t passenger=actor;uintptr_t mounted_holders[]={P::ptr(f.holder),P::ptr(f.peer_holder),P::ptr(horse_holder)};
 field<uintptr_t>(h,0)=base+0x15e8524;field<uintptr_t>(h,0x214)=unit;field<uintptr_t>(h,0x20c)=P::ptr(mount_config);field<uintptr_t>(h,0x28)=environment;
 field<unsigned char>(h,0xf0)=field<unsigned char>(h,0xa1)=1;field<unsigned>(h,0x1c8)=44;field<unsigned>(h,0x6cc)=1;field<uintptr_t>(h,0x6d0)=reinterpret_cast<uintptr_t>(&passenger);
 field<uintptr_t>(actor,0x210)=h;field<uintptr_t>(unit,0xf9c)=P::ptr(mount_config);field<uintptr_t>(unit,0x2c)=P::ptr(record);
 field<uintptr_t>(P::ptr(horse_holder),8)=h;field<unsigned>(P::ptr(f.view),0x18)=3;field<uintptr_t>(P::ptr(f.view),0x1c)=reinterpret_cast<uintptr_t>(mounted_holders);
 require(followed(actor,true),"Mounted production lease");state->heartbeat=GetTickCount64();
 field<unsigned char>(h,0x384)=1;field<unsigned char>(h,0x308)=0;actors[0]=h;
 require(formation_owned129(h)&&arrived(view),"Proved reciprocal horse still blocks formation");
 passenger=peer;require(!arrived(view),"Broken horse passenger proof bypassed arrival");passenger=actor;
 ++field<unsigned>(h,0x1c8);require(!arrived(view),"Reused horse serial admitted");--field<unsigned>(h,0x1c8);
 require(arrived(view),"Restored mounted proof failed");
 field<uintptr_t>(actor,0x210)=0;field<uintptr_t>(unit,0xf9c)=P::ptr(f.config);field<unsigned>(P::ptr(f.view),0x18)=2;field<uintptr_t>(P::ptr(f.view),0x1c)=reinterpret_cast<uintptr_t>(f.holders.data());
 require(followed(actor,true),"Restore infantry lease");state->heartbeat=GetTickCount64();actors[0]=actor;
 field<unsigned char>(peer,0x384)=1;field<unsigned char>(peer,0x308)=0;require(!arrived(view),"Unarrived AI peer ignored");
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Disable hook");MH_Uninitialize();
 std::cout<<"PASS: production full possession identity, "<<cases<<" release/heartbeat/serial/selection/camera failures, reciprocal mounted principal, forged mount rejection, AI waits and 100000 cheap peer rejections.\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
