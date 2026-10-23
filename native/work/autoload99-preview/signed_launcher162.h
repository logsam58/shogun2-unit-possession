#pragma once
#include <wintrust.h>
#include <softpub.h>
#include <wincrypt.h>
namespace launch162 {
inline bool signed_ca(const std::wstring& path){
    WINTRUST_FILE_INFO file{};file.cbStruct=sizeof file;file.pcwszFilePath=path.c_str();
    WINTRUST_DATA data{};data.cbStruct=sizeof data;data.dwUIChoice=WTD_UI_NONE;data.fdwRevocationChecks=WTD_REVOKE_NONE;
    data.dwUnionChoice=WTD_CHOICE_FILE;data.pFile=&file;data.dwStateAction=WTD_STATEACTION_VERIFY;
    data.dwProvFlags=WTD_CACHE_ONLY_URL_RETRIEVAL|WTD_REVOCATION_CHECK_NONE;
    GUID action=WINTRUST_ACTION_GENERIC_VERIFY_V2;
    auto status=WinVerifyTrust(nullptr,&action,&data);bool accepted=false;
    if(status==ERROR_SUCCESS){
        auto provider=WTHelperProvDataFromStateData(data.hWVTStateData);
        auto signer=provider?WTHelperGetProvSignerFromChain(provider,0,FALSE,0):nullptr;
        auto cert=signer?WTHelperGetProvCertFromChain(signer,0):nullptr;
        wchar_t name[256]{};
        if(cert&&cert->pCert&&CertGetNameStringW(cert->pCert,CERT_NAME_SIMPLE_DISPLAY_TYPE,0,nullptr,name,256)>1)
            accepted=wcscmp(name,L"Creative Assembly")==0;
    }
    data.dwStateAction=WTD_STATEACTION_CLOSE;WinVerifyTrust(nullptr,&action,&data);return accepted;
}
}
