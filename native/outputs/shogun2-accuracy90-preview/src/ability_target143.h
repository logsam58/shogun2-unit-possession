#pragma once
#include "../../shogun2-possession-cleanup-v46/src/actor_types152.h"
static bool command_native_ready100(HWND);
static bool command_native_context100(uintptr_t,uintptr_t);
static LPARAM command_native_point100(HWND,RECT&);
static void command_refresh103(uintptr_t);
// Retail1872a0 case23 requires an explicit friendly unit. Generic formation
// ability toggles contain no target and therefore cannot implement Inspire.
static bool ability_friendly143(uintptr_t target){
 if(!target||target==unit||!field<unsigned>(target,4)||field<uintptr_t>(target,0x14)!=environment||
    field<uintptr_t>(target,0)!=field<uintptr_t>(unit,0))return false;
 auto source_side=field<uintptr_t>(possessed,0x44);if(!source_side)return false;
 unsigned team=field<unsigned>(source_side,0x10);if(team>31)return false;
 // Use the same proved actor-side/team fields as native Q targeting. Unit+10
 // is not used as a guessed allegiance pointer. Bounded only on the G edge.
 for(unsigned list:{0x384u,0x2bcu}){
  auto count=field<unsigned>(target,list),array=field<uintptr_t>(target,list+4);
  if(count>2000||(!array&&count))return false;
  for(unsigned i=0;i<std::min(count,32u);++i){
   __try {
    auto actor=field<uintptr_t>(array,i*4);
    if(!actor||!actor152::human(base,field<uintptr_t>(actor,0))||field<uintptr_t>(actor,0x214)!=target||
       field<uintptr_t>(actor,0x28)!=environment||field<unsigned char>(actor,0xf0)!=1)continue;
    auto side=field<uintptr_t>(actor,0x44);if(side)return field<unsigned>(side,0x10)==team;
   }__except(EXCEPTION_EXECUTE_HANDLER){continue;}
  }
 }
 return false;
}
static uintptr_t ability_aim_friend143(){
 if(command_placing100||!command_native_ready100(input_window))return 0;
 auto root=field<uintptr_t>(base,0x1bc8180);if(!command_native_context100(root,environment))return 0;
 RECT client{};auto point=command_native_point100(input_window,client);if(point==-1)return 0;
 // Refresh the original cursor provider at the crosshair, without a selection
 // click or Q's enemy-priority filter. Selection stays on the possessed unit.
 auto previous=ability_target_query143;ability_target_query143=possession_lease.generation;
 __try {CallWindowProcW(native_window_proc,input_window,WM_MOUSEMOVE,0,point);command_refresh103(root);}
 __finally {ability_target_query143=previous;}
 auto hit=field<uintptr_t>(root,0x130),metadata=hit?field<uintptr_t>(hit,0x20):0;
 auto target=metadata?field<uintptr_t>(metadata,0):0;
 return ability_friendly143(target)?target:0;
}
static bool ability_inspire143(uintptr_t target){
 if(!followed(possessed,false)||!selected_possessed_group()||!ability_friendly143(target)||!equipment_unit_ability81(35,true))return false;
 alignas(16) unsigned char context[16]{},packet[QUEUE_CAPACITY+4]{},point[20]{};bool live=false,ok=false;
 __try{
  auto serial=field<unsigned>(target,4),queue=field<uintptr_t>(environment,0x28048);unsigned flags=0,key=field<unsigned>(base,0x192da98);
  if(!queue||!descriptor_flags(key,flags))return false;
  reinterpret_cast<ContextConstructor>(base+0x1cc200)(context,packet,key,flags);live=true;
  // Exact inverse of retail2ab840: unit27, enum4, unit27, bool1, native point.
  reinterpret_cast<WriteObject>(base+0x285c80)(context,reinterpret_cast<void*>(unit));
  reinterpret_cast<WriteUInt>(base+0x285a00)(context,35);
  reinterpret_cast<WriteObject>(base+0x285c80)(context,reinterpret_cast<void*>(target));
  using WriteByte143=void*(__cdecl*)(void*,unsigned char);reinterpret_cast<WriteByte143>(base+0x285d00)(context,0);
  using EmptyPoint143=void(__thiscall*)(void*);reinterpret_cast<EmptyPoint143>(base+0x359180)(point);
  reinterpret_cast<WriteObject>(base+0x285990)(context,point);
  reinterpret_cast<NativeCleanup>(base+0x1d0d40)(context);live=false;
  auto length=field<unsigned>(reinterpret_cast<uintptr_t>(packet),QUEUE_CAPACITY);auto& used=field<unsigned>(queue+4,QUEUE_CAPACITY);
  if(field<unsigned>(target,4)==serial&&ability_friendly143(target))ok=append_packet(reinterpret_cast<unsigned char*>(queue+4),used,packet,length,key);
 }__except(EXCEPTION_EXECUTE_HANDLER){ok=false;}
 if(live){__try {reinterpret_cast<NativeCleanup>(base+0x1d0d40)(context);}__except(EXCEPTION_EXECUTE_HANDLER){}}
 return ok;
}
