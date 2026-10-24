// Inert native startup observation. No hooks, input, UI, Steam or game writes.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <string>
#include <vector>

static HMODULE module164;
static INIT_ONCE once164 = INIT_ONCE_STATIC_INIT;

static std::string engine_hash164(const std::wstring& path) {
    HANDLE file=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file==INVALID_HANDLE_VALUE)return {};
    BCRYPT_ALG_HANDLE algorithm{};BCRYPT_HASH_HANDLE hash{};
    DWORD size{},used{};std::string result;
    if(BCryptOpenAlgorithmProvider(&algorithm,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0 &&
       BCryptGetProperty(algorithm,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&size),sizeof size,&used,0)>=0){
        std::vector<unsigned char> object(size),buffer(65536);unsigned char digest[32]{};
        if(BCryptCreateHash(algorithm,&hash,object.data(),size,nullptr,0,0)>=0){
            DWORD count{};bool ok=true;unsigned long long total=0;
            for(;;){if(!ReadFile(file,buffer.data(),DWORD(buffer.size()),&count,nullptr)){ok=false;break;}
                if(!count)break;total+=count;if(total>64ull*1024*1024 || BCryptHashData(hash,buffer.data(),count,0)<0){ok=false;break;}}
            if(ok && BCryptFinishHash(hash,digest,sizeof digest,0)>=0){const char* hex="0123456789abcdef";for(auto b:digest){result+=hex[b>>4];result+=hex[b&15];}}
        }
    }
    if(hash)BCryptDestroyHash(hash);if(algorithm)BCryptCloseAlgorithmProvider(algorithm,0);CloseHandle(file);return result;
}

static DWORD WINAPI observe164(void* argument) {
    HMODULE retained=static_cast<HMODULE>(argument);
    wchar_t exe[32768]{},own[32768]{};
    DWORD n=GetModuleFileNameW(nullptr,exe,32768),m=GetModuleFileNameW(retained,own,32768);
    if(n && n<32768 && m && m<32768){
        std::wstring game(exe),path(own);auto slash=game.find_last_of(L"\\");
        std::wstring name=slash==game.npos?game:game.substr(slash+1);
        bool host=!_wcsicmp(name.c_str(),L"shogun2.exe") || !_wcsicmp(name.c_str(),L"shogun2.retail.exe");
        std::string digest=host && slash!=game.npos?engine_hash164(game.substr(0,slash)+L"\\Empire.Retail.dll"):"";
        bool supported=host && digest=="022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5";
        std::string receipt="{\"schema\":1,\"test\":164,\"pid\":"+std::to_string(GetCurrentProcessId())+
            ",\"native_entry\":true,\"supported_game_host\":"+(supported?"true":"false")+
            ",\"gameplay_loaded\":false,\"hooks_installed\":false,\"engine_sha256\":\""+digest+"\"}\n";
        path+=L".pid"+std::to_wstring(GetCurrentProcessId())+L".json";
        HANDLE file=CreateFileW(path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(file!=INVALID_HANDLE_VALUE){DWORD written{};WriteFile(file,receipt.data(),DWORD(receipt.size()),&written,nullptr);FlushFileBuffers(file);CloseHandle(file);}
    }
    FreeLibraryAndExitThread(retained,0);
}

static BOOL CALLBACK begin164(PINIT_ONCE,void*,void**) {
    HMODULE retained{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,reinterpret_cast<LPCWSTR>(&module164),&retained))return FALSE;
    HANDLE thread=CreateThread(nullptr,0,observe164,retained,0,nullptr);
    if(!thread){FreeLibrary(retained);return FALSE;}CloseHandle(thread);return TRUE;
}

extern "C" __declspec(dllexport) int __cdecl luaopen_s2up_workshop_probe164(void*) {
    InitOnceExecuteOnce(&once164,begin164,nullptr,nullptr);return 0;
}
BOOL WINAPI DllMain(HINSTANCE module,DWORD reason,void*) {
    if(reason==DLL_PROCESS_ATTACH)module164=module;return TRUE;
}
