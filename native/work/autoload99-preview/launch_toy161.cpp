#include "../../common/startup_view160.h"
#include <fstream>
int wmain(int argc,wchar_t** argv){
    if(argc!=5)return 91;
    wchar_t directory[32768]{};if(!GetCurrentDirectoryW(32768,directory))return 92;
    std::ofstream out(argv[1],std::ios::binary);
    std::wstring data=startup160::third_person()?L"third\n":L"first\n";data+=directory;data+=L'\n';
    for(int i=2;i<argc;++i){data+=argv[i];data+=L'\n';}
    wchar_t steam[32]{};GetEnvironmentVariableW(L"SteamAppId",steam,32);data+=steam;data+=L'\n';
    out.write(reinterpret_cast<const char*>(data.data()),data.size()*sizeof(wchar_t));
    return out?37:93;
}
