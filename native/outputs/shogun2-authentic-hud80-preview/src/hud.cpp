// Read-only authentic HUD candidate. Never writes game memory or handles gameplay input.
#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "../../shogun2-possession-cleanup-v46/src/protocol.h"
#include "hud_renderer.h"
static_assert(sizeof(void*)==4,"Build x86 for the game's telemetry");
static DWORD pid;static uintptr_t base;static HANDLE process;static Status* shared;
static HWND game,overlay;static bool visible;static hud80::View display;static hud80::Assets assets;
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
    d.ranged=read<unsigned>(unit+0xf7c)!=0;d.bow=s.pose_weapon_record==3;
    d.aim_pose=s.aim_pose;d.aiming=s.aiming!=0;d.following=s.follow!=0;
    d.ammo=read<float>(unit+0xf78);d.reload=read<float>(actor+0x390);
    d.speed=std::hypot(read<float>(actor+0x8c),read<float>(actor+0x94));
    if(d.fatigue>5||d.morale>7||!d.initial||d.initial>100000||d.men>d.initial||
        !std::isfinite(d.ammo)||d.ammo<0||!std::isfinite(d.reload)||!std::isfinite(d.speed))return false;
    if(read<uintptr_t>(manager+0x8e0)!=camera||read<uintptr_t>(camera+0x188)!=actor||read<unsigned>(actor+0x1c8)!=s.actor_id||
        read<uintptr_t>(actor+0x214)!=unit||read<uintptr_t>(unit+0x1c)!=definition||read<uintptr_t>(unit+0x2a4)!=army||read<unsigned>(unit+4)!=s.unit_id)return false;
    Status final{};if(!snapshot(final)||final.lease_generation!=s.lease_generation||final.actor_id!=s.actor_id||final.unit_id!=s.unit_id||final.mode!=POSSESSED)return false;
    display=d;return true;
}
static BOOL CALLBACK choose_window(HWND w,LPARAM){DWORD p=0;GetWindowThreadProcessId(w,&p);
    if(p==pid&&IsWindowVisible(w)&&!GetWindow(w,GW_OWNER)){RECT r{};GetClientRect(w,&r);if(r.right>=640&&r.bottom>=360){game=w;return FALSE;}}return TRUE;}
