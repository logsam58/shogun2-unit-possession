#pragma once
#include <cmath>
// Native skin matrices are model-space affine3x4; holder+24 is its world3x4.
// Bone3 has an identity bind transform in the installed rig. Its origin is the
// visible arrow attachment. Apply exactly the matrices used by native rendering.
inline bool arrow_attachment_world(const float* arrow,const float* world,float* output){
    if(!arrow||!world||!output)return false;
    for(unsigned i=0;i<12;++i)if(!std::isfinite(arrow[i])||!std::isfinite(world[i]))return false;
    float point[3];
    for(unsigned row=0;row<3;++row){
        point[row]=world[row*4+3];
        for(unsigned col=0;col<3;++col)point[row]+=world[row*4+col]*arrow[col*4+3];
        if(!std::isfinite(point[row])||std::abs(point[row])>100000)return false;
    }
    for(unsigned i=0;i<3;++i)output[i]=point[i];return true;
}
// mesh1.variant_part_mesh: rigid_equip_arrow_lod1..4 are rigid on bone3.
// Reduce only its basis below visible size, preserving translation and all other bones.
// Keep a nonzero basis as in the existing head hiding path, so render normals
// and any native inverse transform remain defined.
inline bool hide_display_arrow(float* matrices,unsigned count,unsigned arrow){
    if(!matrices||count<4||count>256||arrow!=3||arrow>=count)return false;
    float* m=matrices+arrow*12;
    for(unsigned i=0;i<12;++i)if(!std::isfinite(m[i]))return false;
    for(unsigned row=0;row<3;++row)for(unsigned col=0;col<3;++col)m[row*4+col]*=0.0001f;
    return true;
}
