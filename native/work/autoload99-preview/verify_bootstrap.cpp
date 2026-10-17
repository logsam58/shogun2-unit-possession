#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unknwn.h>
#include <iostream>
#include <string>
#include <stdexcept>
extern "C" __declspec(dllimport) HRESULT WINAPI DirectInput8Create(HINSTANCE,DWORD,REFIID,void**,IUnknown*);
static void check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
int wmain(){try{
    HMODULE proxy=GetModuleHandleW(L"DINPUT8.dll");check(proxy!=nullptr,"Static input import did not resolve");
    wchar_t loaded[MAX_PATH];GetModuleFileNameW(proxy,loaded,MAX_PATH);
    wchar_t executable[MAX_PATH];GetModuleFileNameW(nullptr,executable,MAX_PATH);
    std::wstring expected=executable;expected=expected.substr(0,expected.find_last_of(L'\\'))+L"\\dinput8.dll";
    check(_wcsicmp(loaded,expected.c_str())==0,"Local bootstrap was not selected by Windows loader");
    wchar_t system[MAX_PATH];GetSystemDirectoryW(system,MAX_PATH);wcscat_s(system,L"\\dinput8.dll");
    HMODULE real=LoadLibraryExW(system,nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);check(real&&real!=proxy,"Original system input DLL absent");
    for(const char* name:{"DirectInput8Create","DllCanUnloadNow","DllGetClassObject","DllRegisterServer","DllUnregisterServer","GetdfDIJoystick"}){
        check(GetProcAddress(proxy,name)!=nullptr,"Proxy export missing");check(GetProcAddress(real,name)!=nullptr,"System export missing");}
    using Create=HRESULT(WINAPI*)(HINSTANCE,DWORD,REFIID,void**,IUnknown*);
    auto real_create=reinterpret_cast<Create>(GetProcAddress(real,"DirectInput8Create"));
    const GUID iid={0xbf798031,0x483a,0x4da2,{0xaa,0x99,0x5d,0x64,0xed,0x36,0x97,0x00}};
    for(DWORD version:{0u,0x0800u}){
        void *a=nullptr,*b=nullptr;
        HRESULT ra=real_create(GetModuleHandleW(nullptr),version,iid,&a,nullptr);
        HRESULT rb=DirectInput8Create(GetModuleHandleW(nullptr),version,iid,&b,nullptr);
        check(ra==rb,"Forwarded DirectInput result differs");
        if(version==0x0800)check(SUCCEEDED(ra)&&a&&b,"DirectInput8 successful creation not preserved");
        if(a)static_cast<IUnknown*>(a)->Release();if(b)static_cast<IUnknown*>(b)->Release();
    }
    using Format=const void*(WINAPI*)();
    check(reinterpret_cast<Format>(GetProcAddress(proxy,"GetdfDIJoystick"))()==reinterpret_cast<Format>(GetProcAddress(real,"GetdfDIJoystick"))(),"Joystick format forwarding differs");
    using Probe=DWORD(WINAPI*)();check(reinterpret_cast<Probe>(GetProcAddress(proxy,"CheckpointAutoloadProbe"))()==1,"Proxy spawned helper for a non-game process");
    std::cout<<"PASS: Windows static import selects local bootstrap; six system input exports present; original system DLL kept; invalid and successful DirectInput calls match; original joystick format pointer retained; no companion launched for test process.\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
