// Full production window -> shared bow edge sampler, and window Z -> native
// position resolver/formation/guard serializers. Physical key state and native
// vector/scene primitives are explicit providers from NativeFixture100.
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static SHORT physical143(int key){return key==VK_RBUTTON?SHORT(0x8000):0;}
#define GetAsyncKeyState physical143
#define main retained_input143_main
#include "verify_native_commands100.cpp"
#undef main
#undef GetAsyncKeyState
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;state->heartbeat=GetTickCount64();
 HWND window=CreateWindowExW(0,L"STATIC",L"Input143",WS_POPUP,0,0,1280,720,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
 require(window!=nullptr,"Hidden fixture window");input_window=window;equipment_slot81=equipment81::Ranged;
 state->actor_id=field<unsigned>(possessed,0x1c8);bow_reset142();bow_family142=1;
 weapon_window_proc(window,WM_LBUTTONDOWN,0,0);require(!state->fire_request,"Window DOWN bypassed bow hold/release");
 bow_sample142(true,true);require(!state->fire_request,"Worker duplicated window DOWN");
 weapon_window_proc(window,WM_LBUTTONUP,0,0);require(state->fire_request,"Quick window-only bow tap was lost");
 auto release=bow_release142;bow_sample142(false,true);require(bow_release142.serial==release.serial,"Worker duplicated window UP");
 std::vector<unsigned char> record_bytes(0x180),db_bytes(0x20),clip_bytes(8),asset_bytes(0x100);std::vector<uintptr_t> clips(0x4a3);
 auto record=P::ptr(record_bytes),db=P::ptr(db_bytes),clip=P::ptr(clip_bytes),asset=P::ptr(asset_bytes);
 const wchar_t name[]=L"shogun_arrow";field<unsigned>(record,4)=unsigned(wcslen(name));field<uintptr_t>(record,0xc)=reinterpret_cast<uintptr_t>(name);field<unsigned>(record,8)=32;field<unsigned>(record,0x60)=150;field<float>(record,0x6c)=50;
 field<uintptr_t>(possessed,0x23c)=db;field<uintptr_t>(db,0xc)=reinterpret_cast<uintptr_t>(clips.data());field<uintptr_t>(clip,4)=asset;
 field<unsigned>(asset,0x64)=51;field<float>(base,0x15b2448)=.05f;clips[126]=clips[758]=clip;
 require(bow_bind142(record,false)&&bow_speed142(record,50)<19,"Native raise without release marker lost weak power");
 require(!bow_bind142(record,false)&&!bow_ticket142.record,"Unbound shot silently became full power");
 auto k=bow_key142();bow_release142={k,100,2.5};require(bow_bind142(record,false)&&bow_speed142(record,50)==50,"Full draw lost native ceiling");
 bow_release142={k,101,1.25};require(bow_bind142(record,false)&&std::abs(bow_speed142(record,50)-33.75f)<.001,"Intermediate power not bound");
 field<unsigned>(asset,0x64)=0;bow_release142={k,102,.1};require(!bow_bind142(record,false),"Invalid draw metadata silently became full power");field<unsigned>(asset,0x64)=51;
 state->fire_request=0;bow_family142=0;weapon_window_proc(window,WM_LBUTTONDOWN,0,0);require(state->fire_request,"Non-bow immediate firing changed");state->fire_request=0;
 bow_family142=-1;weapon_window_proc(window,WM_LBUTTONDOWN,0,0);require(!state->fire_request,"Unknown family gained fire permission");
 for(float height:{0.f,1.6f,4.f,30.f})for(bool run:{false,true}){
  f.clear_queue();command_key_held100=0;command_input_reset100();state->follow=0;
  command100::Frame frame{{10,height,20},.7f,GetTickCount64(),possession_lease.generation,possession_lease.identity.actor_id,possession_lease.identity.unit_id};
  command_publish_frame100(frame);require(command_window100(window,WM_KEYDOWN,'Z',0),"Window Z not captured");
  if(run){command100::Event event{};require(command_pop100(event),"Missing Z event");event.running102=true;command_events100[command_write100++%32]=event;}
  process_orders(P::ptr(f.camera));
  require(command_feedback100==command100::Feedback::Holding&&packets100.size()==2,"Z failed above native terrain");
  require(packets100[0].point.position[0]==10&&packets100[0].point.position[2]==20&&packets100[0].shorts[0]==command100::bearing(.7f)&&packets100[0].bytes[0]==unsigned(run)&&packets100[1].bytes[0]==1,"Z changed captured anchor/facing/gait/guard");
 }
 input_window=nullptr;DestroyWindow(window);bow_reset142();
 std::cout<<"PASS: actual window and duplicate worker bow edges; quick/full/intermediate positive shot binding, no-marker native raise metadata, invalid/unknown refusal, immediate non-bows; production Z native projection at four heights and both gaits.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
