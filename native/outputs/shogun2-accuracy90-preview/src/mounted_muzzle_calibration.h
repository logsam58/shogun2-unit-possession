#pragma once
#include "mounted_firearm_geometry.h"
// User-observed Test70 carbine origin was slightly below the visible barrel.
// Keep the decoded FIRE_POSITION unchanged; tune only this verified profile.
// Weapon1 local -Y is up on the carbine, so this offset follows the gun's pitch.
constexpr float CARBINE_VISUAL_MUZZLE_LIFT=.02f;
inline bool calibrated_carbine_muzzle_world(const float* weapon,const float* world,float* output){
    float native[3];if(!mounted_carbine_muzzle_world(weapon,world,native))return false;
    float result[3];
    for(unsigned r=0;r<3;++r){float up=0;for(unsigned c=0;c<3;++c)up-=world[r*4+c]*weapon[c*4+1];
        result[r]=native[r]+up*CARBINE_VISUAL_MUZZLE_LIFT;if(!std::isfinite(result[r])||std::abs(result[r])>100000)return false;}
    memcpy(output,result,sizeof result);return true;
}
