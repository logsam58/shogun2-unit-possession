#pragma once
#include "../../shogun2-melee-qte63-preview/src/melee_commitment.h"
namespace look126 {
// Numeric poses captured only from fully proved native pair motion callbacks.
// No roster scan, camera-only turn rate, timer, or new native movement is used.
struct Pose {
    float x{},z{};unsigned bearing{};uint64_t tick{};uintptr_t animation{};bool playing{},valid{};
};
struct Pair {
    uint64_t serial{};uint32_t lease{},player{},player_unit{},opponent{},opponent_unit{},player_kind{},opponent_kind{};
    Pose poses[2];
    bool matches(const melee60::Request& r)const{
        return serial==r.serial&&lease==r.lease&&player==r.player&&player_unit==r.player_unit&&
            opponent==r.opponent&&opponent_unit==r.opponent_unit&&player_kind==r.player_kind&&opponent_kind==r.opponent_kind;
    }
    void observe(const melee60::Request& r,bool is_player,const Pose& pose){
        if(!matches(r)){*this={};serial=r.serial;lease=r.lease;player=r.player;player_unit=r.player_unit;
            opponent=r.opponent;opponent_unit=r.opponent_unit;player_kind=r.player_kind;opponent_kind=r.opponent_kind;}
        poses[is_player?0:1]=pose;
    }
    bool locked(const melee60::Request& r,uint64_t now)const{
        if(!matches(r))return false;
        for(auto& p:poses)if(!p.valid||now<p.tick||now-p.tick>500||!std::isfinite(p.x)||!std::isfinite(p.z))return false;
        auto& p=poses[0];auto& e=poses[1];
        // Both independently proved participants belong to the same playing
        // native two-actor witness, even if the published phase lags behind.
        if(p.playing&&e.playing&&p.animation&&p.animation==e.animation)return true;
        if(std::hypot(e.x-p.x,e.z-p.z)<.01f)return false;
        constexpr float tolerance=.261799388f; // Existing QTE facing_ready: 15 degrees.
        return melee60::angle_error(melee60::facing(p.x,p.z,e.x,e.z),p.bearing)<=tolerance&&
            melee60::angle_error(melee60::facing(e.x,e.z,p.x,p.z),e.bearing)<=tolerance;
    }
};
}
