#include "identity.h"
// Offline exact-byte fixture. This never calls or writes into the game process.
using IntentSetter=void(__thiscall*)(void*,const float*,unsigned);
static std::vector<unsigned char> code_at(const std::vector<unsigned char>& image,unsigned rva,unsigned size){
    auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(image.data());
    auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
    auto sections=IMAGE_FIRST_SECTION(nt);
    for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];
        if(rva>=s.VirtualAddress&&rva+size<=s.VirtualAddress+s.SizeOfRawData){
            unsigned off=s.PointerToRawData+rva-s.VirtualAddress;
            require(off+size<=image.size(),"Native intent bounds");
            return {image.begin()+off,image.begin()+off+size};}}
    throw std::runtime_error("Native intent section absent");
}
int main(){void* memory=nullptr;try{
    std::wstring path=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Total War SHOGUN 2\\empire.retail.dll";
    require(hash_file(path)==EXPECTED_HASH,"Native intent requires verified engine build");
    std::ifstream stream(path,std::ios::binary);
    std::vector<unsigned char> image((std::istreambuf_iterator<char>(stream)),{});
    auto bytes=code_at(image,0x3936f0,105);
    require(bytes[0]==0x83&&bytes[1]==0x79&&bytes[2]==0x18&&bytes[3]==0&&bytes[102]==0xc2&&bytes[103]==8&&bytes[104]==0,"Native intent boundary and RET8");
    memory=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory!=nullptr,"Intent fixture allocation");
    memcpy(memory,bytes.data(),bytes.size());DWORD prior=0;
    require(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&prior)!=0,"Intent fixture protection");
    FlushInstructionCache(GetCurrentProcess(),memory,4096);
    auto setter=reinterpret_cast<IntentSetter>(memory);
    std::array<unsigned char,0x800> actor{},before{};
    auto word=[&](unsigned off)->unsigned&{return *reinterpret_cast<unsigned*>(actor.data()+off);};
    auto number=[&](unsigned off)->float&{return *reinterpret_cast<float*>(actor.data()+off);};
    auto bearing=[&](unsigned off)->unsigned short&{return *reinterpret_cast<unsigned short*>(actor.data()+off);};
    for(unsigned i=0;i<50000;++i){
        actor.fill(0xa5);word(0x18)=0;
        number(0x48)=float(i%101)*2;number(0x4c)=19.25f;number(0x50)=float(i%79)*-2;
        bearing(0x64)=static_cast<unsigned short>(i*37);
        float dx=float(int(i%13)-6)*.125f,dz=float(int(i%17)-8)*.125f;
        float target[2]={number(0x48)+dx,number(0x50)+dz};
        unsigned wanted=0x12340000u+static_cast<unsigned short>(i*71);
        auto expected_turn=static_cast<unsigned short>(wanted-bearing(0x64));
        before=actor;setter(actor.data(),target,wanted);
        require(number(0xd8)==dx&&number(0xdc)==0&&number(0xe0)==dz,"Native XZ intent and zero vertical intent");
        require(bearing(0xe4)==expected_turn,"Native signed/wrapped bearing delta");
        require(actor[0x198]==1,"Native magnitude animation-drive flag");
        require(std::abs(number(0x150)-std::sqrt(dx*dx+dz*dz))<.000001f,"Native horizontal intent magnitude");
        for(unsigned off=0;off<actor.size();++off){
            bool owned=(off>=0xd8&&off<0xe6)||(off>=0x150&&off<0x154)||off==0x198;
            if(!owned)require(actor[off]==before[off],"Native intent preserves actor position, rig, inventory, identifiers and unrelated data");
        }
    }
    actor.fill(0xa5);word(0x18)=1;before=actor;
    setter(actor.data(),nullptr,0);
    require(actor==before,"Parented actor rejects before target dereference and preserves all bytes");
    VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS:50000 exact native intent calls; ECX actor, two stack arguments, RET8, XZ delta, zero vertical intent, wrapped bearing, magnitude/animation flag, unrelated actor preservation and parent rejection. Actual gait selection still requires live verification.\n";return 0;
}catch(const std::exception& e){if(memory)VirtualFree(memory,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
