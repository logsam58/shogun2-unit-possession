#pragma once
// Test82: every failed ninja shot was stopped by reset-mode's CALL return
// RVA3e93c8 at frame0 of the 35-frame native sli_throw asset. Protect only
// that reset, for the leased personal sling pose, while RMB ownership is live.
// Native completion, explicit lowering, impacts/death and other actors forward.
static bool protect_throw_stop83(uintptr_t actor,uintptr_t caller){
    __try {
        if(caller!=base+0x3e93c8||own_weapon_call||!weapon_owned(actor)||weapon_mounted()||
           equipment_slot81!=equipment81::Ranged||!weapon_ready_held()||field<uintptr_t>(actor,0x688)||
           field<unsigned>(actor,0x1f0)!=14)return false;
        auto animation=field<uintptr_t>(actor,0x474);if(!animation)return false;
        unsigned id=field<unsigned>(animation,0);
        bool hold=!state->weapon_phase&&state->aim_pose&&id==124;
        bool firing=actor==weapon_actor&&weapon_clip==130&&id==130&&
            (state->weapon_phase==WEAPON_DRAW||state->weapon_phase==WEAPON_FIRED);
        if(!hold&&!firing)return false;
        auto db=field<uintptr_t>(actor,0x23c),array=db?field<uintptr_t>(db,0xc):0;
        auto fire=array?field<uintptr_t>(array,130*4):0;
        if(!fire||field<unsigned>(fire,0)!=130)return false;
        auto asset=field<uintptr_t>(fire,4);if(!asset||field<unsigned>(asset,0x64)!=35)return false;
        static const wchar_t path[]=L"animations/shogun_animation/battle/men/sling/sling_poise/sli_throw.anim";
        auto n=field<unsigned>(asset,8),chars=field<uintptr_t>(asset,16);
        if(n!=sizeof(path)/sizeof(wchar_t)-1||!chars||memcmp(reinterpret_cast<void*>(chars),path,n*sizeof(wchar_t)))return false;
        auto active_asset=field<uintptr_t>(animation,4);
        return active_asset&&field<unsigned>(actor,0x1d8)<field<unsigned>(active_asset,0x64);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
