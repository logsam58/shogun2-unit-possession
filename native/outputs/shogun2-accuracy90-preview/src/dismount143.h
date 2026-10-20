#pragma once
// Original unit tasks1214f0 (dismount) /1cf5d0 (Stand and Fight) derive
// from12c480: +4 environment, +8 unit. No elapsed grace or nearest-actor swap.
static volatile LONG dismount_cleanup143,dismount_transitions143;
struct DismountWitness143 {unsigned generation{},actor_id{},unit_id{};uintptr_t actor{},unit{},environment{},mount{};};
static DismountWitness143 dismount_witness143;
static bool native_dismount_task143(uintptr_t u,uintptr_t env){
 auto task=field<uintptr_t>(u,0xba8);if(!task)return false;
 auto vt=field<uintptr_t>(task,0);
 return (vt==base+0x15c00a4||vt==base+0x15c8324)&&
     field<uintptr_t>(task,4)==env&&field<uintptr_t>(task,8)==u;
}
static bool dismount_body143(const PossessionIdentity& c){
 const auto& p=possession_lease.identity;
 return possession_lease.active&&p.mount&&p.mount_id&&p.mount_config&&p.mount_holder&&c.actor==p.actor&&c.actor_id==p.actor_id&&
     c.unit==p.unit&&c.unit_id==p.unit_id&&c.environment==p.environment&&
     c.common==p.common&&c.phase==p.phase&&c.manager==p.manager&&c.camera==p.camera&&
     field<uintptr_t>(c.actor,0x28)==c.environment&&field<unsigned char>(c.actor,0xf0)==1;
}
static bool dismount_pending143(const PossessionIdentity& c){
 if(!dismount_body143(c)||!native_dismount_task143(c.unit,c.environment))return false;
 auto mount=field<uintptr_t>(c.actor,0x210);
 return (!mount||mount==possession_lease.identity.mount)&&
     (c.config==possession_lease.identity.config||c.config==field<uintptr_t>(c.unit,0xf9c));
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
 return dismount_body143(c)&&!c.mount&&!field<uintptr_t>(c.actor,0x210)&&
     c.config&&c.config==field<uintptr_t>(c.unit,0xf9c)&&
     (native_dismount_task143(c.unit,c.environment)||dismount_witnessed143(c));
}
static bool dismount_native_motion143(){
 if(!possession_lease.active||!possession_lease.identity.mount||!possessed)return false;
 return !field<uintptr_t>(possessed,0x210)||native_dismount_task143(unit,environment);
}
