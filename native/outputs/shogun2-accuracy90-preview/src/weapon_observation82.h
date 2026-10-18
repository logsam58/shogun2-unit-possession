#pragma once
#include <intrin.h>
// Retain bounded observations. The reset guard now covers the currently owned
// infantry personal-weapon animation; every other stop/clock forwards.
#include "throw_stop_scope83.h"
struct WeaponObservation82 {
    uint64_t tick;
    unsigned kind,actor,unit,lease,phase,clip,frame,count,animation,mode,pose,task;
    unsigned caller,own,thread,sim_tick,stack[6];
};
static SRWLOCK weapon_observation_lock82=SRWLOCK_INIT;
static WeaponObservation82 weapon_observation_ring82[512]{};
static unsigned weapon_observation_write82,weapon_observation_read82,weapon_observation_dropped82;
static RangedAction original_animation_stop82;
static unsigned weapon_rva82(uintptr_t address){return address>=base&&address-base<0x2000000?unsigned(address-base):0;}
static bool weapon_observation_subject82(uintptr_t actor){
    return state&&(state->weapon_phase||state->aim_pose)&&weapon_owned(actor);
}
static void observe_weapon82(uintptr_t actor,unsigned kind,uintptr_t caller,bool stack=false){
    WeaponObservation82 r{};
    __try {
        if(!weapon_observation_subject82(actor))return;
        r.tick=GetTickCount64();r.kind=kind;r.actor=state->actor_id;r.unit=state->unit_id;
        r.lease=state->lease_generation;r.phase=state->weapon_phase;r.own=own_weapon_call;
        r.thread=GetCurrentThreadId();r.caller=weapon_rva82(caller);
        r.clip=field<unsigned>(actor,0x1d4);r.frame=field<unsigned>(actor,0x1d8);
        r.animation=field<unsigned>(actor,0x474);r.mode=field<unsigned>(actor,0x1f0);
        r.pose=field<unsigned>(actor,0x1d0);r.task=field<unsigned>(actor,0x68c);
        auto common=environment?field<uintptr_t>(environment,8):0;
        if(common)r.sim_tick=field<unsigned>(common,0x50);
        if(r.animation){auto asset=field<uintptr_t>(r.animation,4);if(asset)r.count=field<unsigned>(asset,0x64);}
        if(stack){void* frames[6]{};unsigned n=CaptureStackBackTrace(0,6,frames,nullptr);
            for(unsigned i=0;i<n;++i)r.stack[i]=weapon_rva82(reinterpret_cast<uintptr_t>(frames[i]));}
    }__except(EXCEPTION_EXECUTE_HANDLER){return;}
    AcquireSRWLockExclusive(&weapon_observation_lock82);
    if(weapon_observation_write82-weapon_observation_read82==512){++weapon_observation_read82;++weapon_observation_dropped82;}
    weapon_observation_ring82[weapon_observation_write82++%512]=r;
    ReleaseSRWLockExclusive(&weapon_observation_lock82);
}
static void __fastcall weapon_animation_stop_hook82(void* actor,void*){
    auto p=reinterpret_cast<uintptr_t>(actor),caller=reinterpret_cast<uintptr_t>(_ReturnAddress());
    observe_weapon82(p,3,caller,true);
    if(protect_throw_stop83(p,caller)){observe_weapon82(p,6,caller);return;}
    original_animation_stop82(actor);
    observe_weapon82(p,4,caller);
}
static void log_weapon_observation82(std::ostream& log){
    WeaponObservation82 batch[512];unsigned count,dropped;
    AcquireSRWLockExclusive(&weapon_observation_lock82);
    count=weapon_observation_write82-weapon_observation_read82;dropped=weapon_observation_dropped82;
    for(unsigned i=0;i<count;++i)batch[i]=weapon_observation_ring82[weapon_observation_read82++%512];
    ReleaseSRWLockExclusive(&weapon_observation_lock82);
    for(unsigned i=0;i<count;++i){const auto& r=batch[i];
        log<<"{\"event\":\"weapon_observation82\",\"tick_ms\":"<<r.tick<<",\"kind\":"<<r.kind
           <<",\"actor_id\":"<<r.actor<<",\"unit_id\":"<<r.unit<<",\"lease\":"<<r.lease
           <<",\"weapon_phase\":"<<r.phase<<",\"clip\":"<<r.clip<<",\"frame\":"<<r.frame
           <<",\"asset_frames\":"<<r.count<<",\"animation\":"<<r.animation<<",\"native_mode\":"<<r.mode
           <<",\"native_pose\":"<<r.pose<<",\"native_motion_task\":"<<r.task<<",\"caller_rva\":"<<r.caller
           <<",\"own_weapon_call\":"<<r.own<<",\"thread\":"<<r.thread<<",\"sim_tick\":"<<r.sim_tick
           <<",\"dropped\":"<<dropped<<",\"stack_rvas\":[";
        for(unsigned j=0;j<6;++j){if(j)log<<',';log<<r.stack[j];}log<<"]}\n";
    }
}