static bool paint(){
    RECT client{};POINT pos{};GetClientRect(game,&client);ClientToScreen(game,&pos);int width=client.right,height=client.bottom;
    if(width<640||height<360)return false;
    HDC screen=GetDC(nullptr),dc=CreateCompatibleDC(screen);BITMAPINFO info{};
    info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=width;info.bmiHeader.biHeight=-height;
    info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;void* pixels=nullptr;
    HBITMAP bitmap=CreateDIBSection(screen,&info,DIB_RGB_COLORS,&pixels,nullptr,0);
    if(!screen||!dc||!bitmap||!pixels){if(bitmap)DeleteObject(bitmap);if(dc)DeleteDC(dc);if(screen)ReleaseDC(nullptr,screen);return false;}
    auto old=SelectObject(dc,bitmap);memset(pixels,0,size_t(width)*height*4);
    {Gdiplus::Bitmap surface(width,height,width*4,PixelFormat32bppPARGB,static_cast<BYTE*>(pixels));Gdiplus::Graphics g(&surface);hud80::render(g,assets,float(width),float(height),display);}
    POINT origin{};SIZE size{width,height};BLENDFUNCTION alpha{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
    bool ok=UpdateLayeredWindow(overlay,screen,&pos,&size,dc,&origin,0,&alpha,ULW_ALPHA)!=0;
    SelectObject(dc,old);DeleteObject(bitmap);DeleteDC(dc);ReleaseDC(nullptr,screen);
    if(ok)SetWindowPos(overlay,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);return ok;
}
static void update(){bool ok=false;try{ok=measure();}catch(...){ok=false;}DWORD fg=0;GetWindowThreadProcessId(GetForegroundWindow(),&fg);
    if(!ok||!IsWindow(game)||IsIconic(game)||!IsWindowVisible(game)||fg!=pid){if(visible)ShowWindow(overlay,SW_HIDE);visible=false;return;}
    visible=paint();if(!visible)ShowWindow(overlay,SW_HIDE);}
static LRESULT CALLBACK proc(HWND w,UINT msg,WPARAM wp,LPARAM lp){
    if(msg==WM_NCHITTEST)return HTTRANSPARENT;if(msg==WM_MOUSEACTIVATE)return MA_NOACTIVATE;if(msg==WM_ERASEBKGND)return 1;
    if(msg==WM_TIMER){if(WaitForSingleObject(process,0)==WAIT_OBJECT_0){DestroyWindow(w);return 0;}update();return 0;}
    if(msg==WM_DESTROY){PostQuitMessage(0);return 0;}return DefWindowProcW(w,msg,wp,lp);}
static int preview(int argc,wchar_t** argv){
    using namespace Gdiplus;require(argc>=4,"Use --render output.png clean|combat|qte|ranged [width height]");
    int width=argc>=6?_wtoi(argv[4]):1920,height=argc>=6?_wtoi(argv[5]):1080;require(width>=640&&width<=3840&&height>=360&&height<=2160,"Invalid preview size");
    bool ranged=wcscmp(argv[3],L"ranged")==0,combat=wcscmp(argv[3],L"combat")==0,qte_active=wcscmp(argv[3],L"qte")==0;
    require(ranged||combat||qte_active||wcscmp(argv[3],L"clean")==0,"Unknown preview preset");
    hud80::View v;v.fixture=true;v.unit=hud80::unit_art(ranged?L"Inf_Missile_Bow_Ashigaru":L"Inf_Sword_Katana_Samurai");require(v.unit!=nullptr,"Fixture catalog entry missing");
    v.faction=L"Oda";v.mon=L"ui/clan_mons/data__oda.png";v.men=combat?73:118;v.initial=120;v.morale=combat?5:2;v.fatigue=combat?4:1;v.veterancy=3;
    v.ranged=ranged;v.bow=ranged;v.aiming=ranged;v.aim_pose=1;v.ammo=19;
    qte::Snapshot s;s.active=qte_active;s.serial=1;s.action=qte::Action::Defend;s.result=qte::Result::Pending;s.duration=1.4f;s.elapsed=.84f;s.placement=.65f;s.zone_width=.17f;
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
            if(argc==2){bool ok=measure();std::wcout<<L"active="<<ok<<L" unit="<<(display.unit?display.unit->name:L"unavailable")<<L" faction="<<display.faction<<L" mon="<<display.mon<<L" men="<<display.men<<L" morale="<<display.morale<<L" fatigue="<<display.fatigue<<L"\n";result=ok?0:2;}
            else{Handle singleton(CreateMutexW(nullptr,FALSE,L"Local\\Shogun2PossessionHUD46"));if(GetLastError()==ERROR_ALREADY_EXISTS)result=0;
                else{EnumWindows(choose_window,0);require(game!=nullptr,"No game window");SetProcessDPIAware();WNDCLASSW cls{};cls.lpfnWndProc=proc;cls.hInstance=GetModuleHandleW(nullptr);cls.lpszClassName=L"ShogunAuthenticHUD80";require(RegisterClassW(&cls)!=0,"HUD class failed");
                    overlay=CreateWindowExW(WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW|WS_EX_TOPMOST,cls.lpszClassName,L"Shogun FPS HUD",WS_POPUP,0,0,0,0,nullptr,nullptr,cls.hInstance,nullptr);require(overlay!=nullptr,"HUD window failed");
                    SetTimer(overlay,1,150,nullptr);MSG msg{};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}result=0;}}
            UnmapViewOfFile(shared);
        }
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';}
    assets.clear();Gdiplus::GdiplusShutdown(token);
    return result;
}
