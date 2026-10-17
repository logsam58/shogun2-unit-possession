#pragma once
#include <array>
// Steam's documented HasRunKey checks, restricted to this app and these two
// obsolete prerequisite entries. No Adobe runtime, system policy or native
// engine imports are modified. Keep the publisher-signed installscript intact.
namespace flashstartup86 {
static constexpr const wchar_t* names[2]={L"Flash Player 11",L"Flash security exception"};
#ifdef VERIFY_FIXTURE
static HKEY hive=HKEY_CURRENT_USER;
static std::wstring key_path=L"Software\\Shogun2PossessionFlashFixture-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetTickCount64());
struct FixtureCleanup {~FixtureCleanup(){RegDeleteKeyExW(hive,key_path.c_str(),KEY_WOW64_32KEY,0);}};
static FixtureCleanup fixture_cleanup;
#else
static HKEY hive=HKEY_LOCAL_MACHINE;
static const std::wstring key_path=L"Software\\Valve\\Steam\\Apps\\34330";
#endif
struct Key {HKEY value{};~Key(){if(value)RegCloseKey(value);}};
struct Value {bool exists{};DWORD value{};};
struct Snapshot {
    std::array<Value,2> before{};bool key_existed{},changed{};
    Snapshot(){Key key;auto r=RegOpenKeyExW(hive,key_path.c_str(),0,KEY_QUERY_VALUE|KEY_WOW64_32KEY,&key.value);
        if(r==ERROR_FILE_NOT_FOUND)return;
        require(r==ERROR_SUCCESS,"Cannot inspect Steam's Shogun 2 Flash prerequisite flags");key_existed=true;
        for(unsigned i=0;i<2;++i){DWORD type{},size=sizeof(DWORD),value{};r=RegQueryValueExW(key.value,names[i],nullptr,&type,reinterpret_cast<BYTE*>(&value),&size);
            if(r==ERROR_FILE_NOT_FOUND)continue;
            require(r==ERROR_SUCCESS&&type==REG_DWORD&&size==sizeof(DWORD),"Unrecognized Steam Flash prerequisite flag; preserved");before[i]={true,value};}}
    bool skipped() const {return before[0].exists&&before[0].value>=1&&before[1].exists&&before[1].value>=1;}
    void apply(int fail_after=-1){Key key;
        require(RegCreateKeyExW(hive,key_path.c_str(),0,nullptr,0,KEY_QUERY_VALUE|KEY_SET_VALUE|KEY_WOW64_32KEY,nullptr,&key.value,nullptr)==ERROR_SUCCESS,
            "Cannot disable the obsolete Flash setup steps. Run this installer as administrator.");
        for(unsigned i=0;i<2;++i){DWORD one=before[i].exists&&before[i].value>=1?before[i].value:1;
            require(RegSetValueExW(key.value,names[i],0,REG_DWORD,reinterpret_cast<const BYTE*>(&one),sizeof one)==ERROR_SUCCESS,"Cannot update Steam Flash prerequisite flag");
            changed=true;
            require(fail_after!=int(i),"Simulated Flash flag interruption");}
        require(Snapshot{}.skipped(),"Steam Flash prerequisite verification failed");}
    void restore(){Key key;
        if(!changed)return;
        auto r=RegOpenKeyExW(hive,key_path.c_str(),0,KEY_QUERY_VALUE|KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value);
        if(r==ERROR_FILE_NOT_FOUND){require(!before[0].exists&&!before[1].exists,"Cannot restore missing Steam prerequisite key");return;}
        require(r==ERROR_SUCCESS,"Cannot restore Steam Flash prerequisite flags");
        for(unsigned i=0;i<2;++i){r=before[i].exists?RegSetValueExW(key.value,names[i],0,REG_DWORD,reinterpret_cast<const BYTE*>(&before[i].value),sizeof(DWORD)):RegDeleteValueW(key.value,names[i]);
            require(r==ERROR_SUCCESS||(!before[i].exists&&r==ERROR_FILE_NOT_FOUND),"Cannot restore previous Steam Flash prerequisite flag");}
        DWORD subkeys{},values{};
        if(!key_existed&&RegQueryInfoKeyW(key.value,nullptr,nullptr,nullptr,&subkeys,nullptr,nullptr,&values,nullptr,nullptr,nullptr,nullptr)==ERROR_SUCCESS&&!subkeys&&!values){
            RegCloseKey(key.value);key.value=nullptr;RegDeleteKeyExW(hive,key_path.c_str(),KEY_WOW64_32KEY,0);}}
    std::string restore_file() const {
        std::wstring text=L"Windows Registry Editor Version 5.00\r\n\r\n; Restore only the previous Shogun 2 Flash prerequisite flags.\r\n; This file does not install or remove Flash.\r\n[";
#ifdef VERIFY_FIXTURE
        text+=L"HKEY_CURRENT_USER\\"+key_path;
#else
        BOOL wow=FALSE;require(IsWow64Process(GetCurrentProcess(),&wow)!=0,"Cannot determine Steam registry view");
        text+=wow?L"HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam\\Apps\\34330":L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Valve\\Steam\\Apps\\34330";
#endif
        text+=L"]\r\n";
        for(unsigned i=0;i<2;++i){text+=L"\"";text+=names[i];text+=L"\"=";
            if(!before[i].exists)text+=L"-\r\n";
            else{wchar_t number[32];swprintf_s(number,L"dword:%08x\r\n",before[i].value);text+=number;}}
        std::string bytes="\xff\xfe";bytes.append(reinterpret_cast<const char*>(text.data()),text.size()*sizeof(wchar_t));return bytes;}
};
}
