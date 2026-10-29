#include "identity.h"
#include "protocol.h"
#include "control_metrics.h"
static_assert(sizeof(void*)==4,"Build launcher x86");
static Module named_module(DWORD pid,const wchar_t* wanted) {
    Handle snap(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid));require(snap.h!=INVALID_HANDLE_VALUE,"Module snapshot failed");
    MODULEENTRY32W m{};m.dwSize=sizeof(m);require(Module32FirstW(snap.h,&m)!=0,"Module list failed");
    do{if(_wcsicmp(m.szModule,wanted)==0)return {reinterpret_cast<uintptr_t>(m.modBaseAddr),m.modBaseSize,m.szExePath};}while(Module32NextW(snap.h,&m));
    return {};
}
#include "replace_v34.h"
static bool any_possession_adapter(DWORD pid){
    Handle snap(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid));require(snap.h!=INVALID_HANDLE_VALUE,"Module snapshot failed");
    MODULEENTRY32W m{};m.dwSize=sizeof(m);require(Module32FirstW(snap.h,&m)!=0,"Module list failed");
    do{if(_wcsnicmp(m.szModule,L"possession-v",12)==0)return true;}while(Module32NextW(snap.h,&m));return false;
}
static void show(Status* s) {
    require(s->magic==POC_MAGIC&&s->version==POC_VERSION&&s->size==sizeof(Status),"Unknown control protocol");
    std::cout<<"{\"event\":\"control_metrics\",\"mode\":"<<s->mode;control_metrics(std::cout,*s);std::cout<<"}\n";
    std::cout<<"{\"pid\":"<<s->pid<<",\"ready\":"<<s->ready<<",\"mode\":"<<s->mode<<",\"error\":"<<s->error<<",\"frame\":"<<s->frame
        <<",\"projectile_camera_suppressed\":"<<s->projectile_camera_suppressed<<",\"weapon_phase\":"<<s->weapon_phase<<",\"weapon_record\":"<<s->weapon_record<<",\"weapon_animation\":"<<s->weapon_animation<<",\"weapon_mouse_captured\":"<<s->weapon_mouse_captured<<",\"aiming\":"<<s->aiming<<",\"aim_pose\":"<<s->aim_pose<<",\"aim_blend\":"<<s->aim_blend<<",\"hip_fov\":"<<s->hip_fov<<",\"view_fov\":"<<s->view_fov<<",\"hud_hidden\":"<<s->hud_hidden<<",\"crosshair_visible\":"<<s->crosshair_visible<<",\"pose_muzzle_updates\":"<<s->pose_muzzle_updates<<",\"pose_weapon_record\":"<<s->pose_weapon_record<<",\"pose_native_clip\":"<<s->pose_native_clip<<",\"pose_native_bias\":"<<s->pose_native_bias<<",\"bow_origin_aligned\":"<<s->bow_origin_aligned<<",\"bow_origin_correction\":"<<s->bow_origin_correction<<",\"shot_camera_origin\":["<<s->shot_camera_origin[0]<<','<<s->shot_camera_origin[1]<<','<<s->shot_camera_origin[2]<<']'<<",\"pose_updates\":"<<s->pose_updates<<",\"pose_aim_updates\":"<<s->pose_aim_updates<<",\"pose_restores\":"<<s->pose_restores<<",\"pose_error\":"<<s->pose_error<<",\"pose_bones\":"<<s->pose_bones<<",\"pose_pitch\":"<<s->pose_pitch<<",\"camera_pitch\":"<<s->camera_pitch<<",\"native_matrix_override\":"<<s->native_matrix_override<<",\"raw_mouse_active\":"<<s->raw_mouse_active<<",\"raw_mouse_error\":"<<s->raw_mouse_error<<",\"raw_mouse_events\":"<<s->raw_mouse_events<<",\"raw_aim_events\":"<<s->raw_aim_events<<",\"raw_y_total\":"<<s->raw_y_total<<",\"raw_aim_y_total\":"<<s->raw_aim_y_total<<",\"interface_error\":"<<s->interface_error<<",\"interface_restores\":"<<s->interface_restores<<",\"cursor_suppressed\":"<<s->cursor_suppressed<<",\"aim_clip\":"<<s->aim_clip<<",\"draws_started\":"<<s->draws_started<<",\"reloads_started\":"<<s->reloads_started<<",\"ai_shots_suppressed\":"<<s->ai_shots_suppressed<<",\"weapon_release_time\":"<<s->weapon_release_time<<",\"weapon_reload_remaining\":"<<s->weapon_reload_remaining<<",\"shots_queued\":"<<s->shots_queued<<",\"shots_spawned\":"<<s->shots_spawned<<",\"shots_blocked\":"<<s->shots_blocked<<",\"shots_canceled\":"<<s->shots_canceled<<",\"spread_removed\":"<<s->spread_removed<<",\"direct_fire\":"<<s->direct_fire<<",\"launch_delta\":"<<s->launch_delta<<",\"shot_origin\":["<<s->shot_origin[0]<<','<<s->shot_origin[1]<<','<<s->shot_origin[2]<<']'<<",\"shot_velocity\":["<<s->shot_velocity[0]<<','<<s->shot_velocity[1]<<','<<s->shot_velocity[2]<<']'<<",\"shot_view_direction\":["<<s->shot_view_direction[0]<<','<<s->shot_view_direction[1]<<','<<s->shot_view_direction[2]<<']'<<",\"shot_error\":"<<s->shot_error<<",\"fire_ticket\":"<<s->fire_ticket<<",\"fire_guard\":"<<s->fire_guard<<",\"shot_packet_size\":"<<s->shot_packet_size<<",\"ammo_before\":"<<s->ammo_before<<",\"ammo_after\":"<<s->ammo_after<<",\"orders_sent\":"<<s->orders_sent<<",\"last_order\":"<<s->last_order<<",\"order_error\":"<<s->order_error<<",\"follow\":"<<s->follow<<",\"packet_size\":"<<s->packet_size<<",\"window_thread\":"<<s->window_thread<<",\"camera_thread\":"<<s->camera_thread<<",\"formation\":["<<s->formation_x<<','<<s->formation_z<<"],\"formation_count\":"<<s->formation_count<<",\"controlled_updates\":"<<s->controlled_updates<<",\"other_updates\":"<<s->other_updates<<",\"after\":["<<s->after[0]<<','<<s->after[1]<<','<<s->after[2]<<"]}\n";
}
int wmain(int argc,wchar_t** argv) {
    try {
        std::wstring action=argc>1?argv[1]:L"--load";require(argc<=2,"Use --load, --status, or --stop");
        DWORD pid=find_process(0);wchar_t name[80];mapping_name(name,pid);
        if(action==L"--status"||action==L"--stop"){
            Handle h(OpenFileMappingW(FILE_MAP_READ|(action==L"--stop"?FILE_MAP_WRITE:0),FALSE,name));require(h.h!=nullptr,"Prototype is not loaded");
            auto* s=static_cast<Status*>(MapViewOfFile(h.h,FILE_MAP_READ|(action==L"--stop"?FILE_MAP_WRITE:0),0,0,sizeof(Status)));require(s!=nullptr,"Mapping failed");
            require(s->pid==pid,"Process identity changed");show(s);
            if(action==L"--stop"){InterlockedExchange(&s->stop,1);for(int i=0;i<100&&s->mode!=DISABLED&&s->error==0;++i)Sleep(50);show(s);require(s->mode==DISABLED&&s->error==0,"Disable/restoration not confirmed");require(memcmp(s->original_code,s->restored_code,16)==0,"Hook restoration differs");std::cout<<"Native motion and camera hooks disabled and restoration confirmed. A pending projectile guard, if reported, remains until game exit. DLL remains inert until game exit.\n";}
            UnmapViewOfFile(s);return 0;
        }
        require(action==L"--load"||action==L"--replace-v44","Unknown action");
        Module engine=find_module(pid);require(engine.size==0x1d88000&&hash_file(engine.path)==EXPECTED_HASH,"Unsupported engine build");
        if(action==L"--replace-v44"){
            if(named_module(pid,L"possession-v34.dll").base)verify_inert_adapter<LegacyStatus34>(pid,engine,34,
                "4da6424d306a8573b1011235cf13f6200b8f8e5bc3a768c1176f796eb9c0520f",0x55630,17,false);
            if(named_module(pid,L"possession-v35.dll").base)verify_inert_adapter<LegacyStatus35>(pid,engine,35,
                "a91b3b47f8827d6dabca2275a7392c55ddf6e54212b5e7b4168f1e35765caada",0x56630,18,true);
            if(named_module(pid,L"possession-v36.dll").base)verify_inert_adapter<LegacyStatus36>(pid,engine,36,
                "00702be25d37a18c25db96ec2b6a3f70b450a26f3dcf9d914b5ed9a3c8be21ae",0x56630,18,true);
            if(named_module(pid,L"possession-v37.dll").base)verify_inert_adapter<LegacyStatus37>(pid,engine,37,
                "39ae04e306c9933f0620dffc5accf073fb563a3def1c8feb880d6fae73de1dde",0x56630,18,true);
            if(named_module(pid,L"possession-v38.dll").base)verify_inert_adapter<LegacyStatus38>(pid,engine,38,
                "4b86b917532b6457078ea80e6bee10d360e70991ab7f069b2e6ef55878aa3565",0x56640,18,true);
            if(named_module(pid,L"possession-v39.dll").base)verify_inert_adapter<LegacyStatus39>(pid,engine,39,
                "73adddfc17d4104e358248d0c4a29a7aa49312619148116f9f8724d85e2ecf38",0x56640,18,true);
            if(named_module(pid,L"possession-v40.dll").base)verify_inert_adapter<LegacyStatus40>(pid,engine,40,
                "7f1a167fd9cde63f40113d31dba8e68b8f192a716795cc38ccfd00e98c1ce8c8",0x56640,19,true);
            if(named_module(pid,L"possession-v41.dll").base)verify_inert_adapter<LegacyStatus41>(pid,engine,41,
                "21c5b7514a22cc6f77bb40ff5258d7d98227737c393c4757c19fce598f1225f1",0x56640,18,true);
            if(named_module(pid,L"possession-v42.dll").base)verify_inert_adapter<LegacyStatus42>(pid,engine,42,
                "db4a3891ef16de208288e15c54a14a6b574c4e8dcada04689d46ea20da52a72e",0x56650,18,true);
            verify_inert_adapter<LegacyStatus44>(pid,engine,44,
                "9bf4d00ab4c209579c7be240e298cf967186d5a89c93a108795bb12cff99e353",0x58650,18,true);
        } else require(!any_possession_adapter(pid),"A possession adapter is already resident; stop it with its matching launcher, then restart the game before changing versions");
        wchar_t exe[MAX_PATH];require(GetModuleFileNameW(nullptr,exe,MAX_PATH)>0,"Launcher path failed");
        std::wstring dll=exe;dll=dll.substr(0,dll.find_last_of(L"\\/")+1)+L"possession-v46.dll";
        require(GetFileAttributesW(dll.c_str())!=INVALID_FILE_ATTRIBUTES,"possession-v46.dll missing beside launcher");
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
            if(h.h){auto* s=static_cast<Status*>(MapViewOfFile(h.h,FILE_MAP_READ,0,0,sizeof(Status)));if(s){bool ready=s->ready!=0,failed=s->error!=0;if(ready||failed)show(s);UnmapViewOfFile(s);if(failed)throw std::runtime_error("Adapter rejected initialization; see session log");if(ready){std::cout<<"Loaded. Select infantry, F9, unpause. WASD move; mouse look; Q/E also turn; Shift run. Hold RMB to raise the bow or matchlock, then click LMB to fire; releasing RMB cancels a pending shot; native reload must finish before the next shot; F5 hold; F6 advance 25m; F7 toggle follow. F10 release.\n";return 0;}}}
            Sleep(50);
        }
        throw std::runtime_error("Adapter initialization pending; inspect --status and session log");
    }catch(const std::exception& e){std::cerr<<"ERROR: "<<e.what()<<'\n';return 1;}
}




