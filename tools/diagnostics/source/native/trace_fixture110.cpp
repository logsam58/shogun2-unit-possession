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
