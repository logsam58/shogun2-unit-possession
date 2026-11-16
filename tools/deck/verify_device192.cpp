// Production query/IPC and layout policies. Fake Steam is explicitly provided
// by this fixture; no game's Steam client or running process is examined.
#include "../../native/deck/device192.h"
#include "../../native/deck/channel181.h"
#include "../../native/outputs/shogun2-possession-cleanup-v46/src/hud_layout108.h"
#include <cassert>
#include "../../native/deck/steam_input174.h"
#include <iostream>
#include <string>
#include "../../native/deck/remote_display130.h"
static LRESULT CALLBACK fixture_window(HWND window,UINT msg,WPARAM wparam,LPARAM lparam){
 return DefWindowProcW(window,msg,wparam,lparam);
}
struct DisplayFixture {
 remote131::Mode mode{};
 HWND window{};
 remote131::Size request{};
 unsigned commits{};
 bool restore_bounds{}, restore_render_size{}, refuse_target{};
};
static void __fastcall set_windowed(DisplayFixture* f,void*,bool windowed){
 f->mode.windowed=windowed;
}
static void __fastcall request_size(remote131::Mode* mode,void*,const remote131::Size* size){
 auto* f=reinterpret_cast<DisplayFixture*>(mode);
 f->request=*size;
 mode->pending=1;
}
static void __fastcall commit_display(DisplayFixture* f,void*){
 ++f->commits;
 if(f->refuse_target&&f->request.width==remote131::deck_width)return;
 if(f->commits==1&&f->restore_bounds)
  assert(SetWindowPos(f->window,nullptr,30,30,960,640,SWP_NOACTIVATE|SWP_NOZORDER));
 if(f->commits==1&&f->restore_render_size){
  RECT client{};assert(GetClientRect(f->window,&client));f->request={client.right,client.bottom};
 }
 f->mode.width=f->request.width;
 f->mode.height=f->request.height;
 f->mode.pending=0;
}
static void verify_display132(){
 using namespace remote131;
 HINSTANCE app=GetModuleHandleW(nullptr);
 WNDCLASSW cls{};cls.lpfnWndProc=fixture_window;cls.hInstance=app;cls.lpszClassName=L"S2UP display fixture";
 assert(RegisterClassW(&cls));
 const DWORD ex=WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE;
 for(unsigned scenario=0;scenario<4;++scenario){
  HWND window=CreateWindowExW(ex,cls.lpszClassName,L"Offscreen display fixture",WS_OVERLAPPEDWINDOW,
                               30,30,960,640,nullptr,nullptr,app,nullptr);
  assert(window&&SetLayeredWindowAttributes(window,0,0,LWA_ALPHA));
  ShowWindow(window,SW_SHOWNOACTIVATE);
  RECT before{},after{},client{};assert(GetWindowRect(window,&before)&&GetClientRect(window,&client));
  DisplayFixture f{};f.window=window;f.mode.width=client.right;f.mode.height=client.bottom;
  f.restore_bounds=scenario==1||scenario==2;
  f.restore_render_size=scenario==2;
  f.refuse_target=scenario==3;
  const auto original=f.mode;
  Engine engine{reinterpret_cast<uintptr_t>(&f),reinterpret_cast<uintptr_t>(&f.mode),
   reinterpret_cast<uintptr_t>(&set_windowed),reinterpret_cast<uintptr_t>(&request_size),
   reinterpret_cast<uintptr_t>(&commit_display)};
  assert(apply_display(window,engine,original)==!f.refuse_target);
  if(f.refuse_target){
   assert(f.commits==2&&!f.mode.windowed&&f.mode.width==original.width&&f.mode.height==original.height);
   assert(GetWindowRect(window,&after)&&EqualRect(&before,&after));
  }else{
   assert(f.commits==(f.restore_render_size?2u:1u));
   assert(f.mode.windowed&&!f.mode.pending&&f.mode.width==deck_width&&f.mode.height==deck_height);
   assert(client_is(window,{deck_width,deck_height}));
  }
  assert(DestroyWindow(window));
 }
 assert(UnregisterClassW(cls.lpszClassName,app));
 std::cout<<"PASS remote display: normal commit, bounds restored during release, size resampled, and exact failure rollback\n";
}

