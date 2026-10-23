#include "../../common/startup_view160.h"
#include <fstream>
int wmain(int argc,wchar_t** argv){
    if(argc==6&&!wcscmp(argv[1],L"--relay")){
        // Model a native launcher spawning its game: raw argument tail,
        // environment and working directory survive a second process hop.
        wchar_t self[32768]{};if(!GetModuleFileNameW(nullptr,self,32768))return 94;
        auto tail=wcsstr(GetCommandLineW(),L" --relay ");if(!tail)return 95;
        std::wstring command=L"\""+std::wstring(self)+L"\" "+(tail+9);
        STARTUPINFOW startup{};startup.cb=sizeof startup;PROCESS_INFORMATION child{};
        if(!CreateProcessW(self,command.data(),nullptr,nullptr,FALSE,0,nullptr,nullptr,&startup,&child))return 96;
        CloseHandle(child.hThread);WaitForSingleObject(child.hProcess,INFINITE);DWORD result=97;GetExitCodeProcess(child.hProcess,&result);CloseHandle(child.hProcess);return int(result);
    }
    if(argc!=5)return 91;
    wchar_t directory[32768]{};if(!GetCurrentDirectoryW(32768,directory))return 92;
    std::ofstream out(argv[1],std::ios::binary);
    std::wstring data=startup160::third_person()?L"third\n":L"first\n";data+=directory;data+=L'\n';
    for(int i=2;i<argc;++i){data+=argv[i];data+=L'\n';}
    wchar_t steam[32]{};GetEnvironmentVariableW(L"SteamAppId",steam,32);data+=steam;data+=L'\n';
    out.write(reinterpret_cast<const char*>(data.data()),data.size()*sizeof(wchar_t));
    return out?37:93;
}
