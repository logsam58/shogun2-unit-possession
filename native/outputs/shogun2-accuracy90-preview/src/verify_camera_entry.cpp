#include "identity.h"
#include "camera_entry_gate.h"
using ToggleFixture=void(__thiscall*)(void*,unsigned);
static unsigned enters,exits,refreshes;
static uintptr_t empty_camera;
static void __fastcall create_fixture(void* manager,void*){
    ++enters;auto p=static_cast<unsigned char*>(manager);
    *reinterpret_cast<uintptr_t*>(p+0x8e4)=44;*reinterpret_cast<uintptr_t*>(p+0x8e0)=44;
}
static uintptr_t* __fastcall refresh_fixture(void*,void*){++refreshes;return &empty_camera;}
static std::vector<unsigned char> native_code(const std::vector<unsigned char>& image,unsigned rva,unsigned count){
    auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(image.data());
    auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
    auto sections=IMAGE_FIRST_SECTION(nt);
    for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];
        if(rva>=s.VirtualAddress&&rva+count<=s.VirtualAddress+s.SizeOfRawData){
            unsigned off=s.PointerToRawData+rva-s.VirtualAddress;require(off+count<=image.size(),"Camera fixture bounds");
            return {image.begin()+off,image.begin()+off+count};}}
    throw std::runtime_error("Camera fixture section absent");
}
static void native_toggle_fixture(){
    std::wstring path=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Total War SHOGUN 2\\empire.retail.dll";
    require(hash_file(path)==EXPECTED_HASH,"Camera fixture requires exact native build");
    std::ifstream stream(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(stream)),{});
    auto toggle=native_code(image,0x319dd0,104),restore=native_code(image,0x2d5fc0,13);
    require(toggle[47]==0xe8&&toggle[70]==0xe8&&toggle[81]==0xe8&&toggle[101]==0xc2&&
        restore.back()==0xc3,"Native toggle boundaries/calls");
    void* memory=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    require(memory!=nullptr,"Camera fixture allocation");
    try{
        auto code=static_cast<unsigned char*>(memory);memcpy(code,toggle.data(),toggle.size());memcpy(code+128,restore.data(),restore.size());
        auto relocate=[&](unsigned off,uintptr_t target){int32_t delta=int32_t(target-reinterpret_cast<uintptr_t>(code+off+5));memcpy(code+off+1,&delta,4);};
        relocate(47,reinterpret_cast<uintptr_t>(&create_fixture));relocate(70,reinterpret_cast<uintptr_t>(code+128));
        relocate(81,reinterpret_cast<uintptr_t>(&refresh_fixture));
        DWORD prior=0;require(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&prior)!=0,"Camera fixture protection");
        FlushInstructionCache(GetCurrentProcess(),memory,4096);
        std::array<unsigned char,0x910> manager{};
        auto word=[&](unsigned off)->uintptr_t&{return *reinterpret_cast<uintptr_t*>(manager.data()+off);};
        word(0x8cc)=11;word(0x8e0)=11;word(0x8e4)=22;word(0x8f8)=33;word(0x8fc)=55;
        auto call=reinterpret_cast<ToggleFixture>(memory);
        for(unsigned i=0;i<50000;++i){
            call(manager.data(),1);require(word(0x8e0)==44&&word(0x8fc)==0,"Native entry clears actor and selects constructed camera");
            call(manager.data(),1);require(enters==i+1,"Already context camera avoids another construction");
            call(manager.data(),0);require(word(0x8e0)==11,"Native release returns to classic +8cc camera");++exits;
        }
        unsigned before=enters;word(0x8f8)=0;call(manager.data(),1);
        require(enters==before&&word(0x8e0)==11,"No selected view prevents entry");
        require(refreshes==150001&&exits==50000,"Native refresh always called, stack intact across repeated entry/exit");
    }catch(...){VirtualFree(memory,0,MEM_RELEASE);throw;}
    VirtualFree(memory,0,MEM_RELEASE);
}
int main(){
    try {
        require(camera_entry_thread(31,31,31,0),"Matching window/camera thread enters outside callbacks");
        require(!camera_entry_thread(0,0,0,0),"Unknown threads reject");
        require(!camera_entry_thread(32,31,31,0),"Foreign execution thread rejects");
        require(!camera_entry_thread(31,32,31,0),"Different window owner rejects");
        require(!camera_entry_thread(31,31,32,0),"Different camera thread rejects");
        require(!camera_entry_thread(31,31,31,1),"Nested message in a native callback rejects");
        require(!camera_entry_thread(31,31,0,0),"No native camera thread observed rejects");
        require(!camera_entry_thread(31,31,31,2),"Current camera/input callback depth rejects");
        require(camera_entry_live(true,false,false,500,1),"Armed fresh request accepts");
        require(!camera_entry_live(true,false,false,501,1),"Stale heartbeat rejects");
        require(!camera_entry_live(false,false,false,0,1),"Disabled adapter rejects");
        require(!camera_entry_live(true,true,false,0,1),"Shutdown rejects");
        require(!camera_entry_live(true,false,true,0,1),"Release wins over entry");
        for(long mode:{0L,2L,3L})require(!camera_entry_live(true,false,false,0,mode),"Only armed request can enter");
        require(camera_entry_owner(31,31,0)==31,"Native matrix owner establishes entry");
        require(camera_entry_owner(31,0,31)==31,"Native input owner establishes entry before matrix evaluation");
        require(camera_entry_owner(31,31,31)==31,"Matching camera observations accept");
        require(camera_entry_owner(31,0,0)==0,"No camera observation waits");
        require(camera_entry_owner(31,32,31)==0,"Conflicting matrix owner rejects");
        require(camera_entry_owner(31,31,32)==0,"Conflicting input owner rejects");
        require(camera_entry_thread(31,31,camera_entry_owner(31,31,0),0),"Camera owner accepts independently of simulation worker");
        native_toggle_fixture();
        std::cout<<"PASS: camera entry guards and 150001 calls through exact native x86 toggle/return bytes with fixture constructor/refresh, selected-view guard, already-active guard and classic camera restoration. Real constructor lifecycle still requires live verification.\n";
        return 0;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
