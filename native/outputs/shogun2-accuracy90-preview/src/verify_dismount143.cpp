// Production native subject + camera lease transition with explicit original
// unit-task layouts. No borrowed actor, timer grace, or manual dismount motion.
#define main retained_dismount143_main
#include "verify_native_commands100.cpp"
#undef main
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;
 std::vector<unsigned char> horse(0x710),horse_config(0x100),horse_holder(0x210),record(0x100),task(0x100);
 auto mount=P::ptr(horse),hc=P::ptr(horse_config),hh=P::ptr(horse_holder),r=P::ptr(record),t=P::ptr(task);
 std::array<uintptr_t,3> holders{P::ptr(f.holder),P::ptr(f.peer_holder),hh};std::array<uintptr_t,1> passengers{possessed};
 field<uintptr_t>(P::ptr(f.view),0x1c)=reinterpret_cast<uintptr_t>(holders.data());field<unsigned>(P::ptr(f.view),0x18)=3;
 field<uintptr_t>(hh,8)=mount;field<uintptr_t>(mount,0)=base+0x15e8524;field<uintptr_t>(mount,0x28)=environment;
 field<uintptr_t>(mount,0x214)=unit;field<uintptr_t>(mount,0x20c)=hc;field<unsigned char>(mount,0xf0)=field<unsigned char>(mount,0xa1)=1;
 field<unsigned>(mount,0x1c8)=77;field<unsigned>(mount,0x6cc)=1;field<uintptr_t>(mount,0x6d0)=reinterpret_cast<uintptr_t>(passengers.data());
 field<uintptr_t>(unit,0x2c)=r;field<uintptr_t>(possessed,0x210)=mount;field<uintptr_t>(unit,0xf9c)=hc;
 field<uintptr_t>(unit,0xba8)=t;field<uintptr_t>(t,4)=environment;field<uintptr_t>(t,8)=unit;
 for(unsigned category:{0u,1u,3u})for(unsigned task_vt:{0x15c00a4u,0x15c8324u}){
  field<unsigned>(r,0x1c)=category;field<uintptr_t>(t,0)=base+task_vt;
  field<uintptr_t>(unit,0xba8)=t;
  field<uintptr_t>(possessed,0x210)=mount;field<uintptr_t>(unit,0xf9c)=hc;
  require(followed(possessed,true)&&possession_lease.identity.mount==mount,"Mounted baseline lease");auto generation=possession_lease.generation;
  require(dismount_native_motion143(),"Native dismount did not retain motion authority");
  require(!formation_owned129(mount)&&!member_owned130(mount)&&!member_owned130(possessed),"Camera continuation granted formation authority during dismount");
  // Engine changes formation movement definition before severing rider link.
  field<uintptr_t>(unit,0xf9c)=P::ptr(f.config);
  require(followed(possessed,false,false)&&possession_lease.generation==generation,"Native intermediate configuration ejected rider");
  if(category==3){camera_callback_depth=1;require(followed(possessed,false),"Camera did not witness original dismount task");camera_callback_depth=0;field<uintptr_t>(unit,0xba8)=0;}
  field<uintptr_t>(possessed,0x210)=0;
  require(followed(possessed,false,false)&&possession_lease.generation==generation&&dismount_native_motion143(),"Guard-only read mutated foot handoff");
  camera_callback_depth=1;require(followed(possessed,false),"Fully proved foot handoff rejected");camera_callback_depth=0;
  require(possession_lease.generation==generation+1&&!possession_lease.identity.mount&&state->lease_generation==possession_lease.generation&&state->mode==POSSESSED&&state->actor_id==field<unsigned>(possessed,0x1c8),"Dismount changed soldier or returned to RTS");
  require(!dismount_native_motion143()&&dismount_cleanup143,"Completed dismount retained horse control");dismount_cleanup143=0;
 }
 field<uintptr_t>(possessed,0x210)=mount;field<uintptr_t>(unit,0xf9c)=hc;require(followed(possessed,true),"Recreate mount lease");
 field<uintptr_t>(unit,0xba8)=t;
 field<uintptr_t>(t,4)=environment+4;field<uintptr_t>(unit,0xf9c)=P::ptr(f.config);require(!followed(possessed,false,false),"Foreign transition environment retained lease");field<uintptr_t>(t,4)=environment;
 ++field<unsigned>(possessed,0x1c8);require(!followed(possessed,false,false),"Reused human serial gained transition authority");--field<unsigned>(possessed,0x1c8);
 field<uintptr_t>(t,0)=base+0x15bb344;require(!followed(possessed,false,false),"Unrelated native task bypassed mount ownership");
 std::cout<<"PASS: same-rider normal/Stand and Fight dismount across general/cavalry categories, native transition motion, camera-only generation renewal, no RTS switch and foreign task/environment/serial rejection.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
