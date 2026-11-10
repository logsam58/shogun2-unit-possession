// Execute production naval authority code in an isolated fixture engine.
// Point projection, environment and duty stations are synthetic. No live game.
#define wmain task_contract_entry127
#include "verify_naval_tasks127.cpp"
#undef wmain
#include <algorithm>
static uintptr_t base;
static volatile LONG enabled=1;
struct FixtureStatus127 {bool stop{};ULONGLONG heartbeat{};};
static FixtureStatus127 status127,*state=&status127;
template<class T>static T& field(uintptr_t p,unsigned offset){return *reinterpret_cast<T*>(p+offset);}
namespace device192 {static bool enabled(){return false;}}

static bool (*naval_keyboard_owned127)()=nullptr;
static bool raw_mouse_active=false;
static bool read_raw_mouse(LPARAM){return false;}
#ifndef NAVAL_REAL_HOOKS128
static constexpr int MH_OK=0;
static int MH_CreateHook(void*,void*,void**){return MH_OK;}
#endif
#ifndef NAVAL_CONTROL_HEADER128
#define NAVAL_CONTROL_HEADER128 "../../../native/outputs/shogun2-accuracy90-preview/src/naval_control127.h"
#endif
#include NAVAL_CONTROL_HEADER128
static bool reject_projection;
static unsigned projections;
static naval127::Location last_projection;
static naval127::Location* __fastcall project(void*,void*,naval127::Location* out,const naval127::Query* query,unsigned char* rejected,unsigned enabled_query){
 require(enabled_query==1&&query&&query->mode==3&&query->allowed==1,"Native projection ABI");
 ++projections;*out=*query->seed;*rejected=reject_projection?1:0;
 if(!reject_projection){out->xyz[0]=query->xz[0];out->xyz[2]=query->xz[1];}
 last_projection=*out;return out;
}
static unsigned char __fastcall allowed(void*,void*){return 1;}
static void __fastcall refresh(void*,void*){}
static float __fastcall motion_scale129(void*,void*){return 1.f;}
static unsigned render_updates129;
static float rendered_x129,rendered_y129,rendered_z129;
static void __fastcall render_update129(void*,void*,void* holder,unsigned mode){
 require(mode==0,"Native render interpolation mode");++render_updates129;
 at<float>(holder,0x30)=rendered_x129;at<float>(holder,0x40)=rendered_y129;at<float>(holder,0x50)=rendered_z129;
}
struct World127 {
 Bytes root=Bytes(0x200),env=Bytes(0x28400),common=Bytes(0x200),phase=Bytes(0x100),manager=Bytes(0x1000),camera=Bytes(0x600),ship=Bytes(0x800),army=Bytes(0x300),registry=Bytes(0x520),surface=Bytes(0xb0),frame=Bytes(0x200),config=Bytes(0x100),station=Bytes(0x40),sides=Bytes(0x40);
 Bytes scene=Bytes(0x100),views=Bytes(0x100),view=Bytes(0xc8),holder=Bytes(0x250),render_data=Bytes(0x110);
 uintptr_t view_rows[6]{},holder_row{};
 Actor sailor,peer;
 uintptr_t actor_row{},side_row{},army_row{},station_handle{};
 NavigationStats nav{};
 World127(Image& im):sailor(im),peer(im){
  base=reinterpret_cast<uintptr_t>(im.base);DWORD prior{};VirtualProtect(im.base,im.size,PAGE_EXECUTE_READWRITE,&prior);
  im.jump(0x37c090,reinterpret_cast<void*>(&project));
  im.jump(0x2d6870,reinterpret_cast<void*>(&render_update129));
  at<void*>(im.base,0x15dfec8+0x88)=reinterpret_cast<void*>(&motion_scale129);
  at<void*>(im.base,0x15dfec8+0x40)=reinterpret_cast<void*>(&allowed);
  at<void*>(im.base,0x15d1a74+0xf4)=reinterpret_cast<void*>(&refresh);
  at<void*>(im.base,0x15e0ddc)=reinterpret_cast<void*>(&old_navigation_destroy);
  at<void*>(im.base,0x1bc8180)=root.data();at<void*>(root.data(),0x110)=env.data();
  at<void*>(env.data(),8)=common.data();at<void*>(common.data(),0xb4)=phase.data();at<void*>(common.data(),0xb8)=common.data();
  at<unsigned>(phase.data(),0x14)=5;side_row=reinterpret_cast<uintptr_t>(sides.data());at<void*>(phase.data(),0x8c)=&side_row;at<unsigned>(phase.data(),0x88)=1;
  army_row=reinterpret_cast<uintptr_t>(army.data());at<void*>(sides.data(),0x24)=&army_row;at<unsigned>(sides.data(),0x20)=1;at<unsigned char>(army.data(),0x270)=1;
  at<void*>(env.data(),0x281fc)=manager.data();at<void*>(manager.data(),0)=im.base+0x15d26fc;
  at<void*>(manager.data(),0x8cc)=at<void*>(manager.data(),0x8e0)=camera.data();at<void*>(camera.data(),0)=im.base+0x15d1a74;
  at<float>(camera.data(),0x10)=-10;at<float>(camera.data(),0x5c)=1;
  at<void*>(ship.data(),0)=im.base+0x15ddb60;at<unsigned>(ship.data(),0x1d8)=6;at<void*>(ship.data(),0x1e8)=env.data();at<void*>(ship.data(),0x4b0)=army.data();
  at<void*>(ship.data(),0x578)=registry.data();at<void*>(ship.data(),0x764)=frame.data();
  at<void*>(registry.data(),0)=im.base+0x15bb22c;at<void*>(registry.data(),4)=env.data();
  actor_row=reinterpret_cast<uintptr_t>(sailor.p());at<unsigned>(registry.data(),0x14)=at<unsigned>(registry.data(),0x18)=1;at<void*>(registry.data(),0x1c)=&actor_row;
  at<void*>(surface.data(),0)=im.base+0x15de30c;at<void*>(surface.data(),0x24)=env.data();at<void*>(surface.data(),0xa4)=ship.data();at<void*>(surface.data(),0x80)=frame.data();
  at<void*>(env.data(),0x28264)=scene.data();at<void*>(scene.data(),0x68)=views.data();
  at<unsigned>(views.data(),0x38)=at<unsigned>(views.data(),0x3c)=6;at<void*>(views.data(),0x40)=view_rows;view_rows[5]=reinterpret_cast<uintptr_t>(view.data());
  at<void*>(view.data(),0)=im.base+0x15d359c;at<void*>(view.data(),0x70)=ship.data();
  at<unsigned>(view.data(),0x14)=at<unsigned>(view.data(),0x18)=1;holder_row=reinterpret_cast<uintptr_t>(holder.data());at<void*>(view.data(),0x1c)=&holder_row;
  at<void*>(holder.data(),0)=im.base+0x15e9e00;at<void*>(holder.data(),8)=sailor.p();at<void*>(holder.data(),0x14)=render_data.data();at<void*>(render_data.data(),0xec)=views.data();
  for(unsigned off:{0x24u,0x38u,0x4cu,0x60u})at<float>(holder.data(),off)=1.f;
  render_updates129=0;rendered_x129=rendered_y129=rendered_z129=0.f;
  // Identity transform; native 399f80 -> 36e710 executes unchanged.
  at<float>(frame.data(),7*4)=at<float>(frame.data(),11*4)=at<float>(frame.data(),15*4)=1;
  at<float>(config.data(),0x1c)=2;at<float>(config.data(),0x20)=4;at<float>(config.data(),0x6c)=6.2831853f;
  at<float>(im.base,0x15b2448)=.1f;at<float>(im.base,0x15b2454)=10.f;
  auto a=sailor.p();at<void*>(a,0)=im.base+0x15dfec8;at<void*>(a,0x28)=env.data();at<void*>(a,0x20c)=config.data();at<void*>(a,0x1cc)=registry.data();
  at<unsigned>(a,0x1c8)=0;at<void*>(a,0x214)=nullptr;at<void*>(a,0x21c)=ship.data();at<void*>(a,0x3b8)=registry.data();
  at<void*>(a,0x10)=surface.data();at<void*>(a,0x18)=nullptr;at<void*>(a,0x98)=frame.data();
  station_handle=reinterpret_cast<uintptr_t>(station.data());at<void*>(a,0x600)=&station_handle;at<void*>(station.data(),0x24)=a;
  at<void*>(a,0x24c)=a;at<void*>(a,0x250)=old_navigation_table;at<unsigned char>(a,0x2e8)=0;
  naval127::Location location{{0,0,0},reinterpret_cast<uintptr_t>(surface.data()),reinterpret_cast<uintptr_t>(frame.data())};
  memcpy(static_cast<unsigned char*>(a)+0x524,&location,sizeof location);
  // Real ordinary walk task's native start/tick, with a station destination.
  at<void*>(a,0x2f8)=im.base+0x15dd7ac;at<unsigned>(a,0x2fc)=9;
  unsigned char task[0x41]{};location.xyz[0]=5;
  reinterpret_cast<TaskRequestInit>(im.base+0x350d10)(task,&location,0,0,2.f,0,1);
  memcpy(static_cast<unsigned char*>(a)+0x318,task,sizeof task);
  at<unsigned char>(a,0x308)=1;at<unsigned char>(a,0x385)=1;
  navigation_stats=&nav;nav.result_state=0;
  naval127::control={};naval127::owned=naval127::quitting=naval127::buttons=naval127::camera_pending=0;
  naval127::original_dispatch=reinterpret_cast<naval127::Dispatch>(im.base+0x3efc70);
  status127={false,GetTickCount64()};enabled=1;reject_projection=false;projections=0;
 }
 bool begin(){return naval127::begin(reinterpret_cast<uintptr_t>(env.data()),reinterpret_cast<uintptr_t>(common.data()),reinterpret_cast<uintptr_t>(phase.data()),reinterpret_cast<uintptr_t>(ship.data()));}
 bool tick(){status127.heartbeat=GetTickCount64();return naval127::dispatch(sailor.data.data()+0x2f0,nullptr);}
 void start_path(Image& im){reinterpret_cast<ActorTick>(im.base+0x3e7f00)(sailor.data.data()+0x250);}
 void peer_unchanged(const std::array<unsigned char,0x720>& before){require(peer.data==before,"Production control changed another crew member");}
};
static void production_tests(Image& image){
 {World127 w(image);auto peer=w.peer.data;auto ship=w.ship,registry=w.registry,station=w.station;
  require(w.begin(),"Eligible selected ship did not acquire sailor");
  naval127::sample(FORWARD,true);require(w.tick()&&naval127::active(),"Production ordinary dispatch not held");
  require(projections==0&&naval127::control.requests==1&&at<unsigned char>(w.sailor.p(),0x385)==0,"Movement not submitted and duty not restartable");
  require(at<unsigned>(w.sailor.p(),0x1c8)==0,"Zero native serial rejected");
  w.start_path(image);require(w.nav.submissions==1&&at<float>(w.nav.location.data(),8)==0,"Forward deck destination not submitted");
  require(w.ship==ship&&w.registry==registry&&w.station==station,"Ship, roster or station changed");w.peer_unchanged(peer);
  naval127::release();require(w.tick()&&!naval127::active(),"Return did not call original dispatch");
  require(naval127::control.returns==1&&at<unsigned char>(w.sailor.p(),0x385)==1,"Duty did not restart");
  w.start_path(image);require(w.nav.submissions==2&&at<float>(w.nav.location.data(),0)==5,"Native ordinary walk did not restore station destination");
  require(w.ship==ship&&w.registry==registry&&w.station==station,"Return changed ship/crew/station");w.peer_unchanged(peer);
  passed("production acquire, native deck request and native return to retained station");
 }
 for(float yaw:std::array<float,4>{0.f,1.5707963f,3.1415926f,-1.5707963f}){
 for(unsigned input:std::array<unsigned,10>{FORWARD,BACKWARD,LEFT,RIGHT,FORWARD|RIGHT,FORWARD|LEFT,BACKWARD|RIGHT,BACKWARD|LEFT,FORWARD|RUN,0u}){
  World127 w(image);require(w.begin(),"Directional fixture acquire");naval127::control.yaw=yaw;
  naval127::sample(input,true);require(w.tick(),"Directional dispatch");w.start_path(image);
  require(naval127::prepare_motion(reinterpret_cast<uintptr_t>(w.sailor.p())),"Native motion not prepared");
  const float x=at<float>(w.sailor.p(),0xd8),z=at<float>(w.sailor.p(),0xe0);
  const float forward=x*std::sin(yaw)+z*std::cos(yaw),right=x*std::cos(yaw)-z*std::sin(yaw);
  if(input&FORWARD)require(forward>.1f,"Forward camera-relative sign");if(input&BACKWARD)require(forward<-.1f,"Backward sign");
  if(input&LEFT)require(right<-.1f,"Left sign");if(input&RIGHT)require(right>.1f,"Right sign");
  if(!input)require(x==0&&z==0,"Idle moved sailor");
  else require(std::fabs(std::hypot(x,z)-((input&RUN)?.4f:.2f))<.0001f,"Diagonal pace changed");
  const int turn=at<short>(w.sailor.p(),0xe4);require(std::abs(turn)<=6554,"Native turn rate exceeded");
  if(std::fabs(yaw)>1.f)require(turn!=0,"Facing was left at old bearing");
  require(at<unsigned>(w.sailor.p(),0x244)==((input&RUN)?1u:0u),"Native walking/running variant not set");
  require(at<unsigned char>(w.sailor.p(),0x198)==1,"Native animation-drive flag absent");
 }}
 passed("40 native intent cases: all directions at four camera headings, no diagonal boost, native facing and run variant");
 {World127 w(image);require(w.begin(),"Continuous acquire");w.tick();w.start_path(image);
  for(unsigned n=0;n<200;++n){naval127::control.yaw=n*.02f;naval127::sample(n%2?FORWARD:RIGHT,true);w.tick();
   require(naval127::prepare_motion(reinterpret_cast<uintptr_t>(w.sailor.p())),"Continuous native intent");}
  require(naval127::control.requests==1&&w.nav.submissions==1&&projections==0,"Movement restarted native walking path");
  naval127::sample(0,true);w.tick();naval127::prepare_motion(reinterpret_cast<uintptr_t>(w.sailor.p()));
  require(at<float>(w.sailor.p(),0xd8)==0&&at<float>(w.sailor.p(),0xe0)==0,"Release retained old waypoint movement");
  passed("200 direction/view changes use one native context; releasing WASD clears old movement");}
 {World127 w(image);require(w.begin(),"Integrator acquire");naval127::sample(FORWARD,true);w.tick();w.start_path(image);
  // Exercise unchanged native integration and ship transport in its bounded
  // simple-body branch. Collision broadphase and terrain stay outside this fixture.
  auto actor=w.sailor.p();at<unsigned char>(actor,0xa1)=0;at<unsigned char>(actor,0xa7)=1;
  at<float>(w.frame.data(),0x128)=at<float>(w.frame.data(),0x13c)=at<float>(w.frame.data(),0x150)=1.f;
  at<float>(w.frame.data(),0x134)=.25f;
  require(naval127::prepare_motion(reinterpret_cast<uintptr_t>(actor)),"Integrator intent");
  reinterpret_cast<void(__thiscall*)(void*)>(image.base+0x367600)(actor);
  require(std::fabs(at<float>(actor,0x48)-.25f)<.0001f&&std::fabs(at<float>(actor,0x50)-.2f)<.0001f,"Native ship transport or actor displacement lost");
  require(at<float>(actor,0xd8)==0&&at<float>(actor,0xe0)==0,"Native integrator did not consume movement");
  passed("real native integrator consumes direct intent and retains moving-ship translation (simple-body fixture)");}
 {World127 w(image);require(w.begin(),"Render acquire");w.tick();
  for(unsigned n=0;n<120;++n){rendered_x129=n*.01f;rendered_y129=.05f;rendered_z129=.2f;
   require(naval127::camera(reinterpret_cast<uintptr_t>(w.camera.data())),"Rendered follow");
   require(std::fabs(naval127::control.shown.target[0]-rendered_x129)<.00001f,"Camera used stale simulation position");}
  require(render_updates129==120&&naval127::control.render_poses==120&&at<float>(w.sailor.p(),0x48)==0,"Render tracking modified actor or missed interpolation");
  at<void*>(w.holder.data(),8)=w.peer.p();rendered_x129=40.f;naval127::camera(reinterpret_cast<uintptr_t>(w.camera.data()));
  require(naval127::control.render_poses==120,"Stale holder identity used");
  passed("120 native render-pose copies follow sub-tick motion without simulation changes; stale holder refused");}
 {World127 w(image);require(w.begin(),"Task replacement acquire");naval127::sample(FORWARD,true);w.tick();
  ++at<unsigned>(w.sailor.p(),0x2fc);at<unsigned char>(w.sailor.p(),0x308)=1;w.tick();
  require(!naval127::active()&&naval127::control.returns==0&&at<unsigned char>(w.sailor.p(),0x308)==1,"Replacement task overwritten");passed("replacement duty preempts without restoring old task");}
 {World127 w(image);require(w.begin(),"Priority acquire");naval127::sample(FORWARD,true);w.tick();at<unsigned char>(w.sailor.p(),0x5f8)=1;at<unsigned char>(w.sailor.p(),0x57c)=0;
  w.tick();require(!naval127::active(),"Priority did not cancel player lease");passed("native priority interruption cancels control");}
 {World127 w(image);require(w.begin(),"Focus acquire");w.tick();naval127::sample(0,false);w.tick();require(!naval127::active()&&naval127::control.returns==1,"Focus release did not return sailor");passed("focus loss returns current duty");}
 {World127 w(image);auto saved=naval127::read_pose(reinterpret_cast<uintptr_t>(w.camera.data()));require(w.begin(),"Camera acquire");
  require(naval127::camera(reinterpret_cast<uintptr_t>(w.camera.data())),"Camera not attached");naval127::release();w.tick();
  require(naval127::camera(reinterpret_cast<uintptr_t>(w.camera.data())),"Camera not restored");auto restored=naval127::read_pose(reinterpret_cast<uintptr_t>(w.camera.data()));
  require(!memcmp(&saved,&restored,sizeof saved)&&!naval127::camera_pending,"RTS numeric camera not restored");passed("owned camera restores original eye target and up");}
 {World127 w(image);at<unsigned char>(w.army.data(),0x270)=0;require(!w.begin(),"Enemy ship accepted");at<unsigned char>(w.army.data(),0x270)=1;at<unsigned>(w.env.data(),0x28248)=1;require(!w.begin(),"Network battle accepted");passed("enemy and network worlds refused");}
 {World127 w(image);require(w.begin(),"Stale world acquire");w.tick();at<void*>(w.root.data(),0x110)=nullptr;
  naval127::camera(reinterpret_cast<uintptr_t>(w.camera.data()));require(!naval127::active()&&!naval127::camera_pending,"Stale world retained");passed("battle retirement drops ownership without stale task writes");}
 {World127 w(image);require(w.begin(),"Death acquire");w.tick();at<unsigned char>(w.sailor.p(),0xf0)=0;
  naval127::camera(reinterpret_cast<uintptr_t>(w.camera.data()));require(!naval127::active()&&naval127::control.returns==0,"Dead sailor duty restarted");passed("death releases camera without restoring dead sailor duty");}
 {World127 w(image);require(w.begin(),"Traversal acquire");w.tick();at<void*>(w.sailor.p(),0x18)=w.station.data();w.tick();require(!naval127::active(),"Traversal retained control");passed("native traversal cancels control");}
 {World127 w(image);require(w.begin(),"Station acquire");w.tick();at<void*>(w.station.data(),0x24)=w.peer.p();w.tick();require(!naval127::active()&&at<void*>(w.station.data(),0x24)==w.peer.p(),"Station owner overwritten");passed("station reassignment releases control without stealing ownership");}
 {World127 w(image);require(w.begin(),"Peer progression acquire");w.tick();
  auto peer_context=w.peer.data.data()+0x2f0;
  require(naval127::dispatch(peer_context,nullptr)&&naval127::dispatch(peer_context,nullptr),"Peer duty not dispatched");
  require(w.peer.duty.starts==1&&w.peer.duty.ticks==2&&naval127::active(),"Peer progression interrupted or possession stolen");
  passed("other crew continue through original native dispatcher during possession");}
 {World127 w(image);require(w.begin(),"Paused return acquire");w.tick();naval127::release();
  require(naval127::active()&&!naval127::input_active(),"Paused release retained physical controls");
  require(!naval_window127(nullptr,WM_LBUTTONDOWN,0,0),"Paused release swallowed RTS click");
  require(naval127::camera(reinterpret_cast<uintptr_t>(w.camera.data()))&&!naval127::camera_pending,"Paused release failed camera restoration");
  w.tick();require(!naval127::active()&&naval127::control.returns==1,"Resume did not restore duty");
  passed("release restores controls and camera before paused simulation resumes");}
 {World127 w(image);auto ship=w.ship,station=w.station,registry=w.registry;
  for(unsigned n=0;n<100;++n){require(w.begin(),"Same sailor reacquisition");naval127::sample(FORWARD,true);w.tick();w.start_path(image);naval127::release();w.tick();w.start_path(image);naval127::camera(reinterpret_cast<uintptr_t>(w.camera.data()));
   require(!naval127::active()&&!naval127::camera_pending&&at<float>(w.nav.location.data(),0)==5,"Repeated native duty destination lost");}
  require(w.ship==ship&&w.station==station&&w.registry==registry,"Repeated transfer changed ship station or roster");
  passed("100 repeated transfers of the same sailor preserve duty destination and membership");}
 for(unsigned n=0;n<100;++n){World127 w(image);require(w.begin(),"Repeat acquire");naval127::sample(FORWARD,true);w.tick();naval127::release();w.tick();require(naval127::control.returns==1&&!naval127::active(),"Repeat release");}
 passed("100 production acquire and release cycles");
}
#ifndef NAVAL_CAMERA_FIXTURE128
int wmain(int argc,wchar_t**argv){try{require(argc==2,"Expected owned engine");Image image(argv[1]);production_tests(image);std::cout<<"PASS "<<cases<<" production naval cases; engine="<<image.digest<<"\nSynthetic world, stations and projection; real native task dispatch and request handling. No live gameplay proof.\n";return 0;}catch(const std::exception&e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}

#endif
