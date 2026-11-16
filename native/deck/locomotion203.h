#pragma once
// 366a10 computes the native motion driver AFTER367600. Update the original
// pose classifier only after that result exists, only for our completed intent.
static UnitTick130 original_motion_complete203;
static float planar_speed62(uintptr_t actor){
    return std::hypot(field<float>(actor,0x8c),field<float>(actor,0x94));
}
static __declspec(thread) uintptr_t motion_completed203;
static __declspec(thread) unsigned motion_gait203;
struct HorseInput62 {uintptr_t actor;unsigned lease;bool forward;};
static __declspec(thread) HorseInput62 horse_input62;
// These are stock standing-idle categories, resolved from the pinned engine's
// animation-name table and owned-game fragments. Never cancel arbitrary clips.
static bool locomotion_idle204(unsigned clip){
    return clip==1||clip==60||clip==158||clip==184||clip==210||clip==217||clip==224||clip==589||
        (clip>=141&&clip<=157)||(clip>=178&&clip<=182)||(clip>=205&&clip<=209)||
        (clip>=212&&clip<=216)||(clip>=219&&clip<=223)||(clip>=226&&clip<=230);
}
// Native 3e3f50 uses CROUCH_IDLE_1 through CROUCH_IDLE_5 in this stance.
static constexpr unsigned concealed_stance=5;
static constexpr unsigned sneak_cycle=845;
static constexpr unsigned forest_terrain=0; // Terrain name table initialized by 3bad30.
static bool locomotion_has_static_idle(uintptr_t actor);
static bool foot_aim_exit203(uintptr_t actor,unsigned mode){
    return !possession_lease.identity.mount&&mode==equipment81::lower_mode(false,false)&&
        foot_aim_lower.actor==actor&&foot_aim_lower.lease==possession_lease.generation;
}
static bool locomotion_ordinary203(uintptr_t actor,bool admit_rider_idle=false){
    // The horse integrates movement, then its original update attaches the rider.
    // A mounted rider's pose must never be classified as infantry locomotion.
    if(actor!=locomotion_actor()||!member_owned130(actor))return false;
    if(possession_lease.identity.mount){
        const auto rider_mode=field<unsigned>(possessed,0x1f0);
        if(native_mounted_jump140(actor)||(rider_mode!=29&&rider_mode!=30)||
           field<uintptr_t>(possessed,0x688)||(field<uintptr_t>(possessed,0x474)&&
               !(admit_rider_idle&&locomotion_has_static_idle(possessed)))||
           field<unsigned char>(possessed,0x488)||field<unsigned char>(possessed,0x489))return false;
    }
    const auto mode=field<unsigned>(actor,0x1f0);
    const bool foot_combat=!possession_lease.identity.mount&&mode==meleedisplay137::ready(false);
    return
        (mode<=7||foot_combat||foot_aim_exit203(actor,mode))&&!field<uintptr_t>(actor,0x18)&&
        !field<uintptr_t>(actor,0x688)&&!field<unsigned char>(actor,0x488)&&!field<unsigned char>(actor,0x489)&&
        !wall_subject97(actor)&&!native_airborne_subject99(actor)&&!native_recovery103(actor)&&
        !charge_input_locked146()&&!charge_native_task146(actor)&&!state->aiming&&!state->aim_pose&&!state->weapon_phase;
}
// Entries verified in the native animation table and owned horse fragment.
static unsigned horse_stop62(unsigned gait){
    switch(gait){
    case 11: return 16; // Walk
    case 29: return 30; // Trot
    case 34: return 35; // Canter
    case 39: return 40; // Gallop
    default: return 0;
    }
}
static constexpr unsigned horse_step_backward95=45; // STEP_BACKWARD in the stock animation-name table.
// The owned step-back animation travels one metre over 38 ticks at 20 Hz.
static constexpr float horse_reverse_pace96=20.f/38.f;
static float horse_forward_velocity96(uintptr_t actor){
    const auto forward=steering_mount(FORWARD,field<unsigned short>(actor,0x64),.125f,8.f,0.f);
    return field<float>(actor,0x8c)*forward.x+field<float>(actor,0x94)*forward.z;
}
static bool horse_braking96(uintptr_t actor){
    const bool leaving_forward=horse_input62.actor==actor&&horse_input62.lease==possession_lease.generation&&horse_input62.forward;
    return leaving_forward||horse_forward_velocity96(actor)>=.01f;
}
static bool horse_stop_clip95(unsigned id){return id==16||id==30||id==35||id==40;}
static uintptr_t locomotion_clip62(uintptr_t actor,unsigned id){
    const auto db=field<uintptr_t>(actor,0x23c);
    const auto clips=db?field<uintptr_t>(db,0xc):0;
    const auto clip=clips?field<uintptr_t>(clips,id*sizeof(uintptr_t)):0;
    return clip&&field<unsigned>(clip,0)==id?clip:0;
}
static bool locomotion_has_static_idle(uintptr_t actor){
    const auto clip=field<uintptr_t>(actor,0x474);
    if(!clip)return false;
    const unsigned id=field<unsigned>(clip,0);
    const unsigned mode=field<unsigned>(actor,0x1f0);
    const bool infantry=!possession_lease.identity.mount&&!equipment_crouched81();
    const bool stance_idle=infantry&&((mode==concealed_stance&&id>=868&&id<=872)||
        (mode==meleedisplay137::ready(false)&&id==109));
    return (locomotion_idle204(id)||stance_idle)&&field<unsigned>(actor,0x1d4)==id&&
        locomotion_clip62(actor,id)==clip;
}
static void release_locomotion_idle(uintptr_t actor){
    if(cheer_active(actor)||!locomotion_ordinary203(actor,true))return;
    const auto stop=reinterpret_cast<UnitTick130>(base+0x3dd9b0);
    if(possession_lease.identity.mount&&locomotion_has_static_idle(possessed))
        stop(reinterpret_cast<void*>(possessed));
    if(locomotion_has_static_idle(actor))stop(reinterpret_cast<void*>(actor));
}
static bool locomotion_root_clip95(uintptr_t clip){
    // 3f0370 uses the animation's frame count. 3f03d0 adds the
    // frame's root translation from the optional track to native intent.
    if(!clip)return false;
    const auto animation=field<uintptr_t>(clip,4),track=field<uintptr_t>(clip,8);
    if(!animation||!track||!field<uintptr_t>(track,0x34))return false;
    const auto frames=field<unsigned>(animation,0x64);
    return frames>0&&frames<=4096;
}
struct CombatFootwork {uintptr_t actor,clip;unsigned lease,id;};
static __declspec(thread) CombatFootwork combat_footwork;
static unsigned combat_step_id(unsigned input){
    if(native_sprint_requested133(input))return 0;
    const int forward=int(bool(input&FORWARD))-int(bool(input&BACKWARD));
    const int right=int(bool(input&RIGHT))-int(bool(input&LEFT));
    // The stock rig supplies four cardinal steps. Keep free movement for
    // diagonals, whose displacement these authored steps cannot reproduce.
    if(forward&&right)return 0;
    if(forward)return forward>0?110u:111u;
    if(right)return right>0?113u:112u;
    return 0;
}
static bool combat_step_has_travel(uintptr_t clip,unsigned id){
    if(!locomotion_root_clip95(clip))return false;
    const auto frames=field<unsigned>(field<uintptr_t>(clip,4),0x64);
    if(frames>120)return false;
    const auto root=field<uintptr_t>(field<uintptr_t>(clip,8),0x34);
    float x=0,z=0;
    for(unsigned i=0;i<frames;++i){
        const float dx=field<float>(root+0x1c*i,0x10),dz=field<float>(root+0x1c*i,0x18);
        if(!std::isfinite(dx)||!std::isfinite(dz))return false;
        x+=dx;z+=dz;
    }
    const float travel=std::hypot(x,z);
    const float in_direction=id==110?z:id==111?-z:id==112?-x:x;
    return in_direction>=.04f&&travel<=4.f;
}
static bool prepare_combat_footwork(uintptr_t actor,unsigned input){
    if(possession_lease.identity.mount||!member_owned130(actor))return false;
    const auto generation=possession_lease.generation;
    const auto active=field<uintptr_t>(actor,0x474);
    const bool owned=combat_footwork.actor==actor&&combat_footwork.lease==generation&&
        active&&active==combat_footwork.clip&&field<unsigned>(actor,0x1d4)==combat_footwork.id;
    const auto id=combat_step_id(input);
    const bool step=id&&!equipment_crouched81()&&locomotion_ordinary203(actor)&&combat_enemy_near73();
    if(owned){
        if(step&&combat_footwork.id==id)return true;
        // Interrupt only our exact clip. A native encounter can take over at
        // any point, including when the player changes direction or sprints.
        if(field<uintptr_t>(actor,0x18)||field<uintptr_t>(actor,0x688))return false;
        if(!original_animation_stop82)return true;
        original_animation_stop82(reinterpret_cast<void*>(actor));
        if(field<uintptr_t>(actor,0x474)==active)return true;
    }
    combat_footwork={};
    if(!step||!original_play)return false;
    release_locomotion_idle(actor);
    if(field<uintptr_t>(actor,0x474))return false;
    const auto clip=locomotion_clip62(actor,id);
    if(!combat_step_has_travel(clip,id))return false;
    // Same native clock and root integration as an ordinary authored step.
    // While live, the caller yields before issuing a manual translation.
    original_play(reinterpret_cast<void*>(actor),id,0,0,1.f);
    if(field<uintptr_t>(actor,0x474)!=clip||field<unsigned>(actor,0x1d4)!=id)return false;
    combat_footwork={actor,clip,generation,id};
    return true;
}
static bool horse_root_owns_motion95(uintptr_t actor,unsigned input){
    if(mounted_forward95(input)||!locomotion_ordinary203(actor))return false;
    const auto clip=field<uintptr_t>(actor,0x474);
    if(!clip)return false;
    const auto id=field<unsigned>(clip,0);
    return (id==horse_step_backward95||horse_stop_clip95(id))&&
        field<unsigned>(actor,0x1d4)==id&&locomotion_clip62(actor,id)==clip&&locomotion_root_clip95(clip);
}
static void locomotion_mark203(uintptr_t actor){
    __try{if(locomotion_ordinary203(actor,true)){
        motion_completed203=actor;
        const unsigned input=personal_travel_input91(unsigned(state->input),
            state->follow!=0,state->aiming||state->aim_pose,possession_lease.identity.mount!=0);
        motion_gait203=native_sprint_requested133(input)&&reinterpret_cast<UnitRunAllowed>(base+0x15cb80)(reinterpret_cast<void*>(unit))?1u:0u;
    }}__except(EXCEPTION_EXECUTE_HANDLER){motion_completed203=0;}
}
static void __fastcall motion_complete203(void* object,void*){
    if(!possession_lease.active||reinterpret_cast<uintptr_t>(object)!=locomotion_actor()){original_motion_complete203(object);return;}
    const auto saved=motion_completed203;const auto saved_gait=motion_gait203;motion_completed203=0;
    InterlockedIncrement(&motion_callback_depth);
    __try{
        const auto actor=reinterpret_cast<uintptr_t>(object);
        unsigned stop_clip=0;
        __try{
            if(possession_lease.identity.mount&&original_play&&locomotion_ordinary203(actor)){
                const auto lease=possession_lease.generation;
                const bool forward=mounted_forward95(unsigned(state->input));
                const bool released=horse_input62.actor==actor&&horse_input62.lease==lease&&
                    horse_input62.forward&&!forward;
                horse_input62={actor,lease,forward};
                if(released&&!field<uintptr_t>(actor,0x474)){
                    const auto speed=planar_speed62(actor);
                    if(std::isfinite(speed)&&speed>=.01f)stop_clip=horse_stop62(field<unsigned>(actor,0x1d4));
                }
            }else horse_input62={};
        }__except(EXCEPTION_EXECUTE_HANDLER){horse_input62={};stop_clip=0;}
        original_motion_complete203(object);
        __try{if(motion_completed203==actor&&locomotion_ordinary203(actor,true)){
            const auto input=unsigned(state->input);
            const bool mounted=possession_lease.identity.mount!=0;
            const bool translating=bool(input&FORWARD)!=bool(input&BACKWARD)||
                (!mounted&&(bool(input&LEFT)!=bool(input&RIGHT)));
            const float turn=field<float>(actor,0x158);
            const float speed=field<float>(actor,0x1ac);
            if(!std::isfinite(speed)||speed<0)return;
            // The native driver can project sideways travel to zero. Use the
            // measured planar velocity only to admit the native infantry gait.
            const float planar=!mounted&&translating?planar_speed62(actor):0.f;
            const bool moving=speed>=.01f||(std::isfinite(planar)&&planar>=.01f);
            const bool concealed_outside_forest=!mounted&&!equipment_crouched81()&&
                field<unsigned>(actor,0x1f0)==concealed_stance&&field<unsigned>(actor,0x1b4)!=forest_terrain;
            if(translating||(std::isfinite(turn)&&turn!=0.f)||moving||concealed_outside_forest)
                release_locomotion_idle(actor);
            if(!locomotion_ordinary203(actor))return;
            const auto mode=field<unsigned>(actor,0x1f0);
            const bool lowered_on_foot=foot_aim_exit203(actor,mode);
            if(lowered_on_foot&&(!moving||!translating||!original_change))return;
            auto clip=field<uintptr_t>(actor,0x474);
            if(clip){
                // Horse braking and the weapon-lowering handoff still use
                // the original movement and ownership checks.
                const unsigned id=field<unsigned>(clip,0);
                const bool horse_transition=possession_lease.identity.mount&&
                    (horse_stop_clip95(id)||id==horse_step_backward95);
                const bool lowering=lowered_on_foot&&foot_aim_lower.clip==clip;
                if(!moving||(!horse_transition&&!lowering)||
                   (horse_transition&&!mounted_forward95(input))||
                   field<unsigned>(actor,0x1d4)!=id||locomotion_clip62(actor,id)!=clip)return;
                reinterpret_cast<UnitTick130>(base+0x3dd9b0)(object);
                if(field<uintptr_t>(actor,0x474))return;
            }
            if(stop_clip&&!mounted_forward95(input)&&locomotion_clip62(actor,stop_clip)){
                // Start at input release, while the native movement step can
                // still be braking. Use the gait from before deceleration,
                // then let the native clip player blend and complete it.
                original_play(object,stop_clip,0,0,1.f);
                if(field<uintptr_t>(actor,0x474))return;
            }
            if(possession_lease.identity.mount&&mounted_backward95(input)&&original_play&&
               horse_forward_velocity96(actor)<.01f&&
               locomotion_root_clip95(locomotion_clip62(actor,horse_step_backward95))){
                original_play(object,horse_step_backward95,0,0,1.f);
                if(field<uintptr_t>(actor,0x474))return;
            }
            if(!mounted&&!equipment_crouched81()&&original_change){
                const bool manual_travel=moving&&translating;
                // Mode 5 with walk gait enters the native crouched walk state.
                // Use the actor's terrain and the current rig's own SNEAK clip.
                const bool sneak=manual_travel&&motion_gait203==0&&
                    field<unsigned>(actor,0x1b4)==forest_terrain&&valid_weapon_clip(actor,sneak_cycle);
                if(sneak&&mode!=concealed_stance)original_change(object,concealed_stance,1,0);
                if(!sneak&&(concealed_outside_forest||(manual_travel&&
                    (lowered_on_foot||mode==concealed_stance||mode==meleedisplay137::ready(false)))))
                    original_change(object,0,1,0);
                if(lowered_on_foot)foot_aim_lower={};
            }
            // 3dd9d0 also requires planner intent+150, which can be zero
            // after integration. Use its pose routines with completed motion.
            field<unsigned>(actor,0x244)=motion_gait203;
            reinterpret_cast<UnitTick130>(base+0x3e4850)(object);
            if(moving)reinterpret_cast<UnitTick130>(base+0x3e44f0)(object);
        }}__except(EXCEPTION_EXECUTE_HANDLER){}
    }__finally{motion_completed203=saved;motion_gait203=saved_gait;InterlockedDecrement(&motion_callback_depth);}
}
static void create_locomotion203(){
    const unsigned char signature[]={0x83,0xec,0x1c,0x56,0x8b,0xf1,0x57,0x80,0xbe,0x99,1,0,0,0};
    require(!memcmp(reinterpret_cast<void*>(base+0x366a10),signature,sizeof signature),"Native motion completion differs");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x366a10),reinterpret_cast<void*>(&motion_complete203),reinterpret_cast<void**>(&original_motion_complete203))==MH_OK,"Native motion completion hook failed");
}
