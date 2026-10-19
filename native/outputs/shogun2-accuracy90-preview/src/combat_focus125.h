#pragma once
#include <algorithm>
#include "animation_look104.h"
#include "combat_turn126.h"
#include "../../shogun2-melee-qte63-preview/src/melee_commitment.h"
namespace look125 {
struct Focus {
    uint64_t serial{};
    uint32_t lease{},player{},player_unit{},opponent{},opponent_unit{},player_kind{},opponent_kind{};
    float yaw{};bool valid{},unlocked{};
    void reset(){*this={};}
    bool begin(const melee60::Request& r){
        if(valid&&serial==r.serial&&lease==r.lease&&player==r.player&&player_unit==r.player_unit&&
           opponent==r.opponent&&opponent_unit==r.opponent_unit&&player_kind==r.player_kind&&opponent_kind==r.opponent_kind)return false;
        serial=r.serial;lease=r.lease;player=r.player;player_unit=r.player_unit;
        opponent=r.opponent;opponent_unit=r.opponent_unit;player_kind=r.player_kind;opponent_kind=r.opponent_kind;valid=true;unlocked=false;return true;
    }
    bool release(bool native_lock,float current_heading){
        if(unlocked||!native_lock||!std::isfinite(current_heading))return false;
        yaw=current_heading;unlocked=true;return true;
    }
};
// Keep the native eye position and roll, but bound head-look about the opponent
// captured at the native engagement lock, not a later dramatic head turn.
inline bool free_eye(const deathpose::Eye& native,float requested_yaw,float requested_pitch,float anchor,deathpose::Eye& result){
    float yaw{},pitch{};
    if(!std::isfinite(anchor)||!std::isfinite(requested_yaw)||!animationview::angles(native,yaw,pitch))return false;
    float target=anchor+std::clamp(std::remainder(requested_yaw-anchor,6.283185307f),-look104::yaw_limit,look104::yaw_limit);
    if(!look96::pitched_eye(native,requested_pitch,result))return false;
    float offset=std::remainder(target-yaw,6.283185307f),c=std::cos(offset),s=std::sin(offset);
    auto rotate=[&](float* v){float x=v[0],z=v[2];v[0]=c*x+s*z;v[2]=-s*x+c*z;};
    float direction[3];for(unsigned i=0;i<3;++i)direction[i]=result.view[i+3]-result.view[i];
    rotate(direction);rotate(result.up);
    for(unsigned i=0;i<3;++i)result.view[i+3]=native.view[i]+direction[i];
    return animationview::angles(result,yaw,pitch);
}
}
