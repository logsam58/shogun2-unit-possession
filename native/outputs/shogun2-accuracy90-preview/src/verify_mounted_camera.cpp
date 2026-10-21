#include "adapter.cpp"
int main(){void* image=nullptr;try{
    image=VirtualAlloc(nullptr,0x1bc9000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(image!=nullptr,"Fixture image");base=reinterpret_cast<uintptr_t>(image);
    const unsigned char predicate[]={0x31,0xc0,0xc3};memcpy(reinterpret_cast<void*>(base+0x1821e0),predicate,sizeof predicate);
    DWORD old=0;require(VirtualProtect(reinterpret_cast<void*>((base+0x1821e0)&~uintptr_t(4095)),4096,PAGE_EXECUTE_READ,&old)!=0,"Fixture routing predicate");
    FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(base+0x1821e0),sizeof predicate);
    Status status{};state=&status;enabled=1;state->heartbeat=GetTickCount64();
    std::vector<unsigned char> root(0x120),env(0x28300),common(0x100),phase(0x40),manager(0x910),cam(0x200),view(0x40),holder(0x210),mount_holder(0x20),
        selected(0x500),selection_owner(0x120),config(0x100),mount_config(0x100),record(0x100),actor(0x700),horse(0x708),actor_unit(0x1000);
    auto ptr=[](auto& v){return reinterpret_cast<uintptr_t>(v.data());};auto a=ptr(actor),h=ptr(horse),u=ptr(actor_unit),v=ptr(view);
    std::array<uintptr_t,2> holders{ptr(holder),ptr(mount_holder)};std::array<uintptr_t,1> passengers{a},units{u};
    field<uintptr_t>(base,0x1bc8180)=ptr(root);field<uintptr_t>(ptr(root),0x110)=ptr(env);field<uintptr_t>(ptr(env),8)=ptr(common);
    field<uintptr_t>(ptr(common),0xb4)=ptr(phase);field<unsigned>(ptr(phase),0x14)=6;
    field<uintptr_t>(ptr(env),0x281fc)=ptr(manager);field<uintptr_t>(ptr(manager),0x8e0)=ptr(cam);field<uintptr_t>(ptr(manager),0x8fc)=a;
    field<uintptr_t>(ptr(cam),0)=base+0x15d22e8;field<uintptr_t>(ptr(cam),0x180)=ptr(manager);field<uintptr_t>(ptr(cam),0x184)=v;field<uintptr_t>(ptr(cam),0x188)=a;
    field<unsigned>(v,0x18)=2;field<uintptr_t>(v,0x1c)=reinterpret_cast<uintptr_t>(holders.data());field<uintptr_t>(ptr(holder),8)=a;field<uintptr_t>(ptr(mount_holder),8)=h;
    field<uintptr_t>(ptr(env),0x2826c)=ptr(selected);field<uintptr_t>(ptr(selected),0x4a8)=ptr(selection_owner);auto group=ptr(selection_owner)+0xb0;
    field<uintptr_t>(group,0)=base+0x15c4da0;field<unsigned>(group,0x10)=field<unsigned>(group,0x14)=1;field<uintptr_t>(group,0x18)=reinterpret_cast<uintptr_t>(units.data());
    field<uintptr_t>(a,0)=base+0x15dfec8;field<uintptr_t>(h,0)=base+0x15e8524;
    for(auto subject:{a,h}){field<unsigned char>(subject,0xf0)=1;field<uintptr_t>(subject,0x214)=u;field<uintptr_t>(subject,0x28)=ptr(env);}
    field<uintptr_t>(a,0x20c)=ptr(config);field<uintptr_t>(h,0x20c)=ptr(mount_config);field<uintptr_t>(a,0x210)=h;
    field<unsigned char>(h,0xa1)=1;field<unsigned>(a,0x1f0)=29;field<unsigned>(h,0x1c8)=32;
    field<uintptr_t>(h,0x6d0)=reinterpret_cast<uintptr_t>(passengers.data());field<unsigned>(h,0x6cc)=1;
    field<unsigned>(u,4)=7;field<uintptr_t>(u,0x2c)=ptr(record);field<unsigned>(ptr(record),0x1c)=0;field<uintptr_t>(u,0xf9c)=ptr(mount_config);
    require(followed(a,true,false)&&!possession_lease.active,"Mounted preflight is read-only");
    require(followed(a,true)&&possession_lease.identity.mount==h&&possession_lease.identity.actor==a,"Human camera and exact horse leased independently");possessed=a;
    require(followed(a,false)&&locomotion_actor()==h&&locomotion_config()==ptr(mount_config),"Mounted live ownership");
    state->mode=POSSESSED;state->lease_generation=possession_lease.generation;state->actor_id=0;state->unit_id=7;
    melee60::Request request;request.pid=GetCurrentProcessId();request.phase=melee60::NativeCombat;request.lease=state->lease_generation;
    request.player_kind=1;request.player=32;request.player_unit=7;request.opponent=9;request.opponent_unit=8;request.serial=1;request.heartbeat=GetTickCount64();request.opponent_position[0]=100;
    require(PossessionMeleeCommitment63(&request)==63,"Animation camera commitment rejected");
    deathpose::Eye eye{{1,2,3,0,3,3},{.70710678f,.70710678f,0}};
    memcpy(reinterpret_cast<void*>(ptr(cam)+8),eye.view,sizeof eye.view);memcpy(reinterpret_cast<void*>(ptr(cam)+0x58),eye.up,sizeof eye.up);
    remember_native_eye78(ptr(cam));ModelEye face_sample;
    require(read_model_eye(native_eye_channel78,face_sample)&&leased_model_eye(face_sample)&&memcmp(face_sample.eye.view,eye.view,sizeof eye.view)==0,"Original native face sample was not retained before FPS override");
    field<uintptr_t>(base+0x15d22e8,0xf4)=base+0x1821e0;
    state->yaw=2;state->pitch=-.9f;adjust_eye(ptr(cam));
    deathpose::Eye actual_eye;memcpy(actual_eye.view,reinterpret_cast<void*>(ptr(cam)+8),sizeof actual_eye.view);memcpy(actual_eye.up,reinterpret_cast<void*>(ptr(cam)+0x58),sizeof actual_eye.up);
    float actual_yaw,actual_pitch;
    require(animation_view_active78&&memcmp(actual_eye.view,eye.view,3*sizeof(float))==0&&animationview::angles(actual_eye,actual_yaw,actual_pitch),"QTE eye translation or basis invalid");
    require(!memcmp(actual_eye.view,eye.view,sizeof eye.view)&&!memcmp(actual_eye.up,eye.up,sizeof eye.up)&&combat_entry_locked126,"Mounted QTE replaced the initial native animated eye");
    float native_yaw104=actual_yaw,native_pitch104=actual_pitch;
    mouse_look101(120,40,0,.008f);require(std::abs(state->yaw-native_yaw104)<.0001f&&std::abs(state->pitch-native_pitch104)<.0001f,"Mounted free-look unlocked before pair facing");
    look126::Pose player126{0,0,16384,GetTickCount64(),0,false,true},opponent126{100,0,49152,GetTickCount64(),0,false,true};
    combat_pair126.observe(request,true,player126);combat_pair126.observe(request,false,opponent126);
    memcpy(reinterpret_cast<void*>(ptr(cam)+8),eye.view,sizeof eye.view);memcpy(reinterpret_cast<void*>(ptr(cam)+0x58),eye.up,sizeof eye.up);adjust_eye(ptr(cam));
    require(!combat_entry_locked126&&std::abs(state->yaw-native_yaw104)<.0001f&&std::abs(state->pitch-native_pitch104)<.0001f,"Mounted native lock caused a camera jump");
    auto body_before104=actor,horse_before104=horse;float yaw_before104=state->yaw;
    mouse_look101(120,40,0,.008f);
    require(std::abs(std::remainder(state->yaw-yaw_before104-.3f,6.283185307f))<.00001f&&std::abs(state->pitch-(native_pitch104-.1f))<.00001f,"Committed production mouse input rejected horizontal/vertical look");
    // The native camera supplies a fresh animated eye each frame; never rotate
    // the already-presented result again or mutate the paired actors.
    memcpy(reinterpret_cast<void*>(ptr(cam)+8),eye.view,sizeof eye.view);memcpy(reinterpret_cast<void*>(ptr(cam)+0x58),eye.up,sizeof eye.up);
    adjust_eye(ptr(cam));memcpy(actual_eye.view,reinterpret_cast<void*>(ptr(cam)+8),sizeof actual_eye.view);memcpy(actual_eye.up,reinterpret_cast<void*>(ptr(cam)+0x58),sizeof actual_eye.up);
    deathpose::Eye expected152{};float expected_y152{},expected_p152{};require(view152::eye(eye,.3f,-.1f,expected152)&&animationview::angles(expected152,expected_y152,expected_p152),"Rolled face-relative expected basis");
    require(animationview::angles(actual_eye,actual_yaw,actual_pitch)&&std::abs(std::remainder(actual_yaw-expected_y152,6.283185307f))<.0001f&&std::abs(actual_pitch-expected_p152)<.0001f,"Committed camera did not apply local eye look relative to current rolled face");
    require(body_before104==actor&&horse_before104==horse,"Head-look changed native combat body/horse state");
    melee60::Request off;PossessionMeleeCommitment63(&off);
    // The authored kill/get-away motion continues after the prompt is gone.
    melee_handoff78.observe(h,possession_lease.generation,32,7,1,true);
    float handoff_yaw104=state->yaw;mouse_look101(40,0,0,.008f);
    memcpy(reinterpret_cast<void*>(ptr(cam)+8),eye.view,sizeof eye.view);memcpy(reinterpret_cast<void*>(ptr(cam)+0x58),eye.up,sizeof eye.up);adjust_eye(ptr(cam));
    require(view152::eye(eye,.4f,-.1f,expected152)&&animationview::angles(expected152,expected_y152,expected_p152),"Handoff local eye basis");
    require(animation_view_active78&&std::abs(std::remainder(state->yaw-expected_y152,6.283185307f))<.0001f&&std::abs(state->pitch-expected_p152)<.0001f&&body_before104==actor&&horse_before104==horse,"Native animation handoff lost face-relative eye freedom or changed combat actors");
    melee_handoff78.reset();
    
    field<unsigned short>(h,0x64)=65000;state->yaw=mountedlook85::heading(65000)+2.5f;state->pitch=.2f;
    adjust_eye(ptr(cam));
    require(!animation_view_active78&&std::abs(mountedlook85::relative(state->yaw,65000)-mountedlook85::half_arc)<.00001f,"Native camera failed mounted arc clamp");
    require(mounted_heading85==65000,"Validated horse heading not published");
    float actual=std::atan2(field<float>(ptr(cam),0x14)-field<float>(ptr(cam),8),field<float>(ptr(cam),0x1c)-field<float>(ptr(cam),0x10));
    require(std::abs(std::remainder(actual-state->yaw,mountedlook85::circle))<.00001f,"Camera target differs from limited aim yaw");
    require(std::abs(state->pitch-.2f)<.00001f,"Horizontal cap changed vertical aim");
    field<unsigned>(h,0x1c8)=33;require(!followed(a,false),"Recycled mount serial cannot survive lease");field<unsigned>(h,0x1c8)=32;
    field<uintptr_t>(ptr(mount_holder),8)=0;require(!followed(a,false),"Retired mount holder cannot survive lease");field<uintptr_t>(ptr(mount_holder),8)=h;
    field<uintptr_t>(h,0x28)=0;require(!followed(a,false),"Mount from another environment rejected");field<uintptr_t>(h,0x28)=ptr(env);
    field<unsigned char>(h,0xf0)=0;require(!followed(a,false)&&!dead_view(),"Dead mount cannot retain live steering or impersonate rider death");field<unsigned char>(h,0xf0)=1;
    units[0]=0;require(!followed(a,false),"Selection changed");units[0]=u;
    field<uintptr_t>(ptr(cam),0x188)=h;require(!followed(a,false),"Camera cannot switch from rider to horse");field<uintptr_t>(ptr(cam),0x188)=a;
    field<uintptr_t>(a,0x210)=0;field<uintptr_t>(u,0xf9c)=ptr(config);field<unsigned char>(a,0xa1)=1;
    require(!followed(a,false)&&followed(a,true,false),"Dismount retires old mounted lease and permits fresh foot admission");
    // Exercise actual infantry skin callback with RMB released, not only pure geometry.
    require(followed(a,true),"Foot lease acquisition");state->lease_generation=possession_lease.generation;state->heartbeat=GetTickCount64();
    field<unsigned>(a,0x1f0)=8;field<unsigned>(u,0xf7c)=1;
    field<unsigned>(ptr(record),0)=3;field<unsigned>(ptr(record),0x60)=150;field<float>(ptr(record),0x6c)=50;
    auto getter=base+0x14ad40;require(VirtualProtect(reinterpret_cast<void*>(getter&~uintptr_t(4095)),4096,PAGE_EXECUTE_READWRITE,&old)!=0,"Fake native projectile getter");
    field<unsigned char>(getter,0)=0xb8;field<uintptr_t>(getter,1)=ptr(record);field<unsigned char>(getter,5)=0xc3;FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(getter),6);
    std::vector<unsigned char> binding(0x100),asset(0x80),hierarchy(8*20),skin(0x20);std::array<std::array<unsigned,2>,7> bones{};
    const unsigned indices[7]={2,3,4,1,5,6,7},offsets[7]={0xa8,0xa4,0x9c,0xa0,0x90,0x94,0x98};const int parents[8]={-1,0,1,1,1,2,3,0};
    field<uintptr_t>(ptr(holder),0x1e4)=ptr(binding);field<uintptr_t>(ptr(binding),0x1c)=ptr(asset);
    field<unsigned>(ptr(asset),0x3c)=8;field<uintptr_t>(ptr(asset),0x40)=ptr(hierarchy);
    for(unsigned i=0;i<8;++i)field<int>(ptr(hierarchy),i*20+16)=parents[i];
    for(unsigned i=0;i<7;++i){bones[i][1]=indices[i];field<uintptr_t>(ptr(binding),offsets[i])=reinterpret_cast<uintptr_t>(bones[i].data());}
    float palette[96]{},native_palette[96]{};for(unsigned i=0;i<8;++i){native_palette[i*12]=native_palette[i*12+5]=native_palette[i*12+10]=1;native_palette[i*12+7]=float(i)*.1f;}
    float world[12]={1,0,0,0,0,1,0,0,0,0,1,0};memcpy(reinterpret_cast<void*>(ptr(holder)+0x24),world,sizeof world);
    field<unsigned>(ptr(skin),4)=8;field<uintptr_t>(ptr(skin),8)=reinterpret_cast<uintptr_t>(palette);
    state->aiming=0;state->aim_pose=0;equipment_slot81=equipment81::Ranged;
    for(unsigned projectile:{3u,60u})for(unsigned clip:{130u,133u})for(float pitch:{-1.1f,0.f,.8f})for(unsigned phase96:{unsigned(WEAPON_IDLE),unsigned(WEAPON_RELOAD)}){
      field<unsigned>(ptr(record),0)=projectile;field<unsigned>(ptr(record),0x60)=projectile==3?150:100;field<float>(ptr(record),0x6c)=projectile==3?50.f:200.f;
      field<unsigned>(a,0x1d4)=clip;state->pitch=pitch;state->weapon_phase=phase96;memcpy(palette,native_palette,sizeof palette);
      capture_aim_frame(ptr(cam));ViewRay eye101{};require(read_aim_frame(eye101,true),"Current foot presentation eye not published by native camera boundary");
      apply_soldier_pose(ptr(holder),ptr(skin));float expected=weapon_pose_pitch(projectile,clip,pitch);
      require(!state->pose_error&&std::abs(palette[2*12+6]-std::sin(expected))<.00001f,"Released RMB/reload lost arm pitch");
      require(!memcmp(palette,native_palette,12*sizeof(float))&&!memcmp(palette+12,native_palette+12,12*sizeof(float)),"Infantry pelvis/neck changed");
      float once[96];memcpy(once,palette,sizeof once);apply_soldier_pose(ptr(holder),ptr(skin));require(!memcmp(once,palette,sizeof once),"Repeated skin callback accumulated pitch");
    }
    equipment_slot81=equipment81::Melee;memcpy(palette,native_palette,sizeof palette);apply_soldier_pose(ptr(holder),ptr(skin));
    require(!memcmp(palette+2*12,native_palette+2*12,24*sizeof(float)),"Melee arms changed");
    VirtualFree(image,0,MEM_RELEASE);image=nullptr;
    std::cout<<"PASS: production followed() mounted camera acquisition, human/mount split, environment/serial/holder/death/selection rejection and dismount retirement. No live game modified.\n";return 0;
}catch(const std::exception& e){if(image)VirtualFree(image,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
