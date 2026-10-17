#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <bcrypt.h>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <memory>

static const char* EXPECTED_HASH="022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5";
static volatile LONG stopped=0;
BOOL WINAPI stop_handler(DWORD code) {
    if (code==CTRL_C_EVENT || code==CTRL_BREAK_EVENT) { InterlockedExchange(&stopped,1); return TRUE; }
    return FALSE;
}
struct Handle {
    HANDLE h=nullptr;
    explicit Handle(HANDLE value=nullptr):h(value){}
    ~Handle(){if(h && h!=INVALID_HANDLE_VALUE)CloseHandle(h);}
    Handle(const Handle&)=delete;
    Handle& operator=(const Handle&)=delete;
};
inline void require(bool ok,const std::string& msg) {if(!ok)throw std::runtime_error(msg+" (Win32="+std::to_string(GetLastError())+")");}
inline std::string hex(uintptr_t n) {std::ostringstream s;s<<"0x"<<std::hex<<n;return s.str();}
inline std::string hash_file(const std::wstring& path) {
    Handle file(CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
    require(file.h!=INVALID_HANDLE_VALUE,"Cannot read the loaded engine file");
    BCRYPT_ALG_HANDLE alg=nullptr;BCRYPT_HASH_HANDLE hash=nullptr;
    require(BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)==0,"SHA256 provider failed");
    try {
        DWORD bytes=0,object_len=0;
        require(BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&object_len),sizeof(object_len),&bytes,0)==0,"SHA256 property failed");
        std::vector<UCHAR> object(object_len),buf(1<<20);
        require(BCryptCreateHash(alg,&hash,object.data(),object_len,nullptr,0,0)==0,"SHA256 init failed");
        for(;;) {
            require(ReadFile(file.h,buf.data(),static_cast<DWORD>(buf.size()),&bytes,nullptr)!=0,"Engine read failed");
            if(!bytes)break;
            require(BCryptHashData(hash,buf.data(),bytes,0)==0,"SHA256 update failed");
        }
        std::array<UCHAR,32> digest{};
        require(BCryptFinishHash(hash,digest.data(),32,0)==0,"SHA256 finish failed");
        std::ostringstream s;for(auto v:digest)s<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(v);
        BCryptDestroyHash(hash);hash=nullptr;BCryptCloseAlgorithmProvider(alg,0);return s.str();
    } catch(...) {if(hash)BCryptDestroyHash(hash);BCryptCloseAlgorithmProvider(alg,0);throw;}
}
struct Module {uintptr_t base=0;DWORD size=0;std::wstring path;};
inline DWORD find_process(DWORD requested) {
    Handle snap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0));
    require(snap.h!=INVALID_HANDLE_VALUE,"Process snapshot failed");
    PROCESSENTRY32W p{};p.dwSize=sizeof(p);std::vector<DWORD> candidates;
    require(Process32FirstW(snap.h,&p)!=0,"Process enumeration failed");
    do {
        if(_wcsicmp(p.szExeFile,L"Shogun2.exe")==0 || _wcsicmp(p.szExeFile,L"Shogun2.retail.exe")==0) {
            if(!requested || requested==p.th32ProcessID)candidates.push_back(p.th32ProcessID);
        }
    } while(Process32NextW(snap.h,&p));
    require(candidates.size()==1,"Expected one Shogun 2 process; start the game or use --pid");return candidates[0];
}
inline Module find_module(DWORD pid) {
    Handle snap(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32,pid));
    require(snap.h!=INVALID_HANDLE_VALUE,"Module snapshot failed");
    MODULEENTRY32W m{};m.dwSize=sizeof(m);Module found;
    require(Module32FirstW(snap.h,&m)!=0,"Module enumeration failed");
    do {
        if(_wcsicmp(m.szModule,L"Empire.Retail.dll")==0) {
            found={reinterpret_cast<uintptr_t>(m.modBaseAddr),m.modBaseSize,m.szExePath};break;
        }
    } while(Module32NextW(snap.h,&m));
    require(found.base!=0,"Empire.Retail.dll is not loaded yet");return found;
}

