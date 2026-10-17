#pragma once
#include "seated_weapon_geometry.h"
#include "mounted_carbine_asset.h"
// First-person tuning, not a claimed native unit statistic. Raise only the
// owned aimed arms and weapon, preserving the native seated body and camera.
constexpr float MOUNTED_GUN_RAISE=.04f;
inline bool mounted_firearm_transform(const float* matrices,const float* world,unsigned spine,unsigned left,unsigned right,float yaw,float pitch,float* rotation,float raise=MOUNTED_GUN_RAISE){
    if(!matrices||!world||!rotation||!std::isfinite(yaw)||!std::isfinite(pitch)||!std::isfinite(raise)||raise<0||raise>.25f)return false;
    float inverse[12];if(!affine_pose_inverse(world,inverse))return false;
    float arm[3],body[3];for(unsigned r=0;r<3;++r){
        arm[r]=(matrices[left*12+r*4+3]+matrices[right*12+r*4+3])*.5f;
        body[r]=matrices[spine*12+r*4+3];
    }
    const float identity[12]={1,0,0,0,0,1,0,0,0,0,1,0};memcpy(rotation,identity,48);
    pitch_matrix(rotation,arm,std::clamp(pitch,-.65f,.65f));
    yaw_matrix(rotation,body,std::clamp(yaw,-mountedlook85::half_arc,mountedlook85::half_arc));
    for(unsigned r=0;r<3;++r)rotation[r*4+3]+=inverse[r*4+1]*raise;
    for(unsigned i=0;i<12;++i)if(!std::isfinite(rotation[i]))return false;
    return true;
}
inline bool pose_mounted_firearm(float* matrices,unsigned n,const int32_t* parents,unsigned spine,unsigned left,unsigned right,unsigned head,unsigned neck,const unsigned* weapons,const float* world,float yaw,float pitch,float raise=MOUNTED_GUN_RAISE){
    if(!matrices||!parents||!weapons||!n||n>256||spine>=n||left>=n||right>=n||head>=n||neck>=n)return false;
    for(unsigned i=0;i<3;++i)if(weapons[i]>=n)return false;
    for(unsigned i:{left,right,head,neck})if(!bone_descends(i,spine,parents,n))return false;
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(matrices[i]))return false;
    float transform[12];if(!mounted_firearm_transform(matrices,world,spine,left,right,yaw,pitch,transform,raise))return false;
    float temp[256*12];memcpy(temp,matrices,n*48);
    // Existing infantry shoulder/arm pitch and head hiding, with native rider
    // yaw applied separately. No infantry animation is played on the rider.
    if(!pose_owned_matrices(temp,n,parents,left,right,head,neck,weapons,std::clamp(pitch,-.65f,.65f),true))return false;
    float pivot[3]={matrices[spine*12+3],matrices[spine*12+7],matrices[spine*12+11]};
    float inverse[12];if(!affine_pose_inverse(world,inverse))return false;
    for(unsigned i=0;i<n;++i){
        bool weapon=i==weapons[0]||i==weapons[1]||i==weapons[2];
        if(bone_descends(i,spine,parents,n)||weapon)yaw_matrix(temp+i*12,pivot,std::clamp(yaw,-mountedlook85::half_arc,mountedlook85::half_arc));
        if(bone_descends(i,left,parents,n)||bone_descends(i,right,parents,n)||weapon)
            for(unsigned r=0;r<3;++r)temp[i*12+r*4+3]+=inverse[r*4+1]*raise;
    }
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(temp[i]))return false;
    memcpy(matrices,temp,n*48);return true;
}
inline bool mounted_carbine_muzzle_world(const float* weapon,const float* world,float* output){
    if(!weapon||!world||!output)return false;
    for(unsigned i=0;i<12;++i)if(!std::isfinite(weapon[i])||!std::isfinite(world[i]))return false;
    float point[3],result[3];
    for(unsigned r=0;r<3;++r){point[r]=weapon[r*4+3];for(unsigned c=0;c<3;++c)point[r]+=weapon[r*4+c]*MOUNTED_CARBINE_MUZZLE[c];}
    for(unsigned r=0;r<3;++r){result[r]=world[r*4+3];for(unsigned c=0;c<3;++c)result[r]+=world[r*4+c]*point[c];if(!std::isfinite(result[r])||std::abs(result[r])>100000)return false;}
    memcpy(output,result,sizeof result);return true;
}
// The verified carbine bore runs along Weapon1 negative X. Correct native
// rider/world lean after mouse aim, around the shoulders, leaving torso/seat
// intact. A bounded correction avoids twisting arms through a reversed view.
inline bool align_mounted_carbine(float* matrices,unsigned n,const int32_t* parents,unsigned left,unsigned right,const unsigned* weapons,const float* world,const float* direction){
    if(!matrices||!parents||!weapons||!world||!direction||!n||n>256||left>=n||right>=n||weapons[0]>=n)return false;
    for(unsigned i=0;i<3;++i)if(weapons[i]>=n||!std::isfinite(direction[i]))return false;
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(matrices[i]))return false;
    float inverse[12];if(!affine_pose_inverse(world,inverse))return false;
    const float* gun=matrices+weapons[0]*12;float from[3]{},to[3],pivot[3],local[3];float f2=0,t2=0;
    for(unsigned r=0;r<3;++r){
        for(unsigned c=0;c<3;++c)from[r]-=world[r*4+c]*gun[c*4];
        to[r]=direction[r];f2+=from[r]*from[r];t2+=to[r]*to[r];
        local[r]=(matrices[left*12+r*4+3]+matrices[right*12+r*4+3])*.5f;
    }
    if(!std::isfinite(f2)||!std::isfinite(t2)||f2<.000001f||t2<.000001f)return false;
    float dot=0;for(unsigned r=0;r<3;++r){from[r]/=std::sqrt(f2);to[r]/=std::sqrt(t2);dot+=from[r]*to[r];
        pivot[r]=world[r*4+3];for(unsigned c=0;c<3;++c)pivot[r]+=world[r*4+c]*local[c];}
    if(!std::isfinite(dot)||dot<.5f)return false;
    dot=std::clamp(dot,-1.f,1.f);
    float v[3]={from[1]*to[2]-from[2]*to[1],from[2]*to[0]-from[0]*to[2],from[0]*to[1]-from[1]*to[0]};
    float k[9]={0,-v[2],v[1],v[2],0,-v[0],-v[1],v[0],0},rotation[12];
    for(unsigned r=0;r<3;++r){
        for(unsigned c=0;c<3;++c){float square=0;for(unsigned q=0;q<3;++q)square+=k[r*3+q]*k[q*3+c];rotation[r*4+c]=(r==c?1.f:0.f)+k[r*3+c]+square/(1+dot);}
        rotation[r*4+3]=pivot[r];for(unsigned c=0;c<3;++c)rotation[r*4+3]-=rotation[r*4+c]*pivot[c];
    }
    float intermediate[12],correction[12];affine_product(rotation,world,intermediate);affine_product(inverse,intermediate,correction);
    float output[256*12];memcpy(output,matrices,n*48);
    for(unsigned i=0;i<n;++i)if(bone_descends(i,left,parents,n)||bone_descends(i,right,parents,n)||i==weapons[0]||i==weapons[1]||i==weapons[2])
        affine_product(correction,matrices+i*12,output+i*12);
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(output[i]))return false;
    memcpy(matrices,output,n*48);return true;
}
