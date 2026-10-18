#pragma once
#include "look_pitch96.h"
namespace look104 {
constexpr float yaw_limit=1.745329252f; // 100 degrees either side of the animated head.
inline bool free_eye(const deathpose::Eye& native,float requested_yaw,float requested_pitch,deathpose::Eye& result){
    float yaw{},pitch{};
    if(!std::isfinite(requested_yaw)||!std::isfinite(requested_pitch)||!animationview::angles(native,yaw,pitch))return false;
    float offset=std::clamp(std::remainder(requested_yaw-yaw,6.283185307f),-yaw_limit,yaw_limit);
    if(!look96::pitched_eye(native,requested_pitch,result))return false;
    const float c=std::cos(offset),s=std::sin(offset);
    float direction[3];for(unsigned i=0;i<3;++i)direction[i]=result.view[i+3]-result.view[i];
    auto rotate=[&](float* v){float x=v[0],z=v[2];v[0]=c*x+s*z;v[2]=-s*x+c*z;};
    rotate(direction);rotate(result.up);
    for(unsigned i=0;i<3;++i)result.view[i+3]=native.view[i]+direction[i];
    return animationview::angles(result,yaw,pitch);
}
inline float mouse_yaw(float yaw,float delta){return std::remainder(yaw+delta,6.283185307f);}
}
