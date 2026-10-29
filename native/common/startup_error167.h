#pragma once
#include <windows.h>
#include <string>
namespace startup167 {
inline SRWLOCK error_lock=SRWLOCK_INIT;
inline std::string last_error;
inline LONG shown=0;
inline void remember(const char* message){
    AcquireSRWLockExclusive(&error_lock);
    last_error=message?std::string(message).substr(0,1024):"Unknown initialization error";
    ReleaseSRWLockExclusive(&error_lock);
}
inline bool game_host(){
    wchar_t path[32768]{};DWORD n=GetModuleFileNameW(nullptr,path,32768);
    if(!n||n>=32768)return false;const wchar_t* name=wcsrchr(path,L'\\');name=name?name+1:path;
    return !_wcsicmp(name,L"shogun2.exe")||!_wcsicmp(name,L"shogun2.retail.exe");
}
inline void notify(){
    if(!game_host()||InterlockedCompareExchange(&shown,1,0)!=0)return;
    AcquireSRWLockShared(&error_lock);auto detail=last_error;ReleaseSRWLockShared(&error_lock);
    if(detail.empty())detail="The native Workshop startup could not finish.";
    std::wstring text=L"Unit Possession could not start.\n\n";
    text.append(detail.begin(),detail.end());
    text+=L"\n\nThe mod has not finished initializing. You can disable it in the launcher and restart."
          L"\nPlease include a screenshot of this message when reporting the problem.";
#ifndef S2UP_STARTUP_TEST167
    MessageBoxW(nullptr,text.c_str(),L"Shogun 2 Unit Possession",MB_OK|MB_ICONWARNING);
#endif
}
}
