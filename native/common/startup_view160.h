#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <cwchar>
// Process-local launch choice, published before the adapter is loaded. No file,
// registry, game preference or shared native protocol layout changes.
namespace startup160 {
constexpr const wchar_t* environment=L"SHOGUN2_UNIT_POSSESSION_STARTING_VIEW";
inline bool publish(bool third){return SetEnvironmentVariableW(environment,third?L"third-person":L"first-person")!=0;}
inline bool third_person(){
    wchar_t value[32]{};DWORD count=GetEnvironmentVariableW(environment,value,32);
    return count>0&&count<32&&!wcscmp(value,L"third-person");
}
}
