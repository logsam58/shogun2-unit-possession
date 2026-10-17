#pragma once
#include <cmath>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include "mounted_look85.h"
constexpr float BOW_NATIVE_AIM_PITCH=0.436332313f; // Verified bow_aim_25 / bow_fire_to_stand_25 assets.
inline float weapon_pose_pitch(unsigned record,unsigned clip,float camera_pitch){
    return record==3&&(clip==124||clip==126||clip==130)?camera_pitch-BOW_NATIVE_AIM_PITCH:camera_pitch;
}
// Native bone arrays contain row-major affine 3x4 matrices in model space.
inline bool bone_descends(unsigned bone,unsigned root,const int32_t* parents,unsigned n){
    for(unsigned steps=0;steps<n&&bone<n;++steps){
        if(bone==root)return true;
        int32_t parent=parents[bone];if(parent<0)return false;bone=unsigned(parent);
    }return false;
}
inline void pitch_matrix(float* m,const float* pivot,float pitch){
    float c=std::cos(pitch),s=std::sin(pitch);
    for(unsigned col=0;col<4;++col){float y=m[4+col],z=m[8+col];
        if(col==3){y-=pivot[1];z-=pivot[2];}
        m[4+col]=c*y+s*z;m[8+col]=-s*y+c*z;
        if(col==3){m[7]+=pivot[1];m[11]+=pivot[2];}
    }
}
inline void yaw_matrix(float* m,const float* pivot,float yaw){
    float c=std::cos(yaw),s=std::sin(yaw);
    for(unsigned col=0;col<4;++col){float x=m[col],z=m[8+col];
        if(col==3){x-=pivot[0];z-=pivot[2];}
        m[col]=c*x+s*z;m[8+col]=-s*x+c*z;
        if(col==3){m[3]+=pivot[0];m[11]+=pivot[2];}
    }
}
inline bool pose_rider_matrices(float* m,unsigned n,const int32_t* parents,unsigned spine,unsigned left,unsigned right,
    unsigned head,unsigned neck,const unsigned* weapons,float yaw,float pitch){
    if(!m||!parents||!weapons||!n||n>256||spine>=n||left>=n||right>=n||head>=n||neck>=n||
       !std::isfinite(yaw)||!std::isfinite(pitch)||!bone_descends(left,spine,parents,n)||
       !bone_descends(right,spine,parents,n)||!bone_descends(head,spine,parents,n)||!bone_descends(neck,spine,parents,n))return false;
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(m[i]))return false;
    for(unsigned i=0;i<3;++i)if(weapons[i]>=n)return false;
    float pivot[3]={m[spine*12+3],m[spine*12+7],m[spine*12+11]};
    // Keep the saddle, pelvis and legs entirely under native horse animations.
    yaw=std::clamp(yaw,-mountedlook85::half_arc,mountedlook85::half_arc);
    pitch=std::clamp(pitch,-.65f,.65f);
    for(unsigned i=0;i<n;++i)if(bone_descends(i,spine,parents,n)||i==weapons[0]||i==weapons[1]||i==weapons[2]){
        pitch_matrix(m+i*12,pivot,pitch);yaw_matrix(m+i*12,pivot,yaw);
    }
    // Collapse only the rider's head/equipment in this owned view.
    float hidden[3]={m[neck*12+3],m[neck*12+7],m[neck*12+11]};
    for(unsigned i=0;i<n;++i)if(bone_descends(i,head,parents,n))for(unsigned row=0;row<3;++row){
        for(unsigned col=0;col<3;++col)m[i*12+row*4+col]*=.0001f;
        m[i*12+row*4+3]=hidden[row];
    }
    return true;
}
inline bool pitch_world_point(float* origin,const float* pivot_model,const float* world,float pitch){
    if(!std::isfinite(pitch))return false;
    for(unsigned i=0;i<12;++i)if(!std::isfinite(world[i]))return false;
    for(unsigned i=0;i<3;++i)if(!std::isfinite(origin[i])||!std::isfinite(pivot_model[i]))return false;
    for(unsigned a=0;a<3;++a)for(unsigned b=0;b<3;++b){float dot=0;
        for(unsigned row=0;row<3;++row)dot+=world[row*4+a]*world[row*4+b];
        if(std::abs(dot-(a==b?1.0f:0.0f))>0.001f)return false;
    }
    float pivot[3],delta[3],local[3]{};
    for(unsigned row=0;row<3;++row){pivot[row]=world[row*4+3];for(unsigned col=0;col<3;++col)pivot[row]+=world[row*4+col]*pivot_model[col];delta[row]=origin[row]-pivot[row];}
    for(unsigned col=0;col<3;++col)for(unsigned row=0;row<3;++row)local[col]+=world[row*4+col]*delta[row];
    float c=std::cos(pitch),s=std::sin(pitch),y=local[1],z=local[2];local[1]=c*y+s*z;local[2]=-s*y+c*z;
    for(unsigned row=0;row<3;++row){origin[row]=pivot[row];for(unsigned col=0;col<3;++col)origin[row]+=world[row*4+col]*local[col];}
    return true;
}
inline bool pose_owned_matrices(float* m,unsigned n,const int32_t* parents,unsigned left,unsigned right,unsigned head,unsigned neck,const unsigned* weapons,float pitch,bool aiming){
    if(!m||!parents||n<1||n>256||left>=n||right>=n||head>=n||neck>=n||!std::isfinite(pitch))return false;
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(m[i]))return false;
    float pivot[3];for(unsigned k=0;k<3;++k)pivot[k]=(m[left*12+3+k*4]+m[right*12+3+k*4])*0.5f;
    float hidden[3]={m[neck*12+3],m[neck*12+7],m[neck*12+11]};
    for(unsigned i=0;i<n;++i){
        if(aiming&&(bone_descends(i,left,parents,n)||bone_descends(i,right,parents,n)||i==weapons[0]||i==weapons[1]||i==weapons[2]))pitch_matrix(m+i*12,pivot,pitch);
        // Hide this actor's head and head-mounted equipment in its own view only.
        if(bone_descends(i,head,parents,n)){
            for(unsigned row=0;row<3;++row){for(unsigned col=0;col<3;++col)m[i*12+row*4+col]*=0.0001f;
                m[i*12+row*4+3]=hidden[row];}
        }
    }return true;
}
