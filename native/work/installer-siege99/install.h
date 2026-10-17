#pragma once
#define wWinMain legacy_installer_entry
#include "../autoload99-preview/setup.cpp"
#undef wWinMain
#include <map>
#include <functional>
struct Json {
    size_t begin{},end{}; char kind{}; std::string value;
    std::vector<Json> items; std::map<std::string,Json> fields;
};
// Parse before changing the launcher's cache; preserve all unrelated JSON bytes.
struct JsonReader {
    const std::string& s; size_t p{};
    void ws(){while(p<s.size()&&isspace(static_cast<unsigned char>(s[p])))++p;}
    char take(){require(p<s.size(),"Incomplete launcher settings");return s[p++];}
    std::string str(){require(take()=='"',"Invalid launcher settings");std::string out;
        while(p<s.size()){char c=take();if(c=='"')return out;require(static_cast<unsigned char>(c)>=32,"Invalid JSON string");
            if(c=='\\'){c=take();if(c=='u'){unsigned n=0;for(int i=0;i<4;++i){char h=take();require(isxdigit(static_cast<unsigned char>(h))!=0,"Invalid JSON escape");n=n*16+(h<='9'?h-'0':tolower(h)-'a'+10);}out+=n<128?char(n):'?';}
                else{require(std::string("\"\\/bfnrt").find(c)!=std::string::npos,"Invalid JSON escape");out+=c;}}
            else out+=c;}throw std::runtime_error("Unterminated JSON string");}
    Json read(unsigned depth=0){require(depth<64,"Launcher settings nesting too deep");ws();Json j;j.begin=p;require(p<s.size(),"Empty launcher settings");j.kind=s[p];
        if(j.kind=='"')j.value=str();
        else if(j.kind=='['||j.kind=='{'){++p;ws();char close=j.kind=='['?']':'}';if(p<s.size()&&s[p]==close)++p;
            else while(true){if(j.kind=='{'){ws();auto key=str();ws();require(take()==':',"Invalid JSON object");require(j.fields.count(key)==0,"Duplicate launcher setting");j.fields.emplace(key,read(depth+1));}else j.items.push_back(read(depth+1));ws();char c=take();if(c==close)break;require(c==',',"Invalid JSON separator");}}
        else{size_t start=p;while(p<s.size()&&s[p]!=','&&s[p]!=']'&&s[p]!='}'&&!isspace(static_cast<unsigned char>(s[p])))++p;j.value=s.substr(start,p-start);
            require(j.value=="true"||j.value=="false"||j.value=="null"||std::regex_match(j.value,std::regex("-?(0|[1-9][0-9]*)(\\.[0-9]+)?([eE][+-]?[0-9]+)?")),"Invalid JSON value");}
        j.end=p;return j;}
    Json document(){if(s.substr(0,3)=="\xef\xbb\xbf")p=3;auto j=read();ws();require(p==s.size(),"Trailing launcher settings data");return j;}
};
static std::string utf8(const std::wstring& s){int n=WideCharToMultiByte(CP_UTF8,0,s.data(),int(s.size()),nullptr,0,nullptr,nullptr);std::string out(n,'\0');if(n)WideCharToMultiByte(CP_UTF8,0,s.data(),int(s.size()),&out[0],n,nullptr,nullptr);return out;}
static std::string json_string(const std::string& s){std::string out="\"";for(unsigned char c:s){if(c=='"'||c=='\\')out+='\\';require(c>=32,"Invalid path character");out+=char(c);}return out+'"';}
static std::string launcher_selection(std::string bytes,const std::wstring& game){
    if(bytes.empty())bytes="[]";auto root=JsonReader{bytes}.document();require(root.kind=='[',"Unrecognized launcher settings; preserved");
    for(auto& j:root.items){require(j.kind=='{',"Unrecognized launcher mod entry");auto g=j.fields.find("game"),id=j.fields.find("uuid");
        if(g!=j.fields.end()&&id!=j.fields.end()&&g->second.value=="shogun2"&&id->second.value=="shogun2_possession_checkpoint45.pack"){
            auto a=j.fields.find("active");require(a!=j.fields.end()&&(a->second.value=="true"||a->second.value=="false"),"Unrecognized launcher activation flag");
            bytes.replace(a->second.begin,a->second.end-a->second.begin,"true");return bytes;}}
    std::wstring path=game+L"\\data\\"+PACK;std::replace(path.begin(),path.end(),L'\\',L'/');
    std::string item="{\"uuid\":\"shogun2_possession_checkpoint45.pack\",\"order\":"+std::to_string(root.items.size()+1)+",\"active\":true,\"game\":\"shogun2\",\"packfile\":"+json_string(utf8(path))+",\"name\":\"shogun2_possession_checkpoint45.pack\",\"short\":\"\",\"category\":\"\",\"owned\":true}";
    bytes.insert(root.end-1,(root.items.empty()?"":",")+item);JsonReader{bytes}.document();return bytes;
}
static std::string read_bytes(const std::wstring& path){std::ifstream in(path,std::ios::binary);if(!in){require(GetFileAttributesW(path.c_str())==INVALID_FILE_ATTRIBUTES,"Cannot read existing installation file");return {};}
    std::string bytes((std::istreambuf_iterator<char>(in)),{});require(bytes.size()<128*1024*1024,"Installation file too large");return bytes;}
