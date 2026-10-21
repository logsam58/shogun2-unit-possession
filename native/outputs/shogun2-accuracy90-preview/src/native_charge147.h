#pragma once
#include "../../shogun2-possession-cleanup-v46/src/charge_protocol146.h"
// The original squad callback assigns each member's OWN scheduled charge.
// Never wait for/copy a peer's personal task: detachment excludes the player
// before that assignment. Native184160/174ec0 choose targets,3508d0 schedules,
// 3824d0/3a9dc0->391830 invoke the proper human/horse virtual154.
static SRWLOCK charge_lock146=SRWLOCK_INIT;
struct ChargeControl146 {
    Charge139 proof{};uintptr_t unit{},squad{},controller{},target_unit{},pair{};
    unsigned physical_serial{},target_id{};bool entered{},assigning{};
};
static ChargeControl146 charge_control146;
static HANDLE charge_map146;static charge146::Shared* charge_shared146;
static volatile LONG charge_entries146,charge_pairs146,charge_returns146,charge_join_failures146;
static volatile LONG charge_callbacks147,charge_scheduled147,charge_rejections147,charge_reject_reason147;
using ChargePlanner147=void(__thiscall*)(void*);
static ChargePlanner147 original_charge_entry147,original_charge_update147;
static void charge_mapping146(){wchar_t name[96];charge146::name(name,GetCurrentProcessId());charge_map146=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(charge146::Shared),name);require(charge_map146!=nullptr,"Native charge observation mapping");charge_shared146=static_cast<charge146::Shared*>(MapViewOfFile(charge_map146,FILE_MAP_ALL_ACCESS,0,0,sizeof(charge146::Shared)));require(charge_shared146!=nullptr,"Native charge observation view");new(charge_shared146)charge146::Shared;}
static void charge_publish146(){
    charge146::Value v{};
    if(charge_control146.proof.active&&state&&possession_lease.active){auto id=possession_lease.identity;v={GetCurrentProcessId(),charge_control146.proof.generation,id.actor_id,id.unit_id,charge_control146.physical_serial,unsigned(id.mount!=0),1,GetTickCount64()};}
    charge146::publish(charge_shared146,v);
}
static void charge_close146(){if(charge_shared146){charge146::publish(charge_shared146,{});UnmapViewOfFile(charge_shared146);charge_shared146=nullptr;}if(charge_map146){CloseHandle(charge_map146);charge_map146=nullptr;}}
static void charge_reset139(){InterlockedExchange(&charge_input_generation146,0);InterlockedExchange(&charge_reset_request139,1);}
static bool charge_member146(uintptr_t actor){
    if(!charge_input_locked146()||!member_owned130(actor))return false;
    AcquireSRWLockShared(&charge_lock146);auto c=charge_control146;ReleaseSRWLockShared(&charge_lock146);
    // A mounted charge belongs to the HORSE. Native459ea0 propagates the rider
    // task; do not insert the detached human into the horse's primary squad.
    return c.proof.active&&c.proof.generation==possession_lease.generation&&c.unit==unit&&actor==c.proof.actor;
}
static uintptr_t charge_squad147(uintptr_t actor){
    if(!charge_member146(actor))return 0;
    AcquireSRWLockShared(&charge_lock146);auto squad=charge_control146.squad;ReleaseSRWLockShared(&charge_lock146);
    return squad_registered130(squad,unit,environment)?squad:0;
}
static bool charge_native_task146(uintptr_t actor){return field<unsigned char>(actor,0x2e8)==1&&field<uintptr_t>(actor,0x250)==base+0x15e0a5c&&field<uintptr_t>(actor,0x254)==actor&&!field<unsigned char>(actor,0x258);}
static bool charge_formation147(uintptr_t actor,bool pending){
    auto task=actor+0x2f8;
    return field<unsigned char>(actor,0x384)==1&&field<uintptr_t>(task,0)==base+0x15dd5cc&&
        field<uintptr_t>(task,8)==actor+0x2f0&&field<uintptr_t>(actor,0x2f0)==actor&&field<uintptr_t>(task,0x18)==environment&&
        (!pending||(!field<unsigned char>(task,0x10)&&!field<unsigned char>(task,0x34)));
}
static bool charge_joined146(uintptr_t actor,uintptr_t expected){auto squad=field<uintptr_t>(actor,0x3b4);return squad==expected&&squad_registered130(squad,unit,environment)&&squad_matches130(squad,actor)==1;}
static bool charge_motion146(uintptr_t actor){__try{auto squad=charge_squad147(actor);return squad&&actor==locomotion_actor()&&charge_joined146(actor,squad);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
static bool charge_admission146(uintptr_t actor){
    return member_owned130(actor)&&actor==locomotion_actor()&&root_motion_subject(actor,false)&&!native_recovery103(actor)&&
        !wall_subject97(possessed)&&!native_airborne_subject99(possessed)&&!equipment_committed81()&&!state->aiming&&!state->aim_pose&&!state->weapon_phase&&
        !equipment_crouched81()&&!command_placing100&&!native_routing(unit)&&field<unsigned>(actor,0x1f0)<=11&&!field<uintptr_t>(actor,0x688)&&
        (!equipment_ranged81||(equipment_slot81==equipment81::Melee&&field<unsigned char>(unit,0xead)));
}
static void charge_end146(){if(charge_control146.proof.active)InterlockedIncrement(&charge_returns146);charge_control146.proof.active=false;InterlockedExchange(&charge_input_generation146,0);charge_publish146();}
static void charge_reject147(unsigned reason){InterlockedIncrement(&charge_rejections147);InterlockedExchange(&charge_reject_reason147,LONG(reason));}
static void charge_initialize147(){
    auto& c=charge_control146;
    if(InterlockedExchange(&charge_reset_request139,0)||c.proof.generation!=possession_lease.generation||c.unit!=unit||c.proof.actor!=locomotion_actor()){
        charge_end146();c={};c.unit=unit;c.proof.generation=possession_lease.generation;c.proof.actor=locomotion_actor();
    }
}
// Caller holds charge_lock146; every address comes from this native callback
// or the independently verified current physical actor, never a cached peer.
static unsigned charge_activate147(uintptr_t actor,uintptr_t squad,uintptr_t target,uintptr_t controller,bool assigning){
    auto& c=charge_control146;
    if(c.proof.active)return 0;
    if(!charge_admission146(actor)){charge_reject147(1);return 0;}
    if(!squad_registered130(squad,unit,environment)){charge_reject147(2);return 0;}
    if(!target||target==unit||field<uintptr_t>(target,0x14)!=environment||!field<unsigned>(target,4)){charge_reject147(3);return 0;}
    c.squad=squad;c.controller=controller;c.target_unit=target;c.target_id=field<unsigned>(target,4);c.physical_serial=field<unsigned>(actor,0x1c8);
    c.entered=charge_native_task146(actor);c.pair=0;c.assigning=assigning;c.proof.active=true;
    InterlockedExchange(&state->follow,0);InterlockedExchange(&entry_movement125,0);InterlockedExchange(&state->input,0);InterlockedExchange(&state->aiming,0);
    InterlockedExchange(&charge_input_generation146,LONG(c.proof.generation));InterlockedIncrement(&charge_starts139);if(c.entered)InterlockedIncrement(&charge_entries146);charge_publish146();return c.proof.generation;
}
static void charge_observe147(){
    auto& c=charge_control146;auto actor=c.proof.actor;
    if(!c.proof.active)return;
    if(!member_owned130(actor)||actor!=locomotion_actor()||field<unsigned>(actor,0x1c8)!=c.physical_serial||native_recovery103(actor)||native_routing(unit)||wall_subject97(possessed)){charge_end146();return;}
    bool task=charge_native_task146(actor),jump=native_mounted_jump140(actor),pending=charge_formation147(actor,true);auto pair=field<uintptr_t>(actor,0x688);
    if(task&&!c.entered){c.entered=true;InterlockedIncrement(&charge_entries146);}
    if(pair&&!c.pair){c.pair=pair;InterlockedIncrement(&charge_pairs146);}
    if(!c.assigning&&((c.pair&&pair!=c.pair)||(!task&&!jump&&!pair&&(!pending||c.entered)))){charge_end146();return;}
    charge_publish146();
}
static unsigned charge_planner_begin147(uintptr_t controller){
    unsigned generation=0;uintptr_t current=unit;
    __try{
        if(!current||(!charge_input_locked146()&&(!state||state->mode!=POSSESSED||!possession_lease.active)))return 0;
        auto squad=field<uintptr_t>(controller,4);
        // Cheap negative path for every other unit; no crowd or peer scan.
        if(!squad||field<uintptr_t>(squad,0x510)!=current)return 0;
        InterlockedIncrement(&charge_callbacks147);
        sync_members130(current);
        // Far players retain a registered independent group for enemy AI,
        // but do not join the distant formation's charge automatically.
        if(proximity_generation147==LONG(possession_lease.generation)&&proximity_joined_generation147!=LONG(possession_lease.generation)){charge_reject147(5);return 0;}
        AcquireSRWLockExclusive(&charge_lock146);
        __try{
            charge_initialize147();
            if(field<uintptr_t>(controller,0x14)!=environment){charge_reject147(4);return 0;}
            generation=charge_activate147(locomotion_actor(),squad,field<uintptr_t>(controller,0x18),controller,true);
        }__finally{ReleaseSRWLockExclusive(&charge_lock146);}
        if(generation){
            sync_members130(current);
            AcquireSRWLockExclusive(&charge_lock146);
            __try{if(charge_control146.proof.generation!=generation||!charge_joined146(locomotion_actor(),charge_control146.squad)){InterlockedIncrement(&charge_join_failures146);charge_end146();generation=0;}}
            __finally{ReleaseSRWLockExclusive(&charge_lock146);}
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){
        InterlockedIncrement(&charge_errors139);
        AcquireSRWLockExclusive(&charge_lock146);charge_end146();ReleaseSRWLockExclusive(&charge_lock146);generation=0;
    }
    return generation;
}
static void charge_planner_finish147(unsigned generation,bool complete){
    if(!generation)return;
    AcquireSRWLockExclusive(&charge_lock146);
    __try{__try{
        auto& c=charge_control146;if(!c.proof.active||c.proof.generation!=generation)return;c.assigning=false;
        if(!complete){charge_end146();return;}
        if(charge_formation147(c.proof.actor,false)&&field<uintptr_t>(c.proof.actor+0x2f8,0x24)==c.target_unit)InterlockedIncrement(&charge_scheduled147);
        charge_observe147();
    }__except(EXCEPTION_EXECUTE_HANDLER){InterlockedIncrement(&charge_errors139);charge_end146();}}
    __finally{ReleaseSRWLockExclusive(&charge_lock146);}
    // Native code selected/scheduled the owned actor or declined naturally.
    // Only after the native callback ends may ordinary detachment resume.
    if(!charge_input_locked146()&&unit)sync_members130(unit);
}
static void __fastcall charge_entry147(void* context,void*){
    unsigned generation=charge_planner_begin147(reinterpret_cast<uintptr_t>(context));bool complete=false;
    __try{original_charge_entry147(context);complete=true;}__finally{charge_planner_finish147(generation,complete);}
}
static void __fastcall charge_update147(void* context,void*){
    unsigned generation=charge_planner_begin147(reinterpret_cast<uintptr_t>(context));bool complete=false;
    __try{original_charge_update147(context);complete=true;}__finally{charge_planner_finish147(generation,complete);}
}
static void charge_boundary146(uintptr_t current_unit,bool after){
    if(current_unit!=unit)return;
    AcquireSRWLockExclusive(&charge_lock146);
    __try{__try{
        charge_initialize147();auto& c=charge_control146;
        if(!member_owned130(possessed)){charge_end146();return;}
        if(c.proof.active){if(after&&!charge_joined146(c.proof.actor,c.squad)){InterlockedIncrement(&charge_join_failures146);charge_end146();}else charge_observe147();return;}
        if(after)return;auto actor=locomotion_actor();
        if(proximity_generation147==LONG(possession_lease.generation)&&proximity_joined_generation147!=LONG(possession_lease.generation))return;
        // Joining an already charging soldier adopts its OWN native action.
        // The scheduled stage precedes pose16/personal task creation.
        if(charge_formation147(actor,true)||charge_native_task146(actor)){
            auto target=charge_formation147(actor,false)?field<uintptr_t>(actor+0x2f8,0x24):field<uintptr_t>(actor+0x250,0x20);
            auto squad=field<uintptr_t>(actor,0x3b4);if(!squad)squad=field<uintptr_t>(unit,0xb9c);
            charge_activate147(actor,squad,target,0,false);
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){InterlockedIncrement(&charge_errors139);charge_end146();}}
    __finally{ReleaseSRWLockExclusive(&charge_lock146);}
}
static void create_native_charge147(){
    const unsigned char entry[]={0x83,0xec,0x6c,0x53,0x56,0x57,0x8b,0xf9};
    const unsigned char update[]={0x55,0x8b,0xec,0x83,0xe4,0xf0,0x81,0xec,0x84,0,0,0};
    const unsigned char dispatch[]={0x8b,0x41,8,0x8b,8,0x8b,1,0xff,0xa0,0x54,1,0,0};
    require(!memcmp(reinterpret_cast<void*>(base+0x184160),entry,sizeof entry)&&!memcmp(reinterpret_cast<void*>(base+0x1aa870),update,sizeof update)&&!memcmp(reinterpret_cast<void*>(base+0x391830),dispatch,sizeof dispatch),"Native charge assignment signatures differ");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x184160),reinterpret_cast<void*>(&charge_entry147),reinterpret_cast<void**>(&original_charge_entry147))==MH_OK,"Native charge assignment hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x1aa870),reinterpret_cast<void*>(&charge_update147),reinterpret_cast<void**>(&original_charge_update147))==MH_OK,"Native charge matching hook failed");
}
static void charge_log139(std::ostream& log){
    static LONG starts=-1,returns=-1,entries=-1,pairs=-1,errors=-1,callbacks=-1,rejections=-1;
    if(starts==charge_starts139&&returns==charge_returns146&&entries==charge_entries146&&pairs==charge_pairs146&&errors==charge_errors139&&callbacks==charge_callbacks147&&rejections==charge_rejections147)return;
    starts=charge_starts139;returns=charge_returns146;entries=charge_entries146;pairs=charge_pairs146;errors=charge_errors139;callbacks=charge_callbacks147;rejections=charge_rejections147;
    log<<"{\"event\":\"native_charge147\",\"tick_ms\":"<<GetTickCount64()<<",\"planner_callbacks\":"<<callbacks<<",\"native_handoffs\":"<<starts<<",\"scheduled_actions\":"<<charge_scheduled147<<",\"native_task_entries\":"<<entries<<",\"native_pairs\":"<<pairs<<",\"returns\":"<<returns<<",\"on_rails\":"<<charge_input_locked146()<<",\"join_failures\":"<<charge_join_failures146<<",\"rejections\":"<<rejections<<",\"last_rejection\":"<<charge_reject_reason147<<",\"errors\":"<<errors<<"}\n";
}
