#pragma once
#include "startup_view160.h"
#include "../../outputs/shogun2-possession-cleanup-v46/src/identity.h"
#include <shellapi.h>
#include <filesystem>
#include "signed_launcher162.h"
// Steam supplies its original command through %command%. No shell evaluation,
// game executable replacement, registry interception or Steam config editing.
namespace launch161 {
struct Plan {std::wstring application,command;};
inline Plan parse(const std::wstring& command){
    require(!command.empty()&&command.size()<32767&&command.find(L'\0')==std::wstring::npos,"Missing or oversized Steam game command");
    int count=0;auto args=CommandLineToArgvW(command.c_str(),&count);
    require(args&&count>0,"Cannot read Steam game command");
    Plan plan{args[0],command};LocalFree(args);
    require(std::filesystem::path(plan.application).is_absolute(),"Steam must supply the original absolute game path");
    return plan;
}
inline bool game_name(const std::wstring& path){
    auto name=std::filesystem::path(path).filename().wstring();
    return !_wcsicmp(name.c_str(),L"shogun2.exe")||!_wcsicmp(name.c_str(),L"shogun2.retail.exe");
}
inline void validate(const Plan& plan){
    auto folder=std::filesystem::path(plan.application).parent_path();
    const bool launcher=!_wcsicmp(std::filesystem::path(plan.application).filename().c_str(),L"launcher.exe")&&
        !_wcsicmp(folder.filename().c_str(),L"launcher");
    require(game_name(plan.application)||launcher,"Use this helper with Shogun 2's original Steam game or Creative Assembly launcher");
    if(launcher)folder=folder.parent_path();
    require(hash_file((folder/L"Empire.Retail.dll").wstring())==EXPECTED_HASH,"Unsupported Shogun 2 engine");
    require(launcher?launch162::signed_ca(plan.application):hash_file(plan.application)=="0ab284186e0be3fbcde34e2f800e3c0328008f3219490c7c25e822add837c4a9","Unsupported or untrusted Shogun 2 executable");
    wchar_t app[32]{};auto n=GetEnvironmentVariableW(L"SteamAppId",app,32);
    require(n>0&&n<32&&!wcscmp(app,L"34330"),"Launch from Shogun 2's Steam Play button so Steam and DLC are initialized");
}
inline DWORD start(const Plan& plan,bool third){
    require(startup160::publish(third),"Cannot pass the starting-view choice to the game");
    auto command=plan.command;STARTUPINFOW startup{};startup.cb=sizeof(startup);PROCESS_INFORMATION child{};
    // Preserve Steam's environment, original arguments and working directory.
    require(CreateProcessW(plan.application.c_str(),command.data(),nullptr,nullptr,FALSE,0,nullptr,nullptr,&startup,&child)!=0,"Cannot start the original Steam game command");
    Handle process(child.hProcess),thread(child.hThread);
    require(WaitForSingleObject(process.h,INFINITE)==WAIT_OBJECT_0,"Cannot wait for the game process");
    DWORD result=0;require(GetExitCodeProcess(process.h,&result)!=0,"Cannot read the game exit status");return result;
}
using Start=DWORD(*)(const Plan&,bool);
inline DWORD choose_then_start(HINSTANCE module,const Plan& plan,const startupdialog160::Observer* observer=nullptr,Start create=start){
    auto result=startupdialog160::choose_result(module,nullptr,observer);
    if(result==-1)throw std::runtime_error("Cannot create the camera chooser; the game was not started");
    if(result!=startupdialog160::third_button&&result!=startupdialog160::first_button)return 0;
    return create(plan,result==startupdialog160::third_button);
}
}
