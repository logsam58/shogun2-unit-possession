#define VERIFY_FIXTURE
#include "install.h"
int wmain(){try{
    wchar_t exe[MAX_PATH];GetModuleFileNameW(nullptr,exe,MAX_PATH);std::wstring root=exe;root=root.substr(0,root.find_last_of(L'\\'))+L"\\fixture-"+std::to_wstring(GetTickCount64());auto game=root+L"\\game",data=root+L"\\user";require(SHCreateDirectoryExW(nullptr,(game+L"\\data").c_str(),nullptr)==ERROR_SUCCESS,"Fixture creation failed");
    auto original_engine=locate()+L"\\Empire.Retail.dll";require(CopyFileW(original_engine.c_str(),(game+L"\\Empire.Retail.dll").c_str(),TRUE)!=0,"Cannot prepare engine fixture");
    require(CopyFileW((locate()+L"\\installscript.vdf").c_str(),(game+L"\\installscript.vdf").c_str(),TRUE)!=0,"Cannot prepare signed startup script fixture");
    std::wstring selection=data+L"\\The Creative Assembly\\Shogun2\\scripts\\user.script.txt",cache=data+L"\\The Creative Assembly\\Launcher\\20190104-moddata.dat";
    std::string original="// keep\r\nmod another.pack;\r\n",other="[{\"uuid\":\"other.pack\",\"active\":false,\"game\":\"shogun2\",\"extra\":{\"quoted\":\"brace }\\\"\"}}]";
    atomic_bytes(selection,original);atomic_bytes(cache,other);bool failed=false;try{install_all(game,data,{},2);}catch(...){failed=true;}require(failed,"Interruption did not fail");require(read_bytes(selection)==original&&read_bytes(cache)==other&&GetFileAttributesW((game+L"\\DINPUT8.dll").c_str())==INVALID_FILE_ATTRIBUTES&&GetFileAttributesW((game+L"\\data\\"+PACK).c_str())==INVALID_FILE_ATTRIBUTES,"Interrupted install did not fully restore state");

    auto flash_backup=game+L"\\possession-flash-startup-restore.reg";
    {flashstartup86::Key key;require(RegCreateKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,nullptr,0,KEY_ALL_ACCESS|KEY_WOW64_32KEY,nullptr,&key.value,nullptr)==ERROR_SUCCESS,"Fixture registry key");DWORD value=77;require(RegSetValueExW(key.value,L"DirectX",0,REG_DWORD,reinterpret_cast<BYTE*>(&value),4)==ERROR_SUCCESS,"Fixture unrelated prerequisite");}
    auto original_vdf=read_bytes(game+L"\\installscript.vdf");
    for(int interruption:{4,5,6}){failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        auto flags=flashstartup86::Snapshot{};
        require(failed&&!flags.before[0].exists&&!flags.before[1].exists&&read_bytes(selection)==original&&read_bytes(cache)==other&&GetFileAttributesW(flash_backup.c_str())==INVALID_FILE_ATTRIBUTES&&GetFileAttributesW((game+L"\\DINPUT8.dll").c_str())==INVALID_FILE_ATTRIBUTES,"Flash/file interrupted upgrade failed rollback");}
    install_all(game,data);require(flashstartup86::Snapshot{}.skipped(),"Flash setup steps not skipped");
    auto original_flash_backup=read_bytes(flash_backup);require(original_flash_backup.size()>100,"Flash restore file missing");
    install_all(game,data);require(read_bytes(flash_backup)==original_flash_backup&&read_bytes(game+L"\\installscript.vdf")==original_vdf,"Repeated install changed restore snapshot or signed script");
    {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_QUERY_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Fixture registry reopen");DWORD value{},size=4,type{};require(RegQueryValueExW(key.value,L"DirectX",nullptr,&type,reinterpret_cast<BYTE*>(&value),&size)==ERROR_SUCCESS&&type==REG_DWORD&&value==77,"Other prerequisites changed");}
    {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Fixture malformed flag");const wchar_t text[]=L"foreign value";require(RegSetValueExW(key.value,flashstartup86::names[0],0,REG_SZ,reinterpret_cast<const BYTE*>(text),sizeof text)==ERROR_SUCCESS,"Fixture malformed write");}
    failed=false;try{install_all(game,data);}catch(...){failed=true;}require(failed&&read_bytes(flash_backup)==original_flash_backup,"Malformed prerequisite flag not rejected");
    {flashstartup86::Key key;RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value);DWORD one=1;RegSetValueExW(key.value,flashstartup86::names[0],0,REG_DWORD,reinterpret_cast<BYTE*>(&one),4);}
    std::cout<<"PASS: Flash-only HasRunKey updates; signed startup script and other prerequisite preservation; persistent restore snapshot; idempotency; partial registry/post-registry/file rollback; malformed flag refusal. HKCU fixture only.\n";
    install_all(game,data);auto after=read_bytes(cache);require(after.find(other.substr(1,other.size()-2))!=std::string::npos,"Other launcher entry changed");auto j=JsonReader{after}.document();require(j.items.size()==2&&j.items[1].fields.at("active").value=="true","Launcher activation failed");require(read_bytes(selection).substr(0,original.size())==original,"Existing selection changed");auto selection_after=read_bytes(selection);install_all(game,data);require(after==read_bytes(cache)&&selection_after==read_bytes(selection),"Repeated install changed selection");
    auto disabled=after;auto active=j.items[1].fields.at("active");disabled.replace(active.begin,active.end-active.begin,"false");atomic_bytes(cache,disabled);install_all(game,data);require(read_bytes(cache)==after,"Existing launcher entry not enabled");
    auto dll=game+L"\\DINPUT8.dll";atomic_bytes(dll,"unrelated mod");failed=false;try{install_all(game,data);}catch(...){failed=true;}require(failed&&read_bytes(dll)=="unrelated mod"&&read_bytes(cache)==after,"Unrelated DLL was overwritten");
    std::vector<std::string> encodings={original,std::string("\xef\xbb\xbf")+original};for(bool be:{false,true}){std::string encoded=be?"\xfe\xff":"\xff\xfe";for(char c:original){if(be){encoded+='\0';encoded+=c;}else{encoded+=c;encoded+='\0';}}encodings.push_back(encoded);}unsigned index=0;
    for(auto& bytes:encodings){auto path=root+L"\\encoding-"+std::to_wstring(index++);atomic_bytes(path,bytes);activate_path(path);require(read_bytes(path).substr(0,bytes.size())==bytes&&selected_text(read_text(path)),"Encoded selection failed");auto first=read_bytes(path);activate_path(path);require(first==read_bytes(path),"Encoded selection duplicated");}
    auto wizard_folder=std::wstring(exe);wizard_folder=wizard_folder.substr(0,wizard_folder.find_last_of(L'\\'));
    auto old_loader=read_bytes(wizard_folder+L"\\..\\autoload45\\dinput8.dll"),old_pack=read_bytes(wizard_folder+L"\\..\\autoload45\\marker.pack");
    require(!old_loader.empty()&&!old_pack.empty(),"Accepted checkpoint45 fixture is missing");
    auto pack=game+L"\\data\\"+PACK;atomic_bytes(dll,old_loader);atomic_bytes(pack,old_pack);
    validate_game(game);failed=false;try{install_all(game,data,{},1);}catch(...){failed=true;}
    require(failed&&read_bytes(dll)==old_loader&&read_bytes(pack)==old_pack,"Interrupted checkpoint45 upgrade failed restoration");
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Checkpoint45 upgrade failed");
    auto prior46_loader=read_bytes(wizard_folder+L"\\..\\autoload46\\dinput8.dll"),prior46_pack=read_bytes(wizard_folder+L"\\..\\autoload46\\marker.pack");
    require(!prior46_loader.empty()&&!prior46_pack.empty(),"Accepted v0.1 fixture is missing");
    atomic_bytes(dll,prior46_loader);atomic_bytes(pack,prior46_pack);
    validate_game(game);failed=false;try{install_all(game,data,{},1);}catch(...){failed=true;}
    require(failed&&read_bytes(dll)==prior46_loader&&read_bytes(pack)==prior46_pack,"Interrupted v0.1 timer repair failed restoration");
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"v0.1 timer repair upgrade failed");
    auto stable_loader=read_bytes(wizard_folder+L"\\..\\autoload46-timeout-fix\\dinput8.dll"),stable_pack=read_bytes(wizard_folder+L"\\..\\autoload46-timeout-fix\\marker.pack");
    require(!stable_loader.empty()&&!stable_pack.empty(),"Accepted timer-fix fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,stable_loader);atomic_bytes(pack,stable_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==stable_loader&&read_bytes(pack)==stable_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted QTE60 upgrade did not restore stable installation");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Accepted timer-fix to QTE60 upgrade failed");
    auto preview_loader=read_bytes(wizard_folder+L"\\..\\autoload56-preview\\dinput8.dll"),preview_pack=read_bytes(wizard_folder+L"\\..\\autoload56-preview\\marker.pack");
    require(!preview_loader.empty()&&!preview_pack.empty(),"Candidate56 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,preview_loader);atomic_bytes(pack,preview_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==preview_loader&&read_bytes(pack)==preview_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted candidate56 upgrade did not restore previous files");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Candidate56 to QTE60 upgrade failed");
    auto previous_loader=read_bytes(wizard_folder+L"\\..\\autoload57-preview\\dinput8.dll"),previous_pack=read_bytes(wizard_folder+L"\\..\\autoload57-preview\\marker.pack");
    require(!previous_loader.empty()&&!previous_pack.empty(),"Candidate57 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,previous_loader);atomic_bytes(pack,previous_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous_loader&&read_bytes(pack)==previous_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted candidate57 upgrade did not restore previous files");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Candidate57 to QTE60 upgrade failed");
    auto candidate58_loader=read_bytes(wizard_folder+L"\\..\\autoload58-preview\\dinput8.dll"),candidate58_pack=read_bytes(wizard_folder+L"\\..\\autoload58-preview\\marker.pack");
    require(!candidate58_loader.empty()&&!candidate58_pack.empty(),"Candidate58 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,candidate58_loader);atomic_bytes(pack,candidate58_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==candidate58_loader&&read_bytes(pack)==candidate58_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted candidate58 upgrade did not restore previous files");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Candidate58 to QTE60 upgrade failed");
    auto accepted_loader=read_bytes(wizard_folder+L"\\..\\autoload74-preview\\dinput8.dll"),accepted_pack=read_bytes(wizard_folder+L"\\..\\autoload74-preview\\marker.pack");
    require(!accepted_loader.empty()&&!accepted_pack.empty(),"Checkpoint74 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,accepted_loader);atomic_bytes(pack,accepted_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==accepted_loader&&read_bytes(pack)==accepted_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted checkpoint74 upgrade did not restore all state");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Checkpoint74 to timing meter79 upgrade failed");
    auto meter75_loader=read_bytes(wizard_folder+L"\\..\\autoload75-preview\\dinput8.dll"),meter75_pack=read_bytes(wizard_folder+L"\\..\\autoload75-preview\\marker.pack");
    require(!meter75_loader.empty()&&!meter75_pack.empty(),"Test75 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,meter75_loader);atomic_bytes(pack,meter75_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==meter75_loader&&read_bytes(pack)==meter75_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted test75 upgrade did not restore all state");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test75 to timing meter79 upgrade failed");
    auto meter77_loader=read_bytes(wizard_folder+L"\\..\\autoload77-preview\\dinput8.dll"),meter77_pack=read_bytes(wizard_folder+L"\\..\\autoload77-preview\\marker.pack");
    require(!meter77_loader.empty()&&!meter77_pack.empty(),"Test77 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,meter77_loader);atomic_bytes(pack,meter77_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==meter77_loader&&read_bytes(pack)==meter77_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted test77 upgrade did not restore all state");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test77 to timing meter79 upgrade failed");
    auto meter78_loader=read_bytes(wizard_folder+L"\\..\\autoload78-preview\\dinput8.dll"),meter78_pack=read_bytes(wizard_folder+L"\\..\\autoload78-preview\\marker.pack");
    require(!meter78_loader.empty()&&!meter78_pack.empty(),"Test78 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,meter78_loader);atomic_bytes(pack,meter78_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==meter78_loader&&read_bytes(pack)==meter78_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted test78 upgrade did not restore all state");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test78 to timing meter79 upgrade failed");

    auto ammo83_loader=read_bytes(wizard_folder+L"\\..\\autoload83-preview\\dinput8.dll"),ammo83_pack=read_bytes(wizard_folder+L"\\..\\autoload83-preview\\marker.pack");
    require(!ammo83_loader.empty()&&!ammo83_pack.empty(),"Test83 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,ammo83_loader);atomic_bytes(pack,ammo83_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==ammo83_loader&&read_bytes(pack)==ammo83_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test83 upgrade failed rollback");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test83 to Test84 upgrade failed");

    auto input84_loader=read_bytes(wizard_folder+L"\\..\\autoload84-preview\\dinput8.dll"),input84_pack=read_bytes(wizard_folder+L"\\..\\autoload84-preview\\marker.pack");
    require(!input84_loader.empty()&&!input84_pack.empty(),"Test84 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,input84_loader);atomic_bytes(pack,input84_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==input84_loader&&read_bytes(pack)==input84_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test84 upgrade failed rollback");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test84 to Test85 upgrade failed");

    auto startup86_loader=read_bytes(wizard_folder+L"\\..\\autoload85-preview\\dinput8.dll"),startup86_pack=read_bytes(wizard_folder+L"\\..\\autoload85-preview\\marker.pack");
    require(!startup86_loader.empty()&&!startup86_pack.empty(),"Test86 upgrade fixture missing");
    for(int interruption=0;interruption<4;++interruption){atomic_bytes(dll,startup86_loader);atomic_bytes(pack,startup86_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);validate_game(game);
        failed=false;try{install_all(game,data,{},interruption);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==startup86_loader&&read_bytes(pack)==startup86_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test86 upgrade failed rollback");}
    install_all(game,data);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test86 to Test87 upgrade failed");
    auto local=root+L"\\retention-local",cache_root=local+L"\\Shogun2PossessionCheckpoint45";
    require(SHCreateDirectoryExW(nullptr,cache_root.c_str(),nullptr)==ERROR_SUCCESS,"Retention fixture root missing");
    auto seed_cache=[&](const std::wstring& build,const std::wstring& package){
        auto folder=cache_root+L"\\integrated-"+build;
        require(SHCreateDirectoryExW(nullptr,folder.c_str(),nullptr)==ERROR_SUCCESS,"Cache fixture missing");
        const CacheIdentity* identity=nullptr;for(auto& row:CACHE_CATALOG)if(build==row.build)identity=&row;
        require(identity!=nullptr,"Cache catalog fixture missing");
        for(auto& file:identity->files){auto source=wizard_folder+L"\\..\\..\\outputs\\"+package+L"\\"+file.name;
            if(std::wstring(file.name)==L"possession-hud.exe"&&package!=L"shogun2-siege99-preview")source=wizard_folder+L"\\..\\..\\outputs\\shogun2-possession-hud-v12\\possession-hud.exe";
            require(CopyFileW(source.c_str(),(folder+L"\\"+file.name).c_str(),TRUE)!=0,"Cannot seed runtime fixture");}
        atomic_bytes(folder+L"\\autoload-123.log","old log");atomic_bytes(folder+L"\\session-v46-123.jsonl","old log");
        atomic_bytes(folder+L"\\private\\qte-playable78-123.jsonl","old log");return folder;};
    auto old_folder=seed_cache(L"8a0a3a1c1552a06a",L"shogun2-melee-meter78-preview");
    auto current_folder=seed_cache(EMBEDDED_BUILD,L"shogun2-siege99-preview");
    atomic_bytes(dll+L".possession-before-111","old rollback");atomic_bytes(pack+L".possession-before-222","old rollback");
    atomic_bytes(selection+L".checkpoint45-before-333","old selection");
    atomic_bytes(dll+L".possession-before-not-ours","keep");
    atomic_bytes(cache_root+L"\\integrated-ffffffffffffffff\\user.txt","keep");
    failed=false;try{install_all(game,data,{},2,local);}catch(...){failed=true;}
    require(failed&&plain_path(old_folder,true)&&plain_path(dll+L".possession-before-111",false),"Failed install pruned previous data");
    install_all(game,data,{},-1,local);
    require(retention_report.caches_removed==1&&!plain_path(old_folder,true)&&plain_path(current_folder,true),"Upgrade cache retention failed");
    require(!plain_path(dll+L".possession-before-111",false)&&!plain_path(pack+L".possession-before-222",false)&&!plain_path(selection+L".checkpoint45-before-333",false),"Legacy backups accumulated");
    require(read_bytes(dll+L".possession-before-not-ours")=="keep"&&read_bytes(cache_root+L"\\integrated-ffffffffffffffff\\user.txt")=="keep","Foreign data was removed");
    for(int repeat=0;repeat<3;++repeat){atomic_bytes(dll,meter78_loader);atomic_bytes(pack,meter78_pack);install_all(game,data,{},-1,local);
        require(read_bytes(dll+L".possession-previous")==meter78_loader&&read_bytes(pack+L".possession-previous")==meter78_pack,"Previous rollback snapshot not retained");
        for(auto& child:children(game))require(!numeric_suffix(child,L"DINPUT8.dll.possession-before-"),"Upgrade stockpiled DLL backups");}
    auto previous_backup=read_bytes(dll+L".possession-previous");install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous_backup,"Identical reinstall rotated rollback needlessly");
    old_folder=seed_cache(L"8a0a3a1c1552a06a",L"shogun2-melee-meter78-preview");
    atomic_bytes(old_folder+L"\\user-notes.txt","keep");prune_runtime_caches(local);
    require(read_bytes(old_folder+L"\\user-notes.txt")=="keep"&&plain_path(old_folder+L"\\possession-v46.dll",false),"Unknown cache contents were pruned");
    require(DeleteFileW((old_folder+L"\\user-notes.txt").c_str())!=0,"Fixture notes cleanup failed");
    {Handle locked(CreateFileW((old_folder+L"\\possession-v46.dll").c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));
        require(locked.h!=INVALID_HANDLE_VALUE,"Cannot lock fixture");prune_runtime_caches(local);
        require(plain_path(old_folder+L"\\possession-hud.exe",false)&&plain_path(old_folder+L"\\possession-v46.dll",false),"Locked cache was partially pruned");}
    prune_runtime_caches(local);require(!plain_path(old_folder,true),"Deferred locked cache cleanup did not retry");
    old_folder=seed_cache(L"8a0a3a1c1552a06a",L"shogun2-melee-meter78-preview");
    require(SetFileAttributesW((old_folder+L"\\possession-hud.exe").c_str(),FILE_ATTRIBUTE_READONLY)!=0,"Cannot protect fixture");
    prune_runtime_caches(local);require(plain_path(old_folder+L"\\possession-v46.dll",false)&&plain_path(old_folder+L"\\possession-hud.exe",false),"Readonly cache was partially pruned");
    require(SetFileAttributesW((old_folder+L"\\possession-hud.exe").c_str(),FILE_ATTRIBUTE_NORMAL)!=0,"Cannot reset fixture protection");
    prune_runtime_caches(local);require(!plain_path(old_folder,true),"Readonly cache cleanup did not retry");
    old_folder=seed_cache(L"8a0a3a1c1552a06a",L"shogun2-melee-meter78-preview");
    atomic_bytes(old_folder+L"\\possession-v46.dll","modified component");prune_runtime_caches(local);
    require(read_bytes(old_folder+L"\\possession-v46.dll")=="modified component"&&plain_path(old_folder+L"\\possession-hud.exe",false),"Modified cache was removed");
    std::cout<<"PASS: bounded previous-version backups; legacy backup cleanup; successful-only cache cleanup; current-cache, foreign-file, modified-file and locked-file preservation; retry and repeated-upgrade idempotency.\n";

    auto previous87_loader=read_bytes(wizard_folder+L"\\..\\autoload87-preview\\dinput8.dll");
    auto previous87_pack=read_bytes(wizard_folder+L"\\..\\autoload87-preview\\marker.pack");
    require(!previous87_loader.empty()&&!previous87_pack.empty(),"Test87 upgrade fixture missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;
            require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test87 fixture flag reset");
            DWORD zero=0;
            for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test87 fixture flag write");}
        atomic_bytes(dll,previous87_loader);atomic_bytes(pack,previous87_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        auto saved_flags=flashstartup86::Snapshot{};
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        auto flags=flashstartup86::Snapshot{};
        require(failed&&read_bytes(dll)==previous87_loader&&read_bytes(pack)==previous87_pack&&
            read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache&&
            flags.skipped()==saved_flags.skipped(),"Interrupted Test87 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);
    require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test87 upgrade failed");
    auto previous87_backup=read_bytes(dll+L".possession-previous");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous87_backup,"v0.15 reinstall rotated backup");
    std::cout<<"PASS: Test87 to v0.15 upgrade, seven interrupted stages, native setup flag preservation and repeated-install idempotency. Workspace/HKCU fixtures only.\n";

    auto previous89_loader=read_bytes(L"..\\autoload89-preview\\dinput8.dll"),previous89_pack=read_bytes(L"..\\autoload89-preview\\marker.pack");
    require(!previous89_loader.empty()&&!previous89_pack.empty(),"Test89 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test89 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test89 fixture flag write");}
        atomic_bytes(dll,previous89_loader);atomic_bytes(pack,previous89_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous89_loader&&read_bytes(pack)==previous89_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test89 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test89 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test89 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous90_loader=read_bytes(L"..\\autoload90-preview\\dinput8.dll"),previous90_pack=read_bytes(L"..\\autoload90-preview\\marker.pack");
    require(!previous90_loader.empty()&&!previous90_pack.empty(),"Test90 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test90 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test90 fixture flag write");}
        atomic_bytes(dll,previous90_loader);atomic_bytes(pack,previous90_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous90_loader&&read_bytes(pack)==previous90_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test90 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test90 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test90 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous91_loader=read_bytes(L"..\\autoload91-preview\\dinput8.dll"),previous91_pack=read_bytes(L"..\\autoload91-preview\\marker.pack");
    require(!previous91_loader.empty()&&!previous91_pack.empty(),"Test91 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test91 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test91 fixture flag write");}
        atomic_bytes(dll,previous91_loader);atomic_bytes(pack,previous91_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous91_loader&&read_bytes(pack)==previous91_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test91 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test91 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test91 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous92_loader=read_bytes(L"..\\autoload92-preview\\dinput8.dll"),previous92_pack=read_bytes(L"..\\autoload92-preview\\marker.pack");
    require(!previous92_loader.empty()&&!previous92_pack.empty(),"Test92 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test92 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test92 fixture flag write");}
        atomic_bytes(dll,previous92_loader);atomic_bytes(pack,previous92_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous92_loader&&read_bytes(pack)==previous92_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test92 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test92 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test92 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous93_loader=read_bytes(L"..\\autoload93-preview\\dinput8.dll"),previous93_pack=read_bytes(L"..\\autoload93-preview\\marker.pack");
    require(!previous93_loader.empty()&&!previous93_pack.empty(),"Test93 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test93 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test93 fixture flag write");}
        atomic_bytes(dll,previous93_loader);atomic_bytes(pack,previous93_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous93_loader&&read_bytes(pack)==previous93_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test93 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test93 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test93 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous94_loader=read_bytes(L"..\\autoload94-preview\\dinput8.dll"),previous94_pack=read_bytes(L"..\\autoload94-preview\\marker.pack");
    require(!previous94_loader.empty()&&!previous94_pack.empty(),"Test94 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test94 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test94 fixture flag write");}
        atomic_bytes(dll,previous94_loader);atomic_bytes(pack,previous94_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous94_loader&&read_bytes(pack)==previous94_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test94 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test94 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test94 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous95_loader=read_bytes(L"..\\autoload95-preview\\dinput8.dll"),previous95_pack=read_bytes(L"..\\autoload95-preview\\marker.pack");
    require(!previous95_loader.empty()&&!previous95_pack.empty(),"Test95 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test95 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test95 fixture flag write");}
        atomic_bytes(dll,previous95_loader);atomic_bytes(pack,previous95_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous95_loader&&read_bytes(pack)==previous95_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test95 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test95 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test95 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous96_loader=read_bytes(L"..\\autoload96-preview\\dinput8.dll"),previous96_pack=read_bytes(L"..\\autoload96-preview\\marker.pack");
    require(!previous96_loader.empty()&&!previous96_pack.empty(),"Test96 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test96 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test96 fixture flag write");}
        atomic_bytes(dll,previous96_loader);atomic_bytes(pack,previous96_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous96_loader&&read_bytes(pack)==previous96_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test96 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test96 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test96 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous97_loader=read_bytes(L"..\\autoload97-preview\\dinput8.dll"),previous97_pack=read_bytes(L"..\\autoload97-preview\\marker.pack");
    require(!previous97_loader.empty()&&!previous97_pack.empty(),"Test97 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test97 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test97 fixture flag write");}
        atomic_bytes(dll,previous97_loader);atomic_bytes(pack,previous97_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous97_loader&&read_bytes(pack)==previous97_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test97 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test97 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test97 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous98_loader=read_bytes(L"..\\autoload98-preview\\dinput8.dll"),previous98_pack=read_bytes(L"..\\autoload98-preview\\marker.pack");
    require(!previous98_loader.empty()&&!previous98_pack.empty(),"Test98 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test98 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test98 fixture flag write");}
        atomic_bytes(dll,previous98_loader);atomic_bytes(pack,previous98_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous98_loader&&read_bytes(pack)==previous98_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test98 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test98 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test98 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";
    auto previous99_loader=read_bytes(L"..\\autoload-test99-baseline\\dinput8.dll"),previous99_pack=read_bytes(L"..\\autoload-test99-baseline\\marker.pack");
    require(!previous99_loader.empty()&&!previous99_pack.empty(),"Test99 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test99 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test99 fixture flag write");}
        atomic_bytes(dll,previous99_loader);atomic_bytes(pack,previous99_pack);
        auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous99_loader&&read_bytes(pack)==previous99_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Interrupted Test99 upgrade failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test99 to v0.15 upgrade failed");
    std::cout<<"PASS: verified Test99 predecessor upgrade and all seven rollback stages; unrelated DLL collision guards retained.\n";

    auto previous99_backup=read_bytes(dll+L".possession-previous");
    auto previous99_pack_backup=read_bytes(pack+L".possession-previous");
    require(previous99_backup==previous99_loader&&previous99_pack_backup==previous99_pack,"v0.15 exact Test99 backup missing");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous99_backup&&read_bytes(pack+L".possession-previous")==previous99_pack_backup,"v0.15 repeated install rotated Test99 backup");
    std::cout<<"PASS: v0.15 repeated install retains the exact Test99 rollback backup.\n";

    auto published_loader=read_bytes(L"../autoload-v015-baseline/dinput8.dll"),published_pack=read_bytes(L"../autoload-v015-baseline/marker.pack");
    require(!published_loader.empty()&&!published_pack.empty(),"Published v0.15 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Published predecessor fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Published predecessor fixture flag write");}
        atomic_bytes(dll,published_loader);atomic_bytes(pack,published_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==published_loader&&read_bytes(pack)==published_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Published v0.15 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Published v0.15 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==published_loader&&read_bytes(pack+L".possession-previous")==published_pack,"Published predecessor rollback backup differs");
    std::cout<<"PASS: exact published v0.15 predecessor upgrade and seven interruption rollbacks.\n";

    auto previous100_loader=read_bytes(L"../autoload-test100-baseline/dinput8.dll"),previous100_pack=read_bytes(L"../autoload-test100-baseline/marker.pack");
    require(!previous100_loader.empty()&&!previous100_pack.empty(),"Test100 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test100 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test100 fixture flag write");}
        atomic_bytes(dll,previous100_loader);atomic_bytes(pack,previous100_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous100_loader&&read_bytes(pack)==previous100_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test100 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test100 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous100_loader&&read_bytes(pack+L".possession-previous")==previous100_pack,"Test100 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous100_loader&&read_bytes(pack+L".possession-previous")==previous100_pack,"Repeat install rotated Test100 backup");
    std::cout<<"PASS: exact Test100 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    auto previous101_loader=read_bytes(L"../autoload-test101-baseline/dinput8.dll"),previous101_pack=read_bytes(L"../autoload-test101-baseline/marker.pack");
    require(!previous101_loader.empty()&&!previous101_pack.empty(),"Test101 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test101 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test101 fixture flag write");}
        atomic_bytes(dll,previous101_loader);atomic_bytes(pack,previous101_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous101_loader&&read_bytes(pack)==previous101_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test101 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test101 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous101_loader&&read_bytes(pack+L".possession-previous")==previous101_pack,"Test101 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous101_loader&&read_bytes(pack+L".possession-previous")==previous101_pack,"Repeat install rotated Test101 backup");
    std::cout<<"PASS: exact Test101 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    auto previous102_loader=read_bytes(L"../autoload-test102-baseline/dinput8.dll"),previous102_pack=read_bytes(L"../autoload-test102-baseline/marker.pack");
    require(!previous102_loader.empty()&&!previous102_pack.empty(),"Test102 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test102 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test102 fixture flag write");}
        atomic_bytes(dll,previous102_loader);atomic_bytes(pack,previous102_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous102_loader&&read_bytes(pack)==previous102_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test102 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test102 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous102_loader&&read_bytes(pack+L".possession-previous")==previous102_pack,"Test102 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous102_loader&&read_bytes(pack+L".possession-previous")==previous102_pack,"Repeat install rotated Test102 backup");
    std::cout<<"PASS: exact Test102 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    auto previous103_loader=read_bytes(L"../autoload-test103-baseline/dinput8.dll"),previous103_pack=read_bytes(L"../autoload-test103-baseline/marker.pack");
    require(!previous103_loader.empty()&&!previous103_pack.empty(),"Test103 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test103 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test103 fixture flag write");}
        atomic_bytes(dll,previous103_loader);atomic_bytes(pack,previous103_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous103_loader&&read_bytes(pack)==previous103_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test103 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test103 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous103_loader&&read_bytes(pack+L".possession-previous")==previous103_pack,"Test103 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous103_loader&&read_bytes(pack+L".possession-previous")==previous103_pack,"Repeat install rotated Test103 backup");
    std::cout<<"PASS: exact Test103 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";

    failed=false;try{launcher_selection("{\"unknown\":true}",game);}catch(...){failed=true;}require(failed,"Unknown cache accepted");
    std::string result="PASS: embedded payload hashes, transaction rollback, install/update/idempotency, actual checkpoint45 upgrade and interrupted-upgrade rollback, launcher activation and unrelated-entry preservation, DLL collision refusal, UTF8/UTF16 selection preservation, unknown-cache refusal. Fixture: "+utf8(root)+"\n";
    std::ofstream report(root.substr(0,root.find_last_of(L'\\'))+L"\\verification-result.txt");report<<result;report.close();std::cout<<result;return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
