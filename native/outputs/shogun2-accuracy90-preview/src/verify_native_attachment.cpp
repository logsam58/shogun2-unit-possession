#include "../../../common/fixture_paths.h"
#include "identity.h"
#include "../vendor/include/MinHook.h"
using Predicate=bool(__thiscall*)(void*);
static Predicate original_predicate;
static unsigned hooked,actor_calls;static void* last_actor;static bool force_release;
static bool __fastcall actor_fixture(void* actor,void*){++actor_calls;last_actor=actor;return actor&&*static_cast<unsigned*>(actor)!=0;}
static bool __fastcall predicate_fixture(void* object,void*){++hooked;bool result=original_predicate(object);return force_release||result;}
int main(){void* memory=nullptr;try{
    const std::wstring path=fixture_paths::engine();
    require(hash_file(path)==EXPECTED_HASH,"Attachment fixture engine differs");
    std::ifstream in(path,std::ios::binary);std::vector<unsigned char> raw((std::istreambuf_iterator<char>(in)),{});
    auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(raw.data());auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(raw.data()+dos->e_lfanew);
    auto sections=IMAGE_FIRST_SECTION(nt);unsigned offset=0;
    for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];if(0x472b70>=s.VirtualAddress&&0x472b98<=s.VirtualAddress+s.SizeOfRawData)offset=s.PointerToRawData+0x472b70-s.VirtualAddress;}
    require(offset&&offset+40<=raw.size(),"Attachment fixture bounds");
    const unsigned char prefix[]={0x8b,0x41,0x14,0x8b,0x40,0x1c,0x83,0x38,9};
    require(!memcmp(raw.data()+offset,prefix,sizeof prefix)&&raw[offset+25]==0xe8&&raw[offset+39]==0xc3,"Exact native attachment predicate differs");
    require(0x10472b70+30+*reinterpret_cast<const int32_t*>(raw.data()+offset+26)==0x103d2f20,"Native actor release callee differs");
    memory=VirtualAlloc(nullptr,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);require(memory!=nullptr,"Attachment allocation");memcpy(memory,raw.data()+offset,40);
    // Entire native predicate, with only its external actor function redirected.
    *reinterpret_cast<int32_t*>(static_cast<unsigned char*>(memory)+26)=int32_t(uintptr_t(&actor_fixture)-(uintptr_t(memory)+30));
    unsigned char saved[40];memcpy(saved,memory,40);DWORD prior=0;require(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&prior)!=0,"Attachment protection");FlushInstructionCache(GetCurrentProcess(),memory,40);
    require(MH_Initialize()==MH_OK&&MH_CreateHook(memory,reinterpret_cast<void*>(&predicate_fixture),reinterpret_cast<void**>(&original_predicate))==MH_OK&&MH_EnableHook(memory)==MH_OK,"Attachment hook");
    unsigned actor=0,kind=9;uintptr_t description[8]{},holder[3]{},object[6]{};description[7]=uintptr_t(&kind);holder[2]=uintptr_t(&actor);object[4]=uintptr_t(holder);object[5]=uintptr_t(description);
    auto predicate=reinterpret_cast<Predicate>(memory);
    for(unsigned i=0;i<10000;++i){
        actor=0;force_release=false;require(!predicate(object)&&last_actor==&actor,"Native false and actor ECX");
        actor=1;require(predicate(object),"Native AL true return");
        kind=8;unsigned before=actor_calls;require(!predicate(object)&&actor_calls==before,"Other attachment skips actor callback");
        kind=9;actor=0;force_release=true;require(predicate(object),"Hook AL override return");
    }
    require(hooked==40000&&actor_calls==30000,"Exact attachment/actor calls");
    require(MH_DisableHook(memory)==MH_OK&&!memcmp(memory,saved,40),"Attachment byte restoration");
    force_release=false;require(!predicate(object)&&hooked==40000,"Native predicate after disable");MH_Uninitialize();VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS:40000 full native attachment predicate calls; ECX actor backlink, type9 filter, AL returns/override, exact native bytes and restoration. Visible asset behavior remains a live check.\n";return 0;
}catch(const std::exception& e){MH_Uninitialize();if(memory)VirtualFree(memory,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