int wmain(int argc,wchar_t** argv){
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);_set_error_mode(_OUT_TO_STDERR);
 if(argc==4&&!wcscmp(argv[1],L"--reader")){device192::Reader reader;assert(reader.deck(DWORD(wcstoul(argv[2],nullptr,10)))==(argv[3][0]==L'1'));return 0;}
 assert(argc==3);unsigned cases=0;
 HMODULE fake=LoadLibraryW(argv[1]);assert(fake);
 auto set=reinterpret_cast<void(__cdecl*)(int)>(GetProcAddress(fake,"FixtureMode192"));
 auto calls=reinterpret_cast<unsigned(__cdecl*)()>(GetProcAddress(fake,"FixtureCalls192"));assert(set&&calls);
 using device192::Mode;using device192::Reason;
 if(argv[2][0]==L'2'){
  auto remote=reinterpret_cast<void(__cdecl*)(uint32_t,uint32_t,bool,int)>(GetProcAddress(fake,"FixtureRemote130"));
  assert(remote);set(0);
  remote(17,17,true,14);assert(device192::query(fake).mode==Mode::Desktop);
  remote(17,17,false,3);assert(device192::query(fake).mode==Mode::Desktop);
  remote(17,22,false,14);assert(device192::query(fake).mode==Mode::Desktop);
  remote(17,0,false,14);assert(device192::query(fake).mode==Mode::Desktop);
  remote(17,17,false,14);
  assert(device192::select()&&device192::enabled()&&device192::streaming());
  deck174::SteamBackend backend;
  assert(backend.open_existing("fixture.vdf",true));
  deck174::Frame frame;
  assert(backend.sample(1000000,true,frame)&&frame.native_bindings);
  const auto epoch=frame.epoch;
  remote(0,17,false,14);
  assert(!backend.sample(1250000,true,frame)&&!frame.connected);
  remote(19,19,false,14);
  assert(backend.sample(1500000,true,frame)&&frame.epoch>epoch);
  remote(19,19,true,14);
  assert(!backend.sample(1750000,true,frame)&&!frame.connected);
  assert(backend.close());
  verify_display132();
  std::cout<<"PASS remote130: direct Deck discovery, wrong/local/guest/other inputs rejected, native bindings, disconnect, reconnect\n";
  return 0;
 }

 assert(device192::query(nullptr).mode==Mode::Desktop);++cases;
 assert(device192::query(GetModuleHandleW(L"kernel32.dll")).reason==Reason::NoExports);++cases;
 for(int mode=0;mode<=5;++mode){set(mode);auto decision=device192::query(fake);assert(decision.mode==(mode==1?Mode::Deck:Mode::Desktop));assert(calls()==unsigned(mode==0||mode==1||mode==5));++cases;}
 const bool deck=argv[2][0]==L'1';set(deck?1:0);assert(device192::select());assert(calls()==1);++cases;
 set(deck?0:1);assert(device192::select());assert(calls()==0);assert(device192::enabled()==deck);++cases;
 device192::Publisher duplicate;assert(!duplicate.publish({Mode::Deck,Reason::VerifiedDeck}));++cases;
 device192::Reader reader;assert(reader.deck(GetCurrentProcessId())==deck&&!reader.deck(0));++cases;
 wchar_t exe[32768]{};assert(GetModuleFileNameW(nullptr,exe,32768));
 std::wstring cmd=std::wstring(L"\"")+exe+L"\" --reader "+std::to_wstring(GetCurrentProcessId())+(deck?L" 1":L" 0");
 STARTUPINFOW startup{sizeof startup};PROCESS_INFORMATION child{};assert(CreateProcessW(exe,cmd.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&child));assert(WaitForSingleObject(child.hProcess,10000)==WAIT_OBJECT_0);DWORD code=1;assert(GetExitCodeProcess(child.hProcess,&code)&&code==0);CloseHandle(child.hThread);CloseHandle(child.hProcess);++cases;
 device192::Record record{device192::magic,192,sizeof(device192::Record),GetCurrentProcessId(),device192::stamp(GetCurrentProcess()),unsigned(Mode::Deck),unsigned(Reason::VerifiedDeck)};
 assert(device192::valid(record,record.pid,record.created));assert(!device192::valid(record,record.pid,record.created+1));record.reason=unsigned(Reason::NoSteam);assert(!device192::valid(record,record.pid,record.created));record.reason=unsigned(Reason::VerifiedDeck);record.mode=7;assert(!device192::valid(record,record.pid,record.created));++cases;
 deck181::Channel channel;assert(channel.open(GetCurrentProcessId(),true));deck181::Sample sample;sample.context=unsigned(deck174::Context::Possession);sample.focused=sample.lease=1;sample.tick=GetTickCount64();sample.keys['W'/32]|=1u<<('W'%32);assert(channel.publish(sample));deck181::Sample read;assert(deck181::read_sample(GetCurrentProcessId(),read)==deck);++cases;
 for(auto wh:{std::pair<float,float>{1280,800},{1920,1080},{2560,1440},{3840,2160}}){
  float base=std::min(wh.first/1920.f,wh.second/1080.f);if(wh.first>=1280&&wh.second>=720)base=std::max(.85f,base);base*=.85f;
  hudlayout108::deck_ui192=device192::enabled();assert(std::fabs(hudlayout108::scale(wh.first,wh.second)-base*(deck?1.15f:1.f))<.00001f);assert(std::fabs(hudlayout108::qte_anchor_y(wh.second)-wh.second*(deck?.25f:.19f))<.001f);++cases;
 }
 std::cout<<"PASS device192: "<<cases<<" production query/profile/layout checks; "<<(deck?"Deck":"desktop")<<"; child HUD record read; fake Steam only\n";return 0;
}
