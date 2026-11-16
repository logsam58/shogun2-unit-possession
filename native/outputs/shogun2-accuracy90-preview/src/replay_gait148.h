#pragma once
static void classify_native_gait203(void* object,unsigned gait,bool moving){
    field<unsigned>(reinterpret_cast<uintptr_t>(object),0x244)=gait;
    reinterpret_cast<UnitTick130>(base+0x3e4850)(object);
    if(moving)reinterpret_cast<UnitTick130>(base+0x3e44f0)(object);
}
static uint32_t gait_proof148(uintptr_t actor){
    uint32_t result=2166136261u;
    for(unsigned offset:{0x1ac,0x1d4,0x1f0,0x244})
        result=(result^field<uint32_t>(actor,offset))*16777619u;
    return result?result:1;
}
static void record_gait148(uintptr_t actor,exactreplay147::Kind kind,unsigned target,unsigned source=0,
                           uint32_t proof=0,exactreplay147::Boundary boundary=exactreplay147::Boundary::none) noexcept {
    const auto scope=replay147::actor_scope;
    if(scope.actor!=actor||!scope.occurrence||possession_lease.identity.mount)return;
    exactreplay147::Event event{};
    event.kind=kind;
    event.boundary=boundary;
    event.target=target;
    event.source=source;
    event.proof=proof;
    event.occurrence=scope.occurrence;
    replay147::record_action(actor,event,unit);
}
static void change_foot_gait148(void* object,unsigned mode){
    const auto actor=reinterpret_cast<uintptr_t>(object);
    const auto before=field<unsigned>(actor,0x1f0);
    if(before!=mode)record_gait148(actor,exactreplay147::Kind::gait_mode,mode,before);
    original_change(object,mode,1,0);
}
static bool apply_gait148(void* object,const exactreplay147::Event& event){
    using exactreplay147::Kind;
    const auto actor=reinterpret_cast<uintptr_t>(object);
    if(field<uintptr_t>(actor,0x18)||field<uintptr_t>(actor,0x688))return false;
    switch(event.kind){
    case Kind::gait_cancel_idle:{
        const auto clip=field<uintptr_t>(actor,0x474);
        const auto mode=field<unsigned>(actor,0x1f0);
        if(!clip||(!locomotion_idle204(event.target)&&!locomotion_stance_idle(mode,event.target))||
           field<unsigned>(clip,0)!=event.target||
           field<unsigned>(actor,0x1d4)!=event.target||locomotion_clip62(actor,event.target)!=clip)return false;
        reinterpret_cast<UnitTick130>(base+0x3dd9b0)(object);
        return true;
    }
    case Kind::gait_mode:{
        const auto combat=meleedisplay137::ready(false);
        if(!original_change||field<unsigned>(actor,0x1f0)!=event.source||
           (event.target!=0&&event.target!=combat&&
            !(event.target==concealed_stance&&field<unsigned>(actor,0x1b4)==forest_terrain&&
              valid_weapon_clip(actor,sneak_cycle))))return false;
        original_change(object,event.target,1,0);
        return true;
    }
    case Kind::gait_pose:{
        if(field<uintptr_t>(actor,0x474))return false;
        const auto pace=field<float>(actor,0x1ac);
        if(!std::isfinite(pace)||pace<0)return false;
        if(event.source==0&&pace>=.01f)return false;
        if(event.source==1&&pace<.01f){
            const auto planar=planar_speed62(actor);
            if(!std::isfinite(planar)||planar<.01f)return false;
        }
        classify_native_gait203(object,event.target,event.source!=0);
        return gait_proof148(actor)==event.proof;
    }
    default:return false;
    }
}
static void replay_gait148(void* object,uint16_t occurrence,exactreplay147::Boundary boundary=exactreplay147::Boundary::none) noexcept {
    using namespace exactreplay147;
    if(!occurrence||!replay147::playing_tick)return;
    const auto actor=reinterpret_cast<uintptr_t>(object);
    __try{
        const auto env=field<uintptr_t>(actor,0x28),formation=field<uintptr_t>(actor,0x214);
        if(!env||!formation||!field<unsigned char>(env,0x28254)||field<uintptr_t>(env,0x28248))return;
        const auto tick=field<uint32_t>(env,0x282bc);
        Event event;
        while(replay147::take(env,tick,field<uint32_t>(formation,4),field<uint32_t>(actor,0x1c8),
                              Stream::gait,boundary,occurrence,event)){
            if(replay147::infantry(actor,formation,env)&&apply_gait148(object,event))continue;
            replay147::fail_playback();
            break;
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){replay147::fail_playback();}
}
