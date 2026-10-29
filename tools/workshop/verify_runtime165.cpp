#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#include "../../native/work/autoload99-preview/integrated.h"
#include "pack165.h"
#include "../../native/work/autoload99-preview/workshop_assets165.h"
int wmain(int argc,wchar_t** argv){try{
    require(argc==5,"Use fixture <owned-game> <new-output> <owned-toy> <reference-assets>");
    std::filesystem::path game=argv[1],out=argv[2];workshop165::plain(out);
    require(!std::filesystem::exists(out),"Fixture output exists");std::filesystem::create_directories(out);
    auto root=workshop165::original_root(game/L"data/data.pack");
    require(root.size()==13739&&assets159::sha(root.data(),root.size())==workshop165::root_sha,"Owned script recovery differs");
    auto cached=out/L"root-cache.luac";workshop165::create_exact(cached,root);workshop165::create_exact(cached,root);
    {std::ofstream file(cached,std::ios::binary);file<<"foreign";}
    bool refused=false;try{workshop165::create_exact(cached,root);}catch(...){refused=true;}
    require(refused&&std::filesystem::file_size(cached)==7,"Foreign original-script cache overwritten");
    auto sparse=out/L"sparse-owned-pack.bin";
    Handle sparse_file(CreateFileW(sparse.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));
    require(sparse_file.h!=INVALID_HANDLE_VALUE,"Cannot create sparse fixture");DWORD written=0;
    require(DeviceIoControl(sparse_file.h,FSCTL_SET_SPARSE,nullptr,0,nullptr,0,&written,nullptr)!=0,"Sparse fixture requires supported local filesystem");
    std::vector<unsigned char> index;
    auto add=[&](unsigned n,const char* name){auto p=(unsigned char*)&n;index.insert(index.end(),p,p+4);index.insert(index.end(),name,name+strlen(name)+1);};
    add(0xfffffff0u,"padding");add(unsigned(root.size()),"ui\\frontend ui\\layout_scripts\\root.luac");
    unsigned header[8]={0x33484650,3,0,0,2,unsigned(index.size()),1,0};
    require(WriteFile(sparse_file.h,header,sizeof header,&written,nullptr)&&written==sizeof header,"Cannot write sparse header");
    require(WriteFile(sparse_file.h,index.data(),DWORD(index.size()),&written,nullptr)&&written==index.size(),"Cannot write sparse index");
    LARGE_INTEGER location{};location.QuadPart=32ull+index.size()+0xfffffff0ull;
    require(location.QuadPart>0x100000000ull&&SetFilePointerEx(sparse_file.h,location,nullptr,FILE_BEGIN),"Sparse offset is not above4GiB");
    require(WriteFile(sparse_file.h,root.data(),DWORD(root.size()),&written,nullptr)&&written==root.size(),"Cannot write sparse root");
    CloseHandle(sparse_file.h);sparse_file.h=nullptr;
    require(workshop165::original_root(sparse)==root,"64-bit pack offsets lost precision");
    require(sparse.parent_path()==out&&DeleteFileW(sparse.c_str())!=0,"Cannot retire exact owned sparse fixture");
    {std::ofstream bad(out/L"truncated.pack",std::ios::binary);bad<<"PFH";}
    refused=false;try{workshop165::original_root(out/L"truncated.pack");}catch(...){refused=true;}require(refused,"Truncated pack accepted");
    runtime_folder=(out/L"runtime cache with spaces").wstring();std::filesystem::create_directory(runtime_folder);
    workshop_assets165(GetModuleHandleW(nullptr),game.wstring());
    for(const wchar_t* file:{L"hud-assets159.s2fp",L"qte-assets159.s2fp"}){
        auto reference=std::filesystem::path(argv[4])/file;
        if(std::filesystem::exists(reference))require(hash_file(reference.wstring())==hash_file(runtime_folder+L"\\"+file),"Automatic local resources differ from retained reference");
    }
    size_t files=std::distance(std::filesystem::directory_iterator(runtime_folder),std::filesystem::directory_iterator{});
    workshop_assets165(GetModuleHandleW(nullptr),game.wstring());
    require(files==size_t(std::distance(std::filesystem::directory_iterator(runtime_folder),std::filesystem::directory_iterator{})),"Cached resources launched another extractor");
    auto first_runtime=runtime_folder;
    runtime_folder=(out/L"second runtime cache").wstring();std::filesystem::create_directory(runtime_folder);
    workshop_assets165(GetModuleHandleW(nullptr),game.wstring());
    for(const wchar_t* file:{L"hud-assets159.s2fp",L"qte-assets159.s2fp"}){
        Handle first(CreateFileW((first_runtime+L"\\"+file).c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr));
        Handle second(CreateFileW((runtime_folder+L"\\"+file).c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr));
        BY_HANDLE_FILE_INFORMATION a{},b{};
        require(first.h!=INVALID_HANDLE_VALUE&&second.h!=INVALID_HANDLE_VALUE&&GetFileInformationByHandle(first.h,&a)&&GetFileInformationByHandle(second.h,&b),"Resource link identity unavailable");
        require(a.dwVolumeSerialNumber==b.dwVolumeSerialNumber&&a.nFileIndexHigh==b.nFileIndexHigh&&a.nFileIndexLow==b.nFileIndexLow,"Same-volume updates duplicated resource bytes");
    }
    size_t before=std::distance(std::filesystem::directory_iterator(runtime_folder),std::filesystem::directory_iterator{});
    for(unsigned i=0;i<1000;++i)record(6,"fixture status");
    require(before==size_t(std::distance(std::filesystem::directory_iterator(runtime_folder),std::filesystem::directory_iterator{})),"Startup status accumulated logs");
    auto recovered=out/L"recovered resources.s2fp";
    require(CopyFileW((runtime_folder+L"\\qte-assets159.s2fp").c_str(),(recovered.wstring()+L".local159.pending166").c_str(),TRUE)!=0,"Cannot create interrupted resource fixture");
    cache_local159(runtime_folder+L"\\qte-assets159.s2fp",recovered.wstring(),2,true);
    require(std::filesystem::exists(recovered)&&!std::filesystem::exists(recovered.wstring()+L".local159.pending166"),"Interrupted resource link not reused");
    auto copied=out/L"fallback copied resources.s2fp";
    cache_local159(runtime_folder+L"\\qte-assets159.s2fp",copied.wstring(),2,false);
    require(hash_file(copied.wstring())==hash_file(runtime_folder+L"\\qte-assets159.s2fp"),"Copy fallback changed resources");
    auto shared=(std::filesystem::path(runtime_folder).parent_path()/(L"resources159-locale167-"+std::wstring(assets159::recipe,assets159::recipe+64))).wstring();
    auto foreign_lock=shared+L"\\prepare166.lock";{std::ofstream foreign(foreign_lock);foreign<<"foreign";}
    auto second_runtime=runtime_folder;runtime_folder=(out/L"blocked preparation fixture").wstring();std::filesystem::create_directory(runtime_folder);
    refused=false;try{workshop_assets165(GetModuleHandleW(nullptr),game.wstring());}catch(...){refused=true;}
    require(refused&&std::filesystem::file_size(foreign_lock)==7&&std::filesystem::is_empty(runtime_folder),"Foreign preparation lock modified or failure accumulated files");
    require(DeleteFileW(foreign_lock.c_str())!=0,"Cannot retire own lock fixture");runtime_folder=second_runtime;
    auto stale=(out/L"stale generated resources").wstring();std::filesystem::create_directory(stale);
    require(CopyFileW((runtime_folder+L"\\qte-assets159.s2fp").c_str(),(stale+L"\\qte-assets159.s2fp").c_str(),TRUE)!=0,"Cannot create owned stale resource fixture");
    require(retire_staging166(stale)&&!std::filesystem::exists(stale),"Known stale generated resources not retired");
    std::filesystem::create_directory(stale);{std::ofstream foreign(stale+L"\\hud-assets159.s2fp");foreign<<"foreign";}
    require(!retire_staging166(stale)&&std::filesystem::file_size(stale+L"\\hud-assets159.s2fp")==7,"Foreign staging deleted");
    auto hud=runtime_folder+L"\\hud-assets159.s2fp";
    require(DeleteFileW(hud.c_str())!=0,"Cannot replace own fixture link");
    {std::ofstream foreign(hud,std::ios::binary);foreign<<"foreign";}
    refused=false;try{workshop_assets165(GetModuleHandleW(nullptr),game.wstring());}catch(...){refused=true;}
    require(refused&&std::filesystem::file_size(hud)==7,"Foreign resources overwritten");
    auto toy=std::filesystem::path(argv[3]).wstring(),log=(out/L"owned timeout.log").wstring();ULONGLONG start=GetTickCount64();
    refused=false;try{owned_extractor165(toy,quoted165(toy),out.wstring(),log,250);}catch(...){refused=true;}
    require(refused&&GetTickCount64()-start<5000,"Owned helper timeout was not bounded");
    std::ifstream log_file(log);std::string line;std::getline(log_file,line);require(line.find("owned-pid=")==0,"Owned helper output not captured");
    DWORD pid=std::stoul(line.substr(10));Handle child(OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid));
    if(child.h){require(WaitForSingleObject(child.h,3000)==WAIT_OBJECT_0,"Owned timed-out child survived job close");}
    auto clone=out/L"owned incomplete game";std::filesystem::create_directory(clone);
    require(CopyFileW((game/L"Empire.Retail.dll").c_str(),(clone/L"Empire.Retail.dll").c_str(),TRUE)!=0,"Cannot create read-only engine fixture");
    auto failed=out/L"failed extraction";
    auto helper=(std::filesystem::path(argv[4])/L"prepare-local-assets159.exe").wstring();
    refused=false;try{owned_extractor165(helper,quoted165(helper)+L" "+quoted165(clone.wstring())+L" "+quoted165(failed.wstring()),out.wstring(),L"",30000);}catch(...){refused=true;}
    require(refused&&!std::filesystem::exists(failed),"Failed owned extraction accumulated a staging folder");
    std::cout<<"PASS owned root/hash/foreign cache; sparse64-bit offsets; malformed input; real automatic resource extraction/repeat/reference/foreign preservation; owned child log/timeout/lifetime.\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
