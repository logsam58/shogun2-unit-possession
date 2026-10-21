#include "../../../common/fixture_paths.h"
#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include <array>
template<class T>static void put92(void* memory,unsigned offset,T value){memcpy(static_cast<unsigned char*>(memory)+offset,&value,sizeof value);}
int main(){unsigned char* mapped=nullptr;try{
 std::wstring path=fixture_paths::engine();require(hash_file(path)==EXPECTED_HASH,"Exact native fixture image");
 std::ifstream in(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());auto nt=reinterpret_cast<IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
 mapped=static_cast<unsigned char*>(VirtualAlloc(nullptr,nt->OptionalHeader.SizeOfImage,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(mapped!=nullptr,"Native fixture allocation");memcpy(mapped,image.data(),nt->OptionalHeader.SizeOfHeaders);
 auto sections=IMAGE_FIRST_SECTION(nt);for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto&s=sections[i];if(s.SizeOfRawData){require(s.PointerToRawData+s.SizeOfRawData<=image.size(),"Native section bounds");memcpy(mapped+s.VirtualAddress,image.data()+s.PointerToRawData,s.SizeOfRawData);}}
 uint32_t delta=uint32_t(reinterpret_cast<uintptr_t>(mapped)-nt->OptionalHeader.ImageBase);auto reloc=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];unsigned offset=0;
 while(offset<reloc.Size){auto block=reinterpret_cast<IMAGE_BASE_RELOCATION*>(mapped+reloc.VirtualAddress+offset);require(block->SizeOfBlock>=8&&offset+block->SizeOfBlock<=reloc.Size,"Relocation bounds");auto entries=reinterpret_cast<uint16_t*>(block+1);
  for(unsigned i=0;i<(block->SizeOfBlock-8)/2;++i)if(entries[i]>>12==IMAGE_REL_BASED_HIGHLOW)*reinterpret_cast<uint32_t*>(mapped+block->VirtualAddress+(entries[i]&4095))+=delta;offset+=block->SizeOfBlock;}
 DWORD old=0;require(VirtualProtect(mapped,nt->OptionalHeader.SizeOfImage,PAGE_EXECUTE_READ,&old)!=0,"Read-only native fixture image");FlushInstructionCache(GetCurrentProcess(),mapped,nt->OptionalHeader.SizeOfImage);

 std::array<unsigned char,0x110> witness{},common{};std::array<unsigned char,0x20> environment{};std::array<unsigned char,0x700> actor{},enemy{};
 auto p32=[](auto& a){return uint32_t(reinterpret_cast<uintptr_t>(a.data()));};
 put92(environment.data(),8,p32(common));put92(witness.data(),4,p32(environment));put92(witness.data(),0x84,p32(actor));put92(witness.data(),0x88,p32(enemy));
 put92(common.data(),0x48,0xcafebabeu);using Prepare=uint32_t(__thiscall*)(void*);auto prepare=reinterpret_cast<Prepare>(mapped+0x3e5d40);unsigned cases=0;
 uintptr_t before{},after{};__asm mov before,esp
 for(unsigned scenario=0;scenario<4;++scenario)for(unsigned i=0;i<100;++i){
  put92(witness.data(),0xc,uint8_t(scenario==0));put92(witness.data(),0xd0,uint8_t(1));
  put92(actor.data(),0xf0,uint8_t(scenario!=1));put92(enemy.data(),0xf0,uint8_t(scenario!=2));
  put92(common.data(),0x50,1000u+i);put92(witness.data(),0x2c,89.f);
  auto result=prepare(witness.data());require((result&0xff)==(scenario==0?0u:1u),"Original native active/dead/expired preparation guard differs");++cases;
 }
 __asm mov after,esp
 require(before==after,"Native preparation thiscall stack imbalance");uint32_t seed{};memcpy(&seed,common.data()+0x48,4);require(seed==0xcafebabeu,"Native early-state guard consumed RNG");
 VirtualFree(mapped,0,MEM_RELEASE);mapped=nullptr;
 std::cout<<"PASS: "<<cases<<" exact original native prepare early-state calls: already registered, dead attacker/defender and deferred ten-second expiry; exact x86 stack, immutable mapped image, no game initialization/RNG. Waiting-completion ports are covered separately by synthetic tests.\n";return 0;
 }catch(const std::exception& e){if(mapped)VirtualFree(mapped,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
