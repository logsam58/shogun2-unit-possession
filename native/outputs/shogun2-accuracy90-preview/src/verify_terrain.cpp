#include "identity.h"
#include "terrain_math.h"
#include <array>
static std::vector<unsigned char> code_at(const std::vector<unsigned char>& image,unsigned rva,unsigned size){
    auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(image.data());
    auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
    auto sections=IMAGE_FIRST_SECTION(nt);
    for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];
        if(rva>=s.VirtualAddress&&rva+size<=s.VirtualAddress+s.SizeOfRawData){unsigned off=s.PointerToRawData+rva-s.VirtualAddress;
            require(off+size<=image.size(),"Native fixture bounds");return {image.begin()+off,image.begin()+off+size};}}
    throw std::runtime_error("Native fixture section absent");
}
static float __fastcall synthetic_heightfield(void* object,void*,const float*){
    return *reinterpret_cast<float*>(object);
}
int main(){void* memory=nullptr;try{
    std::wstring path=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Total War SHOGUN 2\\empire.retail.dll";
    require(hash_file(path)==EXPECTED_HASH,"Native terrain fixture requires verified engine");
    std::ifstream stream(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(stream)),{});
    auto native=code_at(image,0x3aa602,0x57),height=code_at(image,0x17c0a0,0x53);
    memory=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory!=nullptr,"Fixture allocation");
    auto bytes=static_cast<unsigned char*>(memory);
    const unsigned char prefix[]={0x53,0x8b,0xd9,0xf3,0x0f,0x10,0x44,0x24,8,0x0f,0x57,0xc9};
    memcpy(bytes,prefix,sizeof prefix);memcpy(bytes+sizeof prefix,native.data(),native.size());
    const unsigned char suffix[]={0x5b,0xc2,4,0};memcpy(bytes+sizeof prefix+native.size(),suffix,sizeof suffix);
    const unsigned constants[]={0x115c26d4,0x115ac9c8,0x115c26ac};
    const float values[]={3.0f,1.0f,1.5f};memcpy(bytes+256,values,sizeof values);
    for(unsigned offset=sizeof prefix;offset+4<=sizeof prefix+native.size();++offset){
        unsigned address=0;memcpy(&address,bytes+offset,4);
        for(unsigned c=0;c<3;++c)if(address==constants[c]){
            auto replacement=reinterpret_cast<uintptr_t>(bytes+256+c*4);memcpy(bytes+offset,&replacement,4);}}
    memcpy(bytes+320,height.data(),height.size());
    int32_t delta=int32_t(reinterpret_cast<uintptr_t>(&synthetic_heightfield)-reinterpret_cast<uintptr_t>(bytes+320+18+5));
    require(bytes[320+18]==0xe8,"Ground query call boundary");memcpy(bytes+320+19,&delta,4);
    unsigned mask[4]={0x80000000,0,0,0};memcpy(bytes+512,mask,sizeof mask);
    auto signmask=reinterpret_cast<uintptr_t>(bytes+512);memcpy(bytes+320+66,&signmask,4);
    require(height[63]==0x0f&&height[64]==0x57&&height[65]==0x0d,"Plane sign-mask boundary");
    DWORD old=0;require(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&old)!=0,"Fixture protection");FlushInstructionCache(GetCurrentProcess(),memory,4096);
    std::array<unsigned char,0x200> actor{};
    using Grade=void(__thiscall*)(void*,float);auto grade_fn=reinterpret_cast<Grade>(bytes);
    unsigned comparisons=0;
    for(float multiplier:{0.2f,0.7f,1.0f,1.2f})for(int i=-2000;i<=3000;++i){
        float grade=float(i)/1000.0f;*reinterpret_cast<float*>(actor.data()+0x1bc)=multiplier;
        grade_fn(actor.data(),grade);
        float observed=*reinterpret_cast<float*>(actor.data()+0x1bc);
        require(std::abs(observed-native_grade_factor(grade,3.0f,1.5f)*multiplier)<0.000001f,"Actual native SSE grade multiplier differs");
        require(*reinterpret_cast<float*>(actor.data()+0x1b8)==grade,"Actual native grade store differs");++comparisons;
    }
    using Height=float(__thiscall*)(void*,const float*,void*);auto height_fn=reinterpret_cast<Height>(bytes+320);
    std::array<unsigned char,0x100> common{},plane{};float flat=7.25f;
    *reinterpret_cast<uintptr_t*>(common.data()+0xa0)=reinterpret_cast<uintptr_t>(&flat);
    *reinterpret_cast<float*>(plane.data()+0x34)=-0.3f;*reinterpret_cast<float*>(plane.data()+0x38)=1.0f;
    *reinterpret_cast<float*>(plane.data()+0x3c)=0.2f;*reinterpret_cast<float*>(plane.data()+0x40)=-2.0f;
    for(float x:{-10.0f,0.0f,10.0f})for(float z:{-5.0f,0.0f,5.0f}){
        float point[2]={x,z};require(height_fn(common.data(),point,nullptr)==flat,"Native context/heightfield call ABI");
        require(std::abs(height_fn(common.data(),point,plane.data())-(0.3f*x-0.2f*z+2.0f))<.000001f,"Native bridge/building plane ABI");}
    require(native_grade_probe(0,0,.1f,.5f,.01f)==.01f&&native_grade_probe(5,3,.1f,.5f,.01f)==.5f,"Native probe bounds");
    VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: "<<comparisons<<" comparisons with actual native slope SSE bytes, native x87 ground query ABI and plane/heightfield routing; game untouched.\n";return 0;
}catch(const std::exception& e){if(memory)VirtualFree(memory,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
