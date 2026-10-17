#pragma once
#include "seated_weapon_geometry.h"
// Called only under the skin-pose lock, before mouse-look and head hiding.
// A constant simulation frame can still receive changing interpolated palettes.
// Hold the owned draw's upper palette relative to its current native pelvis.
namespace heldpose {
struct Key {
    uintptr_t actor,asset,binding;
    unsigned lease,draw;
    bool operator==(const Key& b)const{return actor==b.actor&&asset==b.asset&&binding==b.binding&&lease==b.lease&&draw==b.draw;}
};
struct Cache {
    bool valid=false;Key key{};unsigned count=0,anchor=0,spine=0;unsigned weapons[3]{};
    unsigned char mask[256]{};int32_t hierarchy[256]{};float relative[256*12]{};
    void reset(){valid=false;}
};
inline bool apply(Cache& cache,const Key& key,float* matrices,unsigned n,const int32_t* parents,unsigned spine,const unsigned* weapons){
    auto reject=[&](){cache.reset();return false;};
    if(!matrices||!parents||!weapons||!key.actor||!key.asset||!key.binding||!n||n>256||spine>=n)return reject();
    for(unsigned i=0;i<n;++i)if(parents[i]>=int32_t(i)||parents[i]<-1)return reject();
    if(parents[spine]<0)return reject();auto anchor=unsigned(parents[spine]);
    if(parents[anchor]>=0)return reject(); // Verified Hips -> Spine topology.
    for(unsigned i=0;i<3;++i)if(weapons[i]>=n||parents[weapons[i]]!=-1)return reject();
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(matrices[i])||std::abs(matrices[i])>100000)return reject();
    float inverse[12];if(!affine_pose_inverse(matrices+anchor*12,inverse))return reject();
    if(!cache.valid||!(cache.key==key)||cache.count!=n||cache.anchor!=anchor||cache.spine!=spine||memcmp(cache.weapons,weapons,3*sizeof(unsigned))||memcmp(cache.hierarchy,parents,n*sizeof(int32_t))){
        cache.reset();cache.key=key;cache.count=n;cache.anchor=anchor;cache.spine=spine;
        memcpy(cache.hierarchy,parents,n*sizeof(int32_t));memcpy(cache.weapons,weapons,3*sizeof(unsigned));
        for(unsigned i=0;i<n;++i){
            cache.mask[i]=bone_descends(i,spine,parents,n)||i==weapons[0]||i==weapons[1]||i==weapons[2];
            if(cache.mask[i])affine_product(inverse,matrices+i*12,cache.relative+i*12);
        }
        cache.valid=true;
    }
    float output[256*12];memcpy(output,matrices,n*48);
    for(unsigned i=0;i<n;++i)if(cache.mask[i])affine_product(matrices+anchor*12,cache.relative+i*12,output+i*12);
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(output[i])||std::abs(output[i])>100000)return reject();
    memcpy(matrices,output,n*48);return true;
}
}
