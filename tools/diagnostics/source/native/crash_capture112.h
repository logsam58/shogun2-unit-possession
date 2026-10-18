#pragma once
#include <map>

struct ModuleCache112 {
 struct Module {uintptr_t base{};DWORD size{};std::wstring path;};
 std::map<uintptr_t,Module> loaded;
 void add(uintptr_t base,HANDLE file,const std::wstring&path){
  if(loaded.size()>=2048&&!loaded.count(base))return;
  DWORD size=0,got=0;IMAGE_DOS_HEADER dos{};IMAGE_NT_HEADERS32 nt{};LARGE_INTEGER at{};
  if(file&&file!=INVALID_HANDLE_VALUE&&SetFilePointerEx(file,at,nullptr,FILE_BEGIN)&&ReadFile(file,&dos,sizeof dos,&got,nullptr)&&got==sizeof dos&&dos.e_magic==IMAGE_DOS_SIGNATURE&&dos.e_lfanew>0&&dos.e_lfanew<1024*1024){
   at.QuadPart=dos.e_lfanew;
   if(SetFilePointerEx(file,at,nullptr,FILE_BEGIN)&&ReadFile(file,&nt,sizeof nt,&got,nullptr)&&got==sizeof nt&&nt.Signature==IMAGE_NT_SIGNATURE&&nt.OptionalHeader.Magic==IMAGE_NT_OPTIONAL_HDR32_MAGIC)size=nt.OptionalHeader.SizeOfImage;
  }
  loaded[base]={base,size,path};
 }
 std::string locate(uintptr_t address)const{
  for(const auto& entry:loaded){const auto& m=entry.second;
   if(address>=m.base&&address-m.base<m.size){auto slash=m.path.find_last_of(L"\\/");return utf8(m.path.substr(slash==std::wstring::npos?0:slash+1))+"+"+hex(address-m.base);}}
  return hex(address);
 }
};

inline std::string bytes112(HANDLE process,uintptr_t address,size_t length){
 unsigned char data[512]{};SIZE_T got=0;SetLastError(0);
 BOOL ok=address>=0x10000&&ReadProcessMemory(process,reinterpret_cast<void*>(address),data,std::min(length,sizeof data),&got);
 DWORD error=ok?0:address<0x10000?ERROR_INVALID_ADDRESS:GetLastError();got=std::min<SIZE_T>(got,SIZE_T(sizeof data));std::ostringstream s;
 s<<"{\"address\":"<<quote(hex(address))<<",\"requested\":"<<std::min(length,sizeof data)<<",\"bytes_read\":"<<got<<",\"complete\":"<<(ok?"true":"false")<<",\"winerror\":"<<error<<",\"hex\":\"";
 for(size_t i=0;i<got;++i)s<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(data[i]);s<<"\"}";return s.str();
}

inline bool fault_snapshot112(HANDLE process,DWORD pid,DWORD tid,const EXCEPTION_RECORD&record,const std::wstring&path,const ModuleCache112&cache){
 // Save independent of DbgHelp, before any potentially failing dump attempt.
 CONTEXT c{};c.ContextFlags=CONTEXT_FULL|CONTEXT_DEBUG_REGISTERS;SetLastError(0);Handle thread(OpenThread(THREAD_GET_CONTEXT,FALSE,tid));
 bool ok=thread.h&&GetThreadContext(thread.h,&c);DWORD error=ok?0:GetLastError();std::ostringstream f;
 auto address=reinterpret_cast<uintptr_t>(record.ExceptionAddress);
 f<<"{\"version\":112,\"pid\":"<<pid<<",\"thread\":"<<tid<<",\"tick_ms\":"<<GetTickCount64()<<",\"code\":"<<quote(hex(record.ExceptionCode))<<",\"address\":"<<quote(hex(address))<<",\"cached_module_offset\":"<<quote(cache.locate(address))<<",\"module_source\":\"debug load/unload events; no fault-time Toolhelp dependency\",\"context_captured\":"<<(ok?"true":"false")<<",\"context_error\":"<<error<<",\"parameters\":[";
 for(DWORD i=0;i<std::min<DWORD>(record.NumberParameters,15);++i){if(i)f<<',';f<<quote(hex(record.ExceptionInformation[i]));}f<<']';
 if(ok){
  f<<",\"registers\":{\"eax\":"<<quote(hex(c.Eax))<<",\"ebx\":"<<quote(hex(c.Ebx))<<",\"ecx\":"<<quote(hex(c.Ecx))<<",\"edx\":"<<quote(hex(c.Edx))<<",\"esi\":"<<quote(hex(c.Esi))<<",\"edi\":"<<quote(hex(c.Edi))<<",\"esp\":"<<quote(hex(c.Esp))<<",\"ebp\":"<<quote(hex(c.Ebp))<<",\"eip\":"<<quote(hex(c.Eip))<<",\"eflags\":"<<quote(hex(c.EFlags))<<",\"dr6\":"<<quote(hex(c.Dr6))<<",\"dr7\":"<<quote(hex(c.Dr7))<<"}"
   <<",\"code_bytes\":"<<bytes112(process,c.Eip,64)<<",\"stack_bytes\":"<<bytes112(process,c.Esp,512)
   <<",\"eax_bytes\":"<<bytes112(process,c.Eax,64)<<",\"ebx_bytes\":"<<bytes112(process,c.Ebx,64)<<",\"ecx_bytes\":"<<bytes112(process,c.Ecx,64)<<",\"edi_bytes\":"<<bytes112(process,c.Edi,64);
 }
 f<<",\"instruction_bytes\":"<<bytes112(process,address,64)<<",\"modules\":[";bool comma=false;
 for(const auto& entry:cache.loaded){if(comma)f<<',';comma=true;const auto& m=entry.second;f<<"{\"base\":"<<quote(hex(m.base))<<",\"size\":"<<m.size<<",\"path\":"<<quote(utf8(m.path))<<'}';}
 f<<"],\"limit\":\"Bounded readable fragments; failed reads explicit, no complete heap/write history.\"}\n";
 auto payload=f.str();Handle file(CreateFileW(path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));
 if(file.h==INVALID_HANDLE_VALUE)return false;DWORD written=0;
 return WriteFile(file.h,payload.data(),DWORD(payload.size()),&written,nullptr)&&written==payload.size()&&FlushFileBuffers(file.h);
}

