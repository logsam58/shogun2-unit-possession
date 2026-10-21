#pragma once
#include <intrin.h>
// Native deployment work (1b6670 -> 14efc0 -> 14ef80) waits on every
// actor's movement task (3cbe40). Manual control cannot finish its AI slot.
// Exempt that actor only at the aggregate call. Keep actual movement/task
// bytes, rosters, peer arrival, fire policy and other callers unchanged.
using FormationArrival129=bool(__thiscall*)(void*);
static FormationArrival129 original_formation_arrival129;
static volatile LONG formation_bypasses129;
static bool formation_owned129(uintptr_t actor){
    __try {
        if(!actor||!possessed||!possession_lease.active)return false;
        auto principal=possession_lease.identity.mount;
        if(actor!=possessed&&(!principal||actor!=principal))return false;
        // A dismount camera continuation never grants the old horse manual
        // movement/formation authority. Native dismount owns this interval.
        if(dismount_native_motion143())return false;
        if(!state||!enabled||state->stop||state->release||state->mode!=POSSESSED||
           GetTickCount64()-state->heartbeat>500||dead_view()||
           field<unsigned char>(possessed,0xf0)!=1)return false;
        // Full current camera/selection/holder/serial/mount proof, without
        // acquiring authority or tying formation readiness to a weapon pose.
        return followed(possessed,false,false);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool __fastcall formation_arrival129(void* subject,void*){
    if(reinterpret_cast<uintptr_t>(_ReturnAddress())==base+0x14ef98&&
       formation_owned129(reinterpret_cast<uintptr_t>(subject))){
        InterlockedIncrement(&formation_bypasses129);return true;
    }
    return original_formation_arrival129(subject);
}
static void create_formation_arrival129(){
    unit_position_owner147=&formation_owned129;
    const unsigned char predicate[]={0x80,0xb9,0x84,3,0,0,1,0x75,0x23,0x80,0xb9,8,3,0,0,1};
    const unsigned char call[]={0x8b,0x0e,0xe8,0xa8,0xce,0x27,0,0x84,0xc0};
    require(!memcmp(reinterpret_cast<void*>(base+0x3cbe40),predicate,sizeof predicate)&&
        !memcmp(reinterpret_cast<void*>(base+0x14ef91),call,sizeof call),"Native formation arrival signatures differ");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x3cbe40),reinterpret_cast<void*>(&formation_arrival129),
        reinterpret_cast<void**>(&original_formation_arrival129))==MH_OK,"Formation arrival hook failed");
}
