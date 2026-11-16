// Original native scene-query, unprojection and dispatch/sorting glue execute.
// Actor-grid geometry, world surfaces, vector sorting/allocation and camera
// construction are explicit providers. Production F9 entry chooses the actor.
#define main retained_camera_pick150_main
#include "verify_native_commands100.cpp"
#undef main
static unsigned picks150,frees150,sorts150,toggles150,hit_count150=1;
static uintptr_t picked150,expected_grid150,render_object150,render_context150;
static uintptr_t test_shape150,test_physical150,nearby_actor150;
static unsigned ray_index150;
static bool ambiguous150;
static uintptr_t __fastcall renderer150(void*,void*){return render_object150;}
static uintptr_t __fastcall context150(void*,void*){return render_context150;}
static std::array<unsigned char,56> hits150{};
static bool __fastcall bounds150(void*,void*,const void*,unsigned mode){require(mode==0,"Native actor bounds mode changed");return true;}
static void __fastcall actors150(void* grid,void*,void* out,const void*,unsigned sorted,unsigned mask,const void* input,unsigned mode,unsigned a,unsigned b,unsigned c,unsigned visible){
    require(reinterpret_cast<uintptr_t>(grid)==expected_grid150&&mode==0&&mask==0x30b&&sorted==1&&!a&&!b&&!c&&!visible,"Native actor query dispatch arguments");
    auto ray=static_cast<const float*>(input);require(*static_cast<const unsigned*>(input)==3,"Native actor ray type");
    require(std::abs(std::hypot(std::hypot(ray[4]-ray[1],ray[5]-ray[2]),ray[6]-ray[3])-4096.f)<.01f,"Center ray length");
    float x151=field<float>(base,0x17acf84)*2-1,y151=1-field<float>(base,0x17acf88)*2;
    const float offset_x=ray[4]/ray[6]-x151,offset_y=(ray[5]-1)/ray[6]-y151;
    require(ray[1]==0&&ray[2]==1&&ray[3]==0&&std::abs(offset_x)<=16.f/1280+.00001f&&
        std::abs(offset_y)<=16.f/720+.00001f,"Hover ray left the bounded native mouse area");
    if(ambiguous150){
        // The center and inner rays are occluded. Exactly one wider-area ray
        // sees an eligible soldier. The rest are world hits.
        field<unsigned>(test_shape150,0)=ray_index150==6?8:1;
        field<uintptr_t>(test_physical150,4)=nearby_actor150;
    }
    ++ray_index150;
    auto v=static_cast<unsigned*>(out);v[0]=v[1]=hit_count150;v[2]=reinterpret_cast<unsigned>(hits150.data());++picks150;
}
static void __fastcall world150(void*,void*,void*,const void*,unsigned mode,unsigned sorted,unsigned mask,unsigned){require(!mode&&sorted==1&&mask==0x303,"Native actor-bit removal before world query");}
static void __cdecl sort150(void* first,void* last,unsigned count,unsigned mode){require(first==hits150.data()&&reinterpret_cast<uintptr_t>(last)-reinterpret_cast<uintptr_t>(first)==count*28&&mode==1,"Native result sort ABI");++sorts150;}
static void __cdecl free150(void* p){require(p==hits150.data(),"Freed foreign picking allocation");++frees150;}
static uintptr_t __fastcall physical_actor150(void* object,void*){return field<uintptr_t>(reinterpret_cast<uintptr_t>(object),4);}
static uintptr_t soldier_camera150;
static void __fastcall toggle150(void* manager,void*,unsigned active){
    require(active==1&&choice_valid97(choice97.view,choice97),"Production F9 passed invalid choice to constructor");
    picked150=choice97.actor;++toggles150;
    if(!soldier_camera150)return;
    const auto owner=reinterpret_cast<uintptr_t>(manager);
    field<uintptr_t>(soldier_camera150,0)=base+0x15d22e8;
    field<uintptr_t>(soldier_camera150,0x180)=owner;
    field<uintptr_t>(soldier_camera150,0x184)=choice97.view;
    field<uintptr_t>(soldier_camera150,0x188)=picked150;
    field<uintptr_t>(owner,0x8fc)=picked150;
    field<uintptr_t>(owner,0x8e0)=soldier_camera150;
}
static void patch150(NativeFixture100& f,unsigned offset,void* function){DWORD old{};require(VirtualProtect(reinterpret_cast<void*>(base+offset),16,PAGE_EXECUTE_READWRITE,&old)!=0,"Pick fixture protection");f.jump(offset,function);VirtualProtect(reinterpret_cast<void*>(base+offset),16,old,&old);FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(base+offset),16);}
int main(){SetUnhandledExceptionFilter(&fault100);try{
    NativeFixture100 f;using P=NativeFixture100;auto camera=P::ptr(f.camera),manager=P::ptr(f.manager),view=P::ptr(f.view);
    possession_lease.release();possessed=0;
    HWND window=CreateWindowExW(0,L"STATIC",L"Test150 private fixture",WS_POPUP,0,0,1280,720,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);require(window!=nullptr,"Pick fixture window");input_window=window;
    state->camera_callback_thread=state->camera_input_thread=state->window_thread=GetCurrentThreadId();
    field<uintptr_t>(manager,0)=base+0x15d26fc;field<uintptr_t>(manager,0x8f8)=view;field<uintptr_t>(manager,0x8cc)=camera;field<uintptr_t>(manager,0x8fc)=P::ptr(f.actor);
    field<uintptr_t>(camera,0)=base+0x15d1a74;unsigned char alternate=0;field<uintptr_t>(camera,0x178)=reinterpret_cast<uintptr_t>(&alternate);
    field<float>(base+0x192d668,0x5c)=1;field<float>(base+0x192e710,0x5c)=2;
    for(auto object:{camera,manager}){field<float>(object,8)=field<float>(object,16)=0;field<float>(object,12)=1;field<float>(object,0x14)=0;field<float>(object,0x18)=1;field<float>(object,0x1c)=10;field<float>(object,0x24)=.75f;field<float>(object,0x5c)=1;}
    field<float>(camera,0x180)=1;field<float>(camera,0x30)=16.f/9.f;
    std::array<unsigned char,0x80> grid{},shape{},physical{};std::array<uintptr_t,8> table{};
    expected_grid150=reinterpret_cast<uintptr_t>(grid.data());field<uintptr_t>(P::ptr(f.common),0xa8)=expected_grid150;
    auto shape_ptr=reinterpret_cast<uintptr_t>(shape.data()),physical_ptr=reinterpret_cast<uintptr_t>(physical.data());
    test_shape150=shape_ptr;test_physical150=physical_ptr;nearby_actor150=P::ptr(f.peer);
    field<unsigned>(shape_ptr,0)=8;field<uintptr_t>(shape_ptr,12)=physical_ptr;field<uintptr_t>(physical_ptr,0)=reinterpret_cast<uintptr_t>(table.data());table[6]=base+0x1450000;
    field<uintptr_t>(reinterpret_cast<uintptr_t>(hits150.data()),0)=shape_ptr;field<uintptr_t>(physical_ptr,4)=P::ptr(f.peer);
    patch150(f,0x1351760,reinterpret_cast<void*>(&bounds150));patch150(f,0x3c7700,reinterpret_cast<void*>(&actors150));patch150(f,0x36e8f0,reinterpret_cast<void*>(&world150));patch150(f,0x101a80,reinterpret_cast<void*>(&sort150));patch150(f,0xf04d0,reinterpret_cast<void*>(&free150));patch150(f,0x1450000,reinterpret_cast<void*>(&physical_actor150));
    std::array<uintptr_t,40> interface_table{},render_table{};uintptr_t interface_object=reinterpret_cast<uintptr_t>(interface_table.data()),interface_ref=reinterpret_cast<uintptr_t>(&interface_object),render_object=reinterpret_cast<uintptr_t>(render_table.data());std::array<unsigned char,0x170> render_context{};
    render_object150=reinterpret_cast<uintptr_t>(&render_object);render_context150=reinterpret_cast<uintptr_t>(render_context.data());
    interface_table[0x94/4]=base+0x1450010;render_table[6]=base+0x1450020;
    field<uintptr_t>(environment,0x2804c)=reinterpret_cast<uintptr_t>(&interface_ref);
    field<float>(render_context150,0x64)=1;field<float>(render_context150,0x164)=1;
    field<float>(render_context150,0x144)=field<float>(render_context150,0x154)=1;
    field<float>(base,0x17acf84)=field<float>(base,0x17acf88)=.5f;
    patch150(f,0x1450010,reinterpret_cast<void*>(&renderer150));patch150(f,0x1450020,reinterpret_cast<void*>(&context150));
    original_toggle=reinterpret_cast<Toggle>(&toggle150);
    auto enter=[&](){state->mode=ARMED;state->acquire=1;state->stop=state->release=0;state->heartbeat=GetTickCount64();picked150=0;ray_index150=0;enter_selected_soldier();rts_view90.active=false;rts_pending90=rts_generation90=0;return picked150;};
    auto first150=enter();if(first150!=P::ptr(f.peer))std::cerr<<"pick150 error="<<state->acquisition_error<<" picked="<<first150<<" expected="<<P::ptr(f.peer)<<" queries="<<picks150<<" frees="<<frees150<<" toggles="<<toggles150<<'\n';
    require(first150==P::ptr(f.peer)&&state->acquisition_error==12,"Closest zoom did not pass exact hit soldier to native camera construction");
    for(float u151:{0.f,.1f,.9f,1.f})for(float v151:{0.f,.25f,.75f,1.f}){
        field<float>(base,0x17acf84)=u151;field<float>(base,0x17acf88)=v151;
        require(enter()==P::ptr(f.peer),"Off-center native mouse cursor was ignored");
    }
    // Test157: exact cursor hit must beat the regular roster member for every
    // native soldier role, including a General with its own config/posture.
    std::vector<unsigned char> role_config157(0x100);auto role_actor157=P::ptr(f.peer);
    for(unsigned role157:{0x15dfec8u,0x15dbd68u,0x15dbfb4u,0x15dc590u,0x15dc1a8u,0x15dc39cu,0x15dcf54u,0x15dc978u,0x15dcb6cu,0x15dcd60u}){
        field<uintptr_t>(role_actor157,0)=base+role157;
        field<uintptr_t>(role_actor157,0x20c)=role157==0x15dfec8u?P::ptr(f.config):P::ptr(role_config157);
        field<unsigned>(role_actor157,0x1f0)=24;
        require(enter()==role_actor157,"Exact cursor special soldier/General lost to regular roster member");
    }
    field<uintptr_t>(role_actor157,0)=base+0x15dfec8;field<uintptr_t>(role_actor157,0x20c)=P::ptr(f.config);field<unsigned>(role_actor157,0x1f0)=0;
    const auto valid_fallback=[&](uintptr_t result){return result==P::ptr(f.actor)||result==P::ptr(f.peer);};
    const unsigned invalid_before=picks150;
    for(float invalid:{-1.f,1.01f,std::numeric_limits<float>::quiet_NaN()}){
        field<float>(base,0x17acf84)=invalid;
        require(valid_fallback(enter())&&picks150==invalid_before,"Invalid cursor ran a query");
    }
    field<float>(base,0x17acf84)=field<float>(base,0x17acf88)=.5f;
    const unsigned before=picks150;
    field<float>(camera,0x180)=50;
    require(enter()==P::ptr(f.peer)&&picks150==before+9,"High RTS view discarded a clear hover");
    ambiguous150=true;
    for(unsigned i=0;i<30;++i)require(enter()==nearby_actor150&&ray_index150==9,
        "Ambiguous hover fell back to a distant soldier instead of the eligible area hit");
    ambiguous150=false;
    field<unsigned>(shape_ptr,0)=8;
    for(unsigned i=0;i<1000;++i){unsigned sp_before{},sp_after{};__asm mov sp_before,esp
        auto result=enter();__asm mov sp_after,esp
        require(result==P::ptr(f.peer)&&sp_before==sp_after,"Repeated F9 actor identity or native ABI changed");}
    field<unsigned char>(P::ptr(f.peer),0xf0)=0;
    for(unsigned type:{1u,2u,0x100u,0x200u,8u}){
        field<unsigned>(shape_ptr,0)=type;
        require(enter()==P::ptr(f.actor),"Occluded or dead hit bypassed current eligibility");
    }
    field<unsigned char>(P::ptr(f.peer),0xf0)=1;
    field<uintptr_t>(physical_ptr,4)=0x12345678;
    require(valid_fallback(enter()),"Foreign hit entered selected unit");
    field<uintptr_t>(physical_ptr,4)=P::ptr(f.peer);
    field<unsigned char>(camera,0xa4)=1;
    require(enter()==P::ptr(f.peer),"Cached camera must use the native rendered cursor");
    field<unsigned char>(camera,0xa4)=0;
    std::array<unsigned char,0x110> pair_storage{};
    const auto pair=reinterpret_cast<uintptr_t>(pair_storage.data()),fighter=P::ptr(f.peer),opponent=P::ptr(f.actor);
    field<uintptr_t>(pair,4)=environment;field<unsigned>(pair,0xc)=1;field<unsigned>(pair,0x38)=2;
    field<uintptr_t>(pair,0x30)=fighter;field<uintptr_t>(pair,0x34)=opponent;
    field<uintptr_t>(fighter,0x688)=field<uintptr_t>(opponent,0x688)=pair;
    field<unsigned char>(pair,0xd1)=1;
    // A playing pair can temporarily own the soldier's grounding state.
    field<unsigned char>(fighter,0xa1)=0;
    std::array<unsigned char,0x200> soldier_camera_storage{};
    soldier_camera150=reinterpret_cast<uintptr_t>(soldier_camera_storage.data());
    const auto original_holders=f.holders;
    for(unsigned picker=0;picker<3;++picker){
        ambiguous150=picker==1;
        field<unsigned>(shape_ptr,0)=picker==2?1:8;
        field<uintptr_t>(physical_ptr,4)=fighter;
        f.holders[0]=picker==2?P::ptr(f.peer_holder):original_holders[0];
        field<unsigned>(view,0x18)=picker==2?1:2;
        const auto before=toggles150;
        require(enter()==fighter&&toggles150==before+1&&state->mode==POSSESSED&&possessed==fighter&&
            possession_lease.active&&followed(fighter,false),"Exact, area or random pick did not possess during the first call");
        require(field<unsigned char>(pair,0xd1)==1,"Camera entry modified native animation playback");
        const auto before_motion=f.peer;
        InterlockedExchange(&state->input,FORWARD|RUN);
        require(!prepare(fighter)&&f.peer==before_motion&&possessed==fighter,
            "Movement overwrote a native exchange already playing when the camera entered");
        possession_lease.release();possessed=0;
        field<uintptr_t>(manager,0x8e0)=camera;
        InterlockedExchange(&state->input,0);
    }
    f.holders=original_holders;field<unsigned>(view,0x18)=2;
    ambiguous150=false;soldier_camera150=0;field<unsigned>(shape_ptr,0)=8;
    field<uintptr_t>(fighter,0x688)=field<uintptr_t>(opponent,0x688)=0;
    field<unsigned char>(fighter,0xa1)=1;
    std::cout<<"PASS: exact, area and random picks enter immediately during the same native paired exchange.\n";
    // A horse hit selects its proved reciprocal rider, not a nearby soldier.
    std::vector<unsigned char> horse(0x700),horse_holder(0x210),horse_config(0x100),unit_record(0x100);std::array<uintptr_t,3> mounted_holders={P::ptr(f.holder),P::ptr(f.peer_holder),P::ptr(horse_holder)};uintptr_t rider=P::ptr(f.peer),mount=P::ptr(horse),passenger=rider;
    field<uintptr_t>(rider,0x20c)=P::ptr(horse_config);field<uintptr_t>(rider,0x210)=mount;field<uintptr_t>(unit,0x2c)=P::ptr(unit_record);field<uintptr_t>(mount,0)=base+0x15e8524;field<uintptr_t>(mount,0x214)=unit;field<uintptr_t>(mount,0x20c)=P::ptr(f.config);field<unsigned char>(mount,0xf0)=field<unsigned char>(mount,0xa1)=1;field<unsigned>(mount,0x6cc)=1;field<uintptr_t>(mount,0x6d0)=reinterpret_cast<uintptr_t>(&passenger);field<uintptr_t>(P::ptr(horse_holder),8)=mount;field<unsigned>(view,0x18)=3;field<uintptr_t>(view,0x1c)=reinterpret_cast<uintptr_t>(mounted_holders.data());field<uintptr_t>(physical_ptr,4)=mount;
    for(unsigned role157:{0x15dfec8u,0x15dbd68u,0x15dbfb4u,0x15dc590u,0x15dc1a8u,0x15dc39cu,0x15dcf54u,0x15dc978u,0x15dcb6cu,0x15dcd60u}){
        field<uintptr_t>(rider,0)=base+role157;require(enter()==rider,"Mounted exact cursor soldier role lost reciprocal rider");
    }
    field<uintptr_t>(rider,0)=base+0x15dfec8;
    require(enter()==rider,"Native horse hit did not select proved rider");passenger=P::ptr(f.actor);require(enter()==P::ptr(f.actor),"Broken horse backlink admitted a rider");
    require(picks150==frees150&&picks150==sorts150,"Repeated query allocation/sort imbalance");
    input_window=nullptr;DestroyWindow(window);state->mode=IDLE;
    std::cout<<"PASS: actual native cursor unprojection/query dispatch, exact pick at high zoom, nearby-only ambiguous fallback, 1000 repeated entries, occlusion and reciprocal rider checks. Geometry and camera construction are fixture providers.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
