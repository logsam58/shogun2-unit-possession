#pragma once
// Called only by the Workshop worker. The existing extractor is a hidden child;
// it reads owned packs, writes a user cache and never changes the game/Steam.
#include <filesystem>
static std::wstring quoted165(const std::wstring& value){
    std::wstring out=L"\"";unsigned slashes=0;
    for(wchar_t c:value){if(c==L'\\'){++slashes;continue;}
        if(c==L'\"')out.append(slashes*2+1,L'\\');else out.append(slashes,L'\\');slashes=0;out+=c;}
    out.append(slashes*2,L'\\');return out+L'\"';
}
static void owned_extractor165(const std::wstring& helper,std::wstring command,const std::wstring& cwd,const std::wstring& logpath,DWORD timeout){
    workshop165::plain(helper);workshop165::plain(cwd);if(!logpath.empty())workshop165::plain(logpath);
    SECURITY_ATTRIBUTES sa{sizeof sa,nullptr,TRUE};
    // Only isolated developer fixtures supply a log path. Player extraction
    // discards output; repeated starts cannot accumulate helper transcripts.
    Handle log(CreateFileW(logpath.empty()?L"NUL":logpath.c_str(),GENERIC_WRITE,
        logpath.empty()?FILE_SHARE_READ|FILE_SHARE_WRITE:FILE_SHARE_READ,&sa,
        logpath.empty()?OPEN_EXISTING:CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));
    require(log.h!=INVALID_HANDLE_VALUE,"Cannot create owned extractor log");
    Handle input(CreateFileW(L"NUL",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,&sa,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
    require(input.h!=INVALID_HANDLE_VALUE,"Cannot create owned extractor input");
    SIZE_T bytes=0;InitializeProcThreadAttributeList(nullptr,1,0,&bytes);std::vector<unsigned char> storage(bytes);
    auto attributes=reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(storage.data());
    require(InitializeProcThreadAttributeList(attributes,1,0,&bytes)!=0,"Cannot prepare owned handle list");
    struct Attributes {LPPROC_THREAD_ATTRIBUTE_LIST value;~Attributes(){DeleteProcThreadAttributeList(value);}} cleanup{attributes};
    HANDLE inherited[]={log.h,input.h};
    require(UpdateProcThreadAttribute(attributes,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,inherited,sizeof inherited,nullptr,nullptr)!=0,"Cannot restrict helper handle inheritance");
    STARTUPINFOEXW si{};si.StartupInfo.cb=sizeof si;si.StartupInfo.dwFlags=STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
    si.StartupInfo.wShowWindow=SW_HIDE;si.StartupInfo.hStdOutput=log.h;si.StartupInfo.hStdError=log.h;si.StartupInfo.hStdInput=input.h;si.lpAttributeList=attributes;
    Handle job(CreateJobObjectW(nullptr,nullptr));require(job.h!=nullptr,"Cannot create owned extractor job");
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    require(SetInformationJobObject(job.h,JobObjectExtendedLimitInformation,&limits,sizeof limits)!=0,"Cannot bound owned extractor lifetime");
    PROCESS_INFORMATION pi{};
    require(CreateProcessW(helper.c_str(),&command[0],nullptr,nullptr,TRUE,CREATE_NO_WINDOW|CREATE_SUSPENDED|EXTENDED_STARTUPINFO_PRESENT,nullptr,cwd.c_str(),&si.StartupInfo,&pi)!=0,"Cannot start owned local resource extractor");
    Handle process(pi.hProcess),thread(pi.hThread);
    if(!AssignProcessToJobObject(job.h,process.h)){
        TerminateProcess(process.h,165);WaitForSingleObject(process.h,5000);require(false,"Cannot assign owned extractor job");
    }
    require(ResumeThread(thread.h)!=DWORD(-1),"Cannot resume owned extractor");
    require(WaitForSingleObject(process.h,timeout)==WAIT_OBJECT_0,"Owned extractor timed out; its job will close");
    DWORD exit=1;require(GetExitCodeProcess(process.h,&exit)&&exit==0,"Local resource preparation failed");
}
static bool retire_staging166(const std::wstring& staging){
    // Recover only exact generated resource data. Unknown, partial or changed
    // files are preserved. A fixed staging name bounds even failed attempts.
    workshop165::plain(staging);
    if(GetFileAttributesW(staging.c_str())==INVALID_FILE_ATTRIBUTES)return true;
    std::vector<std::filesystem::path> files;
    for(const auto& item:std::filesystem::directory_iterator(staging)){
        workshop165::plain(item.path());if(!item.is_regular_file())return false;
        auto name=item.path().filename().wstring();assets159::Store store;
        if(name==L"hud-assets159.s2fp"||name==L"qte-assets159.s2fp"){
            if(!store.load(item.path().wstring(),name[0]==L'h'?1:2))return false;
        }else if(name==L"advisor-source.tmp.mp3"){
            if(std::filesystem::file_size(item.path())!=57992||hash_file(item.path().wstring())!="6b672c729f8f8b5e7667a4cfddbaecabcf1a7e620b24dd06ad4d77a9afe29836")return false;
        }else return false;
        files.push_back(item.path());
    }
    for(const auto& file:files)if(!DeleteFileW(file.c_str()))return false;
    return RemoveDirectoryW(staging.c_str())!=0;
}
static void workshop_assets165(HMODULE module,const std::wstring& game){
    workshop165::plain(runtime_folder);
    auto valid=[](const std::wstring& folder){
        workshop165::plain(folder+L"\\hud-assets159.s2fp");workshop165::plain(folder+L"\\qte-assets159.s2fp");
        assets159::Store hud,qte;return hud.load(folder+L"\\hud-assets159.s2fp",1)&&qte.load(folder+L"\\qte-assets159.s2fp",2);
    };
    if(valid(runtime_folder)){record(2,"Owned local resources verified");return;}
    for(const wchar_t* name:{L"hud-assets159.s2fp",L"qte-assets159.s2fp"}){
        auto path=runtime_folder+L"\\"+name;
        if(GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES){assets159::Store store;require(store.load(path,name[0]==L'h'?1:2),"Existing local cache is foreign/corrupt; preserved");}
    }
    auto shared=(std::filesystem::path(runtime_folder).parent_path()/(L"resources159-"+std::wstring(assets159::recipe,assets159::recipe+64))).wstring();
    workshop165::plain(shared);
    require(CreateDirectoryW(shared.c_str(),nullptr)||GetLastError()==ERROR_ALREADY_EXISTS,"Shared local resources unavailable");
    // Exclusive, crash-safe preparation lock. Its only file is removed by the OS.
    Handle lock(CreateFileW((shared+L"\\prepare166.lock").c_str(),GENERIC_WRITE|DELETE,FILE_SHARE_READ|FILE_SHARE_DELETE,
        nullptr,CREATE_NEW,FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,nullptr));
    require(lock.h!=INVALID_HANDLE_VALUE,"Another resource preparation is active; restart after it finishes");
    if(!valid(shared)){
        for(const wchar_t* name:{L"hud-assets159.s2fp",L"qte-assets159.s2fp"}){
            auto path=shared+L"\\"+name;assets159::Store store;
            if(GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES)
                require(store.load(path,name[0]==L'h'?1:2),"Changed shared resources preserved");
        }
        auto staging=shared+L"\\resources-pending166";
        require(retire_staging166(staging),"Unknown or incomplete resource staging preserved; no new copies created");
        auto helper=extract(module,104,L"prepare-local-assets159.exe",EMBEDDED_PREPARER_SHA);
        record(2,"Preparing artwork, fonts, cards, ability icons and voice from this owned game");
        std::wstring command=quoted165(helper)+L" "+quoted165(game)+L" "+quoted165(staging);
        try{
            owned_extractor165(helper,command,runtime_folder,L"",120000);
            require(valid(staging),"Prepared local resources failed validation");
            for(const wchar_t* name:{L"hud-assets159.s2fp",L"qte-assets159.s2fp"}){
                auto from=staging+L"\\"+name,to=shared+L"\\"+name;
                if(GetFileAttributesW(to.c_str())!=INVALID_FILE_ATTRIBUTES){require(hash_file(from)==hash_file(to),"Existing shared resource cache differs; preserved");require(DeleteFileW(from.c_str())!=0,"Cannot retire owned staging duplicate");}
                else require(MoveFileExW(from.c_str(),to.c_str(),MOVEFILE_WRITE_THROUGH)!=0,"Cannot commit shared resources; target preserved");
            }
            require(retire_staging166(staging)&&valid(shared),"Shared resource commit failed");
        }catch(...){retire_staging166(staging);throw;}
    }
    for(unsigned kind:{1u,2u}){
        const wchar_t* name=kind==1?L"hud-assets159.s2fp":L"qte-assets159.s2fp";
        cache_local159(shared+L"\\"+name,runtime_folder+L"\\"+name,kind,true);
    }
    require(valid(runtime_folder),"Local resource cache commit failed");
    record(2,"Owned local resources ready; no resources were downloaded");
}
