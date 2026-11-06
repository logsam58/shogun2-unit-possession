// Exercise production Escape queue; window/focus queries are explicit providers.
// Never creates a window, steals focus, sends input or runs a game process.
#include <windows.h>
#include <cassert>
#include <iostream>
static HWND focused218=reinterpret_cast<HWND>(0x12000);static DWORD owner218=7;
static BOOL fixture_window218(HWND w){return w==reinterpret_cast<HWND>(0x12000);}
static DWORD fixture_thread218(HWND,LPDWORD pid){*pid=owner218;return 10;}
static HWND fixture_focus218(){return focused218;}
static DWORD fixture_pid218(){return 7;}
#define IsWindow fixture_window218
#define GetWindowThreadProcessId fixture_thread218
#define GetForegroundWindow fixture_focus218
#define GetCurrentProcessId fixture_pid218
#include "../../native/deck/escape218.h"
#undef IsWindow
#undef GetWindowThreadProcessId
#undef GetForegroundWindow
#undef GetCurrentProcessId
int main(){unsigned cases=0;auto check=[&](bool good){assert(good);++cases;};deck218::Escape q;unsigned char keys[256]{};auto own=reinterpret_cast<HWND>(0x12000),other=reinterpret_cast<HWND>(0x13000);
 check(!q.request(nullptr,1));check(!q.request(other,1));owner218=8;check(!q.request(own,1));owner218=7;focused218=other;check(!q.request(own,1));focused218=own;
 check(q.request(own,1000));keys[0x11]=0x80;check(q.apply(keys,true,own,1001)&&keys[1]==0x80&&keys[0x11]==0x80);keys[1]=0;check(!q.apply(keys,true,own,1002)&&keys[1]==0);
 check(q.request(own,2000));check(!q.apply(keys,false,own,2010));check(q.apply(keys,true,own,2011));keys[1]=0;
 check(q.request(own,3000));check(!q.apply(keys,true,own,3251));check(!q.apply(keys,true,own,3252));
 check(q.request(own,4000));check(!q.apply(keys,true,other,4001));check(!q.apply(keys,true,own,4002));
 check(q.request(own,5000));q.clear();check(!q.apply(keys,true,own,5001));
 check(q.request(own,6000));check(!q.apply(keys,true,own,5999));check(!q.apply(keys,true,own,6001));
 // A held physical Escape stays physical; this queue only ORs its one pulse.
 keys[1]=0x80;check(q.request(own,7000));check(q.apply(keys,true,own,7001)&&keys[1]==0x80);check(!q.apply(keys,true,own,7002)&&keys[1]==0x80);
 std::cout<<"PASS escape218: "<<cases<<" native-key pulse/window/focus/expiry/handoff cases; window APIs are providers; no live input\n";
}
