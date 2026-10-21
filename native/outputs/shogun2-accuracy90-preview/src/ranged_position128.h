#pragma once
#include <intrin.h>
// These are two distinct native waits. 1aef50 asks whether any member of
// the unit is still moving before its firing-state decision. 3b2d90 asks
// the personal fire task to wait for that actor's movement to settle.
// Manual control never promises to finish the AI's destination. Exempt
// only its proved ranged subject, only in these firing contexts; do not
// alter the actor's physical flag, formation roster or the unit fire mode.
using RangedSettled128=bool(__cdecl*)(uintptr_t);
static RangedSettled128 original_ranged_settled128;
static RangedGate original_ranged_position128;
static volatile LONG unit_position_bypasses128,task_position_bypasses128;
// Full adapter initialization supplies the camera/lease ownership policy.
// Isolated weapon users retain their narrower personal weapon policy.
static bool(*unit_position_owner147)(uintptr_t);
static bool ranged_position_owned128(uintptr_t actor){
    // This predicate runs over native rosters. Reject unrelated subjects before
    // the full proof (especially its rider/horse branch); never cache authority.
    __try {
        if(!actor||!possessed)return false;
        if(actor!=possessed&&(!weapon_mounted()||actor!=field<uintptr_t>(possessed,0x210)))return false;
        return state&&GetTickCount64()-state->heartbeat<=500&&ranged_subject_owned(actor);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool __cdecl ranged_settled128(uintptr_t actor){
    auto caller=reinterpret_cast<uintptr_t>(_ReturnAddress());
    if(caller==base+0x1aef82&&(unit_position_owner147?unit_position_owner147(actor):ranged_position_owned128(actor))){
        InterlockedIncrement(&unit_position_bypasses128);return true;
    }
    return original_ranged_settled128(actor);
}
static bool __fastcall ranged_position128(void* task,void*){
    bool owned=false;
    __try {owned=ranged_position_owned128(field<uintptr_t>(reinterpret_cast<uintptr_t>(task),4));}
    __except(EXCEPTION_EXECUTE_HANDLER){}
    if(owned){InterlockedIncrement(&task_position_bypasses128);return true;}
    return original_ranged_position128(task);
}
static void create_ranged_position128(){
    const unsigned char subject[]={0x8b,0x44,0x24,4,0x80,0xb8,0x1f,1,0,0,1,0x0f,0x94,0xc0,0xc3};
    const unsigned char task[]={0x8b,0x41,4,0x8a,0x80,0x1f,1,0,0,0xc3};
    const unsigned char call[]={0xe8,0xde,0x0a,0x22,0,0x83,0xc4,4,0x84,0xc0};
    require(!memcmp(reinterpret_cast<void*>(base+0x3cfa60),subject,sizeof subject)&&
        !memcmp(reinterpret_cast<void*>(base+0x3d41d0),task,sizeof task)&&
        !memcmp(reinterpret_cast<void*>(base+0x1aef7d),call,sizeof call),"Native firing position signatures differ");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x3cfa60),reinterpret_cast<void*>(&ranged_settled128),reinterpret_cast<void**>(&original_ranged_settled128))==MH_OK,"Unit firing position hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x3d41d0),reinterpret_cast<void*>(&ranged_position128),reinterpret_cast<void**>(&original_ranged_position128))==MH_OK,"Personal firing position hook failed");
}
