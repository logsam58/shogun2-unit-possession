#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <initializer_list>
namespace melee60 {
constexpr uint32_t Magic=0x33364d53,Version=63;
enum Phase:uint32_t {Off=0,Facing=1,NativeCombat=2,Aligning=3};
inline uint32_t preparation_phase(uint32_t phase,bool canceled,bool current_event){
    // Answered means a verdict exists, not that Shogun started its animation.
    // Repeated prepare calls must retain native-target alignment assistance.
    // finish_replay clears the UI event using Canceled. That empty publication
    // must not cancel the separately reserved native alignment/playback.
    return canceled&&current_event?Off:phase;
}
inline uint32_t playback_phase(uint32_t phase,bool playing){
    return phase==Aligning&&playing?NativeCombat:phase;
}
// Same-process optional adapter API. Numeric identity/positions only; never
// authorize an arbitrary native address through this control request.
struct Request {
    uint32_t magic=Magic,version=Version,size=sizeof(Request),pid{},phase{};
    uint32_t lease{},player{},player_unit{},opponent{},opponent_unit{};
    uint64_t serial{},heartbeat{};
    float player_position[3]{},opponent_position[3]{};
    float player_anchor[3]{},opponent_anchor[3]{};
    uint32_t player_kind{},opponent_kind{};
};
using Submit=uint32_t(__cdecl*)(const Request*);
inline bool valid(const Request& r){
    if(r.magic!=Magic||r.version!=Version||r.size!=sizeof(Request)||r.phase>Aligning||r.player_kind>1||r.opponent_kind>1)return false;
    if(r.phase==Off)return true;
    if(!r.pid||!r.lease||!r.serial||!r.player_unit||!r.opponent_unit||r.player_unit==r.opponent_unit)return false;
    for(unsigned i=0;i<3;++i)for(float value:{r.player_position[i],r.opponent_position[i],r.player_anchor[i],r.opponent_anchor[i]})
        if(!std::isfinite(value)||std::abs(value)>100000)return false;
    return true;
}
inline bool live(const Request& r,uint32_t pid,uint32_t lease,uint32_t actor,uint32_t owner_unit,uint64_t now){
    return valid(r)&&r.phase!=Off&&r.pid==pid&&r.lease==lease&&r.player==actor&&r.player_unit==owner_unit&&
        now>=r.heartbeat&&now-r.heartbeat<=500;
}
inline float facing(float x,float z,float tx,float tz){return std::atan2(tx-x,tz-z);}
inline float angle_error(float wanted,uint32_t bearing){return std::abs(std::remainder(wanted-float(bearing)*6.28318530718f/65536.f,6.28318530718f));}
inline float eyelid_fraction(uint64_t elapsed_ms){
    float t=float(elapsed_ms>10000?elapsed_ms-10000:0)/5000.f;t=std::fmin(1.f,t);return t*t*(3.f-2.f*t);
}
struct alignas(8) Channel {volatile LONG sequence{};Request value{};};
inline void publish(Channel& c,const Request& r){InterlockedIncrement(&c.sequence);MemoryBarrier();c.value=r;MemoryBarrier();InterlockedIncrement(&c.sequence);}
inline bool read(const Channel& c,Request& r){for(unsigned i=0;i<4;++i){LONG n=c.sequence;if(n&1)continue;MemoryBarrier();Request copy=c.value;MemoryBarrier();if(n==c.sequence){r=copy;return true;}}return false;}
}
