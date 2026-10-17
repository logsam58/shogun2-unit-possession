#pragma once
#include <windows.h>
#include <cstdint>
#ifndef OBSERVER_VERSION
#define OBSERVER_VERSION 47
#endif
#ifndef OBSERVER_DLL_NAME
#define OBSERVER_DLL_NAME L"qte-native-observer-v47.dll"
#endif
constexpr uint32_t POC_MAGIC=0x31545153,POC_VERSION=OBSERVER_VERSION;
enum {IDLE=0,DISABLED=3};
struct Status {
    uint32_t magic,version,pid,size;
    volatile LONG stop,ready,mode,error,dropped,faults,calls[4],stop_reason;
    uint64_t heartbeat;
#if OBSERVER_VERSION >= 56
    unsigned char original_code[96],restored_code[96];
#else
    unsigned char original_code[64],restored_code[64];
#endif
};
inline void mapping_name(wchar_t* out,DWORD pid){swprintf_s(out,80,L"Local\\Shogun2QteObserver%u-%lu",POC_VERSION,pid);}
