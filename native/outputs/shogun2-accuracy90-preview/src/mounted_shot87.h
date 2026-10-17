#pragma once
#include "aim_geometry.h"
#include "seated_weapon_geometry.h"
#include "arrow_render_geometry.h"
namespace mountedshot87 {
// 420050 compares half this width with the actor's bearing, which may differ
// from the horse heading. The owned camera already constrains mounted look to
// +/-100 degrees about the validated horse. Do not apply a second actor gate.
inline float native_arc(bool mounted){return mounted?6.2831853071795864769f:3.141592654f;}
// Move the visible arms/bow/arrow together, then launch from that same socket.
// This is a bounded owned-render correction; no collision shapes are modified.
inline bool align_bow(float* m,unsigned n,const int32_t* parents,unsigned left,unsigned right,
                      const unsigned* weapons,const float* world,const ViewRay& ray,float& correction){
    if(!m||!parents||!weapons||!world||!n||n>256||left>=n||right>=n||weapons[2]!=3||3>=n)return false;
    for(unsigned i=0;i<n;++i)if(parents[i]>=int32_t(i)||parents[i]<-1)return false;
    for(unsigned i=0;i<3;++i)if(weapons[i]>=n||parents[weapons[i]]!=-1)return false;
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(m[i])||std::abs(m[i])>100000)return false;
    float inverse[12],socket[3];if(!affine_pose_inverse(world,inverse)||!arrow_attachment_world(m+36,world,socket))return false;
    float aligned[3];memcpy(aligned,socket,sizeof aligned);
    if(!align_bow_release(ray,aligned,correction)||correction>.75f)return false;
    float shift[3]{};
    for(unsigned row=0;row<3;++row)for(unsigned col=0;col<3;++col)shift[row]+=inverse[row*4+col]*(aligned[col]-socket[col]);
    float out[256*12];memcpy(out,m,n*48);
    for(unsigned i=0;i<n;++i)if(bone_descends(i,left,parents,n)||bone_descends(i,right,parents,n)||i==weapons[0]||i==weapons[2]){
        for(unsigned row=0;row<3;++row)out[i*12+row*4+3]+=shift[row];
    }
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(out[i])||std::abs(out[i])>100000)return false;
    memcpy(m,out,n*48);return true;
}
}
