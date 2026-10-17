#pragma once
#include <cmath>
struct ViewRay {float origin[3],direction[3];};
constexpr float LOOK_PITCH_LIMIT=1.553343034f; // 89 degrees, avoiding the pole singularity.
constexpr float POSSESSION_FOV=1.308996939f; // 75-degree native vertical FOV.
inline bool camera_ray(const float* eye,const float* look_at,ViewRay& out){
    float n=0;
    for(unsigned i=0;i<3;++i){
        if(!std::isfinite(eye[i])||!std::isfinite(look_at[i])||std::abs(eye[i])>100000||std::abs(look_at[i])>100000)return false;
        out.origin[i]=eye[i];out.direction[i]=look_at[i]-eye[i];n+=out.direction[i]*out.direction[i];
    }
    if(!std::isfinite(n)||n<0.000001f)return false;
    n=std::sqrt(n);for(float& v:out.direction)v/=n;
    return true;
}
inline void crosshair_target(const ViewRay& ray,float* point,float distance){
    for(unsigned i=0;i<3;++i)point[i]=ray.origin[i]+ray.direction[i]*distance;
}
inline bool align_bow_release(const ViewRay& ray,float* origin,float& correction){
    float depth=0,norm=0;
    for(unsigned i=0;i<3;++i){
        if(!std::isfinite(origin[i])||!std::isfinite(ray.origin[i])||!std::isfinite(ray.direction[i]))return false;
        depth+=(origin[i]-ray.origin[i])*ray.direction[i];norm+=ray.direction[i]*ray.direction[i];
    }
    if(std::abs(norm-1)>0.001f)return false;
    // Keep the native muzzle's forward depth, within the close first-person rig.
    // Only the manual bow launch receives this reticle-aligned presentation origin.
    depth=std::fmax(0.25f,std::fmin(depth,1.5f));float adjusted[3];correction=0;
    for(unsigned i=0;i<3;++i){adjusted[i]=ray.origin[i]+ray.direction[i]*depth;float delta=adjusted[i]-origin[i];correction+=delta*delta;}
    correction=std::sqrt(correction);for(unsigned i=0;i<3;++i)origin[i]=adjusted[i];return true;
}
inline float crosshair_distance(unsigned projectile_record){
    // Native direct-fire branch also requires squared aim distance >= 25,000,000.
    // A distant point supplies direction; native projectile physics still owns flight.
    return projectile_record==3||projectile_record==60?6000.0f:80.0f;
}
inline bool level_camera_up(const ViewRay& ray,float* up){
    float h=std::sqrt(ray.direction[0]*ray.direction[0]+ray.direction[2]*ray.direction[2]);
    if(!std::isfinite(h)||h<0.00001f)return false;
    up[0]=-ray.direction[1]*ray.direction[0]/h;
    up[1]=h;
    up[2]=-ray.direction[1]*ray.direction[2]/h;
    return true;
}
