#include "../../../common/fixture_paths.h"
#include "identity.h"
#include "camera_entry_gate.h"
#include "../vendor/include/MinHook.h"
using Input=uintptr_t(__thiscall*)(void*,uintptr_t,uintptr_t);
static Input original_input;
static __declspec(thread) LONG depth;
static unsigned calls;
static uintptr_t __fastcall input_hook(void* object,void*,uintptr_t a,uintptr_t b){
    ++depth;uintptr_t value=original_input(object,a,b);--depth;++calls;return value;
}
struct Sync{HANDLE entered,release;};
static DWORD WINAPI unrelated_worker(void* param){
    auto sync=static_cast<Sync*>(param);depth=1;SetEvent(sync->entered);
    WaitForSingleObject(sync->release,5000);depth=0;return 0;
}
int main(){void* memory=nullptr;try{
    // Exact native dispatcher prologue plus a fixture tail with its verified RET8.
    // The full dispatcher and real input lifecycle are not emulated here.
    std::wstring path=fixture_paths::engine();
    require(hash_file(path)==EXPECTED_HASH,"Input fixture engine differs");
    std::ifstream in(path,std::ios::binary);std::vector<unsigned char> raw((std::istreambuf_iterator<char>(in)),{});
    auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(raw.data());
    auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(raw.data()+dos->e_lfanew);
    auto sections=IMAGE_FIRST_SECTION(nt);unsigned offset=0;
    for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];
        if(0x2fdcf0>=s.VirtualAddress&&0x2fdcfa<=s.VirtualAddress+s.SizeOfRawData)
            offset=s.PointerToRawData+0x2fdcf0-s.VirtualAddress;}
    require(offset&&offset+10<=raw.size(),"Input fixture bounds");
    const unsigned char prefix[]={0x83,0xec,0x1c,0x56,0x8b,0xf1,0x57,0x8b,0x46,0x1c};
    require(!memcmp(raw.data()+offset,prefix,10),"Exact native input prologue differs");
    const unsigned char tail[]={0x8b,0x44,0x24,0x28,0x33,0x44,0x24,0x2c,0x33,0x46,0x1c,0x5f,0x5e,0x83,0xc4,0x1c,0xc2,0x08,0};
    memory=VirtualAlloc(nullptr,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);require(memory!=nullptr,"Input fixture allocation");
    memcpy(memory,prefix,sizeof prefix);memcpy(static_cast<unsigned char*>(memory)+sizeof prefix,tail,sizeof tail);
    DWORD prior=0;require(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&prior)!=0,"Input fixture protection");
    FlushInstructionCache(GetCurrentProcess(),memory,4096);
    require(MH_Initialize()==MH_OK,"Input hook initialize");
    require(MH_CreateHook(memory,reinterpret_cast<void*>(&input_hook),reinterpret_cast<void**>(&original_input))==MH_OK,"Input hook create");
    require(MH_EnableHook(memory)==MH_OK,"Input hook enable");
    std::array<unsigned,8> object{};object[7]=0x35172819;
    auto input=reinterpret_cast<Input>(memory);
    for(unsigned i=0;i<50000;++i){uintptr_t a=i*71+11,b=i*37+17;
        require(input(object.data(),a,b)==(a^b^object[7]),"ECX, both stack arguments and EAX are forwarded exactly");
        require(depth==0,"Input depth clears after forwarding");}
    require(calls==50000,"Every input call forwarded");
    Handle entered(CreateEventW(nullptr,TRUE,FALSE,nullptr)),release(CreateEventW(nullptr,TRUE,FALSE,nullptr));
    require(entered.h&&release.h,"Input thread fixture events");Sync sync{entered.h,release.h};
    Handle worker(CreateThread(nullptr,0,unrelated_worker,&sync,0,nullptr));require(worker.h!=nullptr,"Input thread fixture worker");
    require(WaitForSingleObject(entered.h,5000)==WAIT_OBJECT_0,"Input thread fixture start");
    bool independent=camera_entry_thread(31,31,31,depth);SetEvent(release.h);
    require(WaitForSingleObject(worker.h,5000)==WAIT_OBJECT_0,"Input thread fixture completion");
    require(independent,"Unrelated worker depth does not reject camera-owner entry");
    depth=1;bool nested=camera_entry_thread(31,31,31,depth);depth=0;
    require(!nested,"Owner-thread nesting rejects camera replacement");
    require(MH_DisableHook(memory)==MH_OK&&!memcmp(memory,prefix,sizeof prefix),"Native input bytes restore");
    require(input(object.data(),123,456)==(123^456^object[7])&&calls==50000,"Native input executes after disable");
    MH_Uninitialize();VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: 50000 input hook calls with exact native prologue and fixture RET8 tail; ECX, both arguments, EAX, stack and restoration; camera callback depth is isolated from other threads. Full native dispatcher lifecycle remains a live check.\n";return 0;
}catch(const std::exception& e){MH_Uninitialize();if(memory)VirtualFree(memory,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
