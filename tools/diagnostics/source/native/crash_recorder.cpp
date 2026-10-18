// External x86 exception recorder. Game exceptions retain normal SEH handling.
#include "identity.h"
#include <dbghelp.h>
#ifdef SHOGUN2_EXPERIMENTAL_TRACE_FIXTURES
#include "targeted_trace110.h"
#include "fixture_identity111.h"
using SessionTrace=TargetedTrace110;
#else
#include "capture_only111.h"
using SessionTrace=CaptureOnly111;
#endif
static std::string utf8(const std::wstring&w){if(w.empty())return {};int n=WideCharToMultiByte(CP_UTF8,0,w.data(),int(w.size()),nullptr,0,nullptr,nullptr);std::string s(n,' ');WideCharToMultiByte(CP_UTF8,0,w.data(),int(w.size()),s.data(),n,nullptr,nullptr);return s;}
static std::string quote(const std::string&s){std::ostringstream o;o<<'"';for(unsigned char c:s){if(c=='"'||c=='\\')o<<'\\'<<c;else if(c<32)o<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<unsigned(c);else o<<c;}o<<'"';return o.str();}
static std::wstring file_name(HANDLE h){wchar_t b[32768];DWORD n=h?GetFinalPathNameByHandleW(h,b,32768,0):0;return n&&n<32768?std::wstring(b,n):L"";}
#include "crash_capture112.h"
static ModuleCache112 module_cache112;
static std::string location(DWORD pid,uintptr_t address){auto cached=module_cache112.locate(address);if(cached!=hex(address))return cached;Handle s(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid));MODULEENTRY32W m{};m.dwSize=sizeof(m);if(s.h!=INVALID_HANDLE_VALUE&&Module32FirstW(s.h,&m)){do{auto base=reinterpret_cast<uintptr_t>(m.modBaseAddr);if(address>=base&&address<base+m.modBaseSize)return utf8(m.szModule)+"+"+hex(address-base);}while(Module32NextW(s.h,&m));}return hex(address);}
static DWORD watch_pid;static HWND watch_window;
static BOOL CALLBACK window_for_pid(HWND w,LPARAM){DWORD pid{};GetWindowThreadProcessId(w,&pid);if(pid==watch_pid&&IsWindowVisible(w)&&!GetWindow(w,GW_OWNER)){watch_window=w;return FALSE;}return TRUE;}
static bool exists(const std::wstring&p){return GetFileAttributesW(p.c_str())!=INVALID_FILE_ATTRIBUTES;}
int wmain(int argc,wchar_t**argv){DWORD pid=0;bool attached=false;std::unique_ptr<SessionTrace> trace;try{
 require(argc==5,"Use --watch-auto PID seconds directory or --fixture PID seconds directory; seconds=0 until exit");pid=DWORD(_wtoi(argv[2]));require(pid!=0,"PID required");bool fixture=std::wstring(argv[1])==L"--fixture";require(fixture||std::wstring(argv[1])==L"--watch-auto","Invalid mode");if(!fixture)require(find_process(pid)==pid,"Game identity differs");
 std::wstring folder=argv[4],stop=folder+L"\\crash.stop",request=folder+L"\\snapshot.request";require(exists(folder)&&!exists(stop),"Directory unavailable or stop requested");
 Handle process(OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|SYNCHRONIZE,FALSE,pid));require(process.h!=nullptr,"Process open failed");
#ifdef SHOGUN2_EXPERIMENTAL_TRACE_FIXTURES
 require(fixture,"Experimental debugger refuses real-game mode");require_trace_fixture111(process.h,folder);
#endif
 require(DebugActiveProcess(pid)!=FALSE,"Debugger attachment failed");attached=true;require(DebugSetProcessKillOnExit(FALSE)!=FALSE,"Non-terminating detach failed");SetConsoleCtrlHandler(stop_handler,TRUE);
 std::ofstream log(folder+L"\\exceptions-"+std::to_wstring(pid)+L".jsonl",std::ios::app);require(bool(log),"Exception log unavailable");
 unsigned exceptions=0,first_dumps=0;bool initial_break=false,hang_dumped=false;uint64_t started=GetTickCount64(),seconds=_wtoi(argv[3]),end=seconds?started+seconds*1000:UINT64_MAX,next_status=0,hung_since=0;
 trace=std::make_unique<SessionTrace>(process.h,pid,folder,fixture);trace->fixture_init();
 watch_pid=pid;log<<"{\"event\":\"attached\",\"tick_ms\":"<<started<<",\"pid\":"<<pid<<",\"fixture\":"<<fixture<<"}\n";log.flush();
 while(GetTickCount64()<end&&!stopped&&!exists(stop)){
  uint64_t now=GetTickCount64();trace->heartbeat();if(now>=next_status){
   if(!watch_window||!IsWindow(watch_window)){watch_window=nullptr;EnumWindows(window_for_pid,0);}bool hung=watch_window&&IsHungAppWindow(watch_window);if(hung&&!hung_since)hung_since=now;if(!hung)hung_since=0;
   bool requested=exists(request);if(requested||(!hang_dumped&&hung_since&&now-hung_since>=30000)){
    std::wstring path=folder+L"\\hang-"+std::to_wstring(pid)+L"-"+std::to_wstring(now)+L".dmp";
    try{auto written=dump112(process.h,pid,0,nullptr,path,false,log);log<<"{\"event\":\"hang_dump\",\"tick_ms\":"<<now<<",\"path\":"<<quote(utf8(written))<<"}\n";}
    catch(const std::exception&e){log<<"{\"event\":\"dump_error\",\"tick_ms\":"<<now<<",\"error\":"<<quote(e.what())<<"}\n";}hang_dumped=true;if(requested)DeleteFileW(request.c_str());
   }
   log<<"{\"event\":\"heartbeat\",\"tick_ms\":"<<now<<",\"pid\":"<<pid<<",\"window_hung\":"<<hung<<"}\n";log.flush();next_status=now+1000;
  }
  DEBUG_EVENT e{};if(!WaitForDebugEvent(&e,100)){DWORD error=GetLastError();if(error==ERROR_SEM_TIMEOUT)continue;log<<"{\"event\":\"debug_wait_error\",\"winerror\":"<<error<<"}\n";break;}
  DWORD disposition=DBG_CONTINUE;now=GetTickCount64();
  if(e.dwDebugEventCode==CREATE_PROCESS_DEBUG_EVENT){module_cache112.add(reinterpret_cast<uintptr_t>(e.u.CreateProcessInfo.lpBaseOfImage),e.u.CreateProcessInfo.hFile,file_name(e.u.CreateProcessInfo.hFile));log<<"{\"event\":\"process_created\",\"tick_ms\":"<<now<<",\"base\":"<<quote(hex(reinterpret_cast<uintptr_t>(e.u.CreateProcessInfo.lpBaseOfImage)))<<",\"path\":"<<quote(utf8(file_name(e.u.CreateProcessInfo.hFile)))<<"}\n";if(e.u.CreateProcessInfo.hFile)CloseHandle(e.u.CreateProcessInfo.hFile);trace->new_thread(e.dwThreadId);/* Debug API owns process/thread event handles until exit; do not close early. */}
  else if(e.dwDebugEventCode==CREATE_THREAD_DEBUG_EVENT){log<<"{\"event\":\"thread_created\",\"tick_ms\":"<<now<<",\"thread\":"<<e.dwThreadId<<",\"start\":"<<quote(hex(reinterpret_cast<uintptr_t>(e.u.CreateThread.lpStartAddress)))<<"}\n";trace->new_thread(e.dwThreadId);/* ContinueDebugEvent closes this event handle on thread exit. */}
  else if(e.dwDebugEventCode==EXIT_THREAD_DEBUG_EVENT){trace->exit_thread(e.dwThreadId);log<<"{\"event\":\"thread_exited\",\"tick_ms\":"<<now<<",\"thread\":"<<e.dwThreadId<<",\"exit_code\":"<<e.u.ExitThread.dwExitCode<<"}\n";}
  else if(e.dwDebugEventCode==LOAD_DLL_DEBUG_EVENT){module_cache112.add(reinterpret_cast<uintptr_t>(e.u.LoadDll.lpBaseOfDll),e.u.LoadDll.hFile,file_name(e.u.LoadDll.hFile));log<<"{\"event\":\"module_loaded\",\"tick_ms\":"<<now<<",\"base\":"<<quote(hex(reinterpret_cast<uintptr_t>(e.u.LoadDll.lpBaseOfDll)))<<",\"path\":"<<quote(utf8(file_name(e.u.LoadDll.hFile)))<<"}\n";trace->module(reinterpret_cast<uintptr_t>(e.u.LoadDll.lpBaseOfDll),file_name(e.u.LoadDll.hFile));if(e.u.LoadDll.hFile)CloseHandle(e.u.LoadDll.hFile);}
  else if(e.dwDebugEventCode==UNLOAD_DLL_DEBUG_EVENT){module_cache112.loaded.erase(reinterpret_cast<uintptr_t>(e.u.UnloadDll.lpBaseOfDll));trace->unload(reinterpret_cast<uintptr_t>(e.u.UnloadDll.lpBaseOfDll));log<<"{\"event\":\"module_unloaded\",\"tick_ms\":"<<now<<",\"base\":"<<quote(hex(reinterpret_cast<uintptr_t>(e.u.UnloadDll.lpBaseOfDll)))<<"}\n";}
  else if(e.dwDebugEventCode==EXCEPTION_DEBUG_EVENT){auto&x=e.u.Exception;auto&r=x.ExceptionRecord;DWORD code=r.ExceptionCode;auto address=reinterpret_cast<uintptr_t>(r.ExceptionAddress);disposition=DBG_EXCEPTION_NOT_HANDLED;
   if(trace->exception(e.dwThreadId,code)){disposition=DBG_CONTINUE;}
   else if(code==EXCEPTION_BREAKPOINT&&!initial_break){initial_break=true;disposition=DBG_CONTINUE;}
   else if(code==0x406d1388&&r.NumberParameters==4&&r.ExceptionInformation[0]==0x1000&&r.ExceptionInformation[3]==0){disposition=DBG_CONTINUE;log<<"{\"event\":\"thread_name_notification\",\"tick_ms\":"<<now<<",\"thread\":"<<e.dwThreadId<<"}\n";}
   else{
    ++exceptions;log<<"{\"event\":\"exception\",\"tick_ms\":"<<now<<",\"pid\":"<<pid<<",\"thread\":"<<e.dwThreadId<<",\"first_chance\":"<<x.dwFirstChance<<",\"code\":"<<quote(hex(code))<<",\"address\":"<<quote(hex(address))<<",\"module_offset\":"<<quote(location(pid,address))<<",\"parameters\":[";
    for(DWORD i=0;i<std::min<DWORD>(r.NumberParameters,15);++i){if(i)log<<',';log<<quote(hex(r.ExceptionInformation[i]));}log<<']';
    if((code==EXCEPTION_ACCESS_VIOLATION||code==EXCEPTION_IN_PAGE_ERROR)&&r.NumberParameters>=2)log<<",\"access\":"<<quote(r.ExceptionInformation[0]==0?"read":r.ExceptionInformation[0]==1?"write":r.ExceptionInformation[0]==8?"execute":"unknown")<<",\"fault_address\":"<<quote(hex(r.ExceptionInformation[1]));
    log<<"}\n";log.flush();bool fatal=code==EXCEPTION_ACCESS_VIOLATION||code==EXCEPTION_STACK_OVERFLOW||code==EXCEPTION_INT_DIVIDE_BY_ZERO||code==0xc0000409||code==EXCEPTION_IN_PAGE_ERROR;
    // Capture the first severe fault fully: custom game crash handlers may exit before second chance.
    bool full=!x.dwFirstChance||(fatal&&first_dumps==0);if(!x.dwFirstChance||(fatal&&first_dumps<2)){
     if(x.dwFirstChance)++first_dumps;
     std::wstring path=folder+L"\\"+(!x.dwFirstChance?L"unhandled-":L"first-chance-")+std::to_wstring(pid)+L"-"+std::to_wstring(exceptions)+L".dmp";
     try{bool saved=fault_snapshot112(process.h,pid,e.dwThreadId,r,path+L".snapshot.json",module_cache112);log<<"{\"event\":\"fault_snapshot112\",\"tick_ms\":"<<GetTickCount64()<<",\"success\":"<<(saved?"true":"false")<<",\"path\":"<<quote(utf8(path+L".snapshot.json"))<<"}\n";log.flush();}catch(const std::exception&failure){log<<"{\"event\":\"snapshot_error112\",\"error\":"<<quote(failure.what())<<"}\n";log.flush();}
     try{dump112(process.h,pid,e.dwThreadId,&r,path,full,log);}
     catch(const std::exception&failure){log<<"{\"event\":\"dump_error\",\"tick_ms\":"<<GetTickCount64()<<",\"error\":"<<quote(failure.what())<<"}\n";}
    }
   }
  }else if(e.dwDebugEventCode==EXIT_PROCESS_DEBUG_EVENT){log<<"{\"event\":\"process_exited\",\"tick_ms\":"<<now<<",\"exit_code\":"<<e.u.ExitProcess.dwExitCode<<"}\n";log.flush();trace->finish();ContinueDebugEvent(e.dwProcessId,e.dwThreadId,disposition);attached=false;break;}
  log.flush();if(!ContinueDebugEvent(e.dwProcessId,e.dwThreadId,disposition)){log<<"{\"event\":\"continue_error\",\"winerror\":"<<GetLastError()<<"}\n";break;}
 }
 if(attached){trace->restore();require(DebugActiveProcessStop(pid)!=FALSE,"Debugger detach failed");attached=false;trace->finish();}log<<"{\"event\":\"detached\",\"tick_ms\":"<<GetTickCount64()<<",\"requested_stop\":"<<exists(stop)<<"}\n";log.flush();return 0;
 }catch(const std::exception&e){if(attached){try{if(trace)trace->restore();}catch(...){}DebugActiveProcessStop(pid);}std::cerr<<e.what()<<std::endl;return 1;}}
