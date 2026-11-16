// Verify the production follow display filter and the actual stock clip
// player and clock in a private mapped fixture. No running game is required.
#define main native_commands_main
#include "verify_native_commands100.cpp"
#undef main

static unsigned arrow_calls, last_visibility;
static const void* last_draw;
static unsigned route_calls, last_route_visibility, last_forced;
static void __fastcall capture_route(void*,void*,const void*,unsigned visible,unsigned forced){
    ++route_calls;last_route_visibility=visible;last_forced=forced;
}
static void __fastcall capture_arrow(void*,void*,const void* draw,unsigned visible){
    ++arrow_calls;last_visibility=visible;last_draw=draw;
}
static bool __fastcall root_motion(void* actor,void*,void*,unsigned){
    const auto subject=reinterpret_cast<uintptr_t>(actor);
    for(unsigned offset:{0xd8u,0xdcu,0xe0u})field<float>(subject,offset)+=3.f;
    ++field<unsigned short>(subject,0xe4);
    return true;
}
static bool run_gait_fixture;
static void __fastcall complete_motion_fixture(void* actor,void*){
    locomotion_mark203(reinterpret_cast<uintptr_t>(actor));
    if(run_gait_fixture)motion_gait203=1;
}
// Isolate the stance setter's rig/personality lookup. The real mapped engine
// still executes the base and continuous gait selectors after this request.
static unsigned stance_requests;
static void __fastcall change_stance_fixture(void* actor,void*,unsigned mode,unsigned skip,unsigned force){
    require((mode==0||mode==5)&&skip==1&&force==0,"Unexpected native stance request");
    ++stance_requests;
    field<unsigned>(reinterpret_cast<uintptr_t>(actor),0x1f0)=mode;
}
int main(){
    SetUnhandledExceptionFilter(&fault100);
    try{
        NativeFixture100 fixture;
        std::array<unsigned char,0x270> display{},draw{};
        const auto presentation=reinterpret_cast<uintptr_t>(display.data());
        field<uintptr_t>(presentation,0x6c)=environment;
        field<uintptr_t>(presentation,0x70)=unit;
        require(MH_Initialize()==MH_OK,"Follow arrow hook setup");
        create_land_move_arrow();
        require(MH_Uninitialize()==MH_OK,"Follow arrow hook cleanup");
        original_land_move_arrow=reinterpret_cast<LandMoveArrow>(&capture_arrow);
        original_land_route_arrows=reinterpret_cast<LandRouteArrows>(&capture_route);
        state->follow=1;command_mode100=command100::MoveMode::Follow;
        auto visible=[&](unsigned expected){
            land_move_arrow(display.data(),nullptr,draw.data(),1);
            require(last_visibility==expected&&last_draw==draw.data(),"Wrong native arrow draw decision");
            land_route_arrows(display.data(),nullptr,draw.data(),0,1);
            require(last_route_visibility==0&&last_forced==expected,"Wrong native route arrow draw decision");
        };
        visible(0);
        field<uintptr_t>(presentation,0x70)=NativeFixture100::ptr(fixture.peer);visible(1);
        field<uintptr_t>(presentation,0x70)=unit;
        field<uintptr_t>(presentation,0x6c)=0;visible(1);
        field<uintptr_t>(presentation,0x6c)=environment;
        command_placing100=1;visible(1);command_placing100=0;
        state->release=1;visible(1);state->release=0;
        state->follow=0;visible(1);
        require(arrow_calls==6&&route_calls==6&&packets100.empty(),"Arrow filter changed native command dispatch");

        std::array<uintptr_t,0x4a3> clips{};
        std::array<unsigned char,32> database{},clip{};
        std::array<unsigned char,0xa0> animation{};
        const auto db=reinterpret_cast<uintptr_t>(database.data());
        const auto clip_ptr=reinterpret_cast<uintptr_t>(clip.data());
        const auto animation_ptr=reinterpret_cast<uintptr_t>(animation.data());
        field<uintptr_t>(db,0xc)=reinterpret_cast<uintptr_t>(clips.data());
        field<uintptr_t>(possessed,0x23c)=db;
        field<unsigned>(clip_ptr,0)=0x486;
        field<uintptr_t>(clip_ptr,4)=animation_ptr;
        field<unsigned>(animation_ptr,0x64)=3;
        clips[0x486]=clip_ptr;
        original_play=reinterpret_cast<PlayAnimation>(base+0x3e8b50);
        original_animation_events=reinterpret_cast<decltype(original_animation_events)>(&root_motion);
        state->heartbeat=GetTickCount64();
        auto request=[&](unsigned lease){cheer_request.set(lease,GetTickCount64());};
        request(possession_lease.generation+1);
        require(!start_or_hold_cheer(possessed),"A stale click started a cheer");
        request(possession_lease.generation);
        require(start_or_hold_cheer(possessed)&&field<unsigned>(possessed,0x1d4)==0x486,
            "Current actor did not start the available native cheer clip");
        require(weapon_animation_events_hook(fixture.actor.data(),nullptr,clip.data(),0)&&
            field<float>(possessed,0xd8)==0&&field<float>(possessed,0xdc)==0&&
            field<float>(possessed,0xe0)==0&&field<unsigned short>(possessed,0xe4)==0,
            "Cheer applied native root displacement");
        const auto peer=NativeFixture100::ptr(fixture.peer);
        require(weapon_animation_events_hook(fixture.peer.data(),nullptr,clip.data(),0)&&
            field<float>(peer,0xd8)==3.f&&field<unsigned short>(peer,0xe4)==1,
            "Other actor's root motion was suppressed");
        using AnimationClock=void(__thiscall*)(void*);
        auto advance=reinterpret_cast<AnimationClock>(base+0x3f0370);
        for(unsigned frame=1;frame<=3;++frame){
            advance(fixture.actor.data());
            require(field<unsigned>(possessed,0x1d8)==frame,"Cheer changed native clip timing");
            if(frame<3){
                request(possession_lease.generation);
                require(start_or_hold_cheer(possessed)&&field<unsigned>(possessed,0x1d8)==frame,
                    "Another click restarted a running cheer");
            }
        }
        require(!start_or_hold_cheer(possessed),"Movement stayed locked after native completion");
        require(cheer_sprint_input(FORWARD|RUN,false)&&cheer_sprint_input(LEFT|RUN,false)&&
            !cheer_sprint_input(RUN,false)&&!cheer_sprint_input(FORWARD,false)&&
            !cheer_sprint_input(BACKWARD|RUN,true)&&cheer_sprint_input(FORWARD|RUN,true),
            "Cheer did not use locomotion's directional sprint intent");
        state->follow=1;
        require(cheer_sprint_input(FORWARD,false)&&!cheer_sprint_input(FORWARD|WALK_OVERRIDE,false),
            "Cheer ignored Follow Me pace or the Deck walking modifier");
        state->follow=0;
        original_animation_stop82=reinterpret_cast<RangedAction>(base+0x3ca920);
        request(possession_lease.generation);
        require(start_or_hold_cheer(possessed),"Could not start a second cheer");
        advance(fixture.actor.data());
        release_locomotion_idle(possessed);
        require(cheer_active(possessed)&&field<unsigned>(possessed,0x1d8)==1,
            "Ordinary locomotion interrupted Cheer");
        request(possession_lease.generation);
        cancel_cheer_for_sprint(possessed);
        require(!field<uintptr_t>(possessed,0x474)&&!start_or_hold_cheer(possessed),
            "Sprint did not stop the native cheer or discarded click restarted it");
        clips[0x486]=0;request(possession_lease.generation);
        require(!start_or_hold_cheer(possessed),"Missing rig clip fell back to a different action");
        original_motion_complete203=reinterpret_cast<UnitTick130>(&complete_motion_fixture);
        auto arm_idle=[&](unsigned id){
            clips[id]=clip_ptr;field<unsigned>(clip_ptr,0)=id;
            field<unsigned>(possessed,0x1d0)=0x21;
            field<unsigned>(possessed,0x1d4)=id;
            field<uintptr_t>(possessed,0x474)=clip_ptr;
        };
        for(unsigned id:{1u,60u,141u,152u,178u,184u,205u,210u,217u,224u,230u}){
            for(unsigned key:{FORWARD,BACKWARD,LEFT,RIGHT}){
                arm_idle(id);state->input=key;
                field<float>(possessed,0x1ac)=field<float>(possessed,0x158)=0.f;
                motion_complete203(fixture.actor.data(),nullptr);
                require(!field<uintptr_t>(possessed,0x474),
                    "Zero-driver first step left static idle active");
            }
        }
        arm_idle(141);state->input=0;
        motion_complete203(fixture.actor.data(),nullptr);
        require(field<uintptr_t>(possessed,0x474)==clip_ptr,"Standing still canceled idle");
        field<float>(possessed,0x158)=.05f;
        motion_complete203(fixture.actor.data(),nullptr);
        require(!field<uintptr_t>(possessed,0x474)&&field<unsigned>(possessed,0x1d0)==0,
            "Mouse body turn left idle latched or forced a walking pose");
        arm_idle(141);state->input=LEFT;
        field<float>(possessed,0x158)=0.f;field<float>(possessed,0x8c)=.5f;
        motion_complete203(fixture.actor.data(),nullptr);
        require(!field<uintptr_t>(possessed,0x474)&&field<unsigned>(possessed,0x1d0)==8,
            "Actual sideways motion with a zero forward driver did not select native walk");
        field<float>(possessed,0x8c)=0.f;
        arm_idle(130);
        motion_complete203(fixture.actor.data(),nullptr);
        require(field<uintptr_t>(possessed,0x474)==clip_ptr,"Movement canceled a non-idle action");
        arm_idle(141);clips[141]=0;
        motion_complete203(fixture.actor.data(),nullptr);
        require(field<uintptr_t>(possessed,0x474)==clip_ptr,"Movement canceled an unverified rig clip");
        arm_idle(141);state->aiming=1;
        release_locomotion_idle(possessed);
        require(field<uintptr_t>(possessed,0x474)==clip_ptr,"Idle override interrupted ADS posture");
        state->aiming=0;
        release_locomotion_idle(peer);
        require(field<uintptr_t>(possessed,0x474)==clip_ptr,"Peer update altered the possessed idle");
        original_change=reinterpret_cast<decltype(original_change)>(&change_stance_fixture);
        auto gait=[&](unsigned mode,unsigned terrain,unsigned input,float speed,unsigned clip){
            const auto old_requests=stance_requests;
            field<unsigned>(possessed,0x1f0)=mode;
            field<unsigned>(possessed,0x1b4)=terrain;
            field<float>(possessed,0x1ac)=speed;
            field<unsigned>(possessed,0x1d0)=12;
            field<unsigned>(possessed,0x1d4)=1157; // Last explicit clip in the live capture.
            field<uintptr_t>(possessed,0x474)=0;
            if(clip)arm_idle(clip);
            state->input=input;
            motion_complete203(fixture.actor.data(),nullptr);
            return stance_requests-old_requests;
        };
        field<unsigned>(peer,0x1b4)=0;
        field<unsigned>(peer,0x1f0)=5;
        field<unsigned>(peer,0x1d4)=869;
        require(gait(5,2,FORWARD,1.4f,0)==1&&field<unsigned>(possessed,0x1d0)==8,
            "Live recorded woodland stance carried onto mud during walking");
        std::array<unsigned char,32> sneak_data{};
        const auto sneak_ptr=reinterpret_cast<uintptr_t>(sneak_data.data());
        field<unsigned>(sneak_ptr,0)=845;
        field<uintptr_t>(sneak_ptr,4)=animation_ptr;
        clips[845]=sneak_ptr;
        require(gait(0,0,FORWARD,1.4f,0)==1&&field<unsigned>(possessed,0x1d0)==12,
            "Entering wooded terrain from upright did not select the native sneak cycle");
        require(gait(5,0,FORWARD,1.4f,871)==0&&field<unsigned>(possessed,0x1d0)==12&&
            !field<uintptr_t>(possessed,0x474),"Static concealment idle overrode the native sneak cycle");
        clips[845]=0;
        require(gait(5,0,FORWARD,1.4f,0)==1&&field<unsigned>(possessed,0x1d0)==8,
            "Rig without sneak continued into a missing gait");
        clips[845]=sneak_ptr;
        run_gait_fixture=true;
        require(gait(5,0,FORWARD,2.5f,0)==1&&field<unsigned>(possessed,0x1f0)==0&&
            field<unsigned>(possessed,0x244)==1,"Woodland stance prevented an upright sprint");
        run_gait_fixture=false;
        require(gait(5,2,0,0,870)==1&&field<unsigned>(possessed,0x1d0)==0&&
            !field<uintptr_t>(possessed,0x474),"Stopping outside the woods retained the formation's concealment stance");
        require(gait(5,0,0,0,869)==0&&field<uintptr_t>(possessed,0x474)==clip_ptr,
            "Standing in cover interrupted its own native concealment idle");
        require(gait(10,1,LEFT,1.4f,109)==1&&field<unsigned>(possessed,0x1d0)==8&&
            !field<uintptr_t>(possessed,0x474),"Stationary melee-ready animation overrode manual walking");
        require(gait(10,1,0,0,109)==0&&field<uintptr_t>(possessed,0x474)==clip_ptr,
            "Idle melee-ready animation was canceled without movement");
        require(gait(5,2,FORWARD,1.4f,130)==0&&field<uintptr_t>(possessed,0x474)==clip_ptr,
            "Forest stance recovery canceled an unrelated explicit animation");
        field<uintptr_t>(possessed,0x688)=NativeFixture100::ptr(fixture.peer);
        require(gait(10,1,FORWARD,1.4f,109)==0&&field<uintptr_t>(possessed,0x474)==clip_ptr,
            "Manual gait overrode a native melee pair");
        field<uintptr_t>(possessed,0x688)=0;
        require(field<unsigned>(peer,0x1f0)==5&&field<unsigned>(peer,0x1b4)==0&&field<unsigned>(peer,0x1d4)==869,
            "Local woodland gait changed the formation's native concealment");
        const uintptr_t clear_hover[]={possessed,peer,0,possessed};
        const uintptr_t ambiguous_hover[]={possessed,peer,0,peer};
        const uintptr_t center_miss[]={possessed,possessed,0,0};
        require(camera_pick_majority150(possessed,clear_hover)&&
            !camera_pick_majority150(possessed,ambiguous_hover)&&!camera_pick_majority150(0,center_miss),
            "Clear hover, competing silhouette, or blocked center used wrong selection path");
        std::array<unsigned char,0x40> track{};
        std::array<unsigned char,0x1c*3> frame_root{};
        const auto track_ptr=reinterpret_cast<uintptr_t>(track.data());
        const auto root_ptr=reinterpret_cast<uintptr_t>(frame_root.data());
        field<uintptr_t>(clip_ptr,8)=track_ptr;
        field<uintptr_t>(track_ptr,0x34)=root_ptr;
        original_animation_stop82=reinterpret_cast<RangedAction>(base+0x3ca920);
        auto footwork=[&](unsigned key,unsigned expected){
            field<unsigned>(possessed,0x1f0)=0;
            field<uintptr_t>(possessed,0x474)=0;
            field<unsigned>(clip_ptr,0)=expected;
            clips[expected]=clip_ptr;
            combat_presence73={GetTickCount64(),possession_lease.generation,true};
            return prepare_combat_footwork(possessed,key);
        };
        for(const auto pair:{std::pair{FORWARD,110u},std::pair{BACKWARD,111u},
                             std::pair{LEFT,112u},std::pair{RIGHT,113u}}){
            const auto expected=steering_view(pair.first,0.f,1.f,.18f);
            for(unsigned frame:{0u,1u}){
                field<float>(root_ptr+0x1c*frame,0x10)=expected.x;
                field<float>(root_ptr+0x1c*frame,0x18)=expected.z;
            }
            field<unsigned short>(possessed,0x64)=0;
            require(footwork(pair.first,pair.second)&&field<unsigned>(possessed,0x1d4)==pair.second,
                "Directional footwork did not start its native clip");
            field<float>(possessed,0xd8)=field<float>(possessed,0xe0)=0;
            advance(fixture.actor.data());
            require(field<unsigned>(possessed,0x1d8)==1&&planar_speed62(possessed)==0.f&&
                std::abs(field<float>(possessed,0xd8)-expected.x)<.01f&&
                std::abs(field<float>(possessed,0xe0)-expected.z)<.01f,
                "Native clock did not advance the authored root-motion frame in the requested direction");
            require(prepare_combat_footwork(possessed,pair.first)&&field<unsigned>(possessed,0x1d8)==1,
                "Held movement restarted the native footwork clip");
            require(!prepare_combat_footwork(possessed,FORWARD|RUN)&&!field<uintptr_t>(possessed,0x474),
                "Sprint failed to yield from directional footwork");
        }
        require(!footwork(FORWARD|RIGHT,110),"A cardinal step swallowed a diagonal move");
        require(!footwork(FORWARD,110),"Sideways-only root displacement was accepted as forward footwork");
        for(unsigned frame:{0u,1u})field<float>(root_ptr+0x1c*frame,0x10)=0;
        require(!footwork(FORWARD,110),"Stationary combat clip was accepted as footwork");
        std::cout<<"PASS: five-ray majority and native directional step clock, root motion, held-input and sprint release checks.\n";
        std::cout<<"PASS: individual woodland stance, explicit idle release, melee-ready walk, and scoped ownership.\n";
        std::cout<<"PASS: all WASD first-step interrupts, stationary body turn, native lateral walk, idle ownership, and Cheer priority.\n";
        std::cout<<"PASS: scoped native arrow visibility, native cheer start/finish, stationary root events, repeated clicks, sprint cancellation and stale/missing rig cases.\n";
        return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
