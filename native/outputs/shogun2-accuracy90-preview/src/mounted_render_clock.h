#pragma once
// Native ABI verified in the local engine assembly:
// 104dd950: ECX=holder, eight 4-byte stack arguments, RET 20h.
// 11356000: ECX=animation, five stack arguments, time at ESP+8, RET 14h.
// Freeze the owned rider's animation sample time, not the shared asset or world
// clock. The holder scope covers both camera evaluation and visible skinning.
using HolderRefresh70=void(__thiscall*)(void*,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,float,uintptr_t);
using SamplePose70=uintptr_t(__thiscall*)(void*,uintptr_t,float,uintptr_t,uintptr_t,uintptr_t);
static HolderRefresh70 original_holder_refresh70;
static SamplePose70 original_sample_pose70;
struct RenderHold70 {uintptr_t holder,actor,animation;unsigned lease,draw;float time;};
static __declspec(thread) RenderHold70 render_hold70;
static volatile LONG render_scopes70,render_clamps70;
struct RenderSample70 {unsigned actor,lease,draw;uintptr_t holder,animation;float native_time,held_time;uint64_t tick;};
static RenderSample70 render_sample70;
static SRWLOCK render_sample_lock70=SRWLOCK_INIT;

static RenderHold70 render_hold_for70(uintptr_t holder){
    RenderHold70 out{};
    __try {
        if(!holder||!state||!possessed||field<uintptr_t>(holder,8)!=possessed||!mounted_clock_held(possessed))return out;
        auto descriptor=field<uintptr_t>(mounted_draw.animation,4);
        auto animation=field<uintptr_t>(descriptor,0x50);
        float step=field<float>(base,0x15b2448),time=mounted_draw.hold_frame*step;
        if(!animation||!std::isfinite(step)||step<=0||step>.25f||!std::isfinite(time)||time<0||
           mounted_draw.hold_frame>=field<unsigned>(descriptor,0x64)||time>=mounted_draw.release||
           !std::isfinite(field<float>(animation,0x48))||field<float>(animation,0x48)<=0||
           !std::isfinite(field<float>(animation,0x4c))||time>=field<float>(animation,0x4c))return out;
        out={holder,possessed,animation,state->lease_generation,state->draws_started,time};
    }__except(EXCEPTION_EXECUTE_HANDLER){}
    return out;
}
static bool render_hold_valid70(const RenderHold70& scope,uintptr_t animation){
    if(!scope.actor||animation!=scope.animation)return false;
    __try {
        return state&&scope.actor==possessed&&scope.lease==state->lease_generation&&scope.draw==state->draws_started&&
            field<uintptr_t>(scope.holder,8)==scope.actor&&mounted_clock_held(scope.actor)&&
            field<uintptr_t>(field<uintptr_t>(mounted_draw.animation,4),0x50)==animation;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void __fastcall holder_refresh_hook70(void* holder,void*,uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d,
                                            uintptr_t e,uintptr_t f,float g,uintptr_t h){
    auto previous=render_hold70;
    // An unrelated nested holder must mask, rather than inherit, our scope.
    render_hold70=render_hold_for70(reinterpret_cast<uintptr_t>(holder));
    if(render_hold70.actor)InterlockedIncrement(&render_scopes70);
    __try {original_holder_refresh70(holder,a,b,c,d,e,f,g,h);}
    __finally {render_hold70=previous;}
}
static uintptr_t __fastcall sample_pose_hook70(void* animation,void*,uintptr_t a,float time,uintptr_t b,uintptr_t c,uintptr_t d){
    if(std::isfinite(time)&&render_hold_valid70(render_hold70,reinterpret_cast<uintptr_t>(animation))){
        RenderSample70 sample{state->actor_id,render_hold70.lease,render_hold70.draw,render_hold70.holder,
                              render_hold70.animation,time,render_hold70.time,GetTickCount64()};
        time=render_hold70.time;InterlockedIncrement(&render_clamps70);
        AcquireSRWLockExclusive(&render_sample_lock70);render_sample70=sample;ReleaseSRWLockExclusive(&render_sample_lock70);
    }
    return original_sample_pose70(animation,a,time,b,c,d);
}
static void log_mounted_render70(std::ostream& log){
    static LONG last=-1;LONG count=render_clamps70;if(count==last)return;last=count;
    RenderSample70 sample;AcquireSRWLockShared(&render_sample_lock70);sample=render_sample70;ReleaseSRWLockShared(&render_sample_lock70);
    log<<"{\"event\":\"mounted_render70\",\"tick_ms\":"<<GetTickCount64()<<",\"scopes\":"<<render_scopes70<<",\"clamps\":"<<count
       <<",\"sample_tick\":"<<sample.tick<<",\"actor\":"<<sample.actor<<",\"lease\":"<<sample.lease<<",\"draw\":"<<sample.draw
       <<",\"holder\":"<<sample.holder<<",\"animation\":"<<sample.animation<<",\"native_time\":"<<sample.native_time<<",\"held_time\":"<<sample.held_time<<"}\n";
}
static void create_mounted_render_hooks70(){
    const unsigned char holder[]={0x55,0x8b,0xec,0x83,0xe4,0xf0,0x81,0xec,0xb8,1,0,0};
    const unsigned char sample[]={0xf3,0x0f,0x10,0x44,0x24,8,0x8d,0x44,0x24,0xf8};
    require(!memcmp(reinterpret_cast<void*>(base+0x4dd950),holder,sizeof holder)&&
            !memcmp(reinterpret_cast<void*>(base+0x1356000),sample,sizeof sample),"Mounted render sample signatures differ");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x4dd950),reinterpret_cast<void*>(&holder_refresh_hook70),reinterpret_cast<void**>(&original_holder_refresh70))==MH_OK,"Mounted holder scope hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x1356000),reinterpret_cast<void*>(&sample_pose_hook70),reinterpret_cast<void**>(&original_sample_pose70))==MH_OK,"Mounted render clock hook failed");
}
