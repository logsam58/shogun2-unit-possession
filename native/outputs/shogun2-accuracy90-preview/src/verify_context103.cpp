// Production full ownership plus bounded original native mouse targeting,
// cycling and get-up clocks. Scene hits, target metadata/eligibility, window,
// count provider and motion integrator are explicit fixture substitutions.
#define main retained_command100_main
#include "verify_native_commands100.cpp"
#undef main
static unsigned clock_events103,original_motion103,preview_calls103,input_calls103;static uintptr_t preview_hit103;
static uintptr_t __fastcall input103(void*,void*,uintptr_t a,uintptr_t b){++input_calls103;return a^b;}
static void __fastcall motion103(void*,void*){++original_motion103;}
static bool __fastcall animation103(void*,void*,void*,unsigned){++clock_events103;return false;}
static unsigned __fastcall count103(void* view,void*){return field<unsigned>(reinterpret_cast<uintptr_t>(view),0x18);}
static uintptr_t __fastcall physical_actor103(void* object,void*){return field<uintptr_t>(reinterpret_cast<uintptr_t>(object),4);}
static uintptr_t __fastcall no_mount103(void*,void*){return 0;}
static unsigned char hits103[4*28],arena103[256];static unsigned arena_used103;
static uintptr_t actor_vtable103[50],physical_vtable103[8];
static uintptr_t shapes103[4][4],physical103[4][2];
static unsigned hit_count103;
static void __fastcall scene103(void*,void*,void* vector,const void* segment,unsigned mode,unsigned sorted,unsigned mask,unsigned a,unsigned b){
    require(*static_cast<const unsigned*>(segment)==1&&mode==2&&sorted==1&&mask==0x3b0b&&a==0&&b==0,"Native target scene contract changed");
    auto v=reinterpret_cast<uintptr_t>(vector);field<unsigned>(v,0)=field<unsigned>(v,4)=hit_count103;field<uintptr_t>(v,8)=reinterpret_cast<uintptr_t>(hits103);
}
static void __cdecl sort103(void* begin,void* end,unsigned count,unsigned){require(reinterpret_cast<uintptr_t>(end)-reinterpret_cast<uintptr_t>(begin)==count*28,"Native target hit stride changed");}
static void* __cdecl alloc103(unsigned size){require(arena_used103+size<=sizeof arena103,"Native target fixture arena exceeded");auto out=arena103+arena_used103;arena_used103+=(size+3)&~3u;memset(out,0,size);return out;}
static void* __fastcall metadata103(void* out,void*,uintptr_t target,const float* point,uintptr_t,uintptr_t){auto p=reinterpret_cast<uintptr_t>(out);field<uintptr_t>(p,0)=target;memcpy(reinterpret_cast<void*>(p+4),point,12);return out;}
static void __fastcall finish103(void*,void*,uintptr_t){}
static uintptr_t __fastcall target_player103(void*,void*){return 2;}
static uintptr_t __fastcall local_player103(void*,void*){return 1;}
static void __fastcall preview103(void*,void*,uintptr_t cursor,uintptr_t hit){require(cursor&&hit,"Native preview lost cursor/hit");preview_hit103=hit;++preview_calls103;}
static void patch103(NativeFixture100& f,unsigned rva,void* function){DWORD old{};require(VirtualProtect(reinterpret_cast<void*>(base+rva),16,PAGE_EXECUTE_READWRITE,&old)!=0,"Fixture patch protection");f.jump(rva,function);VirtualProtect(reinterpret_cast<void*>(base+rva),16,old,&old);FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(base+rva),16);}
int main(){SetUnhandledExceptionFilter(&fault100);try{
    NativeFixture100 f;using P=NativeFixture100;
    HWND window=CreateWindowExW(0,L"STATIC",L"Context103 fixture",WS_POPUP,0,0,1280,720,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);require(window!=nullptr,"Hidden context fixture window");
    input_window=window;native_window_proc=&stock_window100;state->heartbeat=GetTickCount64();
    // Original camera cycling writes both actor pointers without a possession
    // guard. Install the production hook and test forwards and suppression.
    uintptr_t view_vt[16]{};memcpy(view_vt,reinterpret_cast<void*>(base+0x15d3540),sizeof view_vt);
    view_vt[0x1c/4]=reinterpret_cast<uintptr_t>(&count103);view_vt[0x34/4]=base+0x340cf0;
    field<uintptr_t>(P::ptr(f.view),0)=reinterpret_cast<uintptr_t>(view_vt);
    field<unsigned>(possessed,0x218)=5;field<unsigned>(P::ptr(f.peer),0x218)=9;
    require(MH_Initialize()==MH_OK,"Context hook initialize");create_context_hooks103();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Context hook enable");
    auto cycle=reinterpret_cast<CameraCycle103>(base+0x2f79a0);unsigned stack_before{},stack_after{};
    for(unsigned i=0;i<200;++i){
        enabled=0;field<unsigned>(P::ptr(f.camera),0x1a4)=5;
        __asm mov stack_before,esp
        cycle(f.camera.data(),0x12345678,1.f);
        __asm mov stack_after,esp
        require(stack_before==stack_after&&field<uintptr_t>(P::ptr(f.camera),0x188)==P::ptr(f.peer)&&field<uintptr_t>(P::ptr(f.manager),0x8fc)==P::ptr(f.peer),"Original native cycle/ABI did not reproduce pointer replacement");
        field<uintptr_t>(P::ptr(f.camera),0x188)=field<uintptr_t>(P::ptr(f.manager),0x8fc)=possessed;field<unsigned>(P::ptr(f.camera),0x1a4)=5;enabled=1;state->heartbeat=GetTickCount64();
        cycle(f.camera.data(),0,1.f);cycle(f.camera.data(),0,-1.f);
        require(field<uintptr_t>(P::ptr(f.camera),0x188)==possessed&&field<uintptr_t>(P::ptr(f.manager),0x8fc)==possessed&&field<unsigned>(P::ptr(f.camera),0x1a4)==5,"Owned native cycle mutated current possession");
    }
    state->release=1;cycle(f.camera.data(),0,1.f);require(field<uintptr_t>(P::ptr(f.camera),0x188)==P::ptr(f.peer),"Exit suppressed native cycling");state->release=0;
    field<uintptr_t>(P::ptr(f.camera),0x188)=field<uintptr_t>(P::ptr(f.manager),0x8fc)=possessed;
    field<unsigned>(P::ptr(f.camera),0x1a4)=5;++field<unsigned>(possessed,0x1c8);cycle(f.camera.data(),0,-1.f);require(field<uintptr_t>(P::ptr(f.camera),0x188)!=possessed,"Stale serial gained cycle ownership");--field<unsigned>(possessed,0x1c8);
    field<uintptr_t>(P::ptr(f.camera),0x188)=field<uintptr_t>(P::ptr(f.manager),0x8fc)=possessed;
    // Exercise the production input wrapper with full current identity rather
    // than the isolated legacy prologue fixture's copied forwarding wrapper.
    unsigned char input_object[32]{};field<uintptr_t>(reinterpret_cast<uintptr_t>(input_object),0x1c)=environment;
    original_camera_input=reinterpret_cast<CameraInput>(&input103);
    for(unsigned i=0;i<200;++i){state->heartbeat=GetTickCount64();require(camera_input(input_object,nullptr,i,77)==0&&input_calls103==0&&camera_callback_depth==0,"Owned dispatcher leaked native camera action");}
    ++field<unsigned>(possessed,0x1c8);require(camera_input(input_object,nullptr,123,77)==(123^77)&&input_calls103==1&&camera_callback_depth==0,"Invalid identity dispatcher did not forward exactly");--field<unsigned>(possessed,0x1c8);
    // Execute the original hover-provider update (the retained fixture replaces
    // this function). Only the provider callback is substituted here.
    const unsigned char preview_prefix[]={0x80,0xb9,0x38,0x05,0,0,0,0x75};DWORD old_preview{};
    VirtualProtect(reinterpret_cast<void*>(base+0x1101da0),8,PAGE_EXECUTE_READWRITE,&old_preview);memcpy(reinterpret_cast<void*>(base+0x1101da0),preview_prefix,8);VirtualProtect(reinterpret_cast<void*>(base+0x1101da0),8,old_preview,&old_preview);
    uintptr_t provider_vt[16]{},provider[1]{};provider_vt[0x28/4]=reinterpret_cast<uintptr_t>(&preview103);provider[0]=reinterpret_cast<uintptr_t>(provider_vt);field<uintptr_t>(P::ptr(f.root),0x138)=reinterpret_cast<uintptr_t>(provider);
    for(unsigned i=0;i<200;++i){command_refresh103(P::ptr(f.root));require(preview_calls103==i+1,"Short Q refresh did not reach original native hover provider");}
    unsigned char held_hover103[60]{};field<uintptr_t>(P::ptr(f.root),0x134)=reinterpret_cast<uintptr_t>(held_hover103);
    command_refresh103(P::ptr(f.root));require(preview_hit103==reinterpret_cast<uintptr_t>(held_hover103),"Q refresh replaced native press/drag hover with release endpoint");field<uintptr_t>(P::ptr(f.root),0x134)=0;
    field<uintptr_t>(P::ptr(f.root),0x138)=0;
    // Full native get-up clock ends the actual474 animation. No elapsed-time
    // latch, synthetic last-frame decision or actor-field cleanup by production.
    patch103(f,0x3f03d0,reinterpret_cast<void*>(&animation103));original=reinterpret_cast<Motion>(&motion103);
    unsigned animation[4]{};unsigned char asset[0xa0]{};animation[1]=reinterpret_cast<unsigned>(asset);field<unsigned>(reinterpret_cast<uintptr_t>(asset),0x64)=60;
    using Clock=void(__thiscall*)(void*);
    for(unsigned clip:{0x441u,0x44bu,0x451u,0x455u,0x456u,0x457u,0x458u}){
        animation[0]=clip;field<uintptr_t>(possessed,0x474)=reinterpret_cast<uintptr_t>(animation);field<unsigned>(possessed,0x1d4)=clip;field<unsigned>(possessed,0x1f0)=0;field<unsigned>(possessed,0x1d8)=0;
        field<float>(possessed,0x8c)=3.5f;field<float>(possessed,0x90)=-2;field<float>(possessed,0xd8)=.25f;state->input=FORWARD|LEFT|RUN;
        for(unsigned frame=0;frame<60;++frame){state->heartbeat=GetTickCount64();auto before=f.actor;auto calls=original_motion103;
            require(followed(possessed,false)&&native_recovery103(possessed),"Current native recovery rejected or ended early");
            detour(f.actor.data(),nullptr);require(f.actor==before&&original_motion103==calls+1,"Recovery changed actor physics or skipped original integration");
            reinterpret_cast<Clock>(base+0x3f0370)(f.actor.data());
        }
        require(!field<uintptr_t>(possessed,0x474)&&!native_recovery103(possessed),"Completed native animation retained manual lock");
    }
    // A shot queued before impact must retire without a stop/lower animation or
    // ammunition consumption when it reaches the native dispatch boundary.
    animation[0]=0x455;field<uintptr_t>(possessed,0x474)=reinterpret_cast<uintptr_t>(animation);field<unsigned>(possessed,0x1d4)=0x455;
    uint32_t queued[13]{};queued[11]=unsigned(possessed);uintptr_t context103[1]={environment};
    ticket_environment=environment;ticket_actor=state->actor_id;ticket_unit=state->unit_id;
    field<float>(unit,0xf78)=50;auto intact_actor103=f.actor;auto canceled103=state->shots_canceled;
    state->weapon_phase=WEAPON_FIRED;state->aim_pose=AIM_READY;manual_bow_apply(queued,reinterpret_cast<uintptr_t>(context103));
    require(state->shots_canceled==canceled103+1&&!state->weapon_phase&&!state->aim_pose&&f.actor==intact_actor103&&field<float>(unit,0xf78)==50,"Queued shot interrupted recovery or consumed ammo");
    field<uintptr_t>(possessed,0x474)=0;
    for(unsigned mode:{12u,13u}){field<unsigned>(possessed,0x1f0)=mode;require(native_recovery103(possessed),"Prone posture without animation lost movement lock");}
    field<unsigned>(possessed,0x1f0)=0;field<unsigned>(possessed,0x1d4)=0x455;require(!native_recovery103(possessed),"Stale get-up clip locked recovered actor");
    field<unsigned>(possessed,0x1d4)=0;field<float>(possessed,0xd8)=0;state->input=0;
    // Original hit constructor, after original native filter. Its scene query
    // and per-target metadata/eligibility providers are replaced explicitly.
    patch103(f,0x158970,reinterpret_cast<void*>(&scene103));patch103(f,0x101a80,reinterpret_cast<void*>(&sort103));patch103(f,0xf7440,reinterpret_cast<void*>(&alloc103));
    patch103(f,0x276a90,reinterpret_cast<void*>(&metadata103));patch103(f,0x288bf0,reinterpret_cast<void*>(&finish103));patch103(f,0x14f2b0,reinterpret_cast<void*>(&target_player103));patch103(f,0x14f2a0,reinterpret_cast<void*>(&local_player103));
    std::vector<unsigned char> options(0x200),enemy(0x700);field<uintptr_t>(P::ptr(f.common),0x5c)=P::ptr(options);field<unsigned char>(P::ptr(options),0x28)=1;
    physical_vtable103[6]=reinterpret_cast<uintptr_t>(&physical_actor103);
    memcpy(actor_vtable103,reinterpret_cast<void*>(base+0x15dfec8),sizeof actor_vtable103);actor_vtable103[0xa8/4]=reinterpret_cast<uintptr_t>(&no_mount103);
    field<uintptr_t>(P::ptr(enemy),0)=base+0x15dfec8;field<uintptr_t>(P::ptr(enemy),0x214)=unit+16;
    // Original actor virtual+a8 remains native. It is a simple counterpart query
    // on this fixture; avoid altering real vtable identity used by the filter.
    auto native_counterpart=field<uintptr_t>(base+0x15dfec8,0xa8);patch103(f,unsigned(native_counterpart-base),reinterpret_cast<void*>(&no_mount103));
    for(unsigned i=0;i<4;++i){shapes103[i][0]=8;shapes103[i][3]=reinterpret_cast<uintptr_t>(physical103[i]);physical103[i][0]=reinterpret_cast<uintptr_t>(physical_vtable103);physical103[i][1]=i<2?possessed:P::ptr(enemy);
        field<uintptr_t>(reinterpret_cast<uintptr_t>(hits103),i*28)=reinterpret_cast<uintptr_t>(shapes103[i]);field<float>(reinterpret_cast<uintptr_t>(hits103),i*28+12)=float(i+1);}
    auto root=P::ptr(f.root),cursor=root+0x10c;field<uintptr_t>(cursor,4)=environment;
    using Pick=void*(__thiscall*)(void*,uintptr_t,uintptr_t,const float*,unsigned,uintptr_t,unsigned);
    unsigned char target[60]{};float ray[6]={0,2,0,0,2,100};hit_count103=3;
    for(unsigned i=0;i<200;++i){
        arena_used103=0;command_native_root100=0;reinterpret_cast<Pick>(base+0x252f50)(target,P::ptr(f.common),1,ray,0,cursor,0);
        require(field<uintptr_t>(field<uintptr_t>(reinterpret_cast<uintptr_t>(target),0x10),0)==possessed,"Native fixture did not reproduce self occlusion");
        // The prior unfiltered call sorts but doesn't change these ordered hits.
        arena_used103=0;command_native_root100=root;command_native_environment100=environment;command_native_lease100=possession_lease.generation;state->heartbeat=GetTickCount64();
        reinterpret_cast<Pick>(base+0x252f50)(target,P::ptr(f.common),1,ray,0,cursor,0);
        require(field<unsigned>(reinterpret_cast<uintptr_t>(target),4)==1&&field<uintptr_t>(field<uintptr_t>(reinterpret_cast<uintptr_t>(target),0x10),0)==P::ptr(enemy)&&field<uintptr_t>(field<uintptr_t>(reinterpret_cast<uintptr_t>(target),0x20),0)==unit+16,"Stock constructor failed to classify enemy after removing own formation hits");
        // Restore the query buffer for the next scene-provider pass.
        for(unsigned j=0;j<3;++j)field<uintptr_t>(reinterpret_cast<uintptr_t>(hits103),j*28)=reinterpret_cast<uintptr_t>(shapes103[j]);
    }
    // Filter is unavailable to stale leases, foreign cursors and ordinary RTS
    // queries, and preserves ground/building/other-unit occlusion byte-for-byte.
    unsigned vector103[3]={4,4,reinterpret_cast<unsigned>(hits103)};
    shapes103[0][0]=1;shapes103[1][0]=0x1000;shapes103[2][0]=8;shapes103[3][0]=8;
    physical103[3][1]=P::ptr(enemy);
    for(unsigned j=0;j<4;++j)field<uintptr_t>(reinterpret_cast<uintptr_t>(hits103),j*28)=reinterpret_cast<uintptr_t>(shapes103[j]);
    unsigned char untouched[sizeof hits103];memcpy(untouched,hits103,sizeof hits103);
    cursor_filter103(reinterpret_cast<void*>(cursor),nullptr,vector103,0);require(vector103[1]==4&&!memcmp(untouched,hits103,sizeof hits103),"Foreign unit/ground/building was filtered");
    std::vector<unsigned char> special103(0x700);field<uintptr_t>(P::ptr(special103),0)=base+0x15dfec8+4;field<uintptr_t>(P::ptr(special103),0x214)=unit;physical103[3][1]=P::ptr(special103);
    cursor_filter103(reinterpret_cast<void*>(cursor),nullptr,vector103,0);require(vector103[1]==3,"Native same-unit special/banner model still occluded Q target");
    vector103[1]=4;memcpy(hits103,untouched,sizeof hits103);
    physical103[3][1]=possessed;++command_native_lease100;
    cursor_filter103(reinterpret_cast<void*>(cursor),nullptr,vector103,0);require(vector103[1]==4&&!memcmp(untouched,hits103,sizeof hits103),"Stale lease filtered scene hits");--command_native_lease100;
    command_native_root100=0;cursor_filter103(reinterpret_cast<void*>(cursor),nullptr,vector103,0);require(vector103[1]==4&&!memcmp(untouched,hits103,sizeof hits103),"RTS query was filtered");
    command_native_lease100=0;command_native_environment100=0;
    require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Context hooks disable");
    const unsigned char cycle_original103[]={0x83,0xec,0x0c,0x56,0x8b,0xf1,0x8b,0x8e},filter_original103[]={0x57,0x8b,0xf9,0x8b,0x8f,0xf0,0x0b,0};
    require(!memcmp(reinterpret_cast<void*>(base+0x2f79a0),cycle_original103,8)&&!memcmp(reinterpret_cast<void*>(base+0x10ee5b0),filter_original103,8),"New context hook bytes were not exactly restored");MH_Uninitialize();input_window=nullptr;native_window_proc=nullptr;DestroyWindow(window);
    std::cout<<"PASS: 200 original native camera cycles reproduce actor replacement; 400 owned cycles suppressed, exit/stale serial forward; 200 production owned-input blocks and exact invalid forward; 200 original hover-provider refreshes; 420 recovery frames through production motion plus original get-up clock/cleanup and queued-shot retirement; 200 original target constructors reproduce self occlusion and select enemy after scoped filter. Scene/metadata/eligibility/window/count/integrator fixture providers; full AI/gameplay needs live validation.\n";return 0;
}catch(const std::exception& e){MH_Uninitialize();std::cerr<<e.what()<<'\n';return 1;}}
