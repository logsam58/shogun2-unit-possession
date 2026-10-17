#pragma once
#include <cmath>
#include <cstring>
namespace deathpose {
struct Frame {float position[3]{},axis[3][3]{};};
struct Eye {float view[6]{},up[3]{};};
struct Calibration {float offset[3]{},direction[3]{},up[3]{};};
inline float dot(const float* a,const float* b){return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];}
inline bool normalize(float* a){float n=std::sqrt(dot(a,a));if(!std::isfinite(n)||n<.001f)return false;for(unsigned i=0;i<3;++i)a[i]/=n;return true;}
inline void cross(const float* a,const float* b,float* c){c[0]=a[1]*b[2]-a[2]*b[1];c[1]=a[2]*b[0]-a[0]*b[2];c[2]=a[0]*b[1]-a[1]*b[0];}
// Native skinning supplies row-major affine 3x4 transforms in model space.
inline bool frame(const float* world,const float* head,Frame& out){
    for(unsigned i=0;i<12;++i)if(!std::isfinite(world[i])||!std::isfinite(head[i]))return false;
    float m[12]{};
    for(unsigned r=0;r<3;++r){for(unsigned c=0;c<3;++c)for(unsigned k=0;k<3;++k)m[r*4+c]+=world[r*4+k]*head[k*4+c];
        m[r*4+3]=world[r*4+3];for(unsigned k=0;k<3;++k)m[r*4+3]+=world[r*4+k]*head[k*4+3];}
    for(unsigned i=0;i<3;++i){out.position[i]=m[i*4+3];if(!std::isfinite(out.position[i])||std::abs(out.position[i])>100000)return false;
        for(unsigned j=0;j<3;++j)out.axis[j][i]=m[i*4+j];}
    if(!normalize(out.axis[2]))return false;
    float d=dot(out.axis[1],out.axis[2]);for(unsigned i=0;i<3;++i)out.axis[1][i]-=d*out.axis[2][i];
    if(!normalize(out.axis[1]))return false;cross(out.axis[1],out.axis[2],out.axis[0]);return normalize(out.axis[0]);
}
inline bool calibrate(const Frame& head,const Eye& eye,Calibration& out){
    float delta[3],direction[3],up[3];
    for(unsigned i=0;i<3;++i){delta[i]=eye.view[i]-head.position[i];direction[i]=eye.view[i+3]-eye.view[i];up[i]=eye.up[i];}
    if(!std::isfinite(dot(delta,delta))||dot(delta,delta)>2.25f||!normalize(direction)||!normalize(up))return false;
    for(unsigned i=0;i<3;++i){out.offset[i]=dot(delta,head.axis[i]);out.direction[i]=dot(direction,head.axis[i]);out.up[i]=dot(up,head.axis[i]);}return true;
}
inline bool follow(const Frame& head,const Calibration& local,Eye& out){
    float direction[3]{},up[3]{};
    for(unsigned i=0;i<3;++i){out.view[i]=head.position[i];for(unsigned j=0;j<3;++j){out.view[i]+=head.axis[j][i]*local.offset[j];direction[i]+=head.axis[j][i]*local.direction[j];up[i]+=head.axis[j][i]*local.up[j];}}
    if(!normalize(direction))return false;float d=dot(direction,up);for(unsigned i=0;i<3;++i)up[i]-=d*direction[i];if(!normalize(up))return false;
    for(unsigned i=0;i<3;++i){out.view[i+3]=out.view[i]+direction[i];out.up[i]=up[i];}return true;
}
}
