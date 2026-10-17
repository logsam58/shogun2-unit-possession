#include "integrated.h"
#include "setup_hashes.h"
#include <commdlg.h>
#include <shellapi.h>
static const wchar_t* PACK=L"shogun2_possession_checkpoint45.pack";
static std::wstring resource_hash(HMODULE module,int id){
    HRSRC item=FindResourceW(module,MAKEINTRESOURCEW(id),MAKEINTRESOURCEW(10));require(item!=nullptr,"Installer payload missing");auto bytes=static_cast<PUCHAR>(LockResource(LoadResource(module,item)));DWORD size=SizeofResource(module,item);
    BCRYPT_ALG_HANDLE alg=nullptr;BCRYPT_HASH_HANDLE hash=nullptr;require(BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)==0,"Hash provider failed");
    UCHAR result[32];NTSTATUS code=BCryptCreateHash(alg,&hash,nullptr,0,nullptr,0,0);if(code==0)code=BCryptHashData(hash,bytes,size,0);if(code==0)code=BCryptFinishHash(hash,result,32,0);if(hash)BCryptDestroyHash(hash);BCryptCloseAlgorithmProvider(alg,0);require(code==0,"Installer hash failed");
    std::wostringstream text;for(auto value:result)text<<std::hex<<std::setw(2)<<std::setfill(L'0')<<unsigned(value);return text.str();
}
static bool acceptable(const std::wstring& path,const char* current,std::initializer_list<const char*> prior){if(GetFileAttributesW(path.c_str())==INVALID_FILE_ATTRIBUTES)return true;auto hash=hash_file(path);if(hash==current)return true;for(auto value:prior)if(hash==value)return true;return false;}
static void put_resource(const std::wstring& path,int id,const char* expected){
    std::wstring temporary=path+L".checkpoint45-new";
    HMODULE module=GetModuleHandleW(nullptr);HRSRC item=FindResourceW(module,MAKEINTRESOURCEW(id),MAKEINTRESOURCEW(10));require(item!=nullptr,"Installer resource missing");
    auto bytes=LockResource(LoadResource(module,item));DWORD size=SizeofResource(module,item);Handle file(CreateFileW(temporary.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));require(file.h!=INVALID_HANDLE_VALUE,"Cannot write mod file; existing staging file preserved");
    DWORD written=0;require(WriteFile(file.h,bytes,size,&written,nullptr)&&written==size,"Cannot write mod file");CloseHandle(file.h);file.h=nullptr;
    require(hash_file(temporary)==expected,"Installer payload integrity failed");require(MoveFileExW(temporary.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0,"Cannot install mod file");
}
static bool running(){Handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0));require(snapshot.h!=INVALID_HANDLE_VALUE,"Process inventory failed");PROCESSENTRY32W entry{};entry.dwSize=sizeof entry;require(Process32FirstW(snapshot.h,&entry)!=0,"Process inventory unavailable");do{if(!_wcsicmp(entry.szExeFile,L"shogun2.exe")||!_wcsicmp(entry.szExeFile,L"shogun2.retail.exe"))return true;}while(Process32NextW(snapshot.h,&entry));return false;}
static std::wstring registry(HKEY key,const wchar_t* subkey,const wchar_t* value){wchar_t text[4096];DWORD size=sizeof text;return RegGetValueW(key,subkey,value,RRF_RT_REG_SZ,nullptr,text,&size)==ERROR_SUCCESS?text:L"";}
static std::wstring locate(){
    std::vector<std::wstring> roots={registry(HKEY_CURRENT_USER,L"Software\\Valve\\Steam",L"SteamPath"),registry(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Valve\\Steam",L"InstallPath"),known_folder(CSIDL_PROGRAM_FILES)+L"\\Steam"};
    auto original=roots;for(auto& root:original){if(root.empty())continue;std::wstring libraries=read_text(root+L"\\steamapps\\libraryfolders.vdf");std::wregex pattern(L"\"path\"\\s*\"([^\"]+)\"");for(auto it=std::wsregex_iterator(libraries.begin(),libraries.end(),pattern);it!=std::wsregex_iterator();++it){auto path=(*it)[1].str();path=std::regex_replace(path,std::wregex(L"\\\\\\\\"),L"\\");roots.push_back(path);}}
    for(auto& root:roots){if(root.empty())continue;auto candidate=root+L"\\steamapps\\common\\Total War SHOGUN 2";auto engine=candidate+L"\\Empire.Retail.dll";if(GetFileAttributesW(engine.c_str())!=INVALID_FILE_ATTRIBUTES&&hash_file(engine)==EXPECTED_HASH)return candidate;}
    wchar_t file[MAX_PATH]={};OPENFILENAMEW dialog{};dialog.lStructSize=sizeof dialog;dialog.lpstrFilter=L"Shogun 2 executable\0Shogun2.exe;Shogun2.retail.exe\0\0";dialog.lpstrFile=file;dialog.nMaxFile=MAX_PATH;dialog.lpstrTitle=L"Select your Shogun 2 executable";dialog.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR;
    require(GetOpenFileNameW(&dialog)!=0,"Game selection cancelled");std::wstring path=file;return path.substr(0,path.find_last_of(L"\\/"));
}
static std::string append_selection(std::string bytes){
    bool le=bytes.size()>1&&((static_cast<unsigned char>(bytes[0])==255&&static_cast<unsigned char>(bytes[1])==254)||bytes[1]==0);
    bool be=bytes.size()>1&&static_cast<unsigned char>(bytes[0])==254&&static_cast<unsigned char>(bytes[1])==255;
    std::string directive="\r\nmod shogun2_possession_checkpoint45.pack;\r\n";
    if(bytes.empty()){bytes="\xff\xfe";le=true;}
    if(le||be){for(auto c:directive){if(be){bytes+='\0';bytes+=c;}else{bytes+=c;bytes+='\0';}}}else bytes+=directive;
    return bytes;
}
static void activate_path(const std::wstring& path){
    if(selected_text(read_text(path)))return;
    auto folder=path.substr(0,path.find_last_of(L'\\'));require(SHCreateDirectoryExW(nullptr,folder.c_str(),nullptr)==ERROR_SUCCESS||GetFileAttributesW(folder.c_str())!=INVALID_FILE_ATTRIBUTES,"Mod selection folder unavailable");
    std::ifstream original(path,std::ios::binary);std::string bytes;if(original)bytes.assign(std::istreambuf_iterator<char>(original),{});original.close();
    bytes=append_selection(bytes);
    if(GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES){auto backup=path+L".checkpoint45-before-"+std::to_wstring(GetTickCount64());require(CopyFileW(path.c_str(),backup.c_str(),TRUE)!=0,"Cannot preserve previous mod selection");}
    auto temporary=path+L".checkpoint45-new";Handle file(CreateFileW(temporary.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));require(file.h!=INVALID_HANDLE_VALUE,"Cannot stage mod selection");DWORD written=0;require(WriteFile(file.h,bytes.data(),DWORD(bytes.size()),&written,nullptr)&&written==bytes.size(),"Cannot save mod selection");CloseHandle(file.h);file.h=nullptr;
    require(MoveFileExW(temporary.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0,"Cannot activate mod");
}
int WINAPI wWinMain(HINSTANCE,HINSTANCE,PWSTR,int){bool quiet=false;try{
    int count=0;auto arguments=CommandLineToArgvW(GetCommandLineW(),&count);require(arguments!=nullptr,"Installer arguments unavailable");std::wstring game;bool check=false;
    for(int i=1;i<count;++i){if(!wcscmp(arguments[i],L"--quiet"))quiet=true;else if(!wcscmp(arguments[i],L"--check-only"))check=true;else if(!wcscmp(arguments[i],L"--game")&&i+1<count)game=arguments[++i];else throw std::runtime_error("Use --game <folder>, --check-only, or --quiet");}LocalFree(arguments);
    if(game.empty())game=locate();require(hash_file(game+L"\\Empire.Retail.dll")==EXPECTED_HASH,"Unsupported Shogun 2 engine build");
    auto dll=game+L"\\DINPUT8.dll",pack=game+L"\\data\\"+PACK;
    require(acceptable(dll,SETUP_BOOTSTRAP_SHA,{"b5a88d2a5f094fe475335d727080acd37c51d5142893ca76dbde9d6295951798","19e609fd3f1cfc4c4142c8c25e5c9a8541d7c99e62b29f8c835d21d9809ebce4","5329483e83fbd0dc736ebc77166a0b2ba59678880d76601ffa360ce4b5d8d0a4"}),"An unrelated DINPUT8.dll is installed; it was preserved");
    require(acceptable(pack,EMBEDDED_PACK_SHA,{"7df2821e37621d47eb30033f67d1ea61b95594f2f2e9d3939c99097cbef4b52c","cfff09b8b1b16838be51f44c4377a12903c2780cb5b67425631183bddef999ae","ebf1f3a075dde4054ec3e9c16b87f0b34c35a33ff17e972ec11ff75b37221fd1","257a460691c72d8d3e16e195f00da30a162f1039424e26d2e374f287d0e9f422"}),"An unrelated pack occupies this filename; it was preserved");
    auto widen=[](const char* p){return std::wstring(p,p+strlen(p));};require(resource_hash(GetModuleHandleW(nullptr),201)==widen(SETUP_BOOTSTRAP_SHA)&&resource_hash(GetModuleHandleW(nullptr),202)==widen(EMBEDDED_PACK_SHA),"Installer embedded payload differs");
    if(!check){require(!running(),"Please close Shogun 2 before installing");
        put_resource(pack,202,EMBEDDED_PACK_SHA);put_resource(dll,201,SETUP_BOOTSTRAP_SHA);activate_path(known_folder(CSIDL_APPDATA)+L"\\The Creative Assembly\\Shogun2\\scripts\\user.script.txt");}
    if(!quiet)MessageBoxW(nullptr,check?L"Installer and installed game verified.":L"Possession checkpoint45 installed and enabled.\n\nLaunch Shogun 2 normally through Steam, load a single-player land battle, select infantry and press F9.\n\nNo companion launcher or ZIP is needed.",L"Shogun 2 Possession",MB_OK|MB_ICONINFORMATION);return 0;
}catch(const std::exception& error){if(!quiet){std::string text=error.what();std::wstring message(text.begin(),text.end());MessageBoxW(nullptr,message.c_str(),L"Shogun 2 Possession installation",MB_OK|MB_ICONERROR);}return 1;}}
