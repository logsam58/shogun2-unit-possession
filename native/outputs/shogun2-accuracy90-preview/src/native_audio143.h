#pragma once
// Native animation-linked bow audio is unchanged. Only per-ability sound
// selection is requested here; bow sound decoupling is deferred for later.
using NativeAudio143=void(__thiscall*)(void*,const unsigned*);
static volatile LONG ability_sound_played143,audio_errors143;
static uintptr_t native_audio_subscriber143(unsigned event_offset,unsigned vtable,unsigned adjustment,uintptr_t common=possession_lease.identity.common,uintptr_t env=environment){
 auto bus=field<uintptr_t>(common,0x98);if(!bus)return 0;
 auto node=field<uintptr_t>(bus,event_offset+4),end=field<uintptr_t>(bus,event_offset+8);
 for(unsigned i=0;node&&node!=end&&i<64;++i){
  auto callback=field<uintptr_t>(node,8);
  if(callback&&field<uintptr_t>(callback,0)==base+vtable){
   auto owner=callback-adjustment;
   if(field<uintptr_t>(owner,0)==base+0x15d4bbc&&field<uintptr_t>(owner,0x1d4)==env&&
      field<uintptr_t>(owner,0x1d8)&&field<uintptr_t>(field<uintptr_t>(owner,0x1d8),0)>=base&&
      field<uintptr_t>(field<uintptr_t>(owner,0x1d8),0)<base+0x1920000)return callback;
  }
  auto next=field<uintptr_t>(node,4);if(next==node)return 0;node=next;
 }
 return 0;
}
static void ability_sound143(unsigned id){
 __try{
  if(!followed(possessed,false))return;
  auto audio=native_audio_subscriber143(0xf14,0x15d4c34,0x118);
  if(!audio){InterlockedIncrement(&audio_errors143);return;}
  unsigned event[3]={field<unsigned>(audio,0x174),id,unsigned(unit)};
  reinterpret_cast<NativeAudio143>(base+0x2fb480)(reinterpret_cast<void*>(audio),event);
  InterlockedIncrement(&ability_sound_played143);
 }__except(EXCEPTION_EXECUTE_HANDLER){InterlockedIncrement(&audio_errors143);}
}
