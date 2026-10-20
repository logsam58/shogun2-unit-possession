#pragma once
#include <algorithm>
#include <cmath>
namespace head131 {
constexpr float tau=6.2831853071795864769f;
constexpr float yaw_limit=tau*80.f/360.f,up=tau*70.f/360.f,down=tau*60.f/360.f;
inline float pitch(float value){return std::clamp(value,-down,up);}
inline float constrain(float yaw,float anchor){
    return std::remainder(anchor+std::clamp(std::remainder(yaw-anchor,tau),-yaw_limit,yaw_limit),tau);
}
// Reject invalid input timing, but do not throttle responsive pitch/animation
// free-look. Ordinary foot yaw is rendered from the actual native body instead.
inline bool delta(float& horizontal,float& vertical,float elapsed){
    if(!std::isfinite(horizontal)||!std::isfinite(vertical)||!std::isfinite(elapsed)||elapsed<=0){horizontal=vertical=0;return false;}
    return true;
}
inline float mouse(float yaw,float change,float anchor){
    float offset=std::clamp(std::remainder(yaw-anchor,tau),-yaw_limit,yaw_limit);
    return std::remainder(anchor+std::clamp(offset+change,-yaw_limit,yaw_limit),tau);
}
}
