#define main retained_commands_main
#include "verify_native_commands100.cpp"
#undef main
int main(){try{
 NativeFixture100 f;state->ready=1;state->heartbeat=GetTickCount64();overlay106::Frame frame;frame.active=1;frame.key={state->lease_generation,state->actor_id,state->unit_id};
 require(overlay_key106(frame,overlay106::HUD),"Current numeric UI lease refused");auto saved=*state;auto lease=possession_lease;
 for(unsigned bad=0;bad<12;++bad){auto copy=frame;
  if(bad==0)enabled=0;if(bad==1)state->ready=0;if(bad==2)state->stop=1;if(bad==3)state->release=1;if(bad==4)state->mode=DISABLED;if(bad==5)state->heartbeat=GetTickCount64()-501;if(bad==6)possession_lease.active=false;if(bad==7)++copy.key.lease;if(bad==8)++copy.key.actor;if(bad==9)++copy.key.unit;if(bad==10)++state->lease_generation;if(bad==11)++possession_lease.generation;
  require(!overlay_key106(copy,overlay106::HUD),"Released/stale/foreign UI lease admitted");*state=saved;possession_lease=lease;enabled=1;
 }
 state->mode=DISABLED;state->release=1;possession_lease.active=false;frame.active=2;
 require(overlay_key106(frame,overlay106::QTE)&&!overlay_key106(frame,overlay106::HUD)&&!overlay_key106(frame,overlay106::Death),"Immutable feedback granted gameplay/other-layer permission");
 ++frame.key.lease;require(!overlay_key106(frame,overlay106::QTE),"Old feedback crossed lease");--frame.key.lease;state->stop=1;require(!overlay_key106(frame,overlay106::QTE),"Shutdown retained feedback");
 std::cout<<"PASS: production overlay identity/current publication, 12 strict guards and released numeric-only QTE feedback; no native actor reads or writes by compositor.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
