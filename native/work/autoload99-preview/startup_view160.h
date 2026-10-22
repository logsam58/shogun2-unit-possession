#pragma once
#include "../../common/startup_view160.h"
#include <vector>
#include <string>
namespace startupdialog160 {
constexpr int third_button=1001,first_button=1002;
constexpr short dialog_width=480,dialog_height=326;
constexpr WORD control_count=19;
struct Observer {void (*created)(HWND,void*){};void* context{};};
inline bool resolve(INT_PTR result){return result==third_button||result==IDCANCEL;}
struct Template {
    std::vector<WORD> words;
    void word(WORD v){words.push_back(v);}
    void dword(DWORD v){word(WORD(v));word(WORD(v>>16));}
    void text(const wchar_t* p){do{word(WORD(*p));}while(*p++);}
    void align(){if(words.size()&1)word(0);}
    void control(DWORD style,short x,short y,short width,short height,WORD id,WORD type,const wchar_t* title){
        align();dword(style);dword(0);word(x);word(y);word(width);word(height);word(id);
        word(0xffff);word(type);text(title);word(0);
    }
    Template(WORD points=9){
        dword(WS_POPUP|WS_CAPTION|WS_SYSMENU|DS_MODALFRAME|DS_SETFONT);dword(0);
        word(control_count);word(0);word(0);word(dialog_width);word(dialog_height);word(0);word(0);
        text(L"Shogun 2 Unit Possession");word(points);text(L"Segoe UI");
        auto label=[this](short x,short y,short w,short h,WORD id,const wchar_t* title){
            control(WS_CHILD|WS_VISIBLE|SS_LEFT|SS_NOPREFIX,x,y,w,h,id,0x82,title);
        };
        auto group=[this](short x,short y,short w,short h,WORD id,const wchar_t* title){
            control(WS_CHILD|WS_VISIBLE|BS_GROUPBOX,x,y,w,h,id,0x80,title);
        };
        auto key=[this](short x,short y,short w,WORD id,const wchar_t* title){
            control(WS_CHILD|WS_VISIBLE|WS_BORDER|SS_CENTER|SS_CENTERIMAGE,x,y,w,18,id,0x82,title);
        };
        label(12,8,456,12,1003,L"Choose your starting view. The game opens after your selection.");
        group(12,26,456,42,2000,L"Get started");
        label(24,38,432,26,2010,
            L"1. Select a friendly unit in a single-player land battle.\r\n"
            L"2. Press F9 to possess a soldier, then use the controls below.\r\n"
            L"3. F10 or Esc returns to the RTS camera.");
        group(12,74,220,136,2001,L"Your soldier");
        key(65,89,26,2101,L"W");key(35,111,26,2102,L"A");
        key(65,111,26,2103,L"S");key(95,111,26,2104,L"D");key(35,135,86,2105,L"Shift");
        label(135,96,84,12,2110,L"Move");label(135,138,84,12,2111,L"Run / gallop");
        label(24,160,196,42,2011,
            L"RMB  Ready ranged weapon\r\nLMB  Fire (bows: hold, then release)\r\n"
            L"Melee: time the shown LMB / RMB click\r\n1  Toggle unit melee / ranged mode");
        group(244,74,224,136,2002,L"Camera and actions");
        label(256,88,200,114,2012,
            L"Mouse  Look around\r\nP  Switch first / third person\r\n"
            L"Wheel  Third-person distance\r\nHold middle mouse  Orbit\r\n\r\n"
            L"T / G  Select / use ability or ammo\r\nF  Climb wall; W / S  Up / down\r\n"
            L"M  Mount / dismount the unit\r\nK  Hide HUD (melee prompts remain)");
        group(12,216,456,48,2003,L"Command your unit");
        label(24,229,432,28,2013,
            L"Q  Move / attack (hold + drag formation)     E  Stop\r\n"
            L"Z  Hold here     C  Follow me     R  Cycle fire orders\r\n"
            L"Q / Z: tap to walk, double-tap to run.");
        label(12,270,456,14,2014,L"Single-player land battles / sieges. Charging and climbing can be unreliable.");
        control(WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,122,291,112,24,third_button,0x80,L"Third-Person");
        control(WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,246,291,112,24,first_button,0x80,L"First-Person");
    }
};
inline INT_PTR CALLBACK procedure(HWND window,UINT message,WPARAM w,LPARAM l){
    if(message==WM_INITDIALOG){
        RECT bounds{},area{};GetWindowRect(window,&bounds);HWND owner=GetWindow(window,GW_OWNER);
        if(!owner||!GetWindowRect(owner,&area)||IsIconic(owner))SystemParametersInfoW(SPI_GETWORKAREA,0,&area,0);
        SetWindowPos(window,HWND_TOP,area.left+(area.right-area.left-(bounds.right-bounds.left))/2,
            area.top+(area.bottom-area.top-(bounds.bottom-bounds.top))/2,0,0,SWP_NOSIZE);
        SendMessageW(window,DM_SETDEFID,third_button,0);SetForegroundWindow(window);
        auto observer=reinterpret_cast<const Observer*>(l);
        if(observer&&observer->created)observer->created(window,observer->context);
        return TRUE;
    }
    if(message==WM_COMMAND&&(LOWORD(w)==third_button||LOWORD(w)==first_button||LOWORD(w)==IDCANCEL)){
        EndDialog(window,LOWORD(w));return TRUE;
    }
    if(message==WM_CLOSE){EndDialog(window,IDCANCEL);return TRUE;}
    return FALSE;
}
inline WORD fitting_font(){
    RECT area{};if(!SystemParametersInfoW(SPI_GETWORKAREA,0,&area,0))return 9;
    HDC dc=GetDC(nullptr);if(!dc)return 9;
    WORD result=7;
    for(WORD points=9;points>=7;--points){
        HFONT font=CreateFontW(-MulDiv(points,GetDeviceCaps(dc,LOGPIXELSY),72),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,L"Segoe UI");
        if(!font)continue;
        auto old=SelectObject(dc,font);TEXTMETRICW metrics{};SIZE alphabet{};
        bool valid=GetTextMetricsW(dc,&metrics)&&GetTextExtentPoint32W(dc,L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",52,&alphabet);
        SelectObject(dc,old);DeleteObject(font);
        int base_x=(alphabet.cx/26+1)/2;
        int width=MulDiv(dialog_width,base_x,4)+GetSystemMetrics(SM_CXFIXEDFRAME)*2;
        int height=MulDiv(dialog_height,metrics.tmHeight,8)+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYFIXEDFRAME)*2;
        if(valid&&width<=area.right-area.left-16&&height<=area.bottom-area.top-16){result=points;break;}
    }
    ReleaseDC(nullptr,dc);return result;
}
inline INT_PTR choose_result(HINSTANCE module,HWND owner=nullptr,const Observer* observer=nullptr){
    Template data(fitting_font());
    // The launch helper calls this before creating the game process.
    return DialogBoxIndirectParamW(module,reinterpret_cast<const DLGTEMPLATE*>(data.words.data()),owner,procedure,reinterpret_cast<LPARAM>(observer));
}
inline bool choose(HINSTANCE module,HWND owner=nullptr,const Observer* observer=nullptr){
    return resolve(choose_result(module,owner,observer));
}
}
