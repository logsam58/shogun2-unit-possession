#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "pack165.h"
using workshop165::check;
int wmain(int argc,wchar_t** argv){try{
    if(argc==3&&std::wstring(argv[1])==L"--owned-child"){
        workshop165::LiveReceipt166 receipt;
        check(receipt.write(argv[2],{'l','i','v','e'}),"Child receipt creation failed");
        HANDLE root168=workshop165::transient_root168(std::wstring(argv[2])+L".root",{'u','i'});
        (void)root168; // Owned child is intentionally killed below; OS closes it.
        Sleep(10000);return 0;
    }
    check(argc==2,"Use a new fixture output directory");
    std::filesystem::path out=argv[1];workshop165::plain(out);
    check(!std::filesystem::exists(out)&&std::filesystem::create_directories(out),"Fixture output exists");
    auto path=out/L"receipt.json";
    for(unsigned i=0;i<100;++i){
        {workshop165::LiveReceipt166 receipt;check(receipt.write(path,{'o','k'}),"Receipt creation failed");
         HANDLE reader=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr);
         check(reader!=INVALID_HANDLE_VALUE,"Live receipt unreadable");char text[2]{};DWORD n=0;
         check(ReadFile(reader,text,2,&n,nullptr)&&n==2&&text[0]=='o'&&text[1]=='k',"Receipt content differs");CloseHandle(reader);}
        check(!std::filesystem::exists(path),"Closed receipt accumulated");
    }
    {std::ofstream foreign(path);foreign<<"foreign";}
    {workshop165::LiveReceipt166 receipt;check(!receipt.write(path,{'n','o'}),"Foreign receipt overwritten");}
    check(std::filesystem::file_size(path)==7,"Foreign receipt removed");std::filesystem::remove(path);
    {workshop165::LiveReceipt166 receipt;check(!receipt.write(path,std::vector<unsigned char>(4097,'x'))&&!std::filesystem::exists(path),"Oversized receipt accepted");}
    wchar_t exe[32768]{};check(GetModuleFileNameW(nullptr,exe,32768)>0,"Fixture path unavailable");
    for(unsigned i=0;i<5;++i){
        std::wstring command=L"\""+std::wstring(exe)+L"\" --owned-child \""+path.wstring()+L"\"";
        STARTUPINFOW si{};si.cb=sizeof si;PROCESS_INFORMATION pi{};
        check(CreateProcessW(exe,&command[0],nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&si,&pi)!=0,"Owned fixture child failed");
        CloseHandle(pi.hThread);
        auto root168=std::filesystem::path(path.wstring()+L".root");
        for(unsigned n=0;n<100&&(!std::filesystem::exists(path)||!std::filesystem::exists(root168));++n)Sleep(20);
        bool created=std::filesystem::exists(path)&&std::filesystem::exists(root168);TerminateProcess(pi.hProcess,166);
        check(WaitForSingleObject(pi.hProcess,5000)==WAIT_OBJECT_0,"Owned fixture child did not exit");CloseHandle(pi.hProcess);
        check(created&&!std::filesystem::exists(path)&&!std::filesystem::exists(root168),"Forced-exit receipt/UI root accumulated");
    }
    std::vector<unsigned char> bytes={'c','a','c','h','e'};
    auto target=out/L"owned-cache.luac",pending=std::filesystem::path(target.wstring()+L".pending166");
    {std::ofstream f(pending,std::ios::binary);f.write((char*)bytes.data(),bytes.size());}
    workshop165::create_exact(target,bytes);check(!std::filesystem::exists(pending),"Complete interrupted stage not reused");
    std::filesystem::remove(target);
    {std::ofstream foreign(pending);foreign<<"foreign";}
    for(unsigned i=0;i<20;++i){bool refused=false;try{workshop165::create_exact(target,bytes);}catch(...){refused=true;}check(refused,"Foreign pending file accepted");}
    check(std::filesystem::file_size(pending)==7&&!std::filesystem::exists(target),"Foreign staging changed");
    check(std::distance(std::filesystem::directory_iterator(out),std::filesystem::directory_iterator{})==1,"Repeated failure accumulated files");
    std::cout<<"PASS 100 receipt lifetimes, five owned forced exits, size bound, foreign preservation, interrupted stage recovery and repeated-failure file count.\n";
    return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
