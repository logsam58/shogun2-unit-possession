#pragma once
#include <cmath>
#include <cstdint>
// Uniform solid-angle sampling inside a circular cone about the solved muzzle
// velocity. Radius is supplied by Shogun's already computed accuracy envelope.
inline bool cone_velocity(const float* initial,float half_angle,float u,float v,float* out){
    if(!initial||!out||!std::isfinite(half_angle)||half_angle<0||half_angle>3.141592654f||
       !std::isfinite(u)||!std::isfinite(v)||u<0||u>1||v<0||v>1)return false;
    float speed2=0;for(unsigned i=0;i<3;++i){if(!std::isfinite(initial[i]))return false;speed2+=initial[i]*initial[i];}
    if(!std::isfinite(speed2)||speed2<0.000001f)return false;
    float speed=std::sqrt(speed2),forward[3];for(unsigned i=0;i<3;++i)forward[i]=initial[i]/speed;
    // Choose a perpendicular basis that stays well conditioned at vertical aim.
    float side[3]={forward[2],0,-forward[0]};
    if(std::abs(forward[1])>.9f){side[0]=0;side[1]=-forward[2];side[2]=forward[1];}
    float n=std::sqrt(side[0]*side[0]+side[1]*side[1]+side[2]*side[2]);
    for(float& x:side)x/=n;
    float up[3]={forward[1]*side[2]-forward[2]*side[1],forward[2]*side[0]-forward[0]*side[2],forward[0]*side[1]-forward[1]*side[0]};
    double sine=std::sin(double(half_angle)*.5),loss=double(u)*2*sine*sine;
    float c=float(1-loss),s=float(std::sqrt(std::fmax(0.0,loss*(2-loss)))),azimuth=v*6.283185307f;
    float result[3];for(unsigned i=0;i<3;++i)result[i]=speed*(forward[i]*c+s*(side[i]*std::cos(azimuth)+up[i]*std::sin(azimuth)));
    for(unsigned i=0;i<3;++i)out[i]=result[i];return true;
}
inline float native_random_sample(uint32_t& seed){seed=seed*0x343fdu+0x269ec3u;return float(seed>>16)/65535.0f;}
