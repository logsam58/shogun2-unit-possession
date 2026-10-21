#pragma once
#include <windows.h>
#include <cstdint>
// Separate, numeric, process-scoped presentation preference. It grants no
// gameplay authority and does not change the retained possession/status ABI.
namespace hudvisibility148 {
constexpr uint32_t Magic=0x38445648;
struct Shared {
    volatile LONG sequence{};
    uint32_t magic{Magic},version{1},bytes{sizeof(Shared)},pid{},hidden{},changes{};
};
inline void name(wchar_t* out,DWORD pid){swprintf_s(out,96,L"Local\\Shogun2HudVisibility148-%lu",pid);}
inline bool pressed(bool valid,bool owned,unsigned char current,unsigned char previous){
    return valid&&owned&&(current&0x80)&&!(previous&0x80);
}
inline bool read(const Shared* shared,DWORD pid,bool& hidden){
    if(!shared)return false;
    for(unsigned i=0;i<4;++i){LONG seq=shared->sequence;if(seq&1)continue;
        MemoryBarrier();Shared copy{};memcpy(&copy,shared,sizeof copy);MemoryBarrier();
        if(seq!=shared->sequence)continue;
        if(copy.magic!=Magic||copy.version!=1||copy.bytes!=sizeof copy||copy.pid!=pid||copy.hidden>1)return false;
        hidden=copy.hidden!=0;return true;
    }return false;
}
}
