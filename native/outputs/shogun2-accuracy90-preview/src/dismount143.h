#pragma once
// Original unit tasks1214f0 (dismount) /1cf5d0 (Stand and Fight) derive
// from12c480: +4 environment, +8 unit. No elapsed grace or nearest-actor swap.
static volatile LONG dismount_cleanup143,dismount_transitions143;
struct DismountWitness143 {unsigned generation{},actor_id{},unit_id{};uintptr_t actor{},unit{},environment{},mount{};};
static DismountWitness143 dismount_witness143;
// Original14aca0 appends task pointers to TWO inline slots ba0/ba4.
// ba8 is their COUNT, never a pointer (recorded first-chance read address1).
static uintptr_t native_mount_task155(uintptr_t u,uintptr_t env){
 if(!u||!env)return 0;
 unsigned count=field<unsigned>(u,0xba8);if(!count||count>2)return 0;
 for(unsigned i=count;i;--i){auto task=field<uintptr_t>(u,0xba0+(i-1)*4);if(task<0x10000)continue;
  auto vt=field<uintptr_t>(task,0);
  if((vt==base+0x15c00a4||vt==base+0x15c8324)&&field<uintptr_t>(task,4)==env&&field<uintptr_t>(task,8)==u&&
     field<unsigned>(u,0xba8)==count&&field<uintptr_t>(u,0xba0+(i-1)*4)==task)return task;
 }return 0;
}
static bool native_dismount_task143(uintptr_t u,uintptr_t env){return native_mount_task155(u,env)!=0;}
struct MountCompleted154 {unsigned generation{},actor_id{};uintptr_t actor{},unit{},task{};};
static MountCompleted154 mount_completed154;
static bool mount_transition_task154(uintptr_t u,uintptr_t env){
 if(!native_dismount_task143(u,env))return false;
 const auto& done=mount_completed154;const auto& live=possession_lease.identity;
 return !(possession_lease.active&&done.generation==possession_lease.generation&&done.actor==live.actor&&done.actor_id==live.actor_id&&done.unit==u&&done.task==native_mount_task155(u,env));
}
static void mount_completed154_now(){const auto& p=possession_lease.identity;mount_completed154={possession_lease.generation,p.actor_id,p.actor,p.unit,native_mount_task155(p.unit,p.environment)};}
// Native slot registry169727c..c4 and decoded shogun_horse_rider_fragment:
// RIDER_MOUNT_LEFT/RIGHT=813/814, RIDER_DISMOUNT_LEFT/RIGHT=815/816.
// Original3e8b50 publishes this CURRENT animation record and slot at474/1d4.
static bool native_mount_clip154(uintptr_t actor){
 auto animation=field<uintptr_t>(actor,0x474);if(!animation)return false;
 unsigned slot=field<unsigned>(animation,0);
 return slot>=813&&slot<=816&&field<uintptr_t>(animation,4)&&field<unsigned>(actor,0x1d4)==slot;
}
static bool dismount_witnessed143(const PossessionIdentity& c);
static bool dismount_body143(const PossessionIdentity& c){
 const auto& p=possession_lease.identity;
 return possession_lease.active&&c.actor==p.actor&&c.actor_id==p.actor_id&&
     c.unit==p.unit&&c.unit_id==p.unit_id&&c.environment==p.environment&&
     c.common==p.common&&c.phase==p.phase&&c.manager==p.manager&&c.camera==p.camera&&
     field<uintptr_t>(c.actor,0x28)==c.environment&&field<unsigned char>(c.actor,0xf0)==1;
}
static bool dismount_pending143(const PossessionIdentity& c){
 if(!dismount_body143(c)||!(mount_transition_task154(c.unit,c.environment)||(dismount_witnessed143(c)&&native_mount_clip154(c.actor))))return false;
 auto mount=field<uintptr_t>(c.actor,0x210);
 bool linked=!mount||mount==possession_lease.identity.mount;
 if(mount&&!possession_lease.identity.mount){
  // Remount only the same human's actual reciprocal live native horse.
  auto n=field<unsigned>(mount,0x6cc),links=field<uintptr_t>(mount,0x6d0);unsigned matches=0;
  if(n&&n<=8&&links)for(unsigned i=0;i<n;++i)matches+=field<uintptr_t>(links,i*4)==c.actor;
  linked=field<uintptr_t>(mount,0)==base+0x15e8524&&field<uintptr_t>(mount,0x214)==c.unit&&field<uintptr_t>(mount,0x28)==c.environment&&field<unsigned char>(mount,0xf0)==1&&matches==1;
 }
 return linked&&(c.config==possession_lease.identity.config||c.config==field<uintptr_t>(c.unit,0xf9c));
}
static void dismount_observe143(const PossessionIdentity& c){
 if(!dismount_pending143(c))return;
 dismount_witness143={possession_lease.generation,c.actor_id,c.unit_id,c.actor,c.unit,c.environment,possession_lease.identity.mount};
}
static bool dismount_witnessed143(const PossessionIdentity& c){
 const auto& w=dismount_witness143;
 return w.generation==possession_lease.generation&&w.actor==c.actor&&w.actor_id==c.actor_id&&w.unit==c.unit&&w.unit_id==c.unit_id&&w.environment==c.environment&&w.mount==possession_lease.identity.mount;
}
static bool dismount_finished143(const PossessionIdentity& c){
 if(!dismount_body143(c)||!c.config||native_mount_clip154(c.actor)||!(mount_transition_task154(c.unit,c.environment)||dismount_witnessed143(c)))return false;
 const auto& old=possession_lease.identity;
 if(old.mount)return !c.mount&&!field<uintptr_t>(c.actor,0x210)&&(c.config==field<uintptr_t>(c.unit,0xf9c)||actor152::individual(base,field<uintptr_t>(c.actor,0)));
 return c.mount&&c.mount_holder&&c.mount_config&&field<uintptr_t>(c.actor,0x210)==c.mount;
}
static bool dismount_native_motion143(){
 if(!possession_lease.active||!possessed)return false;
 const auto& live=possession_lease.identity;
 bool own_clip=live.actor==possessed&&field<unsigned>(possessed,0x1c8)==live.actor_id&&field<uintptr_t>(possessed,0x214)==live.unit&&field<uintptr_t>(possessed,0x28)==live.environment&&field<unsigned char>(possessed,0xf0)==1&&native_mount_clip154(possessed)&&
     (native_dismount_task143(unit,environment)||(dismount_witness143.generation==possession_lease.generation&&dismount_witness143.actor==possessed&&dismount_witness143.actor_id==live.actor_id));
 return own_clip||mount_transition_task154(unit,environment)||(possession_lease.identity.mount&&!field<uintptr_t>(possessed,0x210));
}
