// Reuse the existing fully proved native actor/camera/group fixture.
#define main retained_commands_fixture125
#include "verify_native_commands100.cpp"
#undef main
static unsigned moves125,voices125,events125,refreshes125;
static unsigned expected_run125;
static uintptr_t event_unit125;
static unsigned char voice_object125[0x180]{};
static void __fastcall plan125(void*,void*,uintptr_t env,const MovePoint100* point,unsigned heading,void*,void*,void*){
    require(env==environment&&point&&heading==1234,"Native move planning arguments changed");
}
static void __fastcall apply125(void*,void*,uintptr_t env,unsigned running,unsigned orient,void*,void*,unsigned queued){
    require(env==environment&&running==expected_run125&&orient==1&&!queued,"Native formation/gait/queue changed");++moves125;
}
static void* __fastcall event125(void* out,void*,const void*,const void*,unsigned type){
    auto p=reinterpret_cast<uintptr_t>(out);field<unsigned>(p,0)=field<unsigned>(p,4)=1;
    field<uintptr_t>(p,8)=reinterpret_cast<uintptr_t>(&event_unit125);field<unsigned>(p,12)=type;return out;
}
static void __cdecl borrowed_free125(void* p){require(p==&event_unit125,"Unexpected native event allocation");}
struct Deferred125 {uintptr_t unit;unsigned type;};
static std::vector<Deferred125> deferred125;
static bool defer125;
static void deliver125(uintptr_t target,unsigned type){
    unsigned message[4]={1,1,unsigned(reinterpret_cast<uintptr_t>(&target)),type};
    reinterpret_cast<OrderVoice125>(base+0x2fc450)(voice_object125+0x64,message);
}
static void flush125(){
    for(auto& e:deferred125)deliver125(e.unit,e.type);deferred125.clear();
}
static void __fastcall dispatch125(void*,void*,const void* event){
    ++events125; // Other event subscribers still receive every follow update.
    if(defer125){auto p=reinterpret_cast<uintptr_t>(event);deferred125.push_back({field<uintptr_t>(field<uintptr_t>(p,8),0),field<unsigned>(p,12)});}
    else reinterpret_cast<OrderVoice125>(base+0x2fc450)(voice_object125+0x64,event);
}
static void* __fastcall play125(void*,void*,int* result,uintptr_t actor_unit,unsigned type,unsigned zero){
    require(actor_unit==event_unit125&&(type==3||type==4||type==12)&&!zero,"Native acknowledgement arguments changed");
    ++voices125;result[0]=-2;result[1]=0;return result;
}
static void __fastcall refresh125(void*,void*){++refreshes125;}
static bool close125(float a,float b){return std::abs(std::remainder(a-b,6.283185307f))<.0002f;}
static void update125(MovePoint100& point,bool ticket){
    unsigned before{},after{};(void)ticket; // No queued-order provenance is required by the native voice gate.
    auto move=reinterpret_cast<MoveApply125>(base+0x45e930);
    __asm mov before,esp
    move(reinterpret_cast<void*>(fixture_group100),environment,&point,1234,expected_run125,0);
    __asm mov after,esp
    require(before==after,"Native move/voice ABI stack imbalance");
}
int main(){SetUnhandledExceptionFilter(&fault100);try{
    NativeFixture100 f;using P=NativeFixture100;auto actor=P::ptr(f.actor),cam=P::ptr(f.camera);
    // Acquisition reads the model heading, even when the old camera faces back.
    for(unsigned bearing:{0u,16384u,32768u,49152u,65535u})for(unsigned gait:{0u,1u,2u}){
        possessed=0;possession_lease.release();state->mode=ARMED;state->acquire=1;state->input=0;
        field<unsigned short>(actor,0x64)=static_cast<unsigned short>(bearing);field<unsigned>(actor,0x1f0)=0;field<unsigned>(actor,0x244)=gait;
        field<float>(actor,0x8c)=.5f;field<float>(actor,0x94)=1.f;
        field<float>(cam,0x64)=-std::sin(entry125::facing(bearing));field<float>(cam,0x6c)=-std::cos(entry125::facing(bearing));
        require(adopt_possession97(actor)&&close125(state->yaw,entry125::facing(bearing)),"Entry did not use soldier facing");
        require(state->input==LONG(FORWARD|(gait?RUN:0)),"Entry lost native walk/run");
        require(entry_input125(0,true)==unsigned(state->input),"Unpressed keys cancelled carried movement");
        require(entry_input125(BACKWARD,true)==BACKWARD&&!entry_movement125,"Manual movement did not take over");
        InterlockedExchange(&entry_movement125,FORWARD|RUN);require(!entry_input125(0,false)&&!entry_movement125,"Exit/focus loss retained auto-move");
    }
    require(!entry125::carry(0,0,0,0)&&!entry125::carry(0,3,2,0)&&!entry125::carry(0x44,1,2,0)&&!entry125::carry(0,1,NAN,0),"Stationary/recovery/unknown state auto-moved");
    // Mounted acquisition must use the proved horse heading/gait, while the
    // camera and weapons remain attached to the original rider.
    auto horse=P::ptr(f.peer);std::array<unsigned char,0x200> horse_config{};uintptr_t passenger=actor;
    field<uintptr_t>(unit,0xf9c)=reinterpret_cast<uintptr_t>(horse_config.data());field<uintptr_t>(unit,0x2c)=P::ptr(f.config);
    field<uintptr_t>(actor,0x210)=horse;field<uintptr_t>(horse,0)=base+0x15e8524;field<uintptr_t>(horse,0x20c)=field<uintptr_t>(unit,0xf9c);
    field<unsigned>(horse,0x6cc)=1;field<uintptr_t>(horse,0x6d0)=reinterpret_cast<uintptr_t>(&passenger);
    field<unsigned short>(horse,0x64)=16384;field<unsigned short>(actor,0x64)=49152;
    field<unsigned>(horse,0x244)=1;field<float>(horse,0x8c)=3;
    possessed=0;possession_lease.release();state->mode=ARMED;state->acquire=1;state->input=0;
    require(adopt_possession97(actor)&&possession_lease.identity.mount==horse&&possessed==actor&&
        close125(state->yaw,1.570796327f)&&state->input==(FORWARD|RUN),"Mounted entry lost native horse heading/gait or rider identity");
    field<uintptr_t>(unit,0xf9c)=P::ptr(f.config);field<uintptr_t>(unit,0x2c)=0;field<uintptr_t>(actor,0x210)=0;
    field<uintptr_t>(horse,0)=base+0x15dfec8;field<uintptr_t>(horse,0x20c)=P::ptr(f.config);
    possessed=0;possession_lease.release();state->mode=ARMED;state->acquire=1;state->input=0;entry_movement125=0;
    field<uintptr_t>(actor,0)=0;require(!adopt_possession97(actor)&&!entry_movement125&&!possessed,"Invalid actor acquired movement");field<uintptr_t>(actor,0)=base+0x15dfec8;
    require(adopt_possession97(actor),"Restore infantry fixture");
    // A moving/rolled native head must not shift the opponent-centered limits.
    for(float authored:{-3.f,-1.f,0.f,1.f,3.f})for(float roll:{-.8f,0.f,.8f})
    for(float anchor:{-3.13f,-1.57f,0.f,1.57f,3.13f})for(float offset:{-2.f,-.5f,0.f,.5f,2.f})for(float elevation:{-1.5f,0.f,1.5f}){
        deathpose::Eye e{{4,10,-20,4+std::sin(authored),10,-20+std::cos(authored)},
            {std::cos(authored)*std::sin(roll),std::cos(roll),-std::sin(authored)*std::sin(roll)}},out{};
        require(look125::free_eye(e,anchor+offset,elevation,anchor,out),"Opponent-look geometry rejected");
        float yaw{},pitch{};require(animationview::angles(out,yaw,pitch),"Opponent-look basis invalid");
        float wanted=anchor+std::clamp(offset,-look104::yaw_limit,look104::yaw_limit);
        float right[3]={std::cos(wanted),0,-std::sin(wanted)};
        require(close125(yaw,wanted)&&std::abs(pitch-elevation)<.0002f&&!memcmp(e.view,out.view,12)&&
            std::abs(deathpose::dot(out.up,right)-std::sin(roll))<.00002f,"Opponent look lost pitch, origin, roll or angular bounds");
    }
    // Execute production camera composition: opponent behind the authored head,
    // one initial snap, same-serial free-look, then a different opponent/serial.
    DWORD old{};require(VirtualProtect(f.image,0x1580000,PAGE_EXECUTE_READWRITE,&old)!=0,"Fixture write protection");
    auto refresh_rva=unsigned(field<uintptr_t>(base+0x15d22e8,0xf4)-base);f.jump(refresh_rva,reinterpret_cast<void*>(&refresh125));
    auto native_eye=[&](float yaw=0.f,float pitch=0.f){
        deathpose::Eye e{{4,10,-20,4+std::sin(yaw)*std::cos(pitch),10+std::sin(pitch),-20+std::cos(yaw)*std::cos(pitch)},
            {-std::sin(yaw)*std::sin(pitch),std::cos(pitch),-std::cos(yaw)*std::sin(pitch)}};
        memcpy(reinterpret_cast<void*>(cam+8),e.view,24);memcpy(reinterpret_cast<void*>(cam+0x58),e.up,12);
    };
    state->heartbeat=GetTickCount64();state->input=0;
    melee60::Request r{};r.pid=GetCurrentProcessId();r.lease=state->lease_generation;r.player=state->actor_id;r.player_unit=state->unit_id;
    r.opponent=15;r.opponent_unit=9;r.serial=1;r.phase=melee60::Facing;r.heartbeat=GetTickCount64();r.opponent_position[2]=-5;
    std::vector<unsigned char> enemy_unit(0x1000);field<unsigned>(P::ptr(enemy_unit),4)=9;
    field<uintptr_t>(horse,0x214)=P::ptr(enemy_unit);field<unsigned>(horse,0x1c8)=15;field<unsigned char>(horse,0xf0)=1;
    field<float>(horse,0x50)=-5;field<unsigned short>(horse,0x64)=0;
    auto poses=[&](){for(auto subject:{actor,horse}){bool player{},opponent{};uintptr_t rider{};
        require(melee_motion_identity(subject,r,player,opponent,rider),"Camera proof fixture participant invalid");observe_combat_pair126(subject,r,player);}};
    field<unsigned short>(actor,0x64)=0;poses();melee60::publish(melee_channel,r);
    native_eye(.7f,.2f);auto presented_eye=std::array<float,6>{};memcpy(presented_eye.data(),reinterpret_cast<void*>(cam+8),24);
    state->yaw=.7f;state->pitch=.2f;adjust_eye(cam);
    require(!memcmp(presented_eye.data(),reinterpret_cast<void*>(cam+8),24)&&combat_entry_locked126&&!combat_focus125.unlocked&&refreshes125==1,"QTE entry replaced the native animated eye");
    mouse_look101(120,40,0,.008f);require(close125(state->yaw,.7f)&&std::abs(state->pitch-.2f)<.0001f,"Mouse look overrode initial native facing");
    for(float angle:{1.f,1.5f,2.f}){field<unsigned short>(actor,0x64)=unsigned(angle*65536.f/6.283185307f);poses();native_eye(angle,.1f);adjust_eye(cam);
        require(close125(state->yaw,angle)&&std::abs(state->pitch-.1f)<.0001f&&combat_entry_locked126,"Camera did not follow native turn frames or unlocked early");}
    field<unsigned short>(actor,0x64)=32768;poses();native_eye(3.141592654f,0);adjust_eye(cam);
    require(close125(state->yaw,3.141592654f)&&combat_focus125.unlocked&&!combat_entry_locked126,"Mutual native facing did not release free-look without a jump");
    mouse_look101(120,40,0,.008f);require(close125(state->yaw,3.141592654f+.3f)&&std::abs(state->pitch+.1f)<.0001f,"Free-look did not resume after the native lock");
    state->yaw=3.141592654f+.5f;state->pitch=.4f;r.phase=melee60::NativeCombat;melee60::publish(melee_channel,r);native_eye(-1.f,0);adjust_eye(cam);
    require(close125(state->yaw,3.141592654f+.5f)&&std::abs(state->pitch-.4f)<.0001f,"Same QTE reset free-look to authored head turn");
    r.serial=2;r.opponent=16;r.opponent_position[0]=5;r.opponent_position[2]=0;r.phase=melee60::Facing;
    field<unsigned>(horse,0x1c8)=16;field<float>(horse,0x48)=5;field<float>(horse,0x50)=0;field<unsigned short>(horse,0x64)=49152;
    poses();melee60::publish(melee_channel,r);native_eye(3.141592654f,.1f);adjust_eye(cam);
    require(close125(state->yaw,3.141592654f)&&combat_entry_locked126,"Next opponent caused a target-vector snap");
    field<unsigned short>(actor,0x64)=16384;poses();native_eye(1.570796327f,0);adjust_eye(cam);
    require(close125(state->yaw,1.570796327f)&&!combat_entry_locked126,"New pair did not unlock on its own native facing");
    state->yaw=-2.f;native_eye();adjust_eye(cam);
    require(close125(state->yaw,1.570796327f+look104::yaw_limit),"Opponent-centered head-look bound lost");
    r.phase=melee60::Off;melee60::publish(melee_channel,r);combat_focus125.reset();combat_pair_reset126();
    // Actual native move wrapper and voice subscriber execute in a private copy
    // of the hash-pinned engine. Planning/audio/event-queue providers are substituted.
    f.jump(0x42df70,reinterpret_cast<void*>(&plan125));f.jump(0x4668f0,reinterpret_cast<void*>(&apply125));
    f.jump(0x276b20,reinterpret_cast<void*>(&event125));f.jump(0xf04d0,reinterpret_cast<void*>(&borrowed_free125));
    f.jump(0x3050a0,reinterpret_cast<void*>(&play125));
    std::array<unsigned char,0x400> bus{};std::array<uintptr_t,3> vtable={0,0,reinterpret_cast<uintptr_t>(&dispatch125)};
    field<uintptr_t>(environment,0x98)=reinterpret_cast<uintptr_t>(bus.data());
    field<uintptr_t>(reinterpret_cast<uintptr_t>(bus.data()),0x2f4)=reinterpret_cast<uintptr_t>(vtable.data());
    require(MH_Initialize()==MH_OK,"Fixture MinHook");require(follow_voice_signatures125(),"Original native voice signatures");create_follow_voice125();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable scoped voice hooks");
    event_unit125=unit;state->follow=1;command_mode100=command100::MoveMode::Follow;command_voice_reset125();
    MovePoint100 point{{10,3,20},0,0};auto update=[&](bool ticket){update125(point,ticket);};
    for(unsigned i=0;i<1000;++i){expected_run125=i%2;point.position[0]=float(i);update(true);}
    require(moves125==1000&&events125==1000&&voices125==1,"Follow did not preserve all moves/events with one acknowledgement");
    update(false);require(voices125==1,"A native follow update without a ticket escaped suppression");
    state->follow=0;command_mode100=command100::MoveMode::Advance;update(false);require(voices125==2,"Ordinary native move was muted");
    state->follow=1;command_mode100=command100::MoveMode::Follow;command_voice_reset125();update(true);require(voices125==3,"Further orders did not rearm acknowledgement");
    state->follow=0;update(true);require(voices125==4,"Follow cancellation muted native voice");state->follow=1;
    command_voice_reset125();update(true);update(true);require(voices125==5,"Follow restart did not acknowledge once");
    auto previous_unit=event_unit125;event_unit125=P::ptr(f.peer);update(true);require(voices125==6,"Foreign unit voice was muted");event_unit125=previous_unit;
    // Arbitrary destinations and both native gait types remain quiet for the
    // entire Follow mode, independently of old ticket age, queues or matching.
    for(unsigned i=0;i<1000;++i){expected_run125=i%2;point.position[0]=float(i)+.001f;point.position[2]-=.03125f;update(false);}
    require(voices125==6,"Follow gate expired or depended on movement destination");
    // A different acknowledgement type stays native and does not rearm walk/run.
    deliver125(unit,12);require(voices125==7,"Other native acknowledgement was muted");update(false);require(voices125==7,"Other audio rearmed follow speech");
    defer125=true;command_voice_reset125();auto voice_start=voices125,move_start=moves125,event_start=events125;
    for(unsigned i=0;i<1000;++i){expected_run125=i%2;point.position[0]=float(i);update(false);}flush125();
    require(voices125==voice_start+1&&moves125==move_start+1000&&events125==event_start+1000,"Deferred follow queue changed movement/events or spoke repeatedly");
    voice_start=voices125;for(unsigned i=0;i<1000;++i)update(false);flush125();
    require(voices125==voice_start,"Large native queues disabled suppression");
    voice_start=voices125;command_voice_reset125();update(false);update(false);flush125();require(voices125==voice_start+1,"New player order did not permit one new acknowledgement");
    voice_start=voices125;update(false);state->follow=0;flush125();state->follow=1;require(voices125==voice_start+1,"Follow exit muted a delayed callback");
    voice_start=voices125;state->mode=IDLE;deliver125(unit,3);state->mode=POSSESSED;require(voices125==voice_start+1,"RTS voice was muted");
    auto saved_actor=field<unsigned>(actor,0x1c8);field<unsigned>(actor,0x1c8)=saved_actor+1;
    voice_start=voices125;deliver125(unit,3);require(voices125==voice_start+1,"Invalid native ownership suppressed audio");field<unsigned>(actor,0x1c8)=saved_actor;
    command_voice_reset125();defer125=false;voice_start=voices125;update(false);update(false);require(voices125==voice_start+1,"Invalid ownership or delayed callback poisoned future Follow mode");
    require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&follow_voice_signatures125(),"Exact native voice code was not restored");require(MH_Uninitialize()==MH_OK,"Fixture hook shutdown");
    combat_focus125.valid=true;InterlockedExchange(&entry_movement125,FORWARD);relinquish();require(!entry_movement125&&!combat_focus125.valid&&!combat_entry_locked126&&!state->follow,"Possession exit retained handoff state");
    std::cout<<"PASS: Test125 production acquisition/cardinal headings/native gait/manual takeover, native-animation QTE entry, proved facing lock and bounded free-look; original retail move/voice ABI, 4000 preserved immediate/deferred native updates, persistent one-ack Follow gate, unrelated orders/units/RTS/ownership/reset and exact hook restoration. No live game.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
