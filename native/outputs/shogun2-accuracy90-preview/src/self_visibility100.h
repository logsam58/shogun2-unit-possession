#pragma once
#include "pose_geometry.h"
namespace visibility100 {
// Only presentation matrices change. Native head/shoulder caches must be sampled
// before this pass; camera, physics, collision and projectile sockets are separate.
inline bool hide(float* visible,const float* native,unsigned n,const int32_t* parents,unsigned head,unsigned neck,unsigned left,unsigned right,const unsigned* weapons){
    if(!visible||!native||!parents||!weapons||!n||n>256||head>=n||neck>=n||left>=n||right>=n||head==left||head==right)return false;
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(native[i])||!std::isfinite(visible[i]))return false;
    for(unsigned i=0;i<n;++i){
        if(parents[i]>=int32_t(n))return false;
        unsigned cursor=i;bool terminated=false;
        for(unsigned k=0;k<n;++k){int32_t parent=parents[cursor];if(parent<0){terminated=true;break;}cursor=unsigned(parent);}
        if(!terminated)return false;
    }
    unsigned root=bone_descends(head,neck,parents,n)?neck:head;
    auto protected_branch=[&](unsigned candidate){
        if(bone_descends(left,candidate,parents,n)||bone_descends(right,candidate,parents,n))return true;
        for(unsigned i=0;i<3;++i)if(weapons[i]<n&&bone_descends(weapons[i],candidate,parents,n))return true;
        return false;
    };
    if(protected_branch(root))root=head;if(protected_branch(root))return false;
    const float anchor[3]={native[root*12+3],native[root*12+7],native[root*12+11]};
    for(unsigned bone=0;bone<n;++bone)if(bone_descends(bone,root,parents,n)){
        for(unsigned row=0;row<3;++row){
            for(unsigned col=0;col<3;++col)visible[bone*12+row*4+col]=native[bone*12+row*4+col]*.0001f;
            visible[bone*12+row*4+3]=anchor[row];
        }
    }
    return true;
}
}
