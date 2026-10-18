#pragma once
#include <windows.h>
#include <cwchar>
// A death eyelid is also an unowned, visible game-process window and can be
// larger than the former minimum size. Identify the actual game window by its
// observed native class, independently of its current visibility/client size.
namespace gamewindow105 {
inline bool belongs(HWND window,DWORD pid){
    DWORD owner=0;wchar_t name[64]{};
    return window&&IsWindow(window)&&!GetWindow(window,GW_OWNER)&&
        GetWindowThreadProcessId(window,&owner)&&owner==pid&&
        GetClassNameW(window,name,_countof(name))&&wcscmp(name,L"SHOGUN 2")==0;
}
struct Search {DWORD pid;HWND window;};
inline BOOL CALLBACK choose(HWND window,LPARAM parameter){
    auto& search=*reinterpret_cast<Search*>(parameter);
    if(belongs(window,search.pid)){search.window=window;return FALSE;}return TRUE;
}
inline HWND find(DWORD pid){Search search{pid,nullptr};EnumWindows(choose,reinterpret_cast<LPARAM>(&search));return search.window;}
inline HWND refresh(HWND window,DWORD pid){return belongs(window,pid)?window:find(pid);}
inline bool client(HWND window,RECT& area,POINT& origin){
    return window&&IsWindowVisible(window)&&!IsIconic(window)&&GetClientRect(window,&area)&&
        area.right>=640&&area.bottom>=360&&ClientToScreen(window,&origin);
}
}
