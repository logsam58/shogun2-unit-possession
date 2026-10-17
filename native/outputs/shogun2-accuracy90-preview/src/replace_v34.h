#pragma once
#include "legacy_v34.h"
#include "legacy_v35.h"
#include "legacy_v36.h"
#include "legacy_v37.h"
#include "legacy_v38.h"
#include "legacy_v39.h"
#include "legacy_v40.h"
#include "legacy_v41.h"
#include "legacy_v42.h"
#include "legacy_v44.h"
// Replacement is restricted to the exact, stopped revision 34. The DLL is
// retained, never unloaded; all engine hooks and input ownership must be native.
static Module legacy_adapter(DWORD pid,const wchar_t* wanted){
    Handle snap(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid));
    require(snap.h!=INVALID_HANDLE_VALUE,"Replacement module snapshot failed");
    MODULEENTRY32W m{};m.dwSize=sizeof(m);Module found;unsigned count=0,matches=0;
    require(Module32FirstW(snap.h,&m)!=0,"Replacement modules absent");
    do{if(_wcsnicmp(m.szModule,L"possession-v",12)==0){
        ++count;require(_wcsicmp(m.szModule,L"possession-v34.dll")==0||_wcsicmp(m.szModule,L"possession-v35.dll")==0||_wcsicmp(m.szModule,L"possession-v36.dll")==0||_wcsicmp(m.szModule,L"possession-v37.dll")==0||_wcsicmp(m.szModule,L"possession-v38.dll")==0||_wcsicmp(m.szModule,L"possession-v39.dll")==0||_wcsicmp(m.szModule,L"possession-v40.dll")==0||_wcsicmp(m.szModule,L"possession-v41.dll")==0||_wcsicmp(m.szModule,L"possession-v42.dll")==0||_wcsicmp(m.szModule,L"possession-v44.dll")==0,"Only verified inert revisions34..42/44 can be replaced");
        if(_wcsicmp(m.szModule,wanted)==0){++matches;found={reinterpret_cast<uintptr_t>(m.modBaseAddr),m.modBaseSize,m.szExePath};}
    }}while(Module32NextW(snap.h,&m));
    require(count>=1&&count<=10&&matches==1,"Replacement legacy module set differs");return found;
}
template<class Legacy> static bool legacy_quiescent(const Legacy& s,DWORD pid,unsigned version){
    return s.magic==POC_MAGIC&&s.version==version&&s.size==sizeof(Legacy)&&s.pid==pid&&
        s.stop&&s.mode==DISABLED&&!s.ready&&!s.error&&!s.input&&!s.fire_ticket&&!s.fire_guard&&
        !memcmp(s.original_code,s.restored_code,16);
}
static std::vector<unsigned char> relocated_engine(const Module& engine){
    std::ifstream in(engine.path,std::ios::binary);
    std::vector<unsigned char> raw((std::istreambuf_iterator<char>(in)),{});
    require(raw.size()>4096,"Replacement engine image absent");
    auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(raw.data());
    require(dos->e_magic==IMAGE_DOS_SIGNATURE&&dos->e_lfanew>0&&size_t(dos->e_lfanew)+sizeof(IMAGE_NT_HEADERS32)<raw.size(),"Replacement DOS header differs");
    auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(raw.data()+dos->e_lfanew);
    require(nt->Signature==IMAGE_NT_SIGNATURE&&nt->OptionalHeader.Magic==IMAGE_NT_OPTIONAL_HDR32_MAGIC&&nt->OptionalHeader.SizeOfImage==engine.size,"Replacement PE image differs");
    std::vector<unsigned char> mapped(engine.size);
    auto sections=IMAGE_FIRST_SECTION(nt);
    for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];
        require(uint64_t(s.PointerToRawData)+s.SizeOfRawData<=raw.size()&&uint64_t(s.VirtualAddress)+s.SizeOfRawData<=mapped.size(),"Replacement section bounds");
        memcpy(mapped.data()+s.VirtualAddress,raw.data()+s.PointerToRawData,s.SizeOfRawData);
    }
    auto reloc=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    require(uint64_t(reloc.VirtualAddress)+reloc.Size<=mapped.size(),"Replacement relocations bounds");
    for(unsigned off=reloc.VirtualAddress;off<reloc.VirtualAddress+reloc.Size;){
        auto block=reinterpret_cast<const IMAGE_BASE_RELOCATION*>(mapped.data()+off);
        require(block->SizeOfBlock>=8&&uint64_t(off)+block->SizeOfBlock<=uint64_t(reloc.VirtualAddress)+reloc.Size,"Replacement relocation block");
        auto entries=reinterpret_cast<const WORD*>(block+1);
        for(unsigned i=0;i<(block->SizeOfBlock-8)/2;++i){
            unsigned type=entries[i]>>12,rva=block->VirtualAddress+(entries[i]&0xfff);
            if(type==IMAGE_REL_BASED_ABSOLUTE)continue;
            require(type==IMAGE_REL_BASED_HIGHLOW&&uint64_t(rva)+4<=mapped.size(),"Replacement relocation type/bounds");
            *reinterpret_cast<uint32_t*>(mapped.data()+rva)+=uint32_t(engine.base-nt->OptionalHeader.ImageBase);
        }
        off+=block->SizeOfBlock;
    }
    return mapped;
}
struct LegacyWindowCheck{DWORD pid;Module old;unsigned windows;bool restored=true;};
static BOOL CALLBACK legacy_window_check(HWND window,LPARAM param){
    auto& check=*reinterpret_cast<LegacyWindowCheck*>(param);DWORD pid=0;GetWindowThreadProcessId(window,&pid);
    if(pid!=check.pid)return TRUE;
    ++check.windows;uintptr_t proc=uintptr_t(GetWindowLongPtrW(window,GWLP_WNDPROC));
    if(proc>=check.old.base&&proc<check.old.base+check.old.size)check.restored=false;
    return TRUE;
}
template<class Legacy> static void verify_inert_adapter(DWORD pid,const Module& engine,unsigned version,const char* expected_hash,unsigned table_rva,unsigned expected_count,bool has_input){
    wchar_t module_name[80];swprintf_s(module_name,80,L"possession-v%u.dll",version);
    Module old=legacy_adapter(pid,module_name);
    require(hash_file(old.path)==expected_hash,"Legacy DLL is not the frozen supported revision");
    wchar_t name[80];swprintf_s(name,80,L"Local\\Shogun2PossessionPOC%u-%lu",version,pid);
    Handle map(OpenFileMappingW(FILE_MAP_READ,FALSE,name));require(map.h!=nullptr,"Legacy control map absent");
    auto s=static_cast<Legacy*>(MapViewOfFile(map.h,FILE_MAP_READ,0,0,sizeof(Legacy)));
    require(s!=nullptr,"Legacy map unavailable");
    bool quiet=legacy_quiescent(*s,pid,version);Sleep(100);quiet=quiet&&legacy_quiescent(*s,pid,version);
    UnmapViewOfFile(s);require(quiet,"Legacy adapter must be fully stopped, restored and free of projectile tickets");
    auto mapped=relocated_engine(engine);
    Handle process(OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid));require(process.h!=nullptr,"Replacement read-only process access failed");
    std::vector<unsigned> hooks={0x367600,0x321310,0x319dd0,0x134fad0,0x31f660,0x2ac0b0,
        0x3e8b50,0x3e3f50,0x3d3630,0x3d4320,0x3e1a20,0x4272e0,0x4c56d0,0xf9c360,0xf9bf20,0x122f740};
    if(has_input)hooks.push_back(0x2fdcf0);
    if(version==40)hooks.push_back(0x472b70);
    for(unsigned rva:hooks){unsigned char bytes[16];SIZE_T read=0;
        require(ReadProcessMemory(process.h,reinterpret_cast<void*>(engine.base+rva),bytes,sizeof bytes,&read)&&read==sizeof bytes,"Replacement hook read failed");
        require(!memcmp(bytes,mapped.data()+rva,sizeof bytes),"Legacy engine hook is still patched at "+hex(rva));
    }
    auto local=GetProcAddress(GetModuleHandleW(L"user32.dll"),"SetCursor");require(local!=nullptr,"Replacement cursor export absent");
    HMODULE owner=nullptr;require(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(local),&owner)!=0,"Replacement cursor owner absent");
    wchar_t owner_path[MAX_PATH];require(GetModuleFileNameW(owner,owner_path,MAX_PATH)!=0,"Replacement cursor owner path failed");
    std::wstring owner_name=owner_path;owner_name=owner_name.substr(owner_name.find_last_of(L"\\/")+1);
    Module remote=named_module(pid,owner_name.c_str());require(remote.base!=0,"Replacement remote cursor owner absent");
    // SetCursor can have a pre-existing game hook, so a clean launcher copy is
    // not its original state. Compare with the exact legacy MinHook backup.
    // RVA55630 is proven by a separate relink whose entire .text matches the
    // frozen legacy DLL byte for byte (evidence/v34-symbol-layout.json).
    struct LegacyHook{uintptr_t target,detour,trampoline;unsigned char backup[8];unsigned char flags;unsigned nIP:4;unsigned char oldIPs[8],newIPs[8];};
    static_assert(sizeof(LegacyHook)==44,"Verified x86 legacy hook entry size");
    unsigned table[3];SIZE_T read=0;
    require(ReadProcessMemory(process.h,reinterpret_cast<void*>(old.base+table_rva),table,sizeof table,&read)&&read==sizeof table,"Legacy hook table read failed");
    require(table[0]&&table[1]==32&&table[2]==expected_count,"Legacy hook table differs");
    std::vector<LegacyHook> entries(table[2]);
    require(ReadProcessMemory(process.h,reinterpret_cast<void*>(table[0]),entries.data(),entries.size()*sizeof(LegacyHook),&read)&&read==entries.size()*sizeof(LegacyHook),"Legacy hook backups read failed");
    uintptr_t cursor_target=remote.base+uintptr_t(local)-uintptr_t(owner);unsigned cursor_found=0;
    for(auto& entry:entries){
        require(!(entry.flags&6)&&entry.detour>=old.base&&entry.detour<old.base+old.size,"Legacy hook is enabled, queued or has an unknown detour");
        if(entry.target!=cursor_target){
            bool known=false;for(unsigned rva:hooks)if(entry.target==engine.base+rva)known=true;
            require(known,"Unexpected legacy hook target");continue;
        }
        ++cursor_found;unsigned count=(entry.flags&1)?7:5;uintptr_t at=cursor_target-((entry.flags&1)?5:0);unsigned char cursor[7];
        require(ReadProcessMemory(process.h,reinterpret_cast<void*>(at),cursor,count,&read)&&read==count,"Replacement cursor read failed");
        require(!memcmp(cursor,entry.backup,count),"Cursor differs from the legacy pre-mod backup");
    }
    require(cursor_found==1,"Legacy cursor backup absent or duplicated");
    LegacyWindowCheck check{pid,old,0,true};require(EnumWindows(legacy_window_check,reinterpret_cast<LPARAM>(&check))!=0&&check.windows&&check.restored,"Legacy window input handler is still installed");
    std::cout<<"Inert revision "<<version<<" verified: exact DLL, stopped protocol, no shot ticket, "<<hooks.size()<<" native hook sites, cursor and window handler restored. DLL remains resident.\n";
}
