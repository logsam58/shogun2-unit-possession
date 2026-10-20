// Read-only authentic HUD candidate. Never writes game memory or handles gameplay input.
#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "../../shogun2-possession-cleanup-v46/src/protocol.h"
#include "../../shogun2-possession-cleanup-v46/src/game_window105.h"
#include "../../shogun2-possession-cleanup-v46/src/overlay_surface106.h"
#include "hud_renderer.h"
#include "hud_surface100.h"
#include "audio_feedback109.h"
#include "accuracy_protocol90.h"
#include "../../shogun2-accuracy90-preview/src/climb_protocol97.h"
#include "../src/equipment_protocol81.h"
static_assert(sizeof(void*)==4,"Build x86 for the game's telemetry");
static DWORD pid;static uintptr_t base;static HANDLE process;static Status* shared;
static EquipmentStatus81* equipment_shared;
static ability141::Status* abilities_shared141;
static ammo83::Status* ammunition_shared83;
static accuracy90::Status* accuracy_shared90;
static climb97::Status* climb_shared97;
static command100::Status* commands_shared100;static HANDLE commands_map100;static uint64_t commands_retry100;
static HudSurface100 hud_surface100;
static AudioFeedback109 audio_feedback109;
static HWND game,overlay;static bool visible;static hud80::View display;static hud80::Assets assets;
static overlay106::Publisher pixels106;static overlay106::Key display_key106;
template<class T>T read(uintptr_t address){T v{};SIZE_T n=0;
    require(ReadProcessMemory(process,reinterpret_cast<void*>(address),&v,sizeof v,&n)&&n==sizeof v,"Native HUD read failed");return v;}
static bool snapshot(Status& s){for(int i=0;i<4;++i){LONG seq=shared->sequence;if(seq&1)continue;
    MemoryBarrier();memcpy(&s,shared,sizeof s);MemoryBarrier();if(seq==shared->sequence)return true;}return false;}
