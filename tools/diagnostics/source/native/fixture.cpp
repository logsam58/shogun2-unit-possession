#include <windows.h>
#include <cwchar>
static int handled_code111(DWORD code){__try{RaiseException(code,0,0,nullptr);}__except(EXCEPTION_EXECUTE_HANDLER){return 0;}return 8;}
static DWORD WINAPI churn111(void*){Sleep(2);return 0;}
int wmain(int argc,wchar_t**argv){SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);Sleep(2500);const wchar_t*mode=argc>1?argv[1]:L"normal";
if(wcscmp(mode,L"handled-single-step")==0)return handled_code111(EXCEPTION_SINGLE_STEP);
if(wcscmp(mode,L"handled-breakpoint")==0)return handled_code111(EXCEPTION_BREAKPOINT);
if(wcscmp(mode,L"handled-custom")==0)return handled_code111(0xe0421111);
if(wcscmp(mode,L"unhandled-illegal")==0)RaiseException(EXCEPTION_ILLEGAL_INSTRUCTION,0,0,nullptr);
if(wcscmp(mode,L"thread-churn")==0){for(unsigned round=0;round<8;++round){HANDLE threads[16]{};for(auto& h:threads)h=CreateThread(nullptr,0,churn111,nullptr,0,nullptr);WaitForMultipleObjects(16,threads,TRUE,5000);for(auto h:threads)CloseHandle(h);}}
if(wcscmp(mode,L"handled")==0){__try{*reinterpret_cast<volatile int*>(0x42)=1;}__except(EXCEPTION_EXECUTE_HANDLER){Sleep(500);return 0;}}if(wcscmp(mode,L"crash")==0)*reinterpret_cast<volatile int*>(0x42)=1;Sleep(500);return 0;}
