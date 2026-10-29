// Optional, offline compatibility fixture. Include the separately obtained MIT
// author source; never bundle it or run its DllMain/Install/gameplay hook bodies.
#include "../../native/outputs/shogun2-accuracy90-preview/src/adapter.cpp"
#include <dbghelp.h>
#include <shlobj.h>
#include <stdarg.h>
#include <wchar.h>
namespace other168 {
#define DllMain ExternalDllMain168
#include S2UP_EXTERNAL_LOADER_SOURCE168
#undef DllMain
}
#include S2UP_RVA_CATALOG168
static engine172::Result binding172;
static void signatures168(){
    require(accuracy_signatures90(),"Patched image changed accuracy/projection signatures");
    require(native_bow_signatures(base),"Patched image changed bow signatures");
    require(native_movement_signatures(base),"Patched image changed movement signatures");
    require(native_speed_modifier_signatures(base),"Patched image changed speed signatures");
    require(native_terrain_signatures(base),"Patched image changed terrain signatures");
    require(equipment_signatures81(base),"Patched image changed equipment signatures");
    require(command_native_signatures100(base),"Patched image changed command signatures");
    require(native_order_signatures(base),"Patched image changed order signatures");
    engine172::live_hooks(base,1,binding172);engine172::live_hooks(base,2,binding172);engine172::live_hooks(base,3,binding172);
    require(!memcmp((void*)(base+0x367600),signature,sizeof signature),"Motion hook is occupied");
}
int wmain(int argc,wchar_t** argv){try{
    require(argc==2&&engine170::supported(hash_file(argv[1])),"Use an exact reviewed owned engine image");
    binding172=engine172::admit(argv[1],hash_file(argv[1]));
    // DONT_RESOLVE_DLL_REFERENCES maps/relocates an owned image without running
    // its entry point or resolving game dependencies. No game functions execute.
    HMODULE image=LoadLibraryExW(argv[1],nullptr,DONT_RESOLVE_DLL_REFERENCES);
    require(image!=nullptr,"Cannot map isolated engine image");base=(uintptr_t)image;
    auto before=std::vector<unsigned char>((unsigned char*)image,(unsigned char*)image+0x1d88000);
    signatures168();
    other168::g_base=(BYTE*)image;other168::g_log=nullptr;other168::g_dir.clear();
    other168::g_frameLog=other168::g_memoryLog=other168::g_crashDumps=false;
    other168::InstallThreads();other168::InstallPackCache();other168::SkipIntroMovies();
    other168::HookFrames();other168::HookTextures();other168::InstallLoadingVsync();
    unsigned patch_rvas[]={other168::kPoolSetupCall,other168::kPinCall,other168::kPackReopenCall,
        other168::kFileOpenCall,other168::kIntroMoviesJump,other168::kFrameSlot,
        other168::kCreateTextureCall,other168::kPresentSync};
    for(auto rva:patch_rvas)require(memcmp(before.data()+rva,(void*)(base+rva),rva==other168::kFrameSlot?4:6)!=0,"An author patch did not apply in the fixture");
    signatures168();
    for(unsigned rva:source_rvas168)require(!memcmp(before.data()+rva,(void*)(base+rva),16),"Loading mod overlaps a source RVA window at "+hex(rva));
    const unsigned char lua[]={0x8b,0x4c,0x24,0x04,0x8b,0x41,0x08,0x2b,0x41,0x0c,0xc1,0xf8,0x03,0xc3};
    require(!memcmp((void*)(base+0xea2d60),lua,sizeof lua),"Loading mod changed the actual Lua API");
    // Negative control: this is an actual conflicting code edit, never allowed
    // to pass simply because the DLL on disk has the supported fingerprint.
    BYTE conflict=0xcc;require(other168::WriteCode((BYTE*)image+0x367600,&conflict,1),"Cannot stage negative fixture patch");
    bool rejected=false;try{signatures168();}catch(...){rejected=true;}
    require(rejected,"Actual occupied hook was accepted");
    require(other168::WriteCode((BYTE*)image+0x367600,before.data()+0x367600,1),"Cannot restore owned negative fixture patch");
    signatures168();
    // Exercise the real F9 request gate with a synthetic status, not a battle.
    Status owned{};state=&owned;enabled=1;
    startup168::pending();request_possession();require(!owned.acquire,"Partial startup accepted F9");
    startup168::failed();request_possession();require(!owned.acquire,"Failed startup accepted F9");
    startup168::ready();request_possession();require(owned.acquire==1&&owned.mode==ARMED,"Complete startup refused F9");
    enabled=0;state=nullptr;SetEnvironmentVariableW(startup168::key,nullptr);
    std::cout<<"PASS reviewed author patch routines at8 sites; production adapter signatures and18 shared hook guards; "<<std::size(source_rvas168)<<" source RVA windows unchanged; actual conflicting hook rejected; real F9 startup barrier. No engine entry/gameplay, affinity change, crash handler, statistic thread or files from the other mod.\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