static std::wstring unit_key(uintptr_t unit,uintptr_t& definition){
    // Native unit:type getter1c1600 + string access95790, verified from assembly.
    definition=read<uintptr_t>(unit+0x1c);uintptr_t object=read<uintptr_t>(definition),chars=read<uintptr_t>(object+8);
    require(chars>0x10000,"Missing unit key");std::wstring out;
    for(unsigned i=0;i<160;++i){wchar_t c=read<wchar_t>(chars+i*2);if(!c){require(!out.empty(),"Empty unit key");
        require(read<uintptr_t>(unit+0x1c)==definition&&read<uintptr_t>(definition)==object&&read<uintptr_t>(object+8)==chars,"Unit key changed");return out;}
        require((c>=L'a'&&c<=L'z')||(c>=L'A'&&c<=L'Z')||(c>=L'0'&&c<=L'9')||c==L'_',"Unexpected unit key character");out+=c;}
    throw std::runtime_error("Unterminated unit key");
}
static const hud80::FactionArt* faction_owner(uintptr_t unit,uintptr_t& army){
    army=read<uintptr_t>(unit+0x2a4);
    require(read<uintptr_t>(army)==base+0x15bc860,"Unexpected battle army");
    unsigned count=read<unsigned>(army+0x114);uintptr_t members=read<uintptr_t>(army+0x118);
    require(count>0&&count<=256,"Invalid army roster");bool found=false;
    for(unsigned i=0;i<count;++i)if(read<uintptr_t>(members+i*4)==unit){found=true;break;}
    require(found,"Unit not owned by army");
    uintptr_t object=read<uintptr_t>(army+0xf4),chars=read<uintptr_t>(object+8);
    std::wstring key;bool terminated=false;
    for(unsigned i=0;i<160;++i){wchar_t c=read<wchar_t>(chars+i*2);if(!c){terminated=true;break;}
        require((c>=L'a'&&c<=L'z')||(c>=L'A'&&c<=L'Z')||(c>=L'0'&&c<=L'9')||c==L'_',"Invalid clan key");key+=c;}
    require(terminated&&!key.empty(),"Missing clan key");
    require(read<uintptr_t>(unit+0x2a4)==army&&read<uintptr_t>(army+0xf4)==object&&read<uintptr_t>(object+8)==chars,"Clan identity changed");
    return hud80::faction_art(key.c_str());
}
static bool measure(){
    Status s{};if(!snapshot(s)||s.magic!=POC_MAGIC||s.version!=46||s.size!=sizeof s||s.pid!=pid)return false;
    uint64_t now=GetTickCount64();if(!s.ready||s.stop||s.release||s.mode!=POSSESSED||now<s.heartbeat||now-s.heartbeat>1000)return false;
    uintptr_t env=read<uintptr_t>(read<uintptr_t>(base+0x1bc8180)+0x110);
    uintptr_t common=read<uintptr_t>(env+8),phase=read<uintptr_t>(common+0xb4);
    unsigned phase_id=read<unsigned>(phase+0x14);if(phase_id<5||phase_id>8)return false;
    uintptr_t manager=read<uintptr_t>(env+0x281fc),camera=read<uintptr_t>(manager+0x8e0);
    if(read<uintptr_t>(camera)!=base+0x15d22e8)return false;
    uintptr_t actor=read<uintptr_t>(camera+0x188);
    if(read<uintptr_t>(actor)!=base+0x15dfec8||read<unsigned>(actor+0x1c8)!=s.actor_id)return false;
    uintptr_t unit=read<uintptr_t>(actor+0x214);
    if(read<unsigned>(unit+4)!=s.unit_id||read<unsigned char>(actor+0xf0)!=1)return false;
    hud80::View d{};d.fatigue=read<unsigned char>(unit+0xd3c)?0:read<unsigned>(unit+0xd34);d.morale=read<unsigned>(unit+0xce8);
    unsigned routing=read<unsigned>(unit+0xcec);d.routing=read<unsigned char>(phase+0x2778)!=0&&(routing==2||routing==3);
    d.men=read<unsigned>(unit+0x2bc);d.initial=read<unsigned>(unit+0xcbc);
    uintptr_t definition=0;std::wstring key=unit_key(unit,definition);d.unit=hud80::unit_art(key.c_str());
    if(!d.unit)return false;
    uintptr_t army=0;d.owner=faction_owner(unit,army);
    if(d.owner){d.faction=d.owner->name;d.mon=d.owner->mon;d.boshin=d.owner->boshin;}
    d.ranged=read<unsigned>(unit+0xf7c)!=0;auto mode=weaponmode133::from_native(read<unsigned char>(unit+0xead),d.ranged);
    d.unit_mode_known=mode.known;d.unit_melee=mode.melee;d.bow=s.pose_weapon_record==3;
    d.aim_pose=s.aim_pose;d.weapon_phase=s.weapon_phase;d.aiming=s.aiming!=0;d.following=s.follow!=0;
    if(!commands_shared100&&now>=commands_retry100){
        commands_retry100=now+1000;wchar_t name[120];swprintf_s(name,L"Local\\Shogun2PossessionUnitCommands100-%lu",pid);
        commands_map100=OpenFileMappingW(FILE_MAP_READ,FALSE,name);
        if(commands_map100)commands_shared100=static_cast<command100::Status*>(MapViewOfFile(commands_map100,FILE_MAP_READ,0,0,sizeof(command100::Status)));
        if(commands_map100&&!commands_shared100){CloseHandle(commands_map100);commands_map100=nullptr;}
    }
    if(commands_shared100)for(unsigned retry=0;retry<4;++retry){
        LONG seq=commands_shared100->sequence;if(seq&1)continue;command100::Status commands{};MemoryBarrier();memcpy(&commands,commands_shared100,sizeof commands);MemoryBarrier();
        if(seq!=commands_shared100->sequence)continue;
        if(command100::valid(commands,pid,s.lease_generation,s.actor_id,s.unit_id,now)){d.commands_known=true;d.commands=commands;}break;
    }
    if(equipment_shared){for(unsigned retry=0;retry<4;++retry){LONG seq=equipment_shared->sequence;if(seq&1)continue;
        EquipmentStatus81 e{};MemoryBarrier();memcpy(&e,equipment_shared,sizeof e);MemoryBarrier();if(seq!=equipment_shared->sequence)continue;
        if(e.magic==EQUIPMENT_MAGIC81&&e.version==81&&e.size==sizeof e&&e.pid==pid&&e.lease==s.lease_generation&&
           e.actor==s.actor_id&&e.unit==s.unit_id&&now>=e.tick&&now-e.tick<1000&&e.slot>=1&&e.slot<=2){
            d.equipment_known=true;d.kind=e.kind<=3?e.kind:0;d.equipment_error=e.error;d.has_ranged=d.ranged;d.slot=e.slot;d.ranged=d.has_ranged&&e.slot==2;d.bow=e.kind==1;
            d.styles=e.styles;d.crouch_supported=e.crouch_supported!=0;d.crouched=e.crouched!=0;d.pending=e.pending!=0;
            e.ammunition[63]=0;d.ammunition=e.ammunition;
        }break;
    }}
    if(abilities_shared141)for(unsigned retry=0;retry<4;++retry){LONG seq=abilities_shared141->sequence;if(seq&1)continue;
        ability141::Status a{};MemoryBarrier();memcpy(&a,abilities_shared141,sizeof a);MemoryBarrier();if(seq!=abilities_shared141->sequence)continue;
        if(ability141::valid(a,pid,s.lease_generation,s.actor_id,s.unit_id,now)){d.abilities_known=true;d.abilities=a;}break;
    }
    if(ammunition_shared83){for(unsigned retry=0;retry<4;++retry){LONG seq=ammunition_shared83->sequence;if(seq&1)continue;
        ammo83::Status a{};MemoryBarrier();memcpy(&a,ammunition_shared83,sizeof a);MemoryBarrier();if(seq!=ammunition_shared83->sequence)continue;
        if(ammo83::valid(a,pid,s.lease_generation,s.actor_id,s.unit_id,now)){d.ammunition_known=true;d.ammunition_state=a;}break;
    }}
    if(accuracy_shared90){for(unsigned retry=0;retry<4;++retry){LONG seq=accuracy_shared90->sequence;if(seq&1)continue;
        accuracy90::Status a{};MemoryBarrier();memcpy(&a,accuracy_shared90,sizeof a);MemoryBarrier();if(seq!=accuracy_shared90->sequence)continue;
        if(accuracy90::valid(a,pid,s.lease_generation,s.actor_id,s.unit_id,now,read<unsigned>(common+0x50))){d.accuracy_known=true;d.cone_half_angle=a.half_angle;d.projection_y=a.projection_y;}break;
    }}
    if(climb_shared97){for(unsigned retry=0;retry<4;++retry){LONG seq=climb_shared97->sequence;if(seq&1)continue;
        climb97::Status c{};MemoryBarrier();memcpy(&c,climb_shared97,sizeof c);MemoryBarrier();if(seq!=climb_shared97->sequence)continue;
        if(climb97::valid(c,pid,s.lease_generation,s.actor_id,s.unit_id,now))d.climb_mode=c.mode;break;
    }}
    d.ammo=read<float>(unit+0xf78);d.reload=read<float>(actor+0x390);
    d.speed=std::hypot(read<float>(actor+0x8c),read<float>(actor+0x94));
    if(d.fatigue>5||d.morale>7||!d.initial||d.initial>100000||d.men>d.initial||
        !std::isfinite(d.ammo)||d.ammo<0||!std::isfinite(d.reload)||!std::isfinite(d.speed))return false;
    if(read<uintptr_t>(manager+0x8e0)!=camera||read<uintptr_t>(camera+0x188)!=actor||read<unsigned>(actor+0x1c8)!=s.actor_id||
        read<uintptr_t>(actor+0x214)!=unit||read<uintptr_t>(unit+0x1c)!=definition||read<uintptr_t>(unit+0x2a4)!=army||read<unsigned>(unit+4)!=s.unit_id)return false;
    Status final{};if(!snapshot(final)||final.lease_generation!=s.lease_generation||final.actor_id!=s.actor_id||final.unit_id!=s.unit_id||final.mode!=POSSESSED)return false;
    display=d;display_key106={s.lease_generation,s.actor_id,s.unit_id};return true;
}
static bool paint(){
    RECT client{};POINT pos{};if(!gamewindow105::client(game,client,pos))return false;int width=client.right,height=client.bottom;
    if(!hud_surface100.ensure(width,height))return false;
    auto screen=hud_surface100.screen(),dc=hud_surface100.memory();auto pixels=hud_surface100.pixels();memset(pixels,0,size_t(width)*height*4);
    {Gdiplus::Bitmap surface(width,height,width*4,PixelFormat32bppPARGB,static_cast<BYTE*>(pixels));Gdiplus::Graphics g(&surface);hud80::render(g,assets,float(width),float(height),display);}
    if(pixels106.open(pid,overlay106::HUD)){
        overlay106::Frame f{};f.key=display_key106;f.width=f.view_width=width;f.height=f.view_height=height;f.rect_count=1;f.rect[0]={0,0,width,height};
        if(pixels106.publish(f,pixels)){overlay106::Ack ack;if(pixels106.ack(ack)){ShowWindow(overlay,SW_HIDE);return ack.number&&overlay106::same(ack.key,display_key106);}}
    }
    POINT origin{};SIZE size{width,height};BLENDFUNCTION alpha{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
    bool ok=UpdateLayeredWindow(overlay,screen,&pos,&size,dc,&origin,0,&alpha,ULW_ALPHA)!=0;
    if(ok)SetWindowPos(overlay,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);return ok;
}
static void update(){game=gamewindow105::refresh(game,pid);bool ok=false;try{ok=measure();}catch(...){ok=false;}DWORD fg=0;GetWindowThreadProcessId(GetForegroundWindow(),&fg);
    bool active=ok&&IsWindow(game)&&!IsIconic(game)&&IsWindowVisible(game)&&fg==pid;
    audio_feedback109.poll(pid,display_key106.lease,display_key106.actor,display_key106.unit,active);
    if(!active){pixels106.hide();if(visible)ShowWindow(overlay,SW_HIDE);visible=false;return;}
    visible=paint();if(!visible)ShowWindow(overlay,SW_HIDE);}
static LRESULT CALLBACK proc(HWND w,UINT msg,WPARAM wp,LPARAM lp){
    if(msg==WM_NCHITTEST)return HTTRANSPARENT;if(msg==WM_MOUSEACTIVATE)return MA_NOACTIVATE;if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_DISPLAYCHANGE||msg==WM_DPICHANGED){hud_surface100.reset();return 0;}
    if(msg==WM_TIMER){if(WaitForSingleObject(process,0)==WAIT_OBJECT_0){DestroyWindow(w);return 0;}update();return 0;}
    if(msg==WM_DESTROY){pixels106.hide();PostQuitMessage(0);return 0;}return DefWindowProcW(w,msg,wp,lp);}
