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
 field<unsigned>(unit,0xba8)=0;field<uintptr_t>(unit,0x14)=environment;
 reinterpret_cast<void(__thiscall*)(void*,uintptr_t,unsigned)>(base+0x12c480)(reinterpret_cast<void*>(t),unit,1);
 require(field<unsigned>(unit,0xba8)==1&&field<uintptr_t>(unit,0xba0)==t&&field<uintptr_t>(t,4)==environment&&field<uintptr_t>(t,8)==unit,"Original12c480/14aca0 task array registration");
 field<uintptr_t>(t,0)=base+0x15c00a4;require(native_mount_task155(unit,environment)==t,"Native count1 task slot rejected");
 for(unsigned invalid155:{0u,3u,0x10000u}){field<unsigned>(unit,0xba8)=invalid155;require(!native_mount_task155(unit,environment),"Invalid native task count acquired authority");}
 field<unsigned>(unit,0xba8)=1;

 for(unsigned category:{0u,1u,3u})for(unsigned task_vt:{0x15c00a4u,0x15c8324u}){
  field<unsigned>(r,0x1c)=category;field<uintptr_t>(t,0)=base+task_vt;
  field<uintptr_t>(unit,0xba0)=t;field<unsigned>(unit,0xba8)=1;
  field<uintptr_t>(possessed,0x210)=mount;field<uintptr_t>(unit,0xf9c)=hc;
  require(followed(possessed,true)&&possession_lease.identity.mount==mount,"Mounted baseline lease");auto generation=possession_lease.generation;
  require(dismount_native_motion143(),"Native dismount did not retain motion authority");
  require(!formation_owned129(mount)&&!member_owned130(mount)&&!member_owned130(possessed),"Camera continuation granted formation authority during dismount");
  // Engine changes formation movement definition before severing rider link.
  field<uintptr_t>(unit,0xf9c)=P::ptr(f.config);
  require(followed(possessed,false,false)&&possession_lease.generation==generation,"Native intermediate configuration ejected rider");
  if(category==3){camera_callback_depth=1;require(followed(possessed,false),"Camera did not witness original dismount task");camera_callback_depth=0;field<unsigned>(unit,0xba8)=0;}
  field<uintptr_t>(possessed,0x210)=0;
  require(followed(possessed,false,false)&&possession_lease.generation==generation&&dismount_native_motion143(),"Guard-only read mutated foot handoff");
  camera_callback_depth=1;require(followed(possessed,false),"Fully proved foot handoff rejected");camera_callback_depth=0;
  require(possession_lease.generation==generation+1&&!possession_lease.identity.mount&&state->lease_generation==possession_lease.generation&&state->mode==POSSESSED&&state->actor_id==field<unsigned>(possessed,0x1c8),"Dismount changed soldier or returned to RTS");
  require(!dismount_native_motion143()&&dismount_cleanup143,"Completed dismount retained horse control");dismount_cleanup143=0;
 }
 // Re-mount through the same native task13. No nearest-horse guess:
 // the actual live reciprocal passenger relationship is required.
 for(unsigned role154:{0x15dfec8u,0x15dbd68u,0x15dbfb4u}){
  field<uintptr_t>(possessed,0)=base+role154;field<uintptr_t>(possessed,0x210)=0;field<uintptr_t>(unit,0xf9c)=P::ptr(f.config);field<unsigned>(unit,0xba8)=0;
  require(followed(possessed,true)&&!possession_lease.identity.mount,"Foot remount baseline");auto gen154=possession_lease.generation;
  third_person152=1;view_lease152=gen154;
  field<uintptr_t>(unit,0xba0)=t;field<unsigned>(unit,0xba8)=1;field<uintptr_t>(t,0)=base+0x15c00a4;
  require(dismount_native_motion143(),"Foot-to-horse native task did not retain motion");
  camera_callback_depth=1;require(followed(possessed,false),"Remount witness rejected same live human");camera_callback_depth=0;
  field<uintptr_t>(unit,0xf9c)=hc;field<uintptr_t>(possessed,0x210)=mount;
  field<unsigned char>(mount,0xa1)=0;
  require(followed(possessed,false,false)&&dismount_native_motion143()&&!member_owned130(possessed),"Transient native remount granted manual authority or lost camera");
  field<unsigned char>(mount,0xa1)=1;
  std::array<uintptr_t,2> animation154{813,P::ptr(f.config)};field<uintptr_t>(possessed,0x474)=uintptr_t(animation154.data());field<unsigned>(possessed,0x1d4)=813;
  camera_callback_depth=1;require(followed(possessed,false)&&possession_lease.generation==gen154&&dismount_native_motion143(),"Native links renewed camera control before rider mount animation completed");camera_callback_depth=0;
  float eye154[6]={3,4,5,3,4,6},up154[3]={.6f,.8f,0};auto cam154=P::ptr(f.camera);memcpy(reinterpret_cast<void*>(cam154+8),eye154,24);memcpy(reinterpret_cast<void*>(cam154+0x58),up154,12);
  state->heartbeat=GetTickCount64();adjust_eye(cam154);if(!animation_view_active78||!mount_camera_locked154)std::cerr<<"mountcam debug owned="<<camera_owned(cam154)<<" stage="<<follow_failure93<<" motion="<<dismount_native_motion143()<<" active="<<animation_view_active78<<" locked="<<mount_camera_locked154<<" error="<<state->error<<" mode="<<state->mode<<" age="<<(GetTickCount64()-state->heartbeat)<<"\n";require(animation_view_active78&&mount_camera_locked154&&!memcmp(reinterpret_cast<void*>(cam154+8),eye154,24)&&!memcmp(animation_view_up78,up154,12),"Native mount eye/roll was replaced before interface refresh");
  auto yaw154=state->yaw;mouse_look101(40,40,0,.01f);require(state->yaw==yaw154,"Ordinary mouse displaced native mounting animation");
  for(unsigned slot154:{813u,814u,815u,816u}){animation154[0]=slot154;field<unsigned>(possessed,0x1d4)=slot154;require(native_mount_clip154(possessed),"Authored native rider transition slot omitted");}
  animation154[0]=817;field<unsigned>(possessed,0x1d4)=817;require(!native_mount_clip154(possessed),"Adjacent unrelated rider slot gained mount authority");animation154[0]=813;field<unsigned>(possessed,0x1d4)=813;
  field<unsigned>(unit,0xba8)=0;
  camera_callback_depth=1;require(followed(possessed,false)&&possession_lease.generation==gen154&&dismount_native_motion143(),"Retired unit task lost witnessed current rider animation");camera_callback_depth=0;
  field<uintptr_t>(possessed,0x474)=0;field<unsigned>(possessed,0x1d4)=0;
  camera_callback_depth=1;require(followed(possessed,false),"Reciprocal native remount failed to renew same soldier lease");camera_callback_depth=0;
  require(possession_lease.generation==gen154+1&&possession_lease.identity.mount==mount&&view_lease152==possession_lease.generation&&third_person152,"Remount changed soldier, lost third person or failed generation renewal");
  field<unsigned>(unit,0xba8)=0;require(!dismount_native_motion143(),"Finished remount retained native transition lock");dismount_cleanup143=0;
 }
 field<uintptr_t>(possessed,0)=base+0x15dfec8;
 std::array<unsigned,64> peer_task155{};auto peer155=uintptr_t(peer_task155.data());
 field<uintptr_t>(peer155,0)=base+0x15bb344;field<uintptr_t>(peer155,4)=environment;field<uintptr_t>(peer155,8)=unit;
 field<uintptr_t>(unit,0xba0)=peer155;field<uintptr_t>(unit,0xba4)=t;field<unsigned>(unit,0xba8)=2;field<uintptr_t>(t,0)=base+0x15c00a4;
 require(native_mount_task155(unit,environment)==t,"Second native task slot rejected");field<uintptr_t>(t,4)=environment+4;require(!native_mount_task155(unit,environment),"Foreign task in real array accepted");field<uintptr_t>(t,4)=environment;
 field<uintptr_t>(possessed,0x210)=mount;field<uintptr_t>(unit,0xf9c)=hc;require(followed(possessed,true),"Recreate mount lease");
 field<uintptr_t>(unit,0xba0)=t;field<unsigned>(unit,0xba8)=1;
 field<uintptr_t>(t,4)=environment+4;field<uintptr_t>(unit,0xf9c)=P::ptr(f.config);require(!followed(possessed,false,false),"Foreign transition environment retained lease");field<uintptr_t>(t,4)=environment;
 ++field<unsigned>(possessed,0x1c8);require(!followed(possessed,false,false),"Reused human serial gained transition authority");--field<unsigned>(possessed,0x1c8);
 field<uintptr_t>(t,0)=base+0x15bb344;require(!followed(possessed,false,false),"Unrelated native task bypassed mount ownership");
 std::cout<<"PASS: same-rider normal/Stand and Fight dismount across general/cavalry categories, native transition motion, camera-only generation renewal, no RTS switch and foreign task/environment/serial rejection.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
