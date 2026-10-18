#define wmain recorder_main112
#include "crash_recorder.cpp"
#undef wmain
static unsigned injected_mode112{},calls112{};
static BOOL WINAPI injected112(HANDLE process,DWORD pid,HANDLE file,MINIDUMP_TYPE type,PMINIDUMP_EXCEPTION_INFORMATION info,PMINIDUMP_USER_STREAM_INFORMATION streams,PMINIDUMP_CALLBACK_INFORMATION callback){
 ++calls112;
 if((injected_mode112==1&&calls112==1)||(injected_mode112==2&&calls112<=2)||injected_mode112==3){SetLastError(HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY));return FALSE;}
 if(injected_mode112==4){SetLastError(ERROR_ACCESS_DENIED);return FALSE;}
 return MiniDumpWriteDump(process,pid,file,type,info,streams,callback);
}
int wmain(int argc,wchar_t**argv){try{
 require(argc==3,"Output directory and generic fixture executable required");std::wstring folder=argv[1];
 STARTUPINFOW startup{};startup.cb=sizeof startup;PROCESS_INFORMATION child{};
 std::wstring command=L"\""+std::wstring(argv[2])+L"\" normal";
 require(CreateProcessW(argv[2],command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&child)!=FALSE,"Toy creation failed");Handle process(child.hProcess),thread(child.hThread);
 std::ofstream log(folder+L"\\attempts.jsonl");unsigned tests=0;
 for(unsigned mode=0;mode<=4;++mode){
  injected_mode112=mode;calls112=0;bool threw=false;std::wstring path=folder+L"\\mode-"+std::to_wstring(mode)+L".dmp",written;
  try{written=dump112(process.h,child.dwProcessId,0,nullptr,path,true,log,injected112);}catch(const std::exception&){threw=true;}
  require(threw==(mode>=3),"Unexpected fallback outcome");
  require(calls112==(mode==0?1:mode==1?2:mode==4?1:3),"Unexpected retry count");
  if(mode==1)require(written.find(L"-readable.dmp")!=std::wstring::npos,"Readable fallback path missing");
  if(mode==2)require(written.find(L"-minimal.dmp")!=std::wstring::npos,"Minimal fallback path missing");++tests;
 }
 ModuleCache112 cache;cache.loaded[0x100000]={0x100000,0x1000,L"toy.dll"};
 require(cache.locate(0x100123)=="toy.dll+0x123","Cached module lookup failed");
 EXCEPTION_RECORD record{};record.ExceptionCode=EXCEPTION_ACCESS_VIOLATION;record.ExceptionAddress=reinterpret_cast<void*>(0x100123);record.NumberParameters=2;record.ExceptionInformation[1]=0x14;
 auto path=folder+L"\\independent.snapshot.json";
 require(fault_snapshot112(process.h,child.dwProcessId,0,record,path,cache),"Independent fallback snapshot missing");
 require(!fault_snapshot112(process.h,child.dwProcessId,0,record,path,cache),"Snapshot overwritten");tests+=3;
 WaitForSingleObject(process.h,5000);DWORD exit=0;GetExitCodeProcess(process.h,&exit);require(exit==0,"Toy did not survive dump tests");
 std::cout<<"PASS "<<tests<<" dump/cache/snapshot cases\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
