#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdexcept>
#include <string>
// Offline fixtures never infer a Steam library or open a live game. Their
// original engine/Miles fingerprint assertions remain at each call site.
namespace fixture_paths {
inline std::wstring game(){
    wchar_t buffer[32768];auto n=GetEnvironmentVariableW(L"SHOGUN2_GAME_DIR",buffer,32768);
    if(!n||n>=32768)throw std::runtime_error("Set SHOGUN2_GAME_DIR or pass tools/build.py --game <supported installation>");
    std::wstring result(buffer);while(result.size()>3&&(result.back()==L'\\'||result.back()==L'/'))result.pop_back();return result;
}
inline const wchar_t* engine(){static const std::wstring path=game()+L"\\Empire.Retail.dll";return path.c_str();}
inline const wchar_t* miles(){static const std::wstring path=game()+L"\\mss32.dll";return path.c_str();}
}
