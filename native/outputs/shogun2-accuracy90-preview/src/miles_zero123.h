#pragma once
// Compatibility guard for the pinned stock Miles 8.0d zero-sample filter overrun.
// This module and its trampoline stay resident until process exit.
namespace mileszero123 {
static const char* expected_sha="84163b5036c15be8e21c40cfd5fe1b83a1bd10c742d80ce7acdeb99a2f7d837d";
static void* original=nullptr;static void* target=nullptr;static HMODULE retained=nullptr;
static volatile LONG zero_blocks=0;static int install_status=0;
static const unsigned char prefix[]={0x83,0xec,0x44,0x53,0x55,0x8b,0x6c,0x24,0x50,0x56,0x8b,0x74,0x24,0x58,0x57};
static bool supported(const std::string& hash,const void* bytes){return hash==expected_sha&&!memcmp(bytes,prefix,sizeof prefix);}
__declspec(naked) static void gate(){
    __asm {
        test edx,edx
        jnz forward
        lock inc dword ptr [zero_blocks]
        mov ecx,[esp+8]
        mov eax,[ecx+0x1c]
        shr eax,16
        cmp eax,3
        sete al
        movzx eax,al
        ret
    forward:
        jmp dword ptr [original]
    }
}
// Optional compatibility protection: failure is recorded and never disables gameplay.
static int install(){
    if(install_status)return install_status;
    HMODULE module=GetModuleHandleW(L"mss32.dll");if(!module)return install_status=-1;
    HMODULE hold=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,reinterpret_cast<LPCWSTR>(module),&hold))return install_status=-2;
    wchar_t path[32768]{};DWORD n=GetModuleFileNameW(hold,path,32768);bool valid=false;
    if(n&&n<32768){try{valid=supported(hash_file(path),reinterpret_cast<void*>(uintptr_t(hold)+0x16bf0));}catch(...){valid=false;}}
    if(!valid){FreeLibrary(hold);return install_status=-3;}
    void* candidate=reinterpret_cast<void*>(uintptr_t(hold)+0x16bf0);
    if(MH_CreateHook(candidate,reinterpret_cast<void*>(&gate),&original)!=MH_OK){FreeLibrary(hold);return install_status=-4;}
    retained=hold;target=candidate; // Retain even on enable failure; no trampoline lifetime race.
    if(MH_EnableHook(target)!=MH_OK)return install_status=-5;
    return install_status=1;
}
static bool disable(){return !target||MH_DisableHook(target)==MH_OK;}
}
