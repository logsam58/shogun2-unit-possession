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
using LandMoveArrow=void(__thiscall*)(void*,const void*,unsigned);
static LandMoveArrow original_land_move_arrow;
using LandRouteArrows=void(__thiscall*)(void*,const void*,unsigned,unsigned);
static LandRouteArrows original_land_route_arrows;
static bool hide_follow_arrow(uintptr_t display){
    __try{
        // The native display constructor stores this battle and formation
        // at +6c/+70. The original renderer's false branch hides all three
        // arrow components and leaves path data, marker state and orders intact.
        return unit&&display&&environment&&!command_placing100&&
            field<uintptr_t>(display,0x6c)==environment&&field<uintptr_t>(display,0x70)==unit&&
            field<unsigned>(unit,4)==possession_lease.identity.unit_id&&follow_voice_owned125();
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void __fastcall land_move_arrow(void* display,void*,const void* draw,unsigned visible){
    const bool hide=(visible&0xff)&&hide_follow_arrow(reinterpret_cast<uintptr_t>(display));
    original_land_move_arrow(display,draw,hide?0u:visible);
}
static void __fastcall land_route_arrows(void* display,void*,const void* draw,unsigned visible,unsigned forced){
    const bool hide=((visible|forced)&0xff)&&hide_follow_arrow(reinterpret_cast<uintptr_t>(display));
    original_land_route_arrows(display,draw,hide?0u:visible,hide?0u:forced);
}
static void create_land_move_arrow(){
    const unsigned char entry[]={0x81,0xec,0x84,0,0,0,0x80,0xbc,0x24,0x8c,0,0,0,0};
    const unsigned char caller[]={0xe8,0xf6,0x24,0xfc,0xff};
    require(!memcmp(reinterpret_cast<void*>(base+0x2bf760),entry,sizeof entry)&&
            !memcmp(reinterpret_cast<void*>(base+0x2fd265),caller,sizeof caller),
            "Native land movement arrow signature differs");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x2bf760),reinterpret_cast<void*>(&land_move_arrow),
            reinterpret_cast<void**>(&original_land_move_arrow))==MH_OK,"Land movement arrow hook failed");
    const unsigned char route_entry[]={0x81,0xec,0xe8,0,0,0,0x80,0xbc,0x24,0xf4,0,0,0,0};
    const unsigned char route_caller[]={0xe8,0x7b,0x38,0xfd,0xff};
    require(!memcmp(reinterpret_cast<void*>(base+0x2d0ac0),route_entry,sizeof route_entry)&&
            !memcmp(reinterpret_cast<void*>(base+0x2fd240),route_caller,sizeof route_caller),
            "Native land route arrow signature differs");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x2d0ac0),reinterpret_cast<void*>(&land_route_arrows),
            reinterpret_cast<void**>(&original_land_route_arrows))==MH_OK,"Land route arrow hook failed");
}
static bool follow_voice_signatures125(){
    const unsigned char voice[]={0x83,0xec,0x14,0x56,0x8b,0x74,0x24,0x1c};
    return !memcmp(reinterpret_cast<void*>(base+0x2fc450),voice,8)&&field<uintptr_t>(base,0x15d5114)==base+0x2fc450;
}
static void create_follow_voice125(){
    require(follow_voice_signatures125(),"Native follow voice signature differs");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x2fc450),reinterpret_cast<void*>(&order_voice125),reinterpret_cast<void**>(&original_order_voice125))==MH_OK,"Follow voice hook failed");
}
