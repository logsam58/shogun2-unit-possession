#include "startup_view160.h"
#include <iostream>
#include <stdexcept>
#include <objidl.h>
#include <gdiplus.h>
static void check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
struct Action {int button;bool valid{};const wchar_t* screenshot{};};
static void capture(HWND window,const wchar_t* path){
    RECT box{};GetWindowRect(window,&box);HDC screen=GetDC(window),copy=CreateCompatibleDC(screen);
    HBITMAP bitmap=CreateCompatibleBitmap(screen,box.right-box.left,box.bottom-box.top);
    auto old=SelectObject(copy,bitmap);RedrawWindow(window,nullptr,nullptr,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);
    BOOL drawn=PrintWindow(window,copy,0);SelectObject(copy,old);
    if(drawn){
        Gdiplus::Bitmap image(bitmap,nullptr);CLSID png={0x557cf406,0x1a04,0x11d3,{0x9a,0x73,0,0,0xf8,0x1e,0xf3,0x2e}};
        drawn=image.Save(path,&png,nullptr)==Gdiplus::Ok;
    }
    DeleteObject(bitmap);DeleteDC(copy);ReleaseDC(window,screen);check(drawn!=FALSE,"Cannot capture actual guide dialog");
}
static void inspect(HWND window,void* raw){
    auto a=static_cast<Action*>(raw);wchar_t text[256]{};GetWindowTextW(window,text,256);
    a->valid=!wcscmp(text,L"Shogun 2 Unit Possession")&&LOWORD(SendMessageW(window,DM_GETDEFID,0,0))==startupdialog160::third_button;
    RECT client{};GetClientRect(window,&client);
    for(int id:{startupdialog160::third_button,startupdialog160::first_button,1003,2000,2001,2002,2003,2010,2011,2012,2013,2014,2101,2102,2103,2104,2105,2110,2111}){
        HWND control=GetDlgItem(window,id);RECT box{};GetWindowRect(control,&box);MapWindowPoints(nullptr,window,reinterpret_cast<POINT*>(&box),2);
        a->valid=a->valid&&control&&box.left>=0&&box.top>=0&&box.right<=client.right&&box.bottom<=client.bottom;
    }
    for(int id:{1003,2010,2011,2012,2013,2014,2110,2111}){
        HWND control=GetDlgItem(window,id);wchar_t content[2048]{};GetWindowTextW(control,content,2048);
        RECT bounds{};GetClientRect(control,&bounds);RECT needed{0,0,bounds.right,0};
        HDC dc=GetDC(control);auto old=SelectObject(dc,reinterpret_cast<HFONT>(SendMessageW(control,WM_GETFONT,0,0)));
        DrawTextW(dc,content,-1,&needed,DT_CALCRECT|DT_WORDBREAK|DT_NOPREFIX);SelectObject(dc,old);ReleaseDC(control,dc);
        a->valid=a->valid&&needed.bottom<=bounds.bottom;
    }
    GetWindowTextW(GetDlgItem(window,2010),text,256);a->valid=a->valid&&wcsstr(text,L"F9")&&wcsstr(text,L"F10");
    GetWindowTextW(GetDlgItem(window,2013),text,256);a->valid=a->valid&&wcsstr(text,L"Q")&&wcsstr(text,L"Hold here")&&wcsstr(text,L"Follow me");
    RECT window_box{},work{};GetWindowRect(window,&window_box);SystemParametersInfoW(SPI_GETWORKAREA,0,&work,0);
    a->valid=a->valid&&window_box.left>=work.left&&window_box.top>=work.top&&window_box.right<=work.right&&window_box.bottom<=work.bottom;
    if(a->screenshot){
        SetWindowLongPtrW(window,DWLP_USER,reinterpret_cast<LONG_PTR>(a));
        SetTimer(window,160,150,[](HWND w,UINT,UINT_PTR id,DWORD){
            KillTimer(w,id);auto a=reinterpret_cast<Action*>(GetWindowLongPtrW(w,DWLP_USER));capture(w,a->screenshot);
            PostMessageW(w,WM_COMMAND,a->button,0);
        });return;
    }
    GetWindowTextW(GetDlgItem(window,startupdialog160::third_button),text,256);a->valid=a->valid&&!wcscmp(text,L"Third-Person");
    GetWindowTextW(GetDlgItem(window,startupdialog160::first_button),text,256);a->valid=a->valid&&!wcscmp(text,L"First-Person");
    if(a->button==IDCANCEL)PostMessageW(window,WM_CLOSE,0,0);else PostMessageW(window,WM_COMMAND,a->button,0);
}
int wmain(int argc,wchar_t** argv){try{
    ULONG_PTR graphics{};Gdiplus::GdiplusStartupInput gdi;check(Gdiplus::GdiplusStartup(&graphics,&gdi,nullptr)==Gdiplus::Ok,"Cannot initialize guide capture");
    wchar_t previous[32768]{};DWORD length=GetEnvironmentVariableW(startup160::environment,previous,32768);
    SetEnvironmentVariableW(startup160::environment,nullptr);check(!startup160::third_person(),"Missing choice changed historical fallback");
    for(const wchar_t* invalid:{L"",L"third",L"THIRD-PERSON",L"third-person-extra",L"abcdefghijklmnopqrstuvwxyz0123456789"}){SetEnvironmentVariableW(startup160::environment,invalid);check(!startup160::third_person(),"Invalid environment value enabled third person");}
    for(int id:{startupdialog160::third_button,startupdialog160::first_button,IDCANCEL}){
        Action action{id,false,(id==startupdialog160::third_button&&argc==2)?argv[1]:nullptr};startupdialog160::Observer observer{inspect,&action};bool third=startupdialog160::choose(GetModuleHandleW(nullptr),nullptr,&observer);
        check(action.valid,"Real dialog title, labels, default or bounds differ");check(third==(id!=startupdialog160::first_button),"Dialog choice differs");
        check(startup160::publish(third)&&startup160::third_person()==third,"Published process choice differs");
    }
    check(!startupdialog160::resolve(-1)&&!startupdialog160::resolve(0),"Dialog failure did not preserve first person");
    SetEnvironmentVariableW(startup160::environment,length&&length<32768?previous:nullptr);
    Gdiplus::GdiplusShutdown(graphics);
    std::cout<<"PASS real native quick-start guide: WASD schematic, all text fits, screen bounds, possession instructions; real native chooser: third/first/close choices, default/labels/control bounds; strict process-local preference and failure fallback; no game or settings accessed.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
