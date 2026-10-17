#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unknwn.h>
#include <string>
#include "integrated.h"
static HMODULE self_module,system_input;
static INIT_ONCE system_once=INIT_ONCE_STATIC_INIT;
static INIT_ONCE startup_once=INIT_ONCE_STATIC_INIT;
static BOOL CALLBACK start_integrated(PINIT_ONCE,void*,void**){
    HANDLE worker=CreateThread(nullptr,0,integrated_worker,self_module,0,nullptr);
    if(!worker){InterlockedExchange(&autoload_error,LONG(GetLastError()));InterlockedExchange(&autoload_stage,7);return FALSE;}
    CloseHandle(worker);return TRUE;
}
static BOOL CALLBACK load_system(PINIT_ONCE,void*,void**){
    wchar_t path[MAX_PATH];unsigned count=GetSystemDirectoryW(path,MAX_PATH);
    if(!count||count>=MAX_PATH-13)return FALSE;
    wcscat_s(path,L"\\dinput8.dll");
    system_input=LoadLibraryExW(path,nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
    return system_input!=nullptr;
}
static FARPROC original(const char* name){
    if(!InitOnceExecuteOnce(&system_once,load_system,nullptr,nullptr))return nullptr;
    return GetProcAddress(system_input,name);
}
extern "C" HRESULT WINAPI forward_DirectInput8Create(HINSTANCE instance,DWORD version,REFIID iid,void** out,IUnknown* outer){
    using Fn=HRESULT(WINAPI*)(HINSTANCE,DWORD,REFIID,void**,IUnknown*);
    auto fn=reinterpret_cast<Fn>(original("DirectInput8Create"));
    HRESULT result=fn?fn(instance,version,iid,out,outer):E_FAIL;
    // Start on the game's first input initialization, after Windows loader startup.
    InitOnceExecuteOnce(&startup_once,start_integrated,nullptr,nullptr);return result;
}
extern "C" HRESULT WINAPI forward_DllCanUnloadNow(){
    using Fn=HRESULT(WINAPI*)();auto fn=reinterpret_cast<Fn>(original("DllCanUnloadNow"));return fn?fn():S_FALSE;
}
extern "C" HRESULT WINAPI forward_DllGetClassObject(REFCLSID clsid,REFIID iid,void** out){
    using Fn=HRESULT(WINAPI*)(REFCLSID,REFIID,void**);auto fn=reinterpret_cast<Fn>(original("DllGetClassObject"));return fn?fn(clsid,iid,out):E_FAIL;
}
extern "C" HRESULT WINAPI forward_DllRegisterServer(){
    using Fn=HRESULT(WINAPI*)();auto fn=reinterpret_cast<Fn>(original("DllRegisterServer"));return fn?fn():E_FAIL;
}
extern "C" HRESULT WINAPI forward_DllUnregisterServer(){
    using Fn=HRESULT(WINAPI*)();auto fn=reinterpret_cast<Fn>(original("DllUnregisterServer"));return fn?fn():E_FAIL;
}
extern "C" const void* WINAPI forward_GetdfDIJoystick(){
    using Fn=const void*(WINAPI*)();auto fn=reinterpret_cast<Fn>(original("GetdfDIJoystick"));return fn?fn():nullptr;
}
extern "C" DWORD WINAPI CheckpointAutoloadProbe(){return (system_input?1u:0u)|(autoload_stage==6?2u:0u);}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,void*){
    if(reason==DLL_PROCESS_ATTACH){
        self_module=instance;
        // No threads, child creation, disk access or system DLL load here.
    }
    return TRUE;
}
