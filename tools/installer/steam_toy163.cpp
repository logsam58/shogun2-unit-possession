// Isolated setup fixture only. Never distributed in the player installer.
#include "steam_launch162.h"
int wmain(int argc,wchar_t** argv){try{
    using namespace steam162;require(argc==2,"Fixture needs one argument");wchar_t own[32768]{};
    require(GetModuleFileNameW(nullptr,own,32768)>0,"Fixture path missing");auto root=fs::path(own).parent_path();
    require(root.filename()==L"synthetic-steam162"&&!fs::exists(root/L"steam.exe")&&
        read(root/L".unit-possession-fixture162")=="synthetic only\n","Refusing nonfixture Steam directory");
    std::wstring arg=argv[1];require(arg==L"-shutdown"||arg==L"-silent","Unknown fixture action");
    {std::ofstream log(root/L"fixture-lifecycle163.log",std::ios::app);log<<utf8(arg)<<'\n';}
    if(arg==L"-shutdown"){
        if(!fs::exists(root/L"fixture-refuse-exit163"))fs::remove(root/L"fixture-running163");
    }else{std::ofstream flag(root/L"fixture-running163");flag<<"synthetic only\n";}
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
