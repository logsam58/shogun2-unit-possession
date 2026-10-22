#pragma once
#include "../../common/startup_view160.h"
#include <vector>
#include <string>
namespace startupdialog160 {
constexpr int third_button=1001,first_button=1002;
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
    Template(){
        dword(WS_POPUP|WS_CAPTION|WS_SYSMENU|DS_MODALFRAME|DS_SETFONT);dword(WS_EX_TOPMOST);
        word(3);word(0);word(0);word(300);word(108);word(0);word(0);
        text(L"Shogun 2 Unit Possession");word(9);text(L"Segoe UI");
        control(WS_CHILD|WS_VISIBLE|SS_LEFT,12,12,276,40,1003,0x82,
            L"Choose your starting camera view.\r\nYou can switch views with P during possession.");
        control(WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,30,68,114,24,third_button,0x80,L"Third-Person");
        control(WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_PUSHBUTTON,156,68,114,24,first_button,0x80,L"First-Person");
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
inline bool choose(HINSTANCE module,HWND owner=nullptr,const Observer* observer=nullptr){
    Template data;
    // Runs on the autoload worker after DirectInput initialization, never in
    // DllMain. Failure preserves the original first-person behavior.
    return resolve(DialogBoxIndirectParamW(module,reinterpret_cast<const DLGTEMPLATE*>(data.words.data()),owner,procedure,reinterpret_cast<LPARAM>(observer)));
}
inline bool launch_choice(HINSTANCE module){
    HWND owner=GetForegroundWindow();DWORD pid=0;if(owner)GetWindowThreadProcessId(owner,&pid);
    if(pid!=GetCurrentProcessId())owner=nullptr;
    return choose(module,owner);
}
}
