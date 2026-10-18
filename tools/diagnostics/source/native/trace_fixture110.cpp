#include <windows.h>
#include <fstream>
#include <string>
#include <cstdint>
struct Owner {volatile DWORD vt;DWORD pad[9];DWORD table,count;};
struct Audio {volatile DWORD vt;};
struct Wrapper {DWORD pad[6];volatile DWORD object;};
static Owner owner{};static DWORD entries[9]{};static DWORD globals[2]{};
static Audio a{},b{};static Wrapper wrapper{};static DWORD table1[7]{},table2[7]{};
static volatile DWORD sink{};
static HANDLE go111{},filter111{},retired111{};static std::wstring root111;
static LONG continuation111(EXCEPTION_POINTERS*p){
    if(p->ExceptionRecord->ExceptionCode!=0xe0421111)return EXCEPTION_CONTINUE_SEARCH;
    {std::ofstream f(root111+L"\\saved-context111.txt");f<<std::hex<<p->ContextRecord->Dr3<<' '<<p->ContextRecord->Dr7;}
    SetEvent(filter111);WaitForSingleObject(retired111,10000);return EXCEPTION_CONTINUE_EXECUTION;
}
static DWORD WINAPI context_writer111(void*){
    WaitForSingleObject(go111,10000);
    __try{RaiseException(0xe0421111,0,0,nullptr);}__except(continuation111(GetExceptionInformation())){}
    a.vt=DWORD(reinterpret_cast<uintptr_t>(table2));return 0;
}
static volatile LONG ready111{};
static DWORD WINAPI race_writer111(void*p){
    InterlockedIncrement(&ready111);WaitForSingleObject(go111,10000);
    if(p)a.vt=DWORD(reinterpret_cast<uintptr_t>(table2));else wrapper.object=0;return 0;
}
__declspec(noinline) DWORD __fastcall scout(Wrapper* w){return w->object;}
__declspec(noinline) void __fastcall destroy_owner(Owner* o){o->count=0;o->table=0;o->vt=0x22222222;}
static DWORD WINAPI writer(void*){b.vt=DWORD(reinterpret_cast<uintptr_t>(table2));return 0;}
static int handled_fault(){__try{*reinterpret_cast<volatile int*>(0x42)=1;}__except(EXCEPTION_EXECUTE_HANDLER){Sleep(150);return 0;}return 8;}
static int foreign_step(){__try{RaiseException(EXCEPTION_SINGLE_STEP,0,0,nullptr);}__except(EXCEPTION_EXECUTE_HANDLER){return 0;}return 9;}
static bool exists(const std::wstring&p){return GetFileAttributesW(p.c_str())!=INVALID_FILE_ATTRIBUTES;}
int wmain(int argc,wchar_t**argv){
    if(argc<3)return 2;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    std::wstring root=argv[1],mode=argv[2];auto address=[](auto p){return DWORD(reinterpret_cast<uintptr_t>(p));};
    for(auto& t:table1)t=address(&writer);for(auto& t:table2)t=address(&writer);
    a.vt=address(table1);b.vt=address(table1);wrapper.object=address(&a);
    owner.vt=0x11111111;owner.table=address(table1);owner.count=2;
    entries[6]=1;entries[7]=1;entries[8]=address(&owner);globals[0]=address(entries);globals[1]=1;
    {std::ofstream f(root+L"\\targeted110-fixture.txt");f<<std::hex<<address(&destroy_owner)<<' '<<address(&scout)<<' '<<address(globals);}
    uint64_t end=GetTickCount64()+20000;
    while(!exists(root+L"\\trace-go")){if(GetTickCount64()>end)return 3;Sleep(10);}
    if(mode==L"context-restore"){
        root111=root;go111=CreateEventW(nullptr,TRUE,FALSE,nullptr);filter111=CreateEventW(nullptr,TRUE,FALSE,nullptr);retired111=CreateEventW(nullptr,TRUE,FALSE,nullptr);
        HANDLE thread=CreateThread(nullptr,0,context_writer111,nullptr,0,nullptr);
        sink=scout(&wrapper);SetEvent(go111);if(WaitForSingleObject(filter111,10000)!=WAIT_OBJECT_0)return 5;
        wrapper.object=0;SetEvent(retired111);if(WaitForSingleObject(thread,10000)!=WAIT_OBJECT_0)return 6;CloseHandle(thread);
        CloseHandle(go111);CloseHandle(filter111);CloseHandle(retired111);
        {std::ofstream f(root+L"\\actions.done");f<<"done";}Sleep(200);return 0;
    }
    if(mode==L"retirement-race"){
        go111=CreateEventW(nullptr,TRUE,FALSE,nullptr);HANDLE threads[16]{};
        for(unsigned i=0;i<16;++i)threads[i]=CreateThread(nullptr,0,race_writer111,reinterpret_cast<void*>(i),0,nullptr);
        while(ready111!=16)Sleep(1);sink=scout(&wrapper);SetEvent(go111);
        if(WaitForMultipleObjects(16,threads,TRUE,10000)!=WAIT_OBJECT_0)return 7;
        for(auto h:threads)CloseHandle(h);CloseHandle(go111);
        {std::ofstream f(root+L"\\actions.done");f<<"done";}Sleep(200);return 0;
    }
    if(mode==L"history-limit"){
        static Audio many[96]{};for(auto& object:many)object.vt=address(table1);
        sink=scout(&wrapper);for(auto& object:many){wrapper.object=address(&object);Sleep(50);}
        {std::ofstream f(root+L"\\actions.done");f<<"done";}Sleep(200);return 0;
    }
    sink=scout(&wrapper);destroy_owner(&owner);
    wrapper.object=address(&b);
    HANDLE writers[4]{};for(auto& h:writers)h=CreateThread(nullptr,0,writer,nullptr,0,nullptr);
    WaitForMultipleObjects(4,writers,TRUE,10000);for(auto h:writers)CloseHandle(h);
    b.vt=0xd4b5ffe7;owner.vt=0x33333333;
    if(mode==L"budget")for(int i=0;i<2000;++i)destroy_owner(&owner);
    {std::ofstream f(root+L"\\actions.done");f<<"done";}
    if(mode==L"detach"){
        while(!exists(root+L"\\after-detach")){if(GetTickCount64()>end)return 4;Sleep(10);}
        b.vt=address(table1);owner.vt=0x44444444;sink=scout(&wrapper);
    }
    if(mode==L"handled")return handled_fault();
    if(mode==L"foreign-step")return foreign_step();
    if(mode==L"crash")*reinterpret_cast<volatile int*>(0x42)=1;
    Sleep(200);return 0;
}
