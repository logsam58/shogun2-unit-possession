#pragma once
#include "seated_weapon_geometry.h"
namespace firstperson101 {
// Presentation tuning in native world metres. Keep an already smaller native
// near plane; never change the native far plane, world geometry or simulation.
constexpr float close_near=.02f;
struct NearClip {
    float native{},presented{};bool active{};
    bool capture(float near_value,float far_value){
        *this={};if(!std::isfinite(near_value)||!std::isfinite(far_value)||near_value<=0||far_value<=near_value)return false;
        native=near_value;presented=std::min(native,close_near);active=true;return true;
    }
    bool apply(float& current,bool close_view=true){
        if(!active||(current!=native&&current!=presented))return false;
        current=close_view?presented:native;return true;
    }
    bool restore(float& current,bool same_camera){
        bool restored=active&&same_camera&&current==presented;
        if(restored)current=native;active=false;return restored;
    }
};
inline bool eye_in_model(const float* world,const float* eye,float* pivot){
    if(!world||!eye||!pivot)return false;float inverse[12];if(!affine_pose_inverse(world,inverse))return false;
    float next[3]{};
    for(unsigned r=0;r<3;++r){
        next[r]=inverse[r*4+3];for(unsigned c=0;c<3;++c){if(!std::isfinite(eye[c]))return false;next[r]+=inverse[r*4+c]*eye[c];}
        if(!std::isfinite(next[r])||std::abs(next[r])>8)return false;
    }
    memcpy(pivot,next,sizeof next);return true;
}
}
