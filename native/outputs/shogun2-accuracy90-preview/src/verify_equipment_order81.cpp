#include "identity.h"
#include <array>
static void* __fastcall copy_vector81(void* destination,void*,const void* source){memcpy(destination,source,12);return destination;}
struct Writer81 {unsigned count{},id{},flag{},action{};};
static void* __cdecl write_group81(void* context,const void* group){require(*static_cast<const unsigned*>(group)==0x115c4da0,"Native group serializer received wrong object");return context;}
static void* __cdecl write_uint81(void* context,unsigned value){auto& w=*static_cast<Writer81*>(context);if(!w.count)w.id=value;else w.action=value;++w.count;return context;}
static void* __cdecl write_byte81(void* context,unsigned char value){static_cast<Writer81*>(context)->flag=value;return context;}
static std::vector<unsigned char> native_code81(const std::vector<unsigned char>& image,unsigned rva,unsigned size){
    auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(image.data());auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);auto sections=IMAGE_FIRST_SECTION(nt);
    for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];if(rva>=s.VirtualAddress&&rva+size<=s.VirtualAddress+s.SizeOfRawData){unsigned off=s.PointerToRawData+rva-s.VirtualAddress;require(off+size<=image.size(),"Native code bounds");return {image.begin()+off,image.begin()+off+size};}}
    throw std::runtime_error("Native code section missing");
}
static void jump81(unsigned char* where,const void* target){where[0]=0xe9;uint32_t relative=uint32_t(reinterpret_cast<uintptr_t>(target)-reinterpret_cast<uintptr_t>(where)-5);memcpy(where+1,&relative,4);}
int main(){void* memory=nullptr;try{
    std::wstring path=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Total War SHOGUN 2\\Empire.Retail.dll";require(hash_file(path)==EXPECTED_HASH,"Native order fixture requires exact engine");
    std::ifstream f(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(f)),{});
    memory=VirtualAlloc(nullptr,0x350000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory!=nullptr,"Fixture allocation");auto code=static_cast<unsigned char*>(memory);
    auto constructor=native_code81(image,0x255290,0x73),serializer=native_code81(image,0x2e6cd0,0x38);
    memcpy(code+0x255290,constructor.data(),constructor.size());memcpy(code+0x2e6cd0,serializer.data(),serializer.size());
    jump81(code+0x10ac90,&copy_vector81);jump81(code+0x342930,&write_group81);jump81(code+0x2859e0,&write_uint81);jump81(code+0x285d00,&write_byte81);
    DWORD old{};require(VirtualProtect(memory,0x350000,PAGE_EXECUTE_READ,&old)!=0,"Fixture protection");FlushInstructionCache(GetCurrentProcess(),memory,0x350000);
    using Constructor=void*(__thiscall*)(void*,const void*,unsigned,unsigned,unsigned);using Serializer=bool(__thiscall*)(void*,void*);
    auto construct=reinterpret_cast<Constructor>(code+0x255290);auto serialize=reinterpret_cast<Serializer>(code+0x2e6cd0);
    std::array<unsigned char,0x44> group{};std::array<unsigned char,0x54> order{};
    unsigned before{},after{},calls=0;
    for(unsigned action:{0u,1u,2u})for(unsigned id:{22u,23u,24u,25u,26u,27u,28u,50u,57u,255u})for(unsigned flag:{0u,1u})for(unsigned repeat=0;repeat<500;++repeat){
        Writer81 writer{};memset(order.data(),0xcd,order.size());
        __asm mov before,esp
        construct(order.data(),group.data(),id,flag,action);require(serialize(order.data(),&writer),"Native serializer refused");
        __asm mov after,esp
        require(before==after&&writer.count==2&&writer.id==id&&writer.flag==flag&&writer.action==action,"Native ability command ABI or payload mismatch");++calls;
    }
    VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: "<<calls<<" exact native ability constructor/serializer calls; RET16/RET4 stack balance, selection-group object, ability ID, byte flag and enable/disable/toggle actions. Primitive/vector dependencies are fixtures; game untouched.\n";return 0;
}catch(const std::exception& e){if(memory)VirtualFree(memory,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
