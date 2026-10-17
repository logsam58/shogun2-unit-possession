#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "playable_protocol.h"
static_assert(sizeof(void*)==4,"Build launcher x86");
static Module named_module(DWORD pid,const wchar_t* wanted) {
    Handle snap(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid));require(snap.h!=INVALID_HANDLE_VALUE,"Module snapshot failed");
    MODULEENTRY32W m{};m.dwSize=sizeof(m);require(Module32FirstW(snap.h,&m)!=0,"Module list failed");
    do{if(_wcsicmp(m.szModule,wanted)==0)return {reinterpret_cast<uintptr_t>(m.modBaseAddr),m.modBaseSize,m.szExePath};}while(Module32NextW(snap.h,&m));
    return {};
}
static void show(Status* s) {
    require(s->magic==POC_MAGIC&&s->version==POC_VERSION&&s->size==sizeof(Status),"Unknown hit probe protocol");
    uint64_t now=GetTickCount64(),age=now>=s->heartbeat?now-s->heartbeat:UINT64_MAX;
    std::cout<<"{\"pid\":"<<s->pid<<",\"ready\":"<<s->ready<<",\"mode\":"<<s->mode<<",\"error\":"<<s->error
        <<",\"heartbeat_age_ms\":"<<age<<",\"worker_responsive\":"<<(s->ready&&age<1000?1:0)
        <<",\"dropped\":"<<s->dropped<<",\"faults\":"<<s->faults<<",\"stop_reason\":"<<s->stop_reason<<",\"calls\":[";
    for(unsigned i=0;i<4;++i){if(i)std::cout<<',';std::cout<<s->calls[i];}std::cout<<"]}\n";
}
int wmain(int argc,wchar_t** argv) {
    try {
        std::wstring action=argc>1?argv[1]:L"--load";require(argc<=2,"Use --load, --status, or --stop");
        DWORD pid=find_process(0);wchar_t name[80];mapping_name(name,pid);
        if(action==L"--status"||action==L"--stop"){
            Handle h(OpenFileMappingW(FILE_MAP_READ|(action==L"--stop"?FILE_MAP_WRITE:0),FALSE,name));require(h.h!=nullptr,"Prototype is not loaded");
            auto* s=static_cast<Status*>(MapViewOfFile(h.h,FILE_MAP_READ|(action==L"--stop"?FILE_MAP_WRITE:0),0,0,sizeof(Status)));require(s!=nullptr,"Mapping failed");
            require(s->pid==pid,"Process identity changed");show(s);
            if(action==L"--stop"){InterlockedExchange(&s->stop,1);for(int i=0;i<100&&s->mode!=DISABLED&&s->error==0;++i)Sleep(50);show(s);
                require(s->error==0,"Disable/restoration failed");
                if(s->mode!=DISABLED){std::cout<<"Stop requested. A pending native pair is draining; unpause, then check --status. Hooks remain installed until stock preparation completes.\n";UnmapViewOfFile(s);return 0;}
                require(memcmp(s->original_code,s->restored_code,sizeof s->original_code)==0,"Hook restoration differs");std::cout<<"QTE hooks disabled and restoration confirmed. DLL remains inert until game exit.\n";}
            UnmapViewOfFile(s);return 0;
        }
        require(action==L"--load","Unknown action");
        Module engine=find_module(pid);require(engine.size==0x1d88000&&hash_file(engine.path)==EXPECTED_HASH,"Unsupported engine build");
        require(!named_module(pid,OBSERVER_DLL_NAME).base,"Diagnostic already loaded; use --status or --stop, then restart the game to reload");
        wchar_t exe[MAX_PATH];require(GetModuleFileNameW(nullptr,exe,MAX_PATH)>0,"Launcher path failed");
        std::wstring dll=exe;dll=dll.substr(0,dll.find_last_of(L"\\/")+1)+OBSERVER_DLL_NAME;
        require(GetFileAttributesW(dll.c_str())!=INVALID_FILE_ATTRIBUTES,"Diagnostic DLL missing beside launcher");
        Handle process(OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_VM_OPERATION|PROCESS_CREATE_THREAD|SYNCHRONIZE,FALSE,pid));require(process.h!=nullptr,"Cannot open game for mod loading");
        // Resolve the module actually containing LoadLibraryW (may be KernelBase).
        FARPROC local=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"LoadLibraryW");require(local!=nullptr,"LoadLibraryW unavailable");
        HMODULE owner=nullptr;require(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(local),&owner)!=0,"Loader owner unknown");
        wchar_t owner_path[MAX_PATH];require(GetModuleFileNameW(owner,owner_path,MAX_PATH)>0,"Loader module path failed");std::wstring owner_name=owner_path;owner_name=owner_name.substr(owner_name.find_last_of(L"\\/")+1);
        Module remote_owner=named_module(pid,owner_name.c_str());require(remote_owner.base!=0,"Remote loader module absent");
        uintptr_t offset=reinterpret_cast<uintptr_t>(local)-reinterpret_cast<uintptr_t>(owner);require(offset<remote_owner.size,"Loader export outside module");
        SIZE_T bytes=(dll.size()+1)*sizeof(wchar_t);void* remote=VirtualAllocEx(process.h,nullptr,bytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);require(remote!=nullptr,"DLL path allocation failed");
        SIZE_T written=0;
        if(!WriteProcessMemory(process.h,remote,dll.c_str(),bytes,&written)||written!=bytes){VirtualFreeEx(process.h,remote,0,MEM_RELEASE);throw std::runtime_error("DLL path write failed");}
        Handle thread(CreateRemoteThread(process.h,nullptr,0,reinterpret_cast<LPTHREAD_START_ROUTINE>(remote_owner.base+offset),remote,0,nullptr));
        if(!thread.h){VirtualFreeEx(process.h,remote,0,MEM_RELEASE);throw std::runtime_error("Mod load thread failed");}
        DWORD waited=WaitForSingleObject(thread.h,10000);
        // Never free an argument still used by a live remote thread.
        require(waited==WAIT_OBJECT_0,"Mod load still pending; path allocation retained; inspect --status");
        DWORD result=0;require(GetExitCodeThread(thread.h,&result)!=0,"Mod load exit unavailable");VirtualFreeEx(process.h,remote,0,MEM_RELEASE);require(result!=0,"LoadLibraryW failed");
        for(int i=0;i<100;++i){
            Handle h(OpenFileMappingW(FILE_MAP_READ,FALSE,name));
            if(h.h){auto* s=static_cast<Status*>(MapViewOfFile(h.h,FILE_MAP_READ,0,0,sizeof(Status)));if(s){bool ready=s->ready!=0,failed=s->error!=0;if(ready||failed)show(s);UnmapViewOfFile(s);if(failed)throw std::runtime_error("Adapter rejected initialization; see session log");if(ready){std::cout<<"Playable QTE candidate loaded. Native casualty/surviving outcomes enabled only for possessed infantry pairs. --stop restores stock combat; unpause if a pair is draining.\n";return 0;}}}
            Sleep(50);
        }
        throw std::runtime_error("Adapter initialization pending; inspect --status and session log");
    }catch(const std::exception& e){std::cerr<<"ERROR: "<<e.what()<<'\n';return 1;}
}




