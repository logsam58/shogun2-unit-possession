#pragma once
#include "terrain_math.h"
using NativeGroundHeight=float(__thiscall*)(void*,const float*,void*);
static bool native_terrain_signatures(uintptr_t b){
    const unsigned char height[]={0x8b,0x54,0x24,8,0x85,0xd2,0x75,0x12,0xff,0x74,0x24,4};
    const unsigned char grade_store[]={0xf3,0x0f,0x11,0x83,0xb8,1,0,0};
    return !memcmp(reinterpret_cast<void*>(b+0x17c0a0),height,sizeof height)&&
        !memcmp(reinterpret_cast<void*>(b+0x3aa605),grade_store,sizeof grade_store)&&
        field<float>(b,0x15c2614)==0.01f&&field<float>(b,0x15c26d4)==3.0f&&
        field<float>(b,0x15c26ac)==1.5f&&field<float>(b,0x15ac9c8)==1.0f;
}
static bool sample_native_grade(uintptr_t actor,Intent direction,float dt,float& grade,float& factor,float& distance){
    __try {
        grade=0;factor=1;distance=0;
        if(direction.x==0&&direction.z==0)return true;
        if(field<uintptr_t>(actor,0x18)||field<unsigned char>(actor,0xa2)||
            field<unsigned char>(actor,0xa3)||field<unsigned char>(actor,0xa4))return false;
        // The native controller clamps its previous animation-driven velocity
        // to +110, then probes (previous speed + +160 acceleration)*dt ahead.
        float previous=field<float>(actor,0x1ac),maximum=field<float>(actor,0x110);
        float acceleration=field<float>(actor,0x160),radius=field<float>(actor,0x58);
        if(!std::isfinite(previous)||!std::isfinite(maximum)||!std::isfinite(acceleration)||
            !std::isfinite(radius)||maximum<0||maximum>100||acceleration<0||acceleration>100||radius<0.01f||radius>10)return false;
        previous=std::clamp(previous,0.0f,maximum);
        distance=native_grade_probe(previous,acceleration,dt,radius,field<float>(base,0x15c2614));
        float position[2]={field<float>(actor,0x48)+direction.x*distance,
                           field<float>(actor,0x50)+direction.z*distance};
        float current_height=field<float>(actor,0x4c);
        if(!std::isfinite(position[0])||!std::isfinite(position[1])||!std::isfinite(current_height))return false;
        // Same context and surface object as native RVA3aa5e5. Supports both the
        // heightfield and the game's building/bridge plane through its own API.
        if(field<uintptr_t>(actor,0x28)!=environment)return false;
        uintptr_t common=field<uintptr_t>(environment,8),surface=field<uintptr_t>(actor,0x10);
        if(!common||common!=possession_lease.identity.common)return false;
        float height=reinterpret_cast<NativeGroundHeight>(base+0x17c0a0)(
            reinterpret_cast<void*>(common),position,reinterpret_cast<void*>(surface));
        grade=(height-current_height)/distance;
        if(!std::isfinite(height)||!std::isfinite(grade)||std::abs(grade)>100)return false;
        factor=native_grade_factor(grade,field<float>(base,0x15c26d4),field<float>(base,0x15c26ac));
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool apply_native_terrain(uintptr_t actor,Intent direction,float dt,float& speed){
    __try {
        float grade=0,factor=1,distance=0;
        if(!sample_native_grade(actor,direction,dt,grade,factor,distance))return false;
        state->ungraded_speed=speed;
        speed*=factor;
        if(!std::isfinite(speed)||speed<0||speed>100)return false;
        state->terrain_grade=grade;state->terrain_speed_factor=factor;
        state->terrain_probe_distance=distance;state->effective_speed=speed;
        ++state->terrain_queries;return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
