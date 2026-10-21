#include "../../../common/fixture_paths.h"
#include "identity.h"
#include <array>
using Speed=float(__thiscall*)(void*,unsigned);
static bool __fastcall can_run(void* object,void*){return reinterpret_cast<unsigned char*>(object)[0x17d]!=0;}
static std::vector<unsigned char> code_at(const std::vector<unsigned char>& image,unsigned rva,unsigned size){
    auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(image.data());
    auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
    auto sections=IMAGE_FIRST_SECTION(nt);
    for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];
        if(rva>=s.VirtualAddress&&rva+size<=s.VirtualAddress+s.SizeOfRawData){unsigned off=s.PointerToRawData+rva-s.VirtualAddress;
            require(off+size<=image.size(),"Native code bounds");return {image.begin()+off,image.begin()+off+size};}}
    throw std::runtime_error("Native function section absent");
}
int main(){void* memory=nullptr;try{
    std::wstring path=fixture_paths::engine();
    require(hash_file(path)==EXPECTED_HASH,"Native movement fixture requires verified game build");
    std::ifstream stream(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(stream)),{});
    auto walk=code_at(image,0x1be960,16),run=code_at(image,0x1a9da0,16),selector=code_at(image,0x18b470,39);
    require(walk.back()==0xc3&&run.back()==0xc3&&selector[8]==0x74&&selector[10]==0xe8&&selector[21]==0xe8&&selector[32]==0xe8,"Verified native getter/selector boundaries");
    memory=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory!=nullptr,"Fixture code allocation");
    auto bytes=static_cast<unsigned char*>(memory);memcpy(bytes,selector.data(),selector.size());memcpy(bytes+64,walk.data(),walk.size());memcpy(bytes+96,run.data(),run.size());
    auto relocate=[&](unsigned offset,uintptr_t target){int32_t delta=int32_t(target-reinterpret_cast<uintptr_t>(bytes+offset+5));memcpy(bytes+offset+1,&delta,4);};
    relocate(10,reinterpret_cast<uintptr_t>(&can_run));relocate(21,reinterpret_cast<uintptr_t>(bytes+96));relocate(32,reinterpret_cast<uintptr_t>(bytes+64));
    DWORD old=0;require(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&old)!=0,"Fixture code protection");FlushInstructionCache(GetCurrentProcess(),memory,4096);
    std::array<unsigned char,0x1000> unit{};std::array<unsigned char,48> entity{};
    *reinterpret_cast<uintptr_t*>(unit.data()+0xf9c)=reinterpret_cast<uintptr_t>(entity.data());
    *reinterpret_cast<float*>(entity.data()+0x1c)=1.4f;*reinterpret_cast<float*>(entity.data()+0x20)=3.4f;
    auto speed=reinterpret_cast<Speed>(memory);
    for(float factor:{1.0f,.7f,.2f,0.0f}){
        *reinterpret_cast<float*>(unit.data()+0x22c)=factor;unit[0x17d]=0;
        require(std::abs(speed(unit.data(),0)-1.4f*factor)<.000001f&&std::abs(speed(unit.data(),1)-1.4f*factor)<.000001f,"Native run denial and live walk factor");
        unit[0x17d]=1;require(std::abs(speed(unit.data(),1)-3.4f*factor)<.000001f,"Native run speed and live unit factor");
    }
    VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: actual verified native x86 walk/run getter and selector bytes, x87 returns, run restriction, changed unit movement factor and zero speed; game process untouched.\n";return 0;
}catch(const std::exception& e){if(memory)VirtualFree(memory,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
