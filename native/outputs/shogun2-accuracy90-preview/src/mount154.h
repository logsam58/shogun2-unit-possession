#pragma once
// Native UI registration10ec21a: dismount actions67/68 -> ability13.
// Original10dfb80/10f97d0 constructs/serializes; native15be80 and executor
// 1ac250/1214f0 retain unit restrictions, nearby mounts and animations.
static volatile LONG mount_orders154,mount_unavailable154;
static void mount_prepare154(uintptr_t camera){
 if(!InterlockedCompareExchange(&mount_request154,0,0))return; // No new roster proof in ordinary frames.
 if(!enabled||!state||state->mode!=POSSESSED||state->stop||state->release||!possession_lease.active){InterlockedExchange(&mount_request154,0);return;}
 // Native UI queue dispatch is window-thread/camera owned. A render or
 // unrelated camera update must neither issue nor consume the pending edge.
 if(!camera_callback_depth||GetCurrentThreadId()!=state->window_thread||camera!=possession_lease.identity.camera||!camera_owned(camera))return;
 unsigned request=unsigned(InterlockedExchange(&mount_request154,0));if(!request)return;
 __try{
  if(!state||state->mode!=POSSESSED||state->stop||state->release||dead_view()||!possession_lease.active||request!=possession_lease.generation||
     !followed(possessed,false)||equipment_committed81()||charge_input_locked146()||native_dismount_task143(unit,environment)||!equipment_unit_ability81(0x13,true)){++mount_unavailable154;return;}
  auto root=field<uintptr_t>(base,0x1bc8180),group=selected_possessed_group(),queue=field<uintptr_t>(environment,0x28048);
  if(!root||!group||!queue||field<uintptr_t>(root,0x5b8)!=environment||field<uintptr_t>(root,0x5b4)+0xb0!=group){++mount_unavailable154;return;}
  auto before=field<unsigned>(queue+4,QUEUE_CAPACITY);if(before>QUEUE_CAPACITY-256){++mount_unavailable154;return;}
  mount_completed154={};
  reinterpret_cast<void(__thiscall*)(void*,unsigned,float)>(base+0x10dfb80)(reinterpret_cast<void*>(root),possession_lease.identity.mount?0x67:0x68,0.f);
  auto after=field<unsigned>(queue+4,QUEUE_CAPACITY);if(after>before&&after<=QUEUE_CAPACITY)++mount_orders154;else ++mount_unavailable154;
 }__except(EXCEPTION_EXECUTE_HANDLER){++mount_unavailable154;}
}