using DumpWriter112=decltype(&MiniDumpWriteDump);
inline std::wstring dump112(HANDLE process,DWORD pid,DWORD tid,const EXCEPTION_RECORD*record,const std::wstring&path,bool full,std::ostream&log,DumpWriter112 writer=MiniDumpWriteDump){
 CONTEXT ctx{};ctx.ContextFlags=CONTEXT_FULL|CONTEXT_DEBUG_REGISTERS;Handle thread(OpenThread(THREAD_GET_CONTEXT,FALSE,tid));EXCEPTION_RECORD copy{};EXCEPTION_POINTERS pointers{};MINIDUMP_EXCEPTION_INFORMATION info{};
 bool captured=record&&thread.h&&GetThreadContext(thread.h,&ctx);if(captured){copy=*record;pointers.ExceptionRecord=&copy;pointers.ContextRecord=&ctx;info.ThreadId=tid;info.ExceptionPointers=&pointers;info.ClientPointers=FALSE;}
 DWORD original=MiniDumpNormal|MiniDumpWithThreadInfo|MiniDumpWithUnloadedModules|MiniDumpWithIndirectlyReferencedMemory|MiniDumpWithFullMemoryInfo|(full?MiniDumpWithFullMemory:0);
 DWORD flags[]={original,original|MiniDumpIgnoreInaccessibleMemory,MiniDumpNormal};
 std::wstring stem=path.substr(0,path.size()-4),paths[]={path,stem+L"-readable.dmp",stem+L"-minimal.dmp"};
 for(unsigned attempt=0;attempt<3;++attempt){
  DWORD error=0;bool success=false;
  {Handle file(CreateFileW(paths[attempt].c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));
   if(file.h==INVALID_HANDLE_VALUE)error=GetLastError();
   else{SetLastError(0);success=writer(process,pid,file.h,static_cast<MINIDUMP_TYPE>(flags[attempt]),captured?&info:nullptr,nullptr,nullptr)!=FALSE;error=success?0:GetLastError();if(success)success=FlushFileBuffers(file.h)!=FALSE;if(!success&&!error)error=GetLastError();}
  }
  log<<"{\"event\":\"dump_attempt112\",\"tick_ms\":"<<GetTickCount64()<<",\"path\":"<<quote(utf8(paths[attempt]))<<",\"attempt\":"<<attempt<<",\"flags\":"<<flags[attempt]<<",\"success\":"<<(success?"true":"false")<<",\"error\":"<<error<<",\"reduced_coverage\":"<<(attempt?"true":"false")<<"}\n";log.flush();
  if(success){log<<"{\"event\":\"dump_written\",\"tick_ms\":"<<GetTickCount64()<<",\"path\":"<<quote(utf8(paths[attempt]))<<",\"full_memory\":"<<(full&&attempt<2?"true":"false")<<",\"reduced_coverage\":"<<(attempt?"true":"false")<<"}\n";log.flush();return paths[attempt];}
  // Permanent output errors cannot be repaired by scanning more target memory.
  if(error!=ERROR_PARTIAL_COPY&&error!=DWORD(HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY))&&error!=ERROR_READ_FAULT&&error!=DWORD(HRESULT_FROM_WIN32(ERROR_READ_FAULT))&&error!=ERROR_INVALID_PARAMETER)break;
 }
 throw std::runtime_error("All permitted dump attempts failed; independent fault snapshot retained when readable");
}
