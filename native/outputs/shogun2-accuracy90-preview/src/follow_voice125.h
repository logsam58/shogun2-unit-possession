#pragma once
// Filter only the native order-acknowledgement subscriber. All event dispatch,
// movement, formation and native audio selection remain original. Delayed
// callbacks are judged by the current, freshly proved Follow Me ownership.
using MoveApply125=void(__thiscall*)(void*,uintptr_t,const MovePoint100*,unsigned,unsigned,unsigned);
using OrderVoice125=void(__thiscall*)(void*,const void*);
static OrderVoice125 original_order_voice125;
static SRWLOCK follow_voice_lock125=SRWLOCK_INIT;
struct FollowVoiceKey127 {unsigned lease{},actor{},unit_id{};uintptr_t unit_pointer{};};
static FollowVoiceKey127 follow_voice_key127;
static bool follow_acknowledged125;
static void command_voice_reset125(){
    AcquireSRWLockExclusive(&follow_voice_lock125);follow_voice_key127={};follow_acknowledged125=false;
    ReleaseSRWLockExclusive(&follow_voice_lock125);
}
static bool follow_voice_owned125(){
    return enabled&&state&&!state->stop&&!state->release&&state->mode==POSSESSED&&state->follow&&
        command_mode100==command100::MoveMode::Follow&&!dead_view()&&possession_lease.active&&followed(possessed,false);
}
static bool quiet_follow_voice125(const void* event){
    __try{
        auto message=reinterpret_cast<uintptr_t>(event);
        if(!message||!unit||field<unsigned>(message,4)!=1)return false;
        auto kind=field<unsigned>(message,12);if(kind!=3&&kind!=4)return false;
        auto units=field<uintptr_t>(message,8);if(!units||field<uintptr_t>(units,0)!=unit||!follow_voice_owned125())return false;
        FollowVoiceKey127 key{possession_lease.generation,possession_lease.identity.actor_id,possession_lease.identity.unit_id,unit};
        bool quiet=false;AcquireSRWLockExclusive(&follow_voice_lock125);
        // Repeat cheap state checks after full native proof and before claiming
        // the acknowledgement, in case an order/possession changed meanwhile.
        if(state->follow&&state->mode==POSSESSED&&!state->release&&!state->stop&&
           command_mode100==command100::MoveMode::Follow&&possession_lease.active&&unit==key.unit_pointer&&
           possession_lease.generation==key.lease&&possession_lease.identity.actor_id==key.actor&&possession_lease.identity.unit_id==key.unit_id){
            if(follow_voice_key127.lease!=key.lease||follow_voice_key127.actor!=key.actor||
               follow_voice_key127.unit_id!=key.unit_id||follow_voice_key127.unit_pointer!=key.unit_pointer){
                follow_voice_key127=key;follow_acknowledged125=false;
            }
            quiet=follow_acknowledged125;follow_acknowledged125=true;
        }
        ReleaseSRWLockExclusive(&follow_voice_lock125);return quiet;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void __fastcall order_voice125(void* voice,void*,const void* event){
    if(!quiet_follow_voice125(event))original_order_voice125(voice,event);
}
static bool follow_voice_signatures125(){
    const unsigned char voice[]={0x83,0xec,0x14,0x56,0x8b,0x74,0x24,0x1c};
    return !memcmp(reinterpret_cast<void*>(base+0x2fc450),voice,8)&&field<uintptr_t>(base,0x15d5114)==base+0x2fc450;
}
static void create_follow_voice125(){
    require(follow_voice_signatures125(),"Native follow voice signature differs");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x2fc450),reinterpret_cast<void*>(&order_voice125),reinterpret_cast<void**>(&original_order_voice125))==MH_OK,"Follow voice hook failed");
}
