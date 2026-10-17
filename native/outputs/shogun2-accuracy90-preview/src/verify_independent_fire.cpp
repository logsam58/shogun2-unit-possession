#include "identity.h"
#include "ranged_task_completion.h"
#include "../vendor/include/MinHook.h"
struct MockTask {unsigned actor{},unit{},calls{};bool ready{};
    __declspec(noinline) bool gate(){++calls;return ready;}};
static unsigned controlled_actor=17;static RangedTaskGate original_gate;
static bool __fastcall gate_hook(void* t,void*){return ranged_task_nonblocking(static_cast<MockTask*>(t)->actor==controlled_actor,t,original_gate);}
int main(){try{
    void* entry;auto member=&MockTask::gate;static_assert(sizeof(member)==4,"x86 member ABI");memcpy(&entry,&member,4);unsigned char code[16];memcpy(code,entry,16);
    require(MH_Initialize()==MH_OK&&MH_CreateHook(entry,reinterpret_cast<void*>(&gate_hook),reinterpret_cast<void**>(&original_gate))==MH_OK&&MH_EnableHook(entry)==MH_OK,"Gate hook failed");
    MockTask personal{17,4,0,false},squadmate{18,4,0,false},enemy{17,5,0,false};
    // Ownership uses native actor pointer identity in production, not a unit-wide
    // flag. Distinct synthetic actor tokens exercise the forwarding boundary.
    enemy.actor=19;auto gate=reinterpret_cast<RangedTaskGate>(entry);
    for(unsigned i=0;i<10000;++i){require(gate(&personal)&&!gate(&squadmate)&&!gate(&enemy),"Gate changed another soldier");squadmate.ready=true;require(gate(&squadmate),"Ready AI result changed");squadmate.ready=false;}
    require(personal.calls==0&&squadmate.calls==20000&&enemy.calls==10000,"Native AI gate forwarding not exact");
    controlled_actor=0;require(!gate(&personal)&&personal.calls==1,"Release did not restore native ownership");
    require(MH_DisableHook(entry)==MH_OK&&memcmp(entry,code,16)==0,"Gate bytes not restored");
    std::array<unsigned char,0x100> task;task.fill(0xa5);auto before=task;acknowledge_skipped_ranged_draw(task.data());
    for(unsigned i=0;i<task.size();++i)require(task[i]==((i==0x84||i==0x6c||i==0x6d||i==0x6e)?1:before[i]),"Task acknowledgement changed unrelated bytes");
    auto engine=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Total War SHOGUN 2\\Empire.Retail.dll";require(hash_file(engine)==EXPECTED_HASH,"Engine identity differs");
    HMODULE image=LoadLibraryExW(engine,nullptr,LOAD_LIBRARY_AS_IMAGE_RESOURCE|LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE);require(image!=nullptr,"Read-only image mapping failed");auto b=reinterpret_cast<uintptr_t>(image)&~uintptr_t(3);
    struct Check{unsigned rva;std::vector<unsigned char> bytes;};std::vector<Check> checks={{0x3d3ce0,{0x56,0x8b,0xf1,0x8b,0x4e,4,0x80,0xb9,0x1f,1,0,0,0}},{0x3d33f0,{0x51,0x56,0x8b,0xf1,0x80,0x7e,0x7c,1}},{0x3d3d40,{0x56,0x8b,0xf1,0x8b,0x46,0x44,0x83,0xf8,7}},{0x3d3de0,{0x80,0x79,0x6e,0,0x74}}};
    for(auto& c:checks){require(!memcmp(reinterpret_cast<void*>(b+c.rva),c.bytes.data(),c.bytes.size()),"Native gate signature differs");
        void* fixture=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(fixture!=nullptr,"Decode fixture allocation failed");
        memcpy(fixture,reinterpret_cast<void*>(b+c.rva),128);DWORD prior;require(VirtualProtect(fixture,4096,PAGE_EXECUTE_READ,&prior)!=0,"Decode fixture protection failed");
        // Decode copied native instructions without executing them; their calls
        // still refer to game objects and are deliberately never enabled here.
        void* trampoline=nullptr;require(MH_CreateHook(fixture,reinterpret_cast<void*>(&gate_hook),&trampoline)==MH_OK,"Native gate trampoline could not be decoded");require(MH_RemoveHook(fixture)==MH_OK,"Unenabled gate cleanup failed");VirtualFree(fixture,0,MEM_RELEASE);}
    FreeLibrary(image);std::cout<<"PASS: 40000 x86 gate calls; only controlled actor bypassed; same-unit AI and other-unit AI forwarded exactly; release/restoration; skipped task writes limited to native completion bytes; four exact original engine signatures and trampoline decoding.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
