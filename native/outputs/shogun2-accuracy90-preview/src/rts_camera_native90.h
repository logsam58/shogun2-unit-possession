#pragma once
#include "rts_camera90.h"
static rts90::Saved rts_view90;static volatile LONG rts_pending90,rts_serial90,rts_generation90;
static rts90::Pose read_rts_pose90(uintptr_t camera){rts90::Pose p;memcpy(p.eye,reinterpret_cast<void*>(camera+8),12);memcpy(p.target,reinterpret_cast<void*>(camera+0x14),12);memcpy(p.up,reinterpret_cast<void*>(camera+0x58),12);p.fov=field<float>(camera,0x24);return p;}
static void write_rts_pose90(uintptr_t camera,const rts90::Pose& p){
 memcpy(reinterpret_cast<void*>(camera+8),p.eye,12);memcpy(reinterpret_cast<void*>(camera+0x14),p.target,12);memcpy(reinterpret_cast<void*>(camera+0x58),p.up,12);field<float>(camera,0x24)=p.fov;
 // Refresh native direction/right caches; do not copy vtables, pointers or owners.
 reinterpret_cast<Cancel>(field<uintptr_t>(field<uintptr_t>(camera,0),0xf4))(reinterpret_cast<void*>(camera));
}
static rts90::Cache read_rts_cache90(uintptr_t camera){rts90::Cache c;c.override_matrix=field<unsigned char>(camera,0xa4);c.aspect=field<float>(camera,0x30);
 memcpy(c.view,reinterpret_cast<void*>(camera+0xa8),64);memcpy(c.projection,reinterpret_cast<void*>(camera+0xe8),64);return c;}
static bool valid_rts_cache90(const rts90::Cache& c){if(!c.override_matrix)return true;if(!std::isfinite(c.aspect)||c.aspect<=0)return false;
 for(unsigned i=0;i<16;++i)if(!std::isfinite(c.view[i])||!std::isfinite(c.projection[i]))return false;return true;}
static void write_rts_cache90(uintptr_t camera,const rts90::Cache& c){
 if(c.override_matrix){memcpy(reinterpret_cast<void*>(camera+0xa8),c.view,64);memcpy(reinterpret_cast<void*>(camera+0xe8),c.projection,64);
  float aspect=field<float>(camera,0x30);if(std::isfinite(aspect)&&aspect>0&&aspect!=c.aspect)field<float>(camera,0xe8)=c.projection[0]*c.aspect/aspect;}
 field<unsigned char>(camera,0xa4)=c.override_matrix;
}
static bool save_rts_view90(uintptr_t env,uintptr_t manager){
 __try {
  auto common=field<uintptr_t>(env,8),phase=common?field<uintptr_t>(common,0xb4):0,overhead=field<uintptr_t>(manager,0x8cc);
  if(!common||!phase||!overhead||field<uintptr_t>(manager,0x8e0)!=overhead||field<uintptr_t>(overhead,0)!=base+0x15d1a74)return false;
  rts90::Saved saved;saved.identity={env,common,phase,manager,overhead};saved.camera=read_rts_pose90(overhead);saved.manager=read_rts_pose90(manager);
  saved.camera_cache=read_rts_cache90(overhead);saved.manager_cache=read_rts_cache90(manager);
  if(!rts90::valid(saved.camera)||!rts90::valid(saved.manager)||!valid_rts_cache90(saved.camera_cache)||!valid_rts_cache90(saved.manager_cache))return false;
  saved.active=true;rts_view90=saved;rts_saved90=1;++rts_captures90;InterlockedExchange(&rts_pending90,0);InterlockedExchange(&rts_generation90,InterlockedIncrement(&rts_serial90));return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){++rts_rejects90;return false;}
}
static void request_rts_restore90(){
 LONG generation=InterlockedCompareExchange(&rts_generation90,0,0);
 if(generation&&input_window&&InterlockedCompareExchange(&rts_pending90,generation,0)==0)
  if(!PostMessageW(input_window,RETURN_RTS_MESSAGE90,WPARAM(generation),0))InterlockedCompareExchange(&rts_pending90,0,generation);
}
static void restore_rts_on_window90(uint32_t message_generation){
 LONG generation=InterlockedCompareExchange(&rts_generation90,0,0);
 if(message_generation&&message_generation!=uint32_t(generation))return;
 if(!generation||!rts_pending90||!rts_view90.active){InterlockedExchange(&rts_pending90,0);return;}
 if(camera_callback_depth||GetCurrentThreadId()!=state->window_thread){PostMessageW(input_window,RETURN_RTS_MESSAGE90,WPARAM(generation),0);return;}
 auto saved=rts_view90;rts_view90.active=false;rts_saved90=0;
 InterlockedExchange(&rts_generation90,0);InterlockedExchange(&rts_pending90,0);
 __try {
  auto root=field<uintptr_t>(base,0x1bc8180),env=root?field<uintptr_t>(root,0x110):0,common=env?field<uintptr_t>(env,8):0,phase=common?field<uintptr_t>(common,0xb4):0;
  auto manager=env?field<uintptr_t>(env,0x281fc):0,overhead=manager?field<uintptr_t>(manager,0x8cc):0;
  unsigned phase_id=phase?field<unsigned>(phase,0x14):0;
  if(phase_id<5||phase_id>8){++rts_rejects90;return;}
  if(!rts90::same(saved.identity,{env,common,phase,manager,overhead})||field<uintptr_t>(manager,0)!=base+0x15d26fc||field<uintptr_t>(overhead,0)!=base+0x15d1a74){++rts_rejects90;return;}
  auto current=field<uintptr_t>(manager,0x8e0);
  if(current!=overhead&&(current!=field<uintptr_t>(manager,0x8e4)||field<uintptr_t>(current,0)!=base+0x15d22e8)){++rts_rejects90;return;}
  interface_restore();relinquish();original_toggle(reinterpret_cast<void*>(manager),0);
  if(field<uintptr_t>(manager,0x8e0)!=overhead){++rts_rejects90;return;}
  write_rts_pose90(overhead,saved.camera);write_rts_pose90(manager,saved.manager);write_rts_cache90(overhead,saved.camera_cache);write_rts_cache90(manager,saved.manager_cache);++rts_restores90;
  // Numeric telemetry remains available after leaving possession.
  publish_accuracy90(nullptr,0,0);
 }__except(EXCEPTION_EXECUTE_HANDLER){++rts_rejects90;}
}
