#pragma once
#include "seated_weapon_geometry.h"
// Observe the native camera's existing bone calculation; no second animation rig.
using RenderEye=void*(__thiscall*)(void*,void*);
using BoneMatrices=void*(__thiscall*)(void*,unsigned*,const float*,unsigned);
static RenderEye original_render_eye;
static BoneMatrices original_bone_matrices;
static __declspec(thread) uintptr_t captured_render_holder;
static uintptr_t captured_rig_actor;
static void* __fastcall bone_matrices_hook(void* source,void*,unsigned* count,const float* world,unsigned allocator){
    void* result=original_bone_matrices(source,count,world,allocator);
    if(captured_render_holder&&enabled&&!state->stop&&state->mode==POSSESSED){
        __try {
            uintptr_t holder=captured_render_holder,binding=field<uintptr_t>(holder,0x1e4);
            if(field<uintptr_t>(holder,8)!=possessed||!binding||!result)return result;
            uintptr_t matrices=field<uintptr_t>(reinterpret_cast<uintptr_t>(result),8);
            unsigned n=field<unsigned>(reinterpret_cast<uintptr_t>(source),0x10);if(count&&*count)n=std::min(n,*count);
            // Observe the native rider head; never re-seat the eye array.
            float* destinations[4]={state->rig_head,state->rig_right,state->rig_left,state->rig_weapon};unsigned mask=0;
            const unsigned fields[4]={0x9c,0xa4,0xa8,0xcc};
            for(unsigned i=0;i<4;++i){uintptr_t bone=field<uintptr_t>(binding,fields[i]);if(!bone)continue;
                unsigned index=field<unsigned>(bone,4);if(index>=n||index>255||!matrices)continue;
                float p[3]={field<float>(matrices,index*48+0xc),field<float>(matrices,index*48+0x1c),field<float>(matrices,index*48+0x2c)};
                bool valid=true;for(unsigned k=0;k<3;++k)if(!std::isfinite(p[k])||std::abs(p[k]-field<float>(possessed,0x48+k*4))>4)valid=false;
                if(valid){for(unsigned k=0;k<3;++k)destinations[i][k]=p[k];mask|=1u<<i;}
            }
            state->bone_valid=mask;state->bone_tick=GetTickCount64();captured_rig_actor=possessed;++state->bone_samples;
        }__except(EXCEPTION_EXECUTE_HANDLER){state->interface_error=105;}
    }
    return result;
}
static void* __fastcall render_eye_hook(void* holder,void*,void* out){
    captured_render_holder=0;
    __try {if(enabled&&state&&!state->stop&&state->mode==POSSESSED&&field<uintptr_t>(reinterpret_cast<uintptr_t>(holder),8)==possessed)
        captured_render_holder=reinterpret_cast<uintptr_t>(holder);}
    __except(EXCEPTION_EXECUTE_HANDLER){}
    void* result=nullptr;
    __try {result=original_render_eye(holder,out);}
    __finally {captured_render_holder=0;}
    return result;
}
static void create_rig_hooks(){
    const unsigned char eye[]={0x81,0xec,0x8c,0,0,0,0x56,0x57};
    const unsigned char bones[]={0x55,0x8b,0xec,0x83,0xe4,0xf0,0x8b,0x45};
    require(memcmp(reinterpret_cast<void*>(base+0x4cd620),eye,8)==0&&memcmp(reinterpret_cast<void*>(base+0x1355650),bones,8)==0,"Render rig signatures differ");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x4cd620),reinterpret_cast<void*>(&render_eye_hook),reinterpret_cast<void**>(&original_render_eye))==MH_OK,"Render eye hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x1355650),reinterpret_cast<void*>(&bone_matrices_hook),reinterpret_cast<void**>(&original_bone_matrices))==MH_OK,"Bone matrix hook failed");
}