static int preview(int argc,wchar_t** argv){
    using namespace Gdiplus;require(argc>=4,"Use --render output.png clean|combat|qte|ranged [width height]");
    int width=argc>=6?_wtoi(argv[4]):1920,height=argc>=6?_wtoi(argv[5]):1080;require(width>=640&&width<=3840&&height>=360&&height<=2160,"Invalid preview size");
    bool bomb=wcscmp(argv[3],L"bomb")==0,gun=wcscmp(argv[3],L"gun")==0||wcscmp(argv[3],L"gun-accurate")==0||wcscmp(argv[3],L"gun-inaccurate")==0,single=wcscmp(argv[3],L"single")==0,pending=wcscmp(argv[3],L"pending")==0,empty=wcscmp(argv[3],L"empty")==0;
    bool reload=wcscmp(argv[3],L"reload")==0,lowered=wcscmp(argv[3],L"lowered")==0,drawing=wcscmp(argv[3],L"drawing")==0,
        mortar=wcscmp(argv[3],L"mortar")==0,rocket=wcscmp(argv[3],L"rocket")==0,fots=wcscmp(argv[3],L"fots")==0,unknown=wcscmp(argv[3],L"unknown")==0;
    bool ranged=wcscmp(argv[3],L"ranged")==0||bomb||gun||single||pending||empty||reload||lowered||drawing||mortar||rocket||fots||unknown||wcscmp(argv[3],L"ammo-qte")==0,combat=wcscmp(argv[3],L"combat")==0,qte_active=wcscmp(argv[3],L"qte")==0||wcsncmp(argv[3],L"qte-",4)==0||wcscmp(argv[3],L"ammo-qte")==0;
    bool command_preset=wcsncmp(argv[3],L"command-",8)==0;
    bool climb_preset=wcsncmp(argv[3],L"climb-",6)==0;
    require(command_preset||climb_preset||ranged||combat||qte_active||(wcscmp(argv[3],L"clean")==0||wcscmp(argv[3],L"melee")==0),"Unknown preview preset");
    hud80::View v;v.fixture=true;v.unit=hud80::unit_art(ranged?L"Inf_Missile_Bow_Ashigaru":L"Inf_Sword_Katana_Samurai");require(v.unit!=nullptr,"Fixture catalog entry missing");
    v.abilities_known=true;v.abilities.count=1;v.abilities.selected=0;v.abilities.items[0]={argc>=7?unsigned(_wtoi(argv[6])):39,ability141::Available,0};
    require(ability141::art(v.abilities.items[0].id)!=nullptr,"Unknown ability preview");
    if(v.abilities.items[0].id==35)v.abilities.items[0].flags|=ability141::Targeted;
    if(argc>=8)v.abilities.error=unsigned(_wtoi(argv[7]));
    v.faction=L"Oda";v.mon=L"ui/clan_mons/data__oda.png";v.men=combat?73:118;v.initial=120;v.morale=combat?5:2;v.fatigue=combat?4:1;v.veterancy=3;
    v.unit_mode_known=true;v.unit_melee=!ranged;v.commands_known=true;v.commands.fire_mode=1;v.equipment_known=true;v.has_ranged=ranged;v.slot=ranged?2:1;v.styles=ranged?3:0;v.ammunition=L"Fire arrows";
    v.climb_mode=wcscmp(argv[3],L"climb-near")==0?1:wcscmp(argv[3],L"climb-top")==0?2:wcscmp(argv[3],L"climb-active")==0?3:wcscmp(argv[3],L"climb-crest")==0?4:0;
    if(wcscmp(argv[3],L"melee")==0){v.unit=hud80::unit_art(L"Inf_Missile_Bow_Ashigaru");v.unit_mode_known=true;v.unit_melee=true;v.has_ranged=true;v.slot=1;}
    v.ranged=ranged;v.bow=ranged&&!gun&&!bomb&&!mortar&&!rocket&&!fots;v.kind=v.bow?1:gun||fots?2:3;
    v.aiming=ranged&&!lowered;v.aim_pose=drawing?2:lowered?0:1;v.ammo=empty?0:19;v.pending=pending;
    v.accuracy_known=gun||fots;v.cone_half_angle=wcscmp(argv[3],L"gun-accurate")==0?.0025f:wcscmp(argv[3],L"gun-inaccurate")==0?.025f:.009f;v.projection_y=1/std::tan(1.309f*.5f);
    v.reload=reload?2.4f:0;v.weapon_phase=reload?4:0;
    if(gun)v.unit=hud80::unit_art(L"Inf_Missile_Matchlock_Ashigaru");
    if(fots){v.boshin=true;v.unit=hud80::unit_art(L"Boshin_Inf_Militia_Levy_Infantry");}
    if(unknown){v.kind=0;v.equipment_known=false;v.unit_mode_known=false;}
    if(ranged){v.ammunition_known=true;auto& a=v.ammunition_state;a.count=single||mortar||rocket?1:gun||bomb||fots?2:3;a.current=single||pending||empty?0:1;a.pending=pending;
        for(unsigned i=0;i<a.count;++i){auto& item=a.items[i];item.icon=mortar?ammo83::Mortar:rocket?ammo83::Rocket:(gun||fots)?(i?ammo83::Suppression:ammo83::Bullet):bomb?(i?ammo83::BlindBomb:ammo83::Bomb):i==0?ammo83::Arrow:i==1?ammo83::FireArrow:ammo83::WhistleArrow;
            item.flags=(i==a.current?ammo83::Selected:0)|(i==0?ammo83::Normal:0)|(!empty&&!(gun&&i==1)?ammo83::Available:0);wcscpy_s(item.label,ammo83::label(ammo83::Icon(item.icon)));}}
    if(command_preset){auto& c=v.commands;v.commands_known=true;c.tick=GetTickCount64();c.feedback_until=c.tick+1800;
        c.move_mode=wcscmp(argv[3],L"command-hold")==0?3:4;c.fire_mode=wcscmp(argv[3],L"command-fire")==0?2:wcscmp(argv[3],L"command-hold")==0?0:1;
        c.placing=wcscmp(argv[3],L"command-drag")==0||wcscmp(argv[3],L"command-point")==0||wcscmp(argv[3],L"command-invalid")==0;

    }
    qte::Snapshot s;s.active=qte_active;s.serial=1;s.action=wcscmp(argv[3],L"qte-left")==0?qte::Action::Attack:qte::Action::Defend;s.result=wcscmp(argv[3],L"qte-interrupted")==0?qte::Result::Canceled:qte::Result::Pending;s.duration=1.4f;s.elapsed=.84f;s.response_elapsed=.84;s.placement=.65f;s.zone_width=.17f;
    Bitmap bitmap(width,height,PixelFormat32bppARGB);{Graphics g(&bitmap);LinearGradientBrush backdrop(Point(0,0),Point(width,height),Color(255,75,83,70),Color(255,26,31,25));g.FillRectangle(&backdrop,0,0,width,height);
        hud80::render(g,assets,float(width),float(height),v,qte_active?&s:nullptr,true);}
    UINT count=0,bytes=0;require(GetImageEncodersSize(&count,&bytes)==Ok,"PNG encoder unavailable");std::vector<BYTE> buffer(bytes);auto enc=reinterpret_cast<ImageCodecInfo*>(buffer.data());require(GetImageEncoders(count,bytes,enc)==Ok,"Encoder read failed");
    for(UINT i=0;i<count;++i)if(!wcscmp(enc[i].MimeType,L"image/png")){require(bitmap.Save(argv[2],&enc[i].Clsid)==Ok,"Preview save failed");std::cout<<"Rendered fixture using production HUD renderer\n";return 0;}
    throw std::runtime_error("No PNG encoder");
}
int wmain(int argc,wchar_t** argv){Gdiplus::GdiplusStartupInput input;ULONG_PTR token{};if(Gdiplus::GdiplusStartup(&token,&input,nullptr)!=Gdiplus::Ok)return 1;
    int result=1;try{
        require(assets.load(GetModuleHandleW(nullptr)),"Embedded assets unavailable");
        if(argc>1&&!wcscmp(argv[1],L"--render"))result=preview(argc,argv);
        else{
            require(argc==1||argc==2&&!wcscmp(argv[1],L"--probe"),"Use no arguments, --probe, or --render");
            pid=find_process(0);Module engine=find_module(pid);base=engine.base;require(engine.size==0x1d88000&&hash_file(engine.path)==EXPECTED_HASH,"Unsupported engine");
            Handle handle(OpenProcess(PROCESS_VM_READ|PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,pid));process=handle.h;require(process!=nullptr,"Cannot read game");
            wchar_t name[80];mapping_name(name,pid);Handle mapping(OpenFileMappingW(FILE_MAP_READ,FALSE,name));require(mapping.h!=nullptr,"Possession not loaded");
            shared=static_cast<Status*>(MapViewOfFile(mapping.h,FILE_MAP_READ,0,0,sizeof(Status)));require(shared!=nullptr,"HUD mapping failed");
            wchar_t equipment_name[96];equipment_mapping_name81(equipment_name,pid);Handle equipment_map(OpenFileMappingW(FILE_MAP_READ,FALSE,equipment_name));
            if(equipment_map.h)equipment_shared=static_cast<EquipmentStatus81*>(MapViewOfFile(equipment_map.h,FILE_MAP_READ,0,0,sizeof(EquipmentStatus81)));
            wchar_t ability_name141[96];ability141::mapping(ability_name141,pid);Handle ability_map141(OpenFileMappingW(FILE_MAP_READ,FALSE,ability_name141));
            if(ability_map141.h)abilities_shared141=static_cast<ability141::Status*>(MapViewOfFile(ability_map141.h,FILE_MAP_READ,0,0,sizeof(ability141::Status)));
            wchar_t ammo_name83[96];ammo83::mapping_name(ammo_name83,pid);Handle ammo_map83(OpenFileMappingW(FILE_MAP_READ,FALSE,ammo_name83));
            if(ammo_map83.h)ammunition_shared83=static_cast<ammo83::Status*>(MapViewOfFile(ammo_map83.h,FILE_MAP_READ,0,0,sizeof(ammo83::Status)));
            wchar_t accuracy_name90[96];accuracy90::mapping_name(accuracy_name90,pid);Handle accuracy_map90(OpenFileMappingW(FILE_MAP_READ,FALSE,accuracy_name90));
            if(accuracy_map90.h)accuracy_shared90=static_cast<accuracy90::Status*>(MapViewOfFile(accuracy_map90.h,FILE_MAP_READ,0,0,sizeof(accuracy90::Status)));
            wchar_t climb_name97[96];climb97::mapping_name(climb_name97,pid);Handle climb_map97(OpenFileMappingW(FILE_MAP_READ,FALSE,climb_name97));
            if(climb_map97.h)climb_shared97=static_cast<climb97::Status*>(MapViewOfFile(climb_map97.h,FILE_MAP_READ,0,0,sizeof(climb97::Status)));
            if(argc==2){bool ok=measure();std::wcout<<L"active="<<ok<<L" unit="<<(display.unit?display.unit->name:L"unavailable")<<L" faction="<<display.faction<<L" mon="<<display.mon<<L" men="<<display.men<<L" morale="<<display.morale<<L" fatigue="<<display.fatigue<<L"\n";result=ok?0:2;}
            else{Handle singleton(CreateMutexW(nullptr,FALSE,L"Local\\Shogun2PossessionHUD46"));if(GetLastError()==ERROR_ALREADY_EXISTS)result=0;
                else{game=gamewindow105::find(pid);SetProcessDPIAware();WNDCLASSW cls{};cls.lpfnWndProc=proc;cls.hInstance=GetModuleHandleW(nullptr);cls.lpszClassName=L"ShogunAuthenticHUD80";require(RegisterClassW(&cls)!=0,"HUD class failed");
                    overlay=CreateWindowExW(WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW|WS_EX_TOPMOST,cls.lpszClassName,L"Shogun FPS HUD",WS_POPUP,0,0,0,0,nullptr,nullptr,cls.hInstance,nullptr);require(overlay!=nullptr,"HUD window failed");
                    SetTimer(overlay,1,150,nullptr);MSG msg{};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}result=0;}}
            if(commands_shared100)UnmapViewOfFile(commands_shared100);if(commands_map100)CloseHandle(commands_map100);
            if(climb_shared97)UnmapViewOfFile(climb_shared97);if(accuracy_shared90)UnmapViewOfFile(accuracy_shared90);if(ammunition_shared83)UnmapViewOfFile(ammunition_shared83);if(equipment_shared)UnmapViewOfFile(equipment_shared);UnmapViewOfFile(shared);
        }
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';}
    assets.clear();hud_surface100.reset();Gdiplus::GdiplusShutdown(token);
    return result;
}
