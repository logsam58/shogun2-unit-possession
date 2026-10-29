#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <cwchar>

// Only the Workshop coordinator publishes this process-local barrier. A
// standalone adapter retains its existing behavior when the variable is absent.
// No persistent preference, shared status ABI or game setting is changed.
namespace startup168 {
inline constexpr const wchar_t* key=L"S2UP_WORKSHOP_COMPONENTS_168";
inline bool pending(){return SetEnvironmentVariableW(key,L"pending")!=0;}
inline bool ready(){return SetEnvironmentVariableW(key,L"ready")!=0;}
inline void failed(){SetEnvironmentVariableW(key,L"failed");}
inline bool can_possess(){
    wchar_t value[16]{};SetLastError(ERROR_SUCCESS);
    DWORD count=GetEnvironmentVariableW(key,value,16);
    if(!count)return GetLastError()==ERROR_ENVVAR_NOT_FOUND;
    return count<16&&!wcscmp(value,L"ready");
}
}
