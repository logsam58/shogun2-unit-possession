#pragma once
// One-sailor prototype. Native addresses/ABIs: docs/research/NAVAL127.md.
// No land-unit lease, roster removal, ship order, or coordinate teleport.
#include <cmath>
#include <cstdint>
#include "steering.h"
namespace naval127 {
struct Location {float xyz[3];uintptr_t surface,frame;};
struct Query {const float* xz;const Location* seed;unsigned mode;unsigned char allowed,pad[3];};
static_assert(sizeof(Location)==20&&sizeof(Query)==16,"Native x86 navigation layout");
struct Identity {
 uintptr_t root,env,common,phase,manager,camera,ship,army,registry,actor,config,actor_registry;
 unsigned ship_id,actor_id,task_id;
};
struct Pose {float eye[3],target[3],up[3];};
struct Control {
 Identity id{};Pose saved{},shown{};float actor_position[3]{};
 float yaw{},pitch{.15f};unsigned last_input{~0u};float last_yaw{};
 ULONGLONG next_order{};bool active{},held{},restore{};
 unsigned requests{},returns{},rejected{},failure{};
 unsigned motion_frames{},render_poses{};
 // Native overhead renderer consumes the interpolated vectors, not just eye/target.
 float saved_render[12]{},saved_height{};bool camera_applied{};unsigned camera_frames{};
};
static SRWLOCK lock=SRWLOCK_INIT;
static Control control;
static volatile LONG owned,quitting,buttons,camera_pending;
using Dispatch=bool(__thiscall*)(void*);
static Dispatch original_dispatch;
using OverheadUpdate=bool(__thiscall*)(void*,float,float);
static OverheadUpdate original_overhead;
static unsigned entry_failure;
static bool active(){return InterlockedCompareExchange(&owned,0,0)!=0;}
static bool input_active(){return active()&&!InterlockedCompareExchange(&quitting,0,0);}
static void release(){if(active())InterlockedExchange(&quitting,1);}
static bool world(const Identity& i){
 if(!i.root||field<uintptr_t>(base,0x1bc8180)!=i.root||field<uintptr_t>(i.root,0x110)!=i.env||
    field<uintptr_t>(i.env,8)!=i.common||field<uintptr_t>(i.common,0xb4)!=i.phase)return false;
 unsigned phase=field<unsigned>(i.phase,0x14);
 return phase>=5&&phase<=8&&field<uintptr_t>(i.env,0x281fc)==i.manager&&
    field<uintptr_t>(i.manager,0)==base+0x15d26fc&&
    field<uintptr_t>(i.manager,0x8cc)==i.camera&&field<uintptr_t>(i.camera,0)==base+0x15d1a74;
}
static bool surface(uintptr_t p,const Identity& i){
 return p&&field<uintptr_t>(p,0)==base+0x15de30c&&field<uintptr_t>(p,0x24)==i.env&&
   field<uintptr_t>(p,0xa4)==i.ship&&field<uintptr_t>(p,0x80)==field<uintptr_t>(i.ship,0x764);
}
static bool member(const Identity& i){
 if(!world(i)||field<uintptr_t>(i.ship,0)!=base+0x15ddb60||
    field<unsigned>(i.ship,0x1d8)!=i.ship_id||field<uintptr_t>(i.ship,0x1e8)!=i.env||
    field<uintptr_t>(i.ship,0x4b0)!=i.army||field<unsigned char>(i.army,0x270)!=1)return false;
 if(i.registry!=field<uintptr_t>(i.ship,0x578)&&i.registry!=field<uintptr_t>(i.ship,0x5cc))return false;
 if(field<uintptr_t>(i.registry,0)!=base+0x15bb22c||field<uintptr_t>(i.registry,4)!=i.env)return false;
 unsigned count=field<unsigned>(i.registry,0x18),capacity=field<unsigned>(i.registry,0x14);
 auto data=field<uintptr_t>(i.registry,0x1c);bool found=false;
 if(count>capacity||capacity>2048||!data)return false;
 for(unsigned n=0;n<count;++n)if(field<uintptr_t>(data,n*4)==i.actor){found=true;break;}
 if(!found)return false;
 return field<uintptr_t>(i.actor,0)==base+0x15dfec8&&
   field<unsigned>(i.actor,0x1c8)==i.actor_id&&field<uintptr_t>(i.actor,0x1cc)==i.actor_registry&&
   field<uintptr_t>(i.actor,0x28)==i.env&&field<uintptr_t>(i.actor,0x20c)==i.config&&
   !field<uintptr_t>(i.actor,0x214)&&field<uintptr_t>(i.actor,0x21c)==i.ship&&
   field<uintptr_t>(i.actor,0x3b8)==i.registry;
}
static bool same_duty(const Identity& i){
 return field<unsigned char>(i.actor,0x384)==1&&field<uintptr_t>(i.actor,0x2f8)==base+0x15dd7ac&&
        field<unsigned>(i.actor,0x2fc)==i.task_id;
}
static bool eligible(const Identity& i){
 if(!member(i)||!same_duty(i)||field<unsigned char>(i.actor,0xf0)!=1||
    field<uintptr_t>(i.actor,0x18)||!surface(field<uintptr_t>(i.actor,0x10),i)||
    (field<unsigned char>(i.actor,0x5f8)==1&&!field<unsigned char>(i.actor,0x57c)))return false;
 // A reserved station must still belong to this sailor. Keep it reserved.
 auto handle=field<uintptr_t>(i.actor,0x600);
 if(!handle)return false;
 auto station=field<uintptr_t>(handle,0);
 return station&&field<uintptr_t>(station,0x24)==i.actor;
}
static bool sole_player(const Identity& i){
 if(field<uintptr_t>(i.env,0x28248)||field<unsigned char>(i.env,0x28254))return false;
 unsigned count=field<unsigned>(i.phase,0x88);auto sides=field<uintptr_t>(i.phase,0x8c);
 if(!count||count>16||!sides)return false;
 uintptr_t human=0;
 for(unsigned n=0;n<count;++n){auto side=field<uintptr_t>(sides,n*4);if(!side)return false;
  auto rows=field<uintptr_t>(side,0x24);unsigned size=field<unsigned>(side,0x20);
  if(size>16||(size&&!rows))return false;
  for(unsigned j=0;j<size;++j){auto army=field<uintptr_t>(rows,j*4);if(!army)return false;
   if(field<unsigned char>(army,0x270)){if(human)return false;human=army;}
  }
 }
 return human==i.army;
}
static Pose read_pose(uintptr_t camera){Pose p{};memcpy(p.eye,reinterpret_cast<void*>(camera+8),12);
 memcpy(p.target,reinterpret_cast<void*>(camera+0x14),12);memcpy(p.up,reinterpret_cast<void*>(camera+0x58),12);return p;}
static bool finite_pose(const Pose& p){for(unsigned n=0;n<3;++n)if(!std::isfinite(p.eye[n])||!std::isfinite(p.target[n])||!std::isfinite(p.up[n]))return false;return true;}
static bool begin(uintptr_t env,uintptr_t common,uintptr_t phase,uintptr_t ship){
 entry_failure=0;
 if(active()||device192::enabled()){entry_failure=1;return false;} // Initial test uses PC keyboard/mouse.
 bool accepted=false;
 AcquireSRWLockExclusive(&lock);
 __try {
  if(control.restore){
   // A retired world or a view that was never applied cannot need restoration.
   if(!world(control.id)||!control.camera_applied){control.restore=false;InterlockedExchange(&camera_pending,0);}
   else {entry_failure=2;__leave;}
  }
  Identity id{};id.root=field<uintptr_t>(base,0x1bc8180);id.env=env;id.common=common;id.phase=phase;
  id.manager=field<uintptr_t>(env,0x281fc);if(!id.manager)__leave;
  id.camera=field<uintptr_t>(id.manager,0x8cc);id.ship=ship;
  if(!ship||field<uintptr_t>(ship,0)!=base+0x15ddb60)__leave;
  id.ship_id=field<unsigned>(ship,0x1d8);id.army=field<uintptr_t>(ship,0x4b0);
  if(!id.ship_id||!id.army||!world(id)||!sole_player(id)||field<uintptr_t>(id.manager,0x8e0)!=id.camera)__leave;
  bool found=false;
  for(unsigned cohort=0;cohort<2&&!found;++cohort){
   id.registry=field<uintptr_t>(ship,cohort?0x5cc:0x578);if(!id.registry)continue;
   if(field<uintptr_t>(id.registry,0)!=base+0x15bb22c||field<unsigned>(id.registry,0x510)!=cohort)continue;
   unsigned count=field<unsigned>(id.registry,0x18),capacity=field<unsigned>(id.registry,0x14);
   auto rows=field<uintptr_t>(id.registry,0x1c);if(!rows||count>capacity||capacity>2048)continue;
   for(unsigned n=0;n<count;++n){id.actor=field<uintptr_t>(rows,n*4);if(!id.actor)continue;
    if(field<uintptr_t>(id.actor,0)!=base+0x15dfec8)continue;
    id.actor_id=field<unsigned>(id.actor,0x1c8);id.config=field<uintptr_t>(id.actor,0x20c);
    id.actor_registry=field<uintptr_t>(id.actor,0x1cc);id.task_id=field<unsigned>(id.actor,0x2fc);
    if(id.config&&id.actor_registry&&eligible(id)){found=true;break;}
   }
  }
  if(!found)__leave;
  Pose saved=read_pose(id.camera);if(!finite_pose(saved))__leave;
  control={};control.id=id;control.saved=saved;control.active=true;control.restore=true;
  memcpy(control.saved_render,reinterpret_cast<void*>(id.camera+0x184),sizeof control.saved_render);
  control.saved_height=field<float>(id.camera,0x180);
  control.yaw=std::atan2(saved.target[0]-saved.eye[0],saved.target[2]-saved.eye[2]);
  memcpy(control.actor_position,reinterpret_cast<void*>(id.actor+0x48),12);
  InterlockedExchange(&quitting,0);InterlockedExchange(&buttons,0);InterlockedExchange(&camera_pending,1);InterlockedExchange(&owned,1);accepted=true;
 }__except(EXCEPTION_EXECUTE_HANDLER){control.failure=1;}
 ReleaseSRWLockExclusive(&lock);if(!accepted&&!entry_failure)entry_failure=3;return accepted;
}
static void resume_current(Control& c){
 // Only reset our still-current ordinary task. Never restore stale task bytes,
 // station pointers, roster state, ship orders, casualties, or priority tasks.
 if(c.held&&member(c.id)&&same_duty(c.id)&&field<unsigned char>(c.id.actor,0xf0)==1){
  field<unsigned char>(c.id.actor,0x308)=0;
  field<unsigned char>(c.id.actor,0x385)=0;
  ++c.returns;
 }
 c.active=false;c.held=false;InterlockedExchange(&owned,0);InterlockedExchange(&buttons,0);
}
static bool submit(Control& c,unsigned input){
 const auto& i=c.id;Location here{},next{};float position[3]{};
 reinterpret_cast<void(__thiscall*)(void*,Location*)>(base+0x3df250)(reinterpret_cast<void*>(i.actor),&here);
 if(!surface(here.surface,i)||!here.frame)return false;
 reinterpret_cast<void*(__thiscall*)(void*,float*)>(base+0x399f80)(&here,position);
 float dx=position[0]-field<float>(i.actor,0x48),dz=position[2]-field<float>(i.actor,0x50);
 if(!std::isfinite(dx)||!std::isfinite(dz)||dx*dx+dz*dz>4.f)return false;
 // Install one stationary native walking context when taking control. Movement
 // below feeds the shared native integrator, never a stream of new path orders.
 const bool run=false;
 const float speed=field<float>(i.config,0x1c);
 if(!std::isfinite(speed)||speed<=0||speed>30.f)return false;
 next=here;(void)input;
 unsigned char task[0x41]{},behavior[0x3d]{};
 const auto heading=field<unsigned short>(i.actor,0x64);
 reinterpret_cast<void*(__thiscall*)(void*,const Location*,unsigned,unsigned,float,unsigned short,unsigned char)>(base+0x350d10)(task,&next,0,run?1u:0u,speed,heading,1);
 reinterpret_cast<void*(__thiscall*)(void*,const void*)>(base+0x3b91c0)(behavior,task);
 reinterpret_cast<void(__thiscall*)(void*,const void*)>(base+0x3d9320)(reinterpret_cast<void*>(i.actor),behavior);
 ++c.requests;return true;
}
static bool __fastcall dispatch(void* context,void*){
 bool consume=false;
 // The native caller owns the actor lifetime. Other contexts take the untouched path.
 if(active()){
  AcquireSRWLockExclusive(&lock);
  __try {
   auto& c=control;
   if(c.active&&reinterpret_cast<uintptr_t>(context)==c.id.actor+0x2f0){
    if(quitting||!enabled||!state||state->stop||GetTickCount64()-state->heartbeat>500||!eligible(c.id))resume_current(c);
    else {
     c.held=true;field<unsigned char>(c.id.actor,0x308)=0;
     field<unsigned char>(c.id.actor,0x385)=0; // Native restart remains armed even on adapter shutdown.
     memcpy(c.actor_position,reinterpret_cast<void*>(c.id.actor+0x48),12);
     if(!c.requests&&!submit(c,0)){c.failure=2;resume_current(c);}
     consume=c.active;
    }
   }
  }__except(EXCEPTION_EXECUTE_HANDLER){control.failure=3;InterlockedExchange(&quitting,1);}
  ReleaseSRWLockExclusive(&lock);
 }
 return consume?true:original_dispatch(context);
}
// 3c1d90 -> 366a10 -> 367600 is shared by native sailors and land actors.
// The existing motion detour calls this immediately before the unmodified
// integrator, after native duty/navigation have produced their intentions.
static bool prepare_motion(uintptr_t actor){
 if(!active())return false;
 bool applied=false;
 AcquireSRWLockExclusive(&lock);
 __try {
  auto& c=control;
  if(!c.active||actor!=c.id.actor)__leave;
  if(quitting||!enabled||!state||state->stop||GetTickCount64()-state->heartbeat>500||!eligible(c.id)){
   resume_current(c);__leave;
  }
  if(!c.held||!c.requests)__leave;
  const float dt=field<float>(base,0x15b2448);
  if(!std::isfinite(dt)||dt<.001f||dt>.2f)__leave;
  const unsigned input=unsigned(InterlockedCompareExchange(&buttons,0,0));
  const bool run=(input&RUN)&&(input&FORWARD)&&!(input&BACKWARD);
  const float speed=field<float>(c.id.config,run?0x20:0x1c);
  const float turn_rate=field<float>(c.id.config,0x6c);
  if(!std::isfinite(speed)||speed<=0||speed>30.f||!std::isfinite(turn_rate)||turn_rate<0||turn_rate>100.f){
   c.failure=5;resume_current(c);__leave;
  }
  const auto bearing=field<unsigned short>(actor,0x64);
  Intent intent=steering_view(input,c.yaw,dt,speed);
  intent.turn=face_view(bearing,c.yaw,dt,turn_rate);
  const float xz[]={field<float>(actor,0x48)+intent.x,field<float>(actor,0x50)+intent.z};
  if(!std::isfinite(xz[0])||!std::isfinite(xz[1])){c.failure=5;resume_current(c);__leave;}
  // Native setters retain the sailor's own rig and transition logic. State0 is
  // ordinary locomotion; 366a10 derives its animation from actual post-collision
  // velocity. Repeated equal state requests do not restart the animation.
  reinterpret_cast<void(__thiscall*)(void*,unsigned,unsigned char,unsigned char)>(base+0x3e3f50)(reinterpret_cast<void*>(actor),0,0,0);
  reinterpret_cast<void(__thiscall*)(void*,unsigned)>(base+0x3e3d20)(reinterpret_cast<void*>(actor+0x250),run?1u:0u);
  reinterpret_cast<void(__thiscall*)(void*,const float*,unsigned short)>(base+0x3936f0)(reinterpret_cast<void*>(actor),xz,static_cast<unsigned short>(bearing+intent.turn));
  c.last_input=input;c.last_yaw=c.yaw;++c.motion_frames;applied=true;
 }__except(EXCEPTION_EXECUTE_HANDLER){control.failure=5;InterlockedExchange(&quitting,1);}
 ReleaseSRWLockExclusive(&lock);return applied;
}
static void sample(unsigned input,bool focused){
 if(!active())return;
 InterlockedExchange(&buttons,focused?LONG(input&(FORWARD|BACKWARD|LEFT|RIGHT|RUN)):0);
 if(!focused)release();
}
static void look(float dx,float dy){
 AcquireSRWLockExclusive(&lock);
 if(control.active&&!quitting){control.yaw+=dx*.0025f;control.pitch=std::max(-.8f,std::min(.9f,control.pitch+dy*.0025f));}
 ReleaseSRWLockExclusive(&lock);
}
// Ghidra: manager3268e0 calls current camera vtable+13c (3290a0 for
// overhead); render accessors324f20/325010 consume184/19c. Land's134fad0
// matrix setter is not the naval overhead frame path.
static void restore_numeric(Control& c,uintptr_t camera){
 memcpy(reinterpret_cast<void*>(camera+8),c.saved.eye,12);
 memcpy(reinterpret_cast<void*>(camera+0x14),c.saved.target,12);
 memcpy(reinterpret_cast<void*>(camera+0x58),c.saved.up,12);
 memcpy(reinterpret_cast<void*>(camera+0x184),c.saved_render,sizeof c.saved_render);
 field<float>(camera,0x180)=c.saved_height;
 reinterpret_cast<void(__thiscall*)(void*)>(base+0x134fe50)(reinterpret_cast<void*>(camera));
 c.camera_applied=false;
}
static void retire_camera(){
 control.active=control.restore=control.held=control.camera_applied=false;
 InterlockedExchange(&camera_pending,0);InterlockedExchange(&owned,0);InterlockedExchange(&buttons,0);
}
static void before_camera(uintptr_t camera){
 AcquireSRWLockExclusive(&lock);
 __try {
  auto& c=control;if(!c.restore)__leave;
  if(!world(c.id)){retire_camera();__leave;}
  if(camera==c.id.camera&&c.camera_applied)restore_numeric(c,camera);
 }__except(EXCEPTION_EXECUTE_HANDLER){control.failure=4;InterlockedExchange(&quitting,1);}
 ReleaseSRWLockExclusive(&lock);
}
// 25b360/2a02a0 construct scene68's bounded ship-view array. 26c1f0
// ties view70 to the ship; 49d240 ties each render holder08 to its actor.
// 4cda80 uses the renderer's own interpolation and returns the same matrix
// used for drawing, instead of following the 10Hz simulation position.
static bool render_position(Control& c,float* position){
 const auto& i=c.id;
 const auto scene=field<uintptr_t>(i.env,0x28264);
 if(!scene)return false;
 const auto views=field<uintptr_t>(scene,0x68);if(!views)return false;
 const unsigned capacity=field<unsigned>(views,0x38),count=field<unsigned>(views,0x3c);
 const auto rows=field<uintptr_t>(views,0x40);
 if(!rows||!i.ship_id||i.ship_id>count||count>capacity||capacity>4096)return false;
 const auto view=field<uintptr_t>(rows,(i.ship_id-1)*4);
 if(!view||field<uintptr_t>(view,0)!=base+0x15d359c||field<uintptr_t>(view,0x70)!=i.ship)return false;
 const unsigned n=field<unsigned>(view,0x18),cap=field<unsigned>(view,0x14);
 const auto actors=field<uintptr_t>(view,0x1c);
 if(!actors||n>cap||cap>2048)return false;
 for(unsigned j=0;j<n;++j){
  const auto holder=field<uintptr_t>(actors,j*4);
  if(!holder||field<uintptr_t>(holder,8)!=i.actor)continue;
  const auto table=field<uintptr_t>(holder,0);
  if(table<base||table>base+0x1d88000-8||field<uintptr_t>(table,4)!=base+0x4cd620)return false;
  const auto render_data=field<uintptr_t>(holder,0x14);
  if(!render_data||!field<uintptr_t>(render_data,0xec))return false;
  float matrix[16]{};
  reinterpret_cast<void(__thiscall*)(void*,float*)>(base+0x4cda80)(reinterpret_cast<void*>(holder),matrix);
  for(unsigned axis=0;axis<3;++axis){
   const float value=matrix[axis*4+3];
   if(!std::isfinite(value)||std::fabs(value-field<float>(i.actor,0x48+axis*4))>50.f)return false;
   position[axis]=value;
  }
  ++c.render_poses;return true;
 }
 return false;
}
static bool camera(uintptr_t camera){
 bool applied=false;
 AcquireSRWLockExclusive(&lock);
 __try {
  auto& c=control;
  if(!c.restore)__leave;
  if(!world(c.id)){retire_camera();__leave;}
  // The native frame callback owns this camera's lifetime. The actor still
  // belongs to the simulation dispatcher: no duty/task writes from rendering.
  if(c.active&&(!member(c.id)||field<unsigned char>(c.id.actor,0xf0)!=1)){
   c.active=c.held=false;InterlockedExchange(&owned,0);
  }
  if(camera!=c.id.camera)__leave;
  if(field<uintptr_t>(c.id.manager,0x8e0)!=camera){InterlockedExchange(&quitting,1);__leave;}
  ++c.camera_frames;
  if(!c.active||quitting||!enabled||!state||state->stop||GetTickCount64()-state->heartbeat>500){
   InterlockedExchange(&quitting,1);restore_numeric(c,camera);
   c.restore=false;InterlockedExchange(&camera_pending,0);applied=true;__leave;
  }
  float rendered[3]{};
  if(render_position(c,rendered))memcpy(c.actor_position,rendered,12);
  Pose p{};const float back=2.8f,cosine=std::cos(c.pitch);
  p.target[0]=c.actor_position[0];p.target[1]=c.actor_position[1]+1.5f;p.target[2]=c.actor_position[2];
  p.eye[0]=p.target[0]-std::sin(c.yaw)*cosine*back;p.eye[2]=p.target[2]-std::cos(c.yaw)*cosine*back;
  p.eye[1]=p.target[1]+std::sin(c.pitch)*back;p.up[1]=1;
  if(!finite_pose(p)){InterlockedExchange(&quitting,1);__leave;}
  memcpy(reinterpret_cast<void*>(camera+8),p.eye,12);memcpy(reinterpret_cast<void*>(camera+0x14),p.target,12);memcpy(reinterpret_cast<void*>(camera+0x58),p.up,12);
  // Both current and destination of each native smoothing vector agree. The
  // regular overhead pose is restored before its next update, so its terrain
  // clamps never operate on the sailor's near-deck camera.
  for(unsigned offset:{0x184u,0x190u})memcpy(reinterpret_cast<void*>(camera+offset),p.eye,12);
  for(unsigned offset:{0x19cu,0x1a8u})memcpy(reinterpret_cast<void*>(camera+offset),p.target,12);
  reinterpret_cast<void(__thiscall*)(void*)>(base+0x134fe50)(reinterpret_cast<void*>(camera));
  c.shown=p;c.camera_applied=true;applied=true;
 }__except(EXCEPTION_EXECUTE_HANDLER){control.failure=4;InterlockedExchange(&quitting,1);}
 ReleaseSRWLockExclusive(&lock);return applied;
}
static bool __fastcall overhead(void* object,void*,float time,float interpolation){
 // All non-owned overhead cameras execute their original update unmodified.
 if(!InterlockedCompareExchange(&camera_pending,0,0))return original_overhead(object,time,interpolation);
 before_camera(reinterpret_cast<uintptr_t>(object));
 bool native_result=original_overhead(object,time,interpolation);
 return camera(reinterpret_cast<uintptr_t>(object))?true:native_result;
}
static void install(){
 naval_keyboard_owned127=&input_active;
 const unsigned char camera_sig[]={0x83,0xec,0x38,0xf3,0x0f,0x10,0x05};
 require(!memcmp(reinterpret_cast<void*>(base+0x3290a0),camera_sig,sizeof camera_sig)&&
  field<uintptr_t>(base,0x15d1a74+0x13c)==base+0x3290a0&&
  field<uintptr_t>(base,0x15d1a74+0x120)==base+0x324f20&&
  field<uintptr_t>(base,0x15d1a74+0x124)==base+0x325010,"Naval camera frame signature differs");
 require(MH_CreateHook(reinterpret_cast<void*>(base+0x3290a0),reinterpret_cast<void*>(&overhead),reinterpret_cast<void**>(&original_overhead))==MH_OK,"Naval camera frame hook failed");
 const unsigned char sig[]={0x56,0x8b,0xf1,0x80,0xbe,0x94,0,0,0,1};
 auto address=reinterpret_cast<void*>(base+0x3efc70);
 require(!memcmp(address,sig,sizeof sig),"Naval duty dispatcher signature differs");
 require(MH_CreateHook(address,reinterpret_cast<void*>(&dispatch),reinterpret_cast<void**>(&original_dispatch))==MH_OK,"Naval duty hook failed");
}
}
static bool naval_active127(){return naval127::active();}
static void naval_release127(){naval127::release();}
static bool naval_window127(HWND window,UINT message,WPARAM key,LPARAM flags){
 if(!naval127::input_active())return false;
 if(message==WM_KILLFOCUS||(message==WM_ACTIVATEAPP&&!key)){naval_release127();return false;}
 if(message==WM_KEYDOWN&&(key==VK_F10||key==VK_ESCAPE||key==VK_INSERT||key==VK_CAPITAL)){
  if(!(flags&(1L<<30)))naval_release127();return key!=VK_ESCAPE;
 }
 if(message==WM_INPUT&&raw_mouse_active&&read_raw_mouse(flags))return true;
 if(message==WM_SETCURSOR){SetCursor(nullptr);return true;}
 if(message>=WM_MOUSEFIRST&&message<=WM_MOUSELAST)return true;
 if(message>=WM_KEYFIRST&&message<=WM_KEYLAST){
  switch(key){case 'W':case 'A':case 'S':case 'D':case 'Q':case 'E':case 'Z':case 'C':case 'R':case 'F':case 'T':case 'G':
   case VK_SHIFT:case VK_CONTROL:case VK_TAB:case VK_F9:case VK_MENU:return true;}
 }
 (void)window;return false;
}
