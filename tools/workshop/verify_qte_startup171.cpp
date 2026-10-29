// Exercise the actual QTE DLL's early worker, not an observer or a copied gate.
// The engine is mapped without initialization. Deliberately omit the adapter:
// startup must fail before hooks, renderers or any engine function can run.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <filesystem>
#include <array>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
static void need171(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
struct Prefix171 {DWORD magic,version,pid,size;LONG stop,ready,mode,error;};
int wmain(int argc,wchar_t** argv){try{
    need171(argc==4||argc==5,"Use named Empire.Retail.dll, actual QTE DLL, expected early failure and optional owned hook-conflict RVA");
    need171(!_wcsicmp(std::filesystem::path(argv[1]).filename().c_str(),L"Empire.Retail.dll"),"Engine fixture must retain its module basename");
    need171(GetModuleHandleW(L"possession-v46.dll")==nullptr,"Fixture must not load the movement adapter");
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    auto reservation=VirtualAlloc((void*)0x10000000,0x1d88000,MEM_RESERVE,PAGE_NOACCESS);
    auto image=LoadLibraryExW(argv[1],nullptr,DONT_RESOLVE_DLL_REFERENCES);
    need171(image!=nullptr,"Cannot map isolated engine without initialization");
    if(reservation)need171((uintptr_t)image!=0x10000000,"Owned ASLR fixture stayed at preferred base");
    auto base=(unsigned char*)image;
    auto dos=(IMAGE_DOS_HEADER*)base;auto nt=(IMAGE_NT_HEADERS32*)(base+dos->e_lfanew);
    need171(nt->Signature==IMAGE_NT_SIGNATURE&&nt->OptionalHeader.SizeOfImage==0x1d88000,"Unexpected isolated image shape");
    const unsigned hooks[]={0x3e5d40,0x3eaac0,0x3cea10,0x433b80,0x3e1930,0x3e3de0,0xb92800,0x3e5aa0,0x3eaa10,0x3c4170};
    if(argc==5){
        wchar_t* end=nullptr;auto rv=wcstoul(argv[4],&end,0);
        need171(end&&!*end&&rv==hooks[0],"Only the owned mapped QTE hook may be altered");
        DWORD old=0,ignored=0;need171(VirtualProtect(base+rv,1,PAGE_EXECUTE_READWRITE,&old)!=0,"Cannot stage owned hook conflict");
        base[rv]^=1;need171(VirtualProtect(base+rv,1,old,&ignored)!=0,"Cannot restore owned page protection");
    }
    std::array<unsigned char,sizeof hooks/sizeof *hooks*16> before{};
    for(unsigned i=0;i<std::size(hooks);++i)memcpy(before.data()+i*16,base+hooks[i],16);
    wchar_t mapping[80]{};swprintf_s(mapping,L"Local\\Shogun2QteObserver79-%lu",GetCurrentProcessId());
    auto prior=OpenFileMappingW(FILE_MAP_READ,FALSE,mapping);if(prior)CloseHandle(prior);
    need171(!prior,"Fixture status mapping already exists");
    auto qte=LoadLibraryW(argv[2]);need171(qte!=nullptr,"Cannot load actual QTE component");
    auto error=(unsigned(__cdecl*)(char*,unsigned))GetProcAddress(qte,"S2UPInitializationError168");
    if(!error)error=(unsigned(__cdecl*)(char*,unsigned))GetProcAddress(qte,"_S2UPInitializationError168");
    need171(error!=nullptr,"Actual QTE error port is missing");
    std::string actual;bool failed=false;auto deadline=GetTickCount64()+5000;
    while(GetTickCount64()<deadline){
        char text[1025]{};if(error(text,sizeof text))actual=text;
        auto channel=OpenFileMappingW(FILE_MAP_READ,FALSE,mapping);
        if(channel){auto prefix=(const Prefix171*)MapViewOfFile(channel,FILE_MAP_READ,0,0,sizeof(Prefix171));
            if(prefix){need171(prefix->magic==0x31545153&&prefix->version==79&&prefix->pid==GetCurrentProcessId(),"Wrong actual QTE status identity");
                failed=prefix->error==1&&!prefix->ready;UnmapViewOfFile(prefix);}CloseHandle(channel);}
        if(failed&&!actual.empty())break;Sleep(5);
    }
    std::wstring expected_wide=argv[3];std::string expected(expected_wide.begin(),expected_wide.end());
    bool matching=actual==expected;
    if(!expected.empty()&&expected.back()=='*')matching=actual.compare(0,expected.size()-1,expected,0,expected.size()-1)==0;
    auto prefix=expected+" (Win32=";
    if(actual.compare(0,prefix.size(),prefix)==0&&actual.size()>prefix.size()+1&&actual.back()==')'){
        matching=true;for(size_t i=prefix.size();i+1<actual.size();++i)if(actual[i]<'0'||actual[i]>'9')matching=false;
    }
    if(!matching)throw std::runtime_error("Actual QTE startup reason: "+actual+"; expected: "+expected);
    need171(failed,"Actual QTE worker did not publish its controlled refusal");
    for(unsigned i=0;i<std::size(hooks);++i)need171(!memcmp(before.data()+i*16,base+hooks[i],16),"Controlled startup changed an engine hook");
    std::cout<<"PASS actual QTE worker reached: "<<actual<<"; all10 hook prefixes unchanged; engine initialization/functions/rendering not reached.\n";
    // Owned helper exits: do not unload a DLL while its failure tail returns.
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
