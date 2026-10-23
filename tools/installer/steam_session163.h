#pragma once
#include "steam_launch162.h"
#include <wintrust.h>
#include <softpub.h>
#include <wincrypt.h>
namespace steam163 {
inline bool signed_valve(const std::wstring& path){
    WINTRUST_FILE_INFO file{};file.cbStruct=sizeof file;file.pcwszFilePath=path.c_str();
    WINTRUST_DATA data{};data.cbStruct=sizeof data;data.dwUIChoice=WTD_UI_NONE;
    data.fdwRevocationChecks=WTD_REVOKE_NONE;data.dwUnionChoice=WTD_CHOICE_FILE;data.pFile=&file;
    data.dwStateAction=WTD_STATEACTION_VERIFY;data.dwProvFlags=WTD_CACHE_ONLY_URL_RETRIEVAL|WTD_REVOCATION_CHECK_NONE;
    GUID action=WINTRUST_ACTION_GENERIC_VERIFY_V2;auto status=WinVerifyTrust(nullptr,&action,&data);bool accepted=false;
    if(status==ERROR_SUCCESS){
        auto provider=WTHelperProvDataFromStateData(data.hWVTStateData);
        auto signer=provider?WTHelperGetProvSignerFromChain(provider,0,FALSE,0):nullptr;
        auto cert=signer?WTHelperGetProvCertFromChain(signer,0):nullptr;wchar_t name[256]{};
        if(cert&&cert->pCert&&CertGetNameStringW(cert->pCert,CERT_NAME_SIMPLE_DISPLAY_TYPE,0,nullptr,name,256)>1)
            accepted=wcscmp(name,L"Valve Corp.")==0;
    }
    data.dwStateAction=WTD_STATEACTION_CLOSE;WinVerifyTrust(nullptr,&action,&data);return accepted;
}
inline bool running(const steam162::fs::path& root){
#ifdef STEAM162_FIXTURE
    return steam162::fs::exists(root/L"fixture-running163");
#else
    // A second Steam installation must not be shut down or silently edited.
    auto expected=(root/L"steam.exe").wstring();steam162::plain(root/L"steam.exe");
    require(signed_valve(expected),"The installed Steam client did not pass its Valve signature check; preserved");
    Handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0));
    require(snapshot.h!=INVALID_HANDLE_VALUE,"Cannot check Steam processes");
    PROCESSENTRY32W e{};e.dwSize=sizeof e;require(Process32FirstW(snapshot.h,&e)!=0,"Cannot check Steam processes");bool found=false;
    do{if(_wcsicmp(e.szExeFile,L"steam.exe"))continue;
        Handle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,e.th32ProcessID));
        wchar_t path[32768]{};DWORD size=32768;
        require(process.h&&QueryFullProcessImageNameW(process.h,0,path,&size),"Cannot identify a running Steam client; preserved");
        require(_wcsicmp(path,expected.c_str())==0,"A different Steam installation is running; preserved");found=true;
    }while(Process32NextW(snapshot.h,&e));return found;
#endif
}
inline void wait_closed(const steam162::fs::path& root){
    const auto deadline=GetTickCount64()+30000;
    // No forced termination. Steam handles its own shutdown and saves its config.
    while(running(root)){
        require(GetTickCount64()<deadline,"Steam did not finish its normal shutdown. No settings were changed; setup can be retried");
        Sleep(250);
    }
}
inline void launch_client(const steam162::fs::path& root,bool shutdown){
#ifdef STEAM162_FIXTURE
    auto exe=root/L"fixture-steam163.exe";
#else
    Handle token;require(OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token.h),"Cannot check Steam launch privileges");
    TOKEN_ELEVATION elevation{};DWORD got{};
    require(GetTokenInformation(token.h,TokenElevation,&elevation,sizeof elevation,&got)&&!elevation.TokenIsElevated,
        "Setup could not use the original unelevated user; Steam was not launched as administrator");
    auto exe=root/L"steam.exe";steam162::plain(exe);
    require(signed_valve(exe.wstring()),"The Steam client signature changed; preserved");
#endif
    auto command=L"\""+exe.wstring()+L"\" "+(shutdown?L"-shutdown":L"-silent");
    STARTUPINFOW startup{};startup.cb=sizeof startup;startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
    PROCESS_INFORMATION child{};
    require(CreateProcessW(exe.c_str(),command.data(),nullptr,nullptr,FALSE,0,nullptr,root.c_str(),&startup,&child),
        "Could not request normal Steam shutdown/restart");
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
}
}
