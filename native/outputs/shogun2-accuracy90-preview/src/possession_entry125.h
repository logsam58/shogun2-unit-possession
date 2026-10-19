#pragma once
#include <cmath>
#include "steering.h"
namespace entry125 {
// Retail actor+0x244: walk=0, run=1, charge=2 (3e44f0/3dda00).
// Capture only actual ground locomotion, never a combat/recovery animation.
inline unsigned carry(unsigned mode,unsigned gait,float vx,float vz){
    if(mode>7||gait>2||!std::isfinite(vx)||!std::isfinite(vz))return 0;
    float speed2=vx*vx+vz*vz;
    if(speed2<.0009f||speed2>40000.f)return 0;
    return FORWARD|(gait?RUN:0);
}
inline float facing(unsigned bearing){return float(bearing&65535u)*6.283185307f/65536.f;}
}