static void atomic_bytes(const std::wstring& path,const std::string& bytes){auto folder=path.substr(0,path.find_last_of(L"\\/"));int r=SHCreateDirectoryExW(nullptr,folder.c_str(),nullptr);require(r==ERROR_SUCCESS||r==ERROR_ALREADY_EXISTS,"Cannot create installation folder");
    auto tmp=path+L".possession-setup-new";Handle f(CreateFileW(tmp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));require(f.h!=INVALID_HANDLE_VALUE,"Cannot stage installation file");
    try{DWORD n=0;require(WriteFile(f.h,bytes.data(),DWORD(bytes.size()),&n,nullptr)&&n==bytes.size(),"Cannot write installation file");require(FlushFileBuffers(f.h)!=0,"Cannot flush installation file");CloseHandle(f.h);f.h=nullptr;require(MoveFileExW(tmp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0,"Cannot replace installation file");}
    catch(...){if(f.h&&f.h!=INVALID_HANDLE_VALUE){CloseHandle(f.h);f.h=nullptr;}DeleteFileW(tmp.c_str());throw;}}
#include "installer_retention.h"
#include "flash_startup86.h"
struct SavedFile {std::wstring path,backup;std::string bytes;bool existed{},changed{};};
static void validate_game(const std::wstring& game){require(hash_file(game+L"\\Empire.Retail.dll")==EXPECTED_HASH,"This game version is unsupported. Select the Steam Shogun 2 installation.");
    require(acceptable(game+L"\\DINPUT8.dll",SETUP_BOOTSTRAP_SHA,{"d18e70424a90eca1545b4bd157e90443d51a3308c57eca5aadb69cf8ad1ecf30","d24dc97f82d95203ace03994705211ebba525ce4edd1f7429652dcd841900717","c25cf6830696c1432d8ba13ff2737fc99faa7963b5dc760fd144e928a6f8a3ab","a0473759fda9bee465bf7fd4a4c535dc9ca07fd205414b61857401cb1ec1f4c3","a886bf22f9794dba560ac42257603b642f11b056c88f71eaa4608ad8185bdfb1","70ca5384c3d9c1c7fbaa2e87f187a5ed357c95dede215d1c0b75432361194e6b","6083a02a6ab375d9f0a7fa6de6bb74b5fd4cba8fcc0f00f065abb4df782d4a7c","8b68371f8b119d07558449aa152833974807eca3d97fd6d862ebebe0db1707ec","e0a08238b3f45e49d12920f4b514ad733bb8ff0b2b322f213bcef3afb65e6b75","9764156a7a82de441813035f866de377fd5c00137b564cc1af611e8dfe9a3d19","ec075725151cce8ad49ff0ab1129e61688adbed70622b57bcdfa28df0979fd27","156246b561d1281c3021f577e5c1ca516ef8c7989d4f6ee83677ca261c100d50","b665ded36fa703e74d5d1a43664d28356daa415688ab6e066026acf0d0f02615","1d3981d46bff3478892015cd259a51d55c3a21269ce3d697bf131db6345f1571","71b0c110fef8975a222c397c0ed9d9f65780450444b6b30889b6bde2d1df3cae","1d3981d46bff3478892015cd259a51d55c3a21269ce3d697bf131db6345f1571","1097dfd31f31eb66187ecfe3605cb0164e031ba431d8fb49d3eef7779b0ba008","1d3981d46bff3478892015cd259a51d55c3a21269ce3d697bf131db6345f1571","f5741f415c77b9b3239f9eb64fb5f66a6d3eb8de2790c82abfba2a76ef7347a2","1d3981d46bff3478892015cd259a51d55c3a21269ce3d697bf131db6345f1571","765a4389da88c0722672d8a174a66475988c93e7a9a9a136901932e48646e8e9","1d3981d46bff3478892015cd259a51d55c3a21269ce3d697bf131db6345f1571","d2ceac7092973d7f81a464fdaef5971839f355efa859ba39ecaf974631ce216c","1d3981d46bff3478892015cd259a51d55c3a21269ce3d697bf131db6345f1571","486956fee44eaae4068428f0b3832cf778841e5686a80642a8a8b9126897dc96","1d3981d46bff3478892015cd259a51d55c3a21269ce3d697bf131db6345f1571","fba97aef1819311e15835efd4667e28a48d3ce60d7442799d1b425d79e7362fe","1d3981d46bff3478892015cd259a51d55c3a21269ce3d697bf131db6345f1571","90f78541984fb8e84fa04d8963f09cbaf49cc7947a73a7f553cf1363939691b6","1d3981d46bff3478892015cd259a51d55c3a21269ce3d697bf131db6345f1571","f020f2b703a50d56c6acff8d145603529659ac1d683d1197c8a960fa1c1f8c70","415195c782c12e7db09fc0f71f849384f2c5e606ce0da53b506b27e387cea48f","415195c782c12e7db09fc0f71f849384f2c5e606ce0da53b506b27e387cea48f","d33af676f0abf035d5be14bd6adc1ffdadafeab256022ceafc4ae1b3cd2e8c1e","1ee93d20601b1c564d94a5b70ca995cc4bbc1c179a61b7c5364836b654eadbd8","2ff440a9034a39d19b2f6d5b17ccb0102c5b8e6fce86fa85996305ff051cf1ed","9ad3089c7879c3e7f359a15eb850547aeeb26638bc17189960691b1eeda6ec5e","04c2b2a082ccbd0c05e760887eb8e5e64a7cba85bf1975e584f6b79b0dae1002","da9f513f3c70fb5cc8de64c96dcf9d34b046f9bb9dc90cbccaf72daffeb6f966","a870a5304dc0592dfb742054b2dcd7febbdd44ab4febe20d6f973983781c8ebd","b5c759a8eb07b32ed0e9b7608e145e632f7351edccdc028ca0543d148d70516d","52e70fbaea7a4cd1597d8e43edf90e4fe77e3f50f0e30656d649f858a7abf41f","f6bc3a144d8db71d0932abe787337b95e8c63e090a11805b60897fd4534e1a61","f6983dd5176cc3af7daff36b2d3fd14ee4a54768d759268a481cb022d43eb6e4","f879198d6ab4ec841d0047366ffe23ed074e62b78a0398c8c6e404b8ee7f9d94","19e609fd3f1cfc4c4142c8c25e5c9a8541d7c99e62b29f8c835d21d9809ebce4","279c4522d098f37abad76f097b982a9fa60619017cdfd8e3e90dbbe68fdba04d","a00707e2d4026e6403f29c82dad3c20f4f0ddcbb1011e50cbb728cbb6128e7bb","5329483e83fbd0dc736ebc77166a0b2ba59678880d76601ffa360ce4b5d8d0a4","aa65b8646f79ae1d8fa5b1f4407a5cb6ab34d6a9eb67b32c532aecda820160da","4c809dec3053ba91e18f68193f5f6f481517882cbf89ef745fbcf00d1ae3abf6","84466944e9f7b2ca599d50ce8a868c26ac2b32133202ca892b969501ab75536d","f17056dbe32c28aa9d350aab9af237e17908bb517c85b013ddff1e216510b765","05e263c50d2e5bb9f6bcd126bbfee28c3831316ca505e94da1f889084c53a9a8","bd55757682472018bb025ab39010b9d7bbe1218c7a0aa65cc17cc0f498d562a9","17d7d087200314e62bb1f2fc7142a19485a427cb13c4774db35e576160d9da82","c5706aa5765f3a2880352c4b8b4ee0b4c5dcab140aedcf14918da5beed9c7f4c","67cdd21573c457202b6d26ab26736898548b20e09c63f46f70a043e8f0836b41","a54fae791bb925e4923efec6f45d72f8c992dfca25b300e99eb751422a1d9e6f"}),"Another mod owns DINPUT8.dll. It has been preserved.");
    require(acceptable(game+L"\\data\\"+PACK,EMBEDDED_PACK_SHA,{"5086260202764f77752a578319e8748bed1d8057f2afd9e3415f9e0213ec9ec5","3ad79c97919e0964c3dd88ffc8f4575652728165a487941d175e22bd30887cd3","8b2eb2f089f7383b1bd0f514b7b10d671fc94a9804fffebefa5558ca3aadcbed","138924f8befbe46c5888eb8cfa38948a7a10ff15b20e4c37f7d7bc1204bf98f0","5dc10951366e6909e7d52d5ff40dec01f78f681d923e1642077c8c75971215ff","99d629cf71309f407f5b2b32e2266c04888b7b891e8e8c4d04933183bb04a8c0","31788c68635c28e115d2b9145022fb67729c99bafb5c9350815f8af714f775d2","9203943eec6b35f8c39a060b5c7df6d1a5dbdb70d2d7439a79a7b1da0c1e2c81","9d7be8da8ad1145f137380a8b1d1ebdb3f942eb4fbfdac280b60f0f5cedf5f91","d159f5faeb4d32adc8ff2236fa1615fa0fa2a307a6d89041e14ce33d94a837d4","b70dd8057f1987e0ffbfc873cc50625fcbe10e18f4c6853b035ec626b5de5d1b","dc442e7c3aca076b323419653e4fe2eb3c575639d7815daaed61fd78c6e4cabe","860b29479724df26d1624da1f6defcac45c588f01798cb4606c5a1689f162d6d","985f0a21458877aa871dcc65c8d63cd0556d5a051530bae73ed78ddb3656ad66","e54dad5e66f1142850a74a67faa36e15ba92dcf8d9a5e52f47d4f1d6ed6ee6e7","985f0a21458877aa871dcc65c8d63cd0556d5a051530bae73ed78ddb3656ad66","d00ea5f2e4ac38c6c52b4c88e7295f7edb02891e97971eab93b9e8eea1e907f2","985f0a21458877aa871dcc65c8d63cd0556d5a051530bae73ed78ddb3656ad66","9f341af472db3679eddf2457a764ac81c269c7c051ea05ac20621b2cb5ba329f","985f0a21458877aa871dcc65c8d63cd0556d5a051530bae73ed78ddb3656ad66","565874937fbb98d2179364cd78a2033350da38b98ff3b147fe8a88177a842eca","985f0a21458877aa871dcc65c8d63cd0556d5a051530bae73ed78ddb3656ad66","060f20e9d710b65d765daa297a4db7dfe9f920a1710ba3e1e9932dc07a60225f","985f0a21458877aa871dcc65c8d63cd0556d5a051530bae73ed78ddb3656ad66","f3fcbca7749c4da3d8034e22863516b843435aa9f6b02b89b44373b744123327","985f0a21458877aa871dcc65c8d63cd0556d5a051530bae73ed78ddb3656ad66","0693281b11323ac60fc381b5ac025d0de406429311a09d4f4ac7c9978ae290e7","985f0a21458877aa871dcc65c8d63cd0556d5a051530bae73ed78ddb3656ad66","66d515f544350a2352ac02d42d01923212d7677400e51cc43677b7e320d189da","985f0a21458877aa871dcc65c8d63cd0556d5a051530bae73ed78ddb3656ad66","0e68f671ed5f3d46860c6af8c81f7be10769e0fa5a31b86fa23b32745de1243d","eb054845dcf4128ffbf0b4c662b7d3c19561028b89e319c4afb91e986f7d09eb","eb054845dcf4128ffbf0b4c662b7d3c19561028b89e319c4afb91e986f7d09eb","d29bfdf6e473c515d5fb2453849737506fa95c253635cf6ea818ae14014af014","83a0c133ea1b03c03d75aacd458705685c863ca1fcdf1d5968f2436c96cb2bc4","6fffd0e3e851ea3c91792e9b2b5563038857d36e934abc028db090dcd6634c27","ec05ec565a3724d674b82b2afa9c43459efd8485c8e7ec16d5b578e634293692","5f6b62513870852569d3a03d9ffffcfb77ec89c95304cbebb7d54bd61740d1ee","d0333d78320be914455e68eabe474ffd8f8c0b08b8fe34336c2bde284f224631","c8765b923734fb2e779a67ddf04e3a889491d4c772a4a935ad2ffad6a8d3f47a","d7f5fa1644c07a4688346f6d609933fadcb3c5604c197fa6462e0b65e236f589","541f78fb379abcb558fd0d306d70beff7c9c0a5498f74ee5aef62d93047ecd66","cfff09b8b1b16838be51f44c4377a12903c2780cb5b67425631183bddef999ae","ebf1f3a075dde4054ec3e9c16b87f0b34c35a33ff17e972ec11ff75b37221fd1","0dade5c375156a9b5515b07da3e1482fa3703c19f418c2b1cab46139ba8eedc1","257a460691c72d8d3e16e195f00da30a162f1039424e26d2e374f287d0e9f422","8df30bd4374524514ee2d05109c9a9d5aa4dfafb6481e7f948c86696d9761623","f4f4b77dfe47b4329f558bc62d83ccdcba6b21248018cf7bbcf72eacfaee8c96","4a7920bc47b8646f3531f91886dbba9bd23d33ed5f7b4a06d9e8d356b7e4c2c5","2adef60d0e26ef18103774a5f5c6be5a4ce4be9850d20f9f4ef2b3c6945d453e","11cac0efe184b3854af11b67b3d72ed63fd3ae67af2bbe78917c67b9273d0dfe","42d2c794b3eead62462a469987f8b0a2049545f5374d8ceafab6335eaf3b54bc","4833f2562d2de3a39031541b7d7ecc47ac5da51c6f65024c0a645f4d6b7ffb56","78ed9bb62c58fcd264c6b87c43e3c3f04eb25b3753043985dd8fe31851176809","dfc2329546a6584ac14c263ed81a19e3509c6b3e7260356f48a176cb1acbc86a","4d513996a66cd55ac4747428959e5302f9ea01fe7792e6a40a53b6540a9c99a1"}),"Another pack occupies this mod filename. It has been preserved.");}
static std::string payload(int id,const char* expected){auto m=GetModuleHandleW(nullptr);std::wstring hash(expected,expected+strlen(expected));require(resource_hash(m,id)==hash,"Installer payload integrity check failed");auto r=FindResourceW(m,MAKEINTRESOURCEW(id),MAKEINTRESOURCEW(10));return std::string(static_cast<const char*>(LockResource(LoadResource(m,r))),SizeofResource(m,r));}
static void install_all(const std::wstring& game,const std::wstring& appdata,std::function<void(const wchar_t*)> progress={},int fail_after=-1,const std::wstring& localdata=L""){
    retention_report={};
    validate_game(game);
#ifndef VERIFY_FIXTURE
    require(!running(),"Close Shogun 2 before installing, then click Install again.");
#endif
    flashstartup86::Snapshot flash;bool registry_started=false;
    std::vector<SavedFile> files(5);files[0].path=game+L"\\data\\"+PACK;files[1].path=game+L"\\DINPUT8.dll";
    files[2].path=appdata+L"\\The Creative Assembly\\Shogun2\\scripts\\user.script.txt";
    files[3].path=appdata+L"\\The Creative Assembly\\Launcher\\20190104-moddata.dat";
    files[4].path=game+L"\\possession-flash-startup-restore.reg";
    for(auto& f:files){f.existed=GetFileAttributesW(f.path.c_str())!=INVALID_FILE_ATTRIBUTES;f.bytes=read_bytes(f.path);}
    std::vector<std::string> after={payload(202,EMBEDDED_PACK_SHA),payload(201,SETUP_BOOTSTRAP_SHA),selected_text(read_text(files[2].path))?files[2].bytes:append_selection(files[2].bytes),launcher_selection(files[3].bytes,game),files[4].existed?files[4].bytes:flash.restore_file()};
    const wchar_t* steps[]={L"Installing the mod pack...",L"Installing integrated soldier control and HUD...",L"Enabling the mod...",L"Updating the launcher selection...",L"Saving previous Flash prerequisite flags..."};
    // Preserve all prior files before the first mutation; a failed backup writes nothing.
    try{for(size_t i=0;i<files.size();++i)if(files[i].existed&&files[i].bytes!=after[i]){auto& f=files[i];auto backup=f.path+L".possession-before-"+std::to_wstring(GetTickCount64());require(CopyFileW(f.path.c_str(),backup.c_str(),TRUE)!=0,"Cannot back up existing files; installation stopped");f.backup=backup;}
    for(size_t i=0;i<files.size();++i){if(progress)progress(steps[i]);if(files[i].bytes!=after[i]||!files[i].existed){atomic_bytes(files[i].path,after[i]);files[i].changed=true;}require(fail_after!=int(i),"Simulated installation interruption");}
        require(hash_file(files[0].path)==EMBEDDED_PACK_SHA&&hash_file(files[1].path)==SETUP_BOOTSTRAP_SHA&&selected_text(read_text(files[2].path)),"Installed file verification failed");
        if(progress)progress(L"Skipping obsolete Steam Flash setup steps...");
        registry_started=true;if(!flash.skipped())flash.apply(fail_after==5?0:-1);
        require(fail_after!=6,"Simulated post-registry interruption");
        require(flashstartup86::Snapshot{}.skipped(),"Flash startup bypass verification failed");}
    catch(...){bool restored=true;if(registry_started){try{flash.restore();}catch(...){restored=false;}}for(auto it=files.rbegin();it!=files.rend();++it)if(it->changed){try{if(it->existed)atomic_bytes(it->path,it->bytes);else require(DeleteFileW(it->path.c_str())!=0,"Rollback failed");}catch(...){restored=false;}}
        if(restored)for(auto& f:files)if(!f.backup.empty())DeleteFileW(f.backup.c_str());
        require(restored,"Installation failed and automatic restoration was incomplete. Previous files are saved beside each changed file as .possession-before-* backups.");throw;}
    // A verified upgrade keeps one previous snapshot per managed file.
    // Cleanup never participates in rollback or touches a failed installation.
    try{for(auto& f:files){bool rotated=true;
        if(!f.backup.empty())rotated=MoveFileExW(f.backup.c_str(),(f.path+L".possession-previous").c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
        if(rotated)prune_legacy_backups(f.path);else ++retention_report.deferred;}
#ifdef VERIFY_FIXTURE
    auto cache_local=localdata.empty()?appdata+L"\\fixture-local":localdata;
#else
    auto cache_local=localdata.empty()?known_folder(CSIDL_LOCAL_APPDATA):localdata;
#endif
    prune_runtime_caches(cache_local);}catch(...){++retention_report.deferred;}
}

