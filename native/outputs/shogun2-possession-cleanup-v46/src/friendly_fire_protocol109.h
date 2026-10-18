#pragma once
#include <windows.h>
#include <cstdint>
#include <cwchar>
namespace friendly109 {
constexpr uint32_t magic=0x39314646,version=109;
struct Status {
    LONG sequence;uint32_t magic,version,size,pid,lease,actor,unit;
    uint64_t tick;uint32_t event,victim,victim_unit,projectile,reserved[2];
};
static_assert(sizeof(Status)==64,"Friendly-fire notification layout changed");
inline void mapping_name(wchar_t (&out)[96],DWORD pid){swprintf_s(out,L"Local\\Shogun2FriendlyFire109-%lu",pid);}
inline bool valid(const Status& s,DWORD pid,uint32_t lease,uint32_t actor,uint32_t unit,uint64_t now){
    return s.magic==magic&&s.version==version&&s.size==sizeof s&&s.pid==pid&&s.event&&s.victim_unit&&
        s.lease==lease&&s.actor==actor&&s.unit==unit&&now>=s.tick&&now-s.tick<=1000;
}
// Consume every event once, even while muted/inactive or another clip is playing.
// Returning to possession/focus never replays an old notification.
struct Consumer {
    uint32_t seen{};
    bool take(const Status& s,DWORD pid,uint32_t lease,uint32_t actor,uint32_t unit,uint64_t now,bool active){
        if(s.magic!=magic||s.version!=version||s.size!=sizeof s||s.pid!=pid||!s.event||s.event==seen)return false;
        seen=s.event;return active&&valid(s,pid,lease,actor,unit,now);
    }
};
}
