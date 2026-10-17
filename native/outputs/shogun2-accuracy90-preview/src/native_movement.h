#pragma once
// Verified engine functions use ECX=the actor's native unit and x87 float returns.
using NativeIntent=void(__thiscall*)(void*,const float*,unsigned);
using UnitSpeed= float(__thiscall*)(void*,unsigned);
using UnitRunAllowed=bool(__thiscall*)(void*);
static bool native_routing(uintptr_t actor_unit){
    // unit:is_routing binding RVA1c1bd0 calls this on unit+cc4.
    auto predicate=reinterpret_cast<UnitRunAllowed>(base+0x1821e0);
    return predicate(reinterpret_cast<void*>(actor_unit+0xcc4));
}
static bool native_movement_signatures(uintptr_t b){
    const unsigned char intent[]={0x83,0x79,0x18,0,0x75,0x60,0x8b,0x44,0x24,4};
    const unsigned char walk[]={0x8b,0x81,0x9c,0x0f,0,0,0xd9,0x40,0x1c};
    const unsigned char run[]={0x8b,0x81,0x9c,0x0f,0,0,0xd9,0x40,0x20};
    const unsigned char wrapper[]={0x80,0x7c,0x24,4,0,0x56,0x8b,0xf1};
    const unsigned char routing[]={0x8b,0x01,0x8b,0x40,8,0x8b,0x80,0xb4,0,0,0};
    return !memcmp(reinterpret_cast<void*>(b+0x3936f0),intent,sizeof intent)&&
        !memcmp(reinterpret_cast<void*>(b+0x1be960),walk,sizeof walk)&&
        !memcmp(reinterpret_cast<void*>(b+0x1a9da0),run,sizeof run)&&
        !memcmp(reinterpret_cast<void*>(b+0x18b470),wrapper,sizeof wrapper)&&
        !memcmp(reinterpret_cast<void*>(b+0x1821e0),routing,sizeof routing);
}
static bool read_native_movement(unsigned input,float& speed,float& turn_rate){
    __try {
        if(!followed(possessed,false))return false;
        auto getter=reinterpret_cast<UnitSpeed>(base+0x18b470);
        // This is the game's own walk/run selector: running is rejected by its
        // native predicate, and both branches include the unit's live +0x22c factor.
        float walk=getter(reinterpret_cast<void*>(unit),0);
        speed=getter(reinterpret_cast<void*>(unit),(input&RUN)?1:0);
        if(!std::isfinite(walk)||!std::isfinite(speed)||walk<0||speed<0||walk>100||speed>100)return false;
        // Exact native UTF-16 entity keys and DB values were read back from the
        // current infantry. Config+6c stores turn_speed converted to radians/s.
        // Read this soldier's configuration, never substitute another unit's rig.
        turn_rate=field<float>(locomotion_config(),0x6c);
        if(!std::isfinite(turn_rate)||turn_rate<0||turn_rate>100)return false;
        state->native_walk_speed=walk;state->effective_speed=speed;
        state->native_movement_factor=field<float>(unit,0x22c);
        state->native_turn_rate=turn_rate;
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}

static bool issue_native_intent(uintptr_t actor,const Intent& intent,unsigned bearing){
    float x=field<float>(actor,0x48),z=field<float>(actor,0x50);
    float target[2]={x+intent.x,z+intent.z};
    if(!std::isfinite(x)||!std::isfinite(z)||!std::isfinite(target[0])||!std::isfinite(target[1]))return false;
    unsigned heading=static_cast<unsigned short>(bearing+intent.turn);
    // Actual native API, not a copied solver: it establishes horizontal intent,
    // yaw delta, magnitude and the animation-drive flag. The original integrator
    // and caller still own collisions, terrain, actual velocity and animation.
    reinterpret_cast<NativeIntent>(base+0x3936f0)(reinterpret_cast<void*>(actor),target,heading);
    state->native_intent_flag=field<unsigned char>(actor,0x198);
    state->native_intent_magnitude=field<float>(actor,0x150);
    state->native_intent_x=field<float>(actor,0xd8);state->native_intent_z=field<float>(actor,0xe0);
    if(state->native_intent_flag!=1||!std::isfinite(state->native_intent_magnitude)||
        !std::isfinite(state->native_intent_x)||!std::isfinite(state->native_intent_z))return false;
    ++state->native_intent_calls;return true;
}
