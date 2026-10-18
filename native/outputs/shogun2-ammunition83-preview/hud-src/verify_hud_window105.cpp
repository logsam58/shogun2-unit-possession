#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "../../shogun2-possession-cleanup-v46/src/game_window105.h"
#include "hud_surface100.h"
// Private off-screen Win32 windows only; no game discovery, input or settings.
int main(){try{
    HINSTANCE instance=GetModuleHandleW(nullptr);WNDCLASSW cls{};
    cls.hInstance=instance;cls.lpfnWndProc=DefWindowProcW;cls.lpszClassName=L"SHOGUN 2";
    require(RegisterClassW(&cls)!=0,"Fixture game window class");DWORD pid=GetCurrentProcessId();
    require(gamewindow105::find(pid)==nullptr,"Absent game window invented");
    HWND fake=CreateWindowExW(WS_EX_NOACTIVATE,L"STATIC",L"Death eyelid fixture",WS_POPUP,-32000,-32000,1920,540,nullptr,nullptr,instance,nullptr);
    require(fake&&!gamewindow105::belongs(fake,pid)&&!gamewindow105::refresh(fake,pid),"Non-game overlay selected as game");
    HWND game=CreateWindowExW(WS_EX_NOACTIVATE,cls.lpszClassName,L"Private display fixture",WS_POPUP,-32000,-32000,0,0,nullptr,nullptr,instance,nullptr);
    require(game&&gamewindow105::find(pid)==game&&gamewindow105::refresh(nullptr,pid)==game,"Startup hidden/zero-size window not retained");
    RECT area{};POINT origin{};require(!gamewindow105::client(game,area,origin),"Hidden/zero-size window drawable");
    require(!gamewindow105::belongs(game,pid+1),"Foreign process window accepted");
    unsigned gdi_before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
    {HudSurface100 surface;
      for(auto size: {SIZE{1280,720},SIZE{1920,1080},SIZE{3840,2160},SIZE{1280,720}}){
        require(SetWindowPos(game,nullptr,-32000,-32000,size.cx,size.cy,SWP_NOACTIVATE|SWP_NOZORDER|SWP_SHOWWINDOW)!=0,"Off-screen fixture resize");
        require(gamewindow105::client(game,area,origin)&&area.right==size.cx&&area.bottom==size.cy,"Current client extent not reacquired");
        require(surface.ensure(area.right,area.bottom),"Display-size DIB creation");
        auto pixels=surface.pixels();require(surface.ensure(area.right,area.bottom)&&surface.pixels()==pixels,"Same-size display reallocated");
        surface.reset();require(surface.ensure(area.right,area.bottom),"Display-change reset did not recover");
      }
    }
    require(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)<=gdi_before+2,"Display transitions leaked GDI resources");
    DestroyWindow(game);require(gamewindow105::refresh(game,pid)==nullptr,"Destroyed game window retained");
    game=CreateWindowExW(WS_EX_NOACTIVATE,cls.lpszClassName,L"Replacement fixture",WS_POPUP,-32000,-32000,1280,720,nullptr,nullptr,instance,nullptr);
    require(game&&gamewindow105::refresh(fake,pid)==game,"Replacement game window not found");
    DestroyWindow(game);DestroyWindow(fake);UnregisterClassW(cls.lpszClassName,instance);
    std::cout<<"PASS: shared HUD/QTE game-window discovery before/after creation, hidden/zero-size waiting, foreign/overlay rejection, replacement, actual 720p/1080p/2160p DIB resize/reset and bounded GDI lifetime. Exclusive fullscreen visibility is not simulated.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
