#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#include <chrono>
#include "../../native/work/autoload99-preview/integrated.h"
#include "pack165.h"
#include "../../native/work/autoload99-preview/workshop_assets165.h"
int wmain(int argc,wchar_t** argv){try{
    if(argc==4&&!wcscmp(argv[1],L"--verify-localization")){
        const auto resource_key=workshop_resource_key165(argv[2]);
        const auto runtime=std::filesystem::path(argv[3])/resource_key;
        workshop165::plain(runtime);
        std::filesystem::create_directories(runtime);
        runtime_folder=runtime.wstring();
        workshop_assets165(GetModuleHandleW(nullptr),argv[2],resource_key);
        std::wcout<<resource_key<<L'\n';
        return 0;
    }
    require(argc==5,"Use fixture <owned-game> <new-output> <owned-toy> <reference-assets>");
    std::filesystem::path game=argv[1],out=argv[2];workshop165::plain(out);
    require(!std::filesystem::exists(out),"Fixture output exists");std::filesystem::create_directories(out);
    const auto resource_key=workshop_resource_key165(game.wstring());
    const auto test_game=out/L"locale-fixture";
    std::filesystem::create_directories(test_game/L"data");
    const auto empty_key=workshop_resource_key165(test_game.wstring());
    auto local=test_game/L"data/local_fr.pack";
    {std::ofstream file(local,std::ios::binary);file<<"French fixture";}
    const auto french_key=workshop_resource_key165(test_game.wstring());
    require(french_key!=empty_key&&french_key!=resource_key,"Localization change reused the cache identity");
    auto write_time=std::filesystem::last_write_time(local);
    std::filesystem::last_write_time(local,write_time-std::chrono::seconds(2));
    require(workshop_resource_key165(test_game.wstring())!=french_key,"Localization file replacement metadata ignored");
    local=test_game/L"data/local_en.pack";
    {std::ofstream file(local,std::ios::binary);file<<"English fixture";}
    const auto english_key=workshop_resource_key165(test_game.wstring());
    require(english_key!=french_key,"English preference did not change the cache identity");
    {std::ofstream file(test_game/L"data/local_fr.pack",std::ios::binary|std::ios::app);file<<" changed";}
    const auto changed_fallback=workshop_resource_key165(test_game.wstring());
    require(changed_fallback!=english_key,"Fallback pack changes must invalidate potentially mixed resources");
    {std::ofstream file(test_game/L"data/language.txt",std::ios::binary);file<<"fr";}
    require(workshop_resource_key165(test_game.wstring())!=changed_fallback,"Active language change reused the cache");
    const auto pack_order=locale167::installed(test_game/L"data");
    require(!pack_order.packs.empty()&&pack_order.packs.front().filename()==L"local_fr.pack",
            "Active language did not take precedence");
    std::cout<<"PASS selected language and available fallback changes invalidate the matching resource cache.\n";
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
    runtime_folder=(out/L"component cache").wstring();std::filesystem::create_directory(runtime_folder);
    auto module=GetModuleHandleW(nullptr);
    auto adapter=runtime_folder+L"\\possession-v46.dll";
    auto adapter_stage=adapter+L".component.pending";
    auto reference_adapter=std::filesystem::path(argv[4])/L"possession-v46.dll";
    require(CopyFileW(reference_adapter.c_str(),adapter_stage.c_str(),TRUE)!=0,"Cannot seed completed component stage");
    require(extract(module,101,L"possession-v46.dll",EMBEDDED_ADAPTER_SHA)==adapter&&
            hash_file(adapter)==EMBEDDED_ADAPTER_SHA&&!std::filesystem::exists(adapter_stage),
            "Complete component staging not consumed");
    require(extract(module,101,L"possession-v46.dll",EMBEDDED_ADAPTER_SHA)==adapter,"Exact component reuse failed");
    auto qte=runtime_folder+L"\\qte-playable-v79.dll";
    auto qte_stage=qte+L".component.pending";
    {std::ofstream foreign(qte_stage,std::ios::binary);foreign<<"foreign";}
    const auto foreign_sha=hash_file(qte_stage);
    refused=false;try{extract(module,103,L"qte-playable-v79.dll",EMBEDDED_QTE_SHA);}catch(...){refused=true;}
    require(refused&&!std::filesystem::exists(qte)&&hash_file(qte_stage)==foreign_sha,"Unknown component staging modified");
    {std::ofstream foreign(qte,std::ios::binary);foreign<<"another component";}
    const auto foreign_qte=hash_file(qte);
    refused=false;try{extract(module,103,L"qte-playable-v79.dll",EMBEDDED_QTE_SHA);}catch(...){refused=true;}
    require(refused&&hash_file(qte)==foreign_qte&&hash_file(qte_stage)==foreign_sha,"Foreign component cache overwritten");
    auto hud_component=runtime_folder+L"\\possession-hud.exe";
    require(extract(module,102,L"possession-hud.exe",EMBEDDED_HUD_SHA)==hud_component&&
            hash_file(hud_component)==EMBEDDED_HUD_SHA&&
            !std::filesystem::exists(hud_component+L".component.pending"),"Fresh component publication failed");
    std::cout<<"PASS component cache fresh publication, exact reuse and recovery, and foreign file preservation.\n";
    runtime_folder=(out/L"runtime cache with spaces").wstring();std::filesystem::create_directory(runtime_folder);
    workshop_assets165(GetModuleHandleW(nullptr),game.wstring(),resource_key);
    for(const wchar_t* file:{L"hud-assets159.s2fp",L"qte-assets159.s2fp"}){
        auto reference=std::filesystem::path(argv[4])/file;
        if(std::filesystem::exists(reference))require(hash_file(reference.wstring())==hash_file(runtime_folder+L"\\"+file),"Automatic local resources differ from retained reference");
    }
    size_t files=std::distance(std::filesystem::directory_iterator(runtime_folder),std::filesystem::directory_iterator{});
    workshop_assets165(GetModuleHandleW(nullptr),game.wstring(),resource_key);
    require(files==size_t(std::distance(std::filesystem::directory_iterator(runtime_folder),std::filesystem::directory_iterator{})),"Cached resources launched another extractor");
    auto first_runtime=runtime_folder;
    runtime_folder=(out/L"second runtime cache").wstring();std::filesystem::create_directory(runtime_folder);
    workshop_assets165(GetModuleHandleW(nullptr),game.wstring(),resource_key);
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
    const auto source_qte=runtime_folder+L"\\qte-assets159.s2fp";
    const auto rejected_target=(out/L"resource conflict.s2fp").wstring();
    const auto rejected_stage=rejected_target+L".local159.pending166";
    {std::ofstream foreign(rejected_stage,std::ios::binary);foreign<<"foreign resource stage";}
    const auto stage_hash=hash_file(rejected_stage);
    refused=false;
    try{cache_local159(source_qte,rejected_target,2,true);}catch(const std::exception& failure){
        refused=std::string(failure.what()).find("Staged local resources differ")!=std::string::npos;
    }
    require(refused&&!std::filesystem::exists(rejected_target)&&hash_file(rejected_stage)==stage_hash,
            "Foreign resource stage replaced or lost");
    {std::ofstream foreign(rejected_target,std::ios::binary);foreign<<"foreign runtime resource";}
    const auto target_hash=hash_file(rejected_target);
    refused=false;
    try{cache_local159(source_qte,rejected_target,2,true);}catch(const std::exception& failure){
        refused=std::string(failure.what()).find("Local resource cache differs")!=std::string::npos;
    }
    require(refused&&hash_file(rejected_target)==target_hash&&hash_file(rejected_stage)==stage_hash,
            "Foreign resource cache replaced or lost");
    std::cout<<"PASS shared resource links, equal copies and preservation of foreign staged and final resource files.\n";
    auto shared=workshop_shared_folder165(runtime_folder,resource_key);
    auto foreign_lock=shared+L"\\prepare166.lock";{std::ofstream foreign(foreign_lock);foreign<<"foreign";}
    auto second_runtime=runtime_folder;runtime_folder=(out/L"blocked preparation fixture").wstring();std::filesystem::create_directory(runtime_folder);
    workshop_assets165(module,(out/L"no game here").wstring(),resource_key);
    require(std::filesystem::file_size(foreign_lock)==7&&
            !std::filesystem::exists(runtime_folder+L"\\prepare-local-assets159.exe"),
            "Warm contention modified the shared lock or started extraction");
    for(const auto* name:{L"hud-assets159.s2fp",L"qte-assets159.s2fp"}){
        const auto source=shared+L"\\"+name,target=runtime_folder+L"\\"+name;
        require(hash_file(source)==hash_file(target)&&!std::filesystem::equivalent(
                    std::filesystem::path(source),std::filesystem::path(target)),
                "Warm contention did not publish an exact independent copy");
    }
    require(DeleteFileW(foreign_lock.c_str())!=0,"Cannot retire own lock fixture");runtime_folder=second_runtime;
    auto stale=(out/L"stale generated resources").wstring();std::filesystem::create_directory(stale);
    require(CopyFileW((runtime_folder+L"\\qte-assets159.s2fp").c_str(),(stale+L"\\qte-assets159.s2fp").c_str(),TRUE)!=0,"Cannot create owned stale resource fixture");
    require(retire_staging166(stale)&&!std::filesystem::exists(stale),"Known stale generated resources not retired");
    std::filesystem::create_directory(stale);{std::ofstream foreign(stale+L"\\hud-assets159.s2fp");foreign<<"foreign";}
    require(!retire_staging166(stale)&&std::filesystem::file_size(stale+L"\\hud-assets159.s2fp")==7,"Foreign staging deleted");
    // All bundle bytes are already available. Recovery must work even when
    // the original game folder is unavailable and the extractor cannot run.
    const auto staged_runtime=out.parent_path()/L"r"/L"c";
    const auto staged_shared=staged_runtime.parent_path()/std::filesystem::path(shared).filename();
    const auto staged_folder=staged_shared/L"resources-pending166";
    std::filesystem::create_directories(staged_runtime);
    std::filesystem::create_directories(staged_folder);
    const auto source_hud=std::filesystem::path(first_runtime)/L"hud-assets159.s2fp";
    std::filesystem::copy_file(source_hud,staged_shared/L"hud-assets159.s2fp");
    std::filesystem::copy_file(source_qte,staged_folder/L"qte-assets159.s2fp");
    {std::ofstream foreign(staged_folder/L"unknown.bin",std::ios::binary);foreign<<"foreign";}
    runtime_folder=staged_runtime.wstring();
    auto recover_resources=[&]{workshop_assets165(module,(out/L"no game here").wstring(),resource_key);};
    const auto cold_lock=staged_shared/L"prepare166.lock";
    {std::ofstream foreign(cold_lock,std::ios::binary);foreign<<"foreign";}
    refused=false;try{recover_resources();}catch(const std::exception& failure){
        refused=std::string(failure.what()).find("Another resource preparation is active")!=std::string::npos;
    }
    require(refused&&std::filesystem::file_size(cold_lock)==7&&
            std::filesystem::exists(staged_folder/L"qte-assets159.s2fp")&&std::filesystem::is_empty(staged_runtime),
            "Cold contention changed the shared preparation state");
    require(DeleteFileW(cold_lock.c_str())!=0,"Cannot remove owned lock fixture");
    refused=false;try{recover_resources();}catch(const std::exception&){refused=true;}
    require(refused&&std::filesystem::file_size(staged_folder/L"unknown.bin")==7&&
            std::filesystem::exists(staged_folder/L"qte-assets159.s2fp")&&std::filesystem::is_empty(staged_runtime),
            "Unknown shared staging changed during recovery");
    require(DeleteFileW((staged_folder/L"unknown.bin").c_str())!=0,"Cannot remove owned foreign fixture");
    recover_resources();
    require(!std::filesystem::exists(staged_folder)&&
            !std::filesystem::exists(staged_runtime/L"prepare-local-assets159.exe")&&
            hash_file((staged_runtime/L"hud-assets159.s2fp").wstring())==hash_file(source_hud.wstring())&&
            hash_file((staged_runtime/L"qte-assets159.s2fp").wstring())==hash_file(source_qte),
            "Interrupted resource commit did not recover exact bundles without extraction");
    std::cout<<"PASS interrupted shared resource publication resumed without running extractor.\n";
    std::cout<<"PASS warm shared contention copies exact bundles; cold contention preserves staging and lock.\n";
    runtime_folder=second_runtime;
    auto hud=runtime_folder+L"\\hud-assets159.s2fp";
    require(DeleteFileW(hud.c_str())!=0,"Cannot replace own fixture link");
    {std::ofstream foreign(hud,std::ios::binary);foreign<<"foreign";}
    refused=false;try{workshop_assets165(GetModuleHandleW(nullptr),game.wstring(),resource_key);}catch(...){refused=true;}
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
