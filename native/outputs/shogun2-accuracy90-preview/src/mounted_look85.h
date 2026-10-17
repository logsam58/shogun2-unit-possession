#pragma once
#include <algorithm>
#include <cmath>
namespace mountedlook85 {
constexpr float circle=6.2831853071795864769f;
constexpr float half_arc=100.f*circle/360.f;
inline float heading(unsigned bearing){return float(bearing&65535u)*circle/65536.f;}
inline float relative(float yaw,unsigned bearing){return std::remainder(yaw-heading(bearing),circle);}
inline float constrain(float yaw,unsigned bearing){
    return std::remainder(heading(bearing)+std::clamp(relative(yaw,bearing),-half_arc,half_arc),circle);
}
inline float mouse_yaw(float yaw,float delta,bool mounted,unsigned bearing){
    if(!mounted)return std::remainder(yaw+delta,circle);
    // Saturate before wrapping: excess motion never accumulates behind the
    // stop, and reversing the mouse immediately moves away from the stop.
    float offset=std::clamp(relative(yaw,bearing),-half_arc,half_arc);
    return std::remainder(heading(bearing)+std::clamp(offset+delta,-half_arc,half_arc),circle);
}
}
