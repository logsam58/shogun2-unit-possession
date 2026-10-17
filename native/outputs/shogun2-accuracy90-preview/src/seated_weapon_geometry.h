#pragma once
#include "pose_geometry.h"
#include "seated_bow_asset.h"
inline void affine_product(const float* a,const float* b,float* out){
    for(unsigned r=0;r<3;++r)for(unsigned c=0;c<4;++c){float v=c==3?a[r*4+3]:0;for(unsigned k=0;k<3;++k)v+=a[r*4+k]*b[k*4+c];out[r*4+c]=v;}
}
inline bool affine_rigid_inverse(const float* a,float* out){
    if(!a||!out)return false;
    for(unsigned i=0;i<12;++i)if(!std::isfinite(a[i]))return false;
    for(unsigned r=0;r<3;++r)for(unsigned c=0;c<3;++c){float dot=0;for(unsigned k=0;k<3;++k)dot+=a[k*4+r]*a[k*4+c];if(std::abs(dot-(r==c?1.f:0.f))>.005f)return false;out[r*4+c]=a[c*4+r];}
    for(unsigned r=0;r<3;++r){out[r*4+3]=0;for(unsigned k=0;k<3;++k)out[r*4+3]-=out[r*4+k]*a[k*4+3];}return true;
}
// Native blends and actor size may produce finite affine transforms rather
// than exactly orthonormal rotations. Invert those mathematically, with bounds
// on coefficients, determinant and conditioning; do not merely loosen epsilon.
inline bool affine_pose_inverse(const float* a,float* out){
    if(!a||!out)return false;
    for(unsigned i=0;i<12;++i)if(!std::isfinite(a[i])||std::abs(a[i])>100000)return false;
    for(unsigned r=0;r<3;++r)for(unsigned c=0;c<3;++c)if(std::abs(a[r*4+c])>8)return false;
    const float adj[9]={a[5]*a[10]-a[6]*a[9],a[2]*a[9]-a[1]*a[10],a[1]*a[6]-a[2]*a[5],
        a[6]*a[8]-a[4]*a[10],a[0]*a[10]-a[2]*a[8],a[2]*a[4]-a[0]*a[6],
        a[4]*a[9]-a[5]*a[8],a[1]*a[8]-a[0]*a[9],a[0]*a[5]-a[1]*a[4]};
    const float det=a[0]*adj[0]+a[1]*adj[3]+a[2]*adj[6];
    if(!std::isfinite(det)||det<.000001f)return false;
    float norm=0,inverse_norm=0;
    for(unsigned r=0;r<3;++r){float row=0,inv_row=0;
        for(unsigned c=0;c<3;++c){out[r*4+c]=adj[r*3+c]/det;row+=std::abs(a[r*4+c]);inv_row+=std::abs(out[r*4+c]);}
        norm=std::max(norm,row);inverse_norm=std::max(inverse_norm,inv_row);
    }
    if(norm*inverse_norm>64)return false;
    for(unsigned r=0;r<3;++r){out[r*4+3]=0;for(unsigned c=0;c<3;++c)out[r*4+3]-=out[r*4+c]*a[c*4+3];}
    for(unsigned i=0;i<12;++i)if(!std::isfinite(out[i]))return false;
    return true;
}
enum SeatFailure {SeatOk=0,SeatArguments=40,SeatCount=41,SeatHierarchy=42,SeatWorld=43,SeatHips=44,SeatLayout=45,SeatFinite=46};
// Reuse the unit's infantry ready/draw/release animation, preserving its arms,
// fingers and native equipment events. Only the owned rider is seated again.
// Native horse root motion, other soldiers and native reload poses are untouched.
inline bool seat_personal_weapon(float* matrices,unsigned n,const int32_t* parents,const float* world=nullptr,unsigned* failure=nullptr){
    if(failure)*failure=SeatOk;
    auto reject=[&](unsigned code){if(failure)*failure=code;return false;};
    if(!matrices||!parents)return reject(SeatArguments);
    if(n!=48)return reject(SeatCount);
    if(memcmp(parents,SEATED_BOW_PARENTS,sizeof SEATED_BOW_PARENTS))return reject(SeatHierarchy);
    float temp[48*12],inverse_world[12];
    if(world&&!affine_pose_inverse(world,inverse_world))return reject(SeatWorld);
    for(unsigned i=0;i<n;++i){
        for(unsigned k=0;k<12;++k)if(!std::isfinite(matrices[i*12+k]))return reject(SeatFinite);
        if(world)affine_product(inverse_world,matrices+i*12,temp+i*12);else memcpy(temp+i*12,matrices+i*12,48);
    }
    float inverse_hips[12],to_seat[12];if(!affine_pose_inverse(temp,inverse_hips))return reject(SeatHips);
    affine_product(SEATED_BOW_MODEL,inverse_hips,to_seat);
    // The recovered 48-bone human rig has Spine7 and the three root weapons1..3.
    if(parents[7]!=0||parents[1]!=-1||parents[2]!=-1||parents[3]!=-1)return reject(SeatLayout);
    for(unsigned i=0;i<n;++i){float m[12];
        if(bone_descends(i,7,parents,n)||(i>=1&&i<=3))affine_product(to_seat,temp+i*12,m);
        else memcpy(m,SEATED_BOW_MODEL+i*12,48);
        if(world)affine_product(world,m,temp+i*12);else memcpy(temp+i*12,m,48);
    }
    for(float v:temp)if(!std::isfinite(v))return reject(SeatFinite);
    memcpy(matrices,temp,sizeof temp);return true;
}
inline bool infantry_personal_clip(unsigned clip){return clip==124||clip==126||clip==130;}
