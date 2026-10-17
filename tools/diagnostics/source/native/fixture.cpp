#include <windows.h>
#include <cwchar>
int wmain(int argc,wchar_t**argv){SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);Sleep(2500);const wchar_t*mode=argc>1?argv[1]:L"normal";if(wcscmp(mode,L"handled")==0){__try{*reinterpret_cast<volatile int*>(0x42)=1;}__except(EXCEPTION_EXECUTE_HANDLER){Sleep(500);return 0;}}if(wcscmp(mode,L"crash")==0)*reinterpret_cast<volatile int*>(0x42)=1;Sleep(500);return 0;}
