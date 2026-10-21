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
        failed=false;std::string failure83;try{install_all(game,data,{},interruption);}catch(const std::exception& e){failed=true;failure83=e.what();}
        bool restored83=failed&&read_bytes(dll)==ammo83_loader&&read_bytes(pack)==ammo83_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache;
        if(!restored83)std::cerr<<"Test83 stage="<<interruption<<" failed="<<failed<<" error="<<failure83<<" loader="<<(read_bytes(dll)==ammo83_loader)<<" pack="<<(read_bytes(pack)==ammo83_pack)<<" selection="<<(read_bytes(selection)==saved_selection)<<" cache="<<(read_bytes(cache)==saved_cache)<<"\n";
        require(restored83,"Interrupted Test83 upgrade failed rollback");}
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

    { // Release this predecessor payload before the next upgrade case.
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
    } // End predecessor payload scope.
    { // Release this predecessor payload before the next upgrade case.
    auto previous101_loader=read_bytes(L"../autoload-test101-baseline/dinput8.dll"),previous101_pack=read_bytes(L"../autoload-test101-baseline/marker.pack");
    require(!previous101_loader.empty()&&!previous101_pack.empty(),"Test101 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test101 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test101 fixture flag write");}
        atomic_bytes(dll,previous101_loader);atomic_bytes(pack,previous101_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        std::string failure101;
        failed=false;try{install_all(game,data,{},interruption,local);}catch(const std::exception& e){failed=true;failure101=e.what();}catch(...){failed=true;failure101="nonstandard exception";}
        auto actual_dll101=read_bytes(dll),actual_pack101=read_bytes(pack),actual_selection101=read_bytes(selection),actual_cache101=read_bytes(cache);
        bool dll101=actual_dll101==previous101_loader,pack101=actual_pack101==previous101_pack,selection101=actual_selection101==saved_selection,cache101=actual_cache101==saved_cache;
        if(!(failed&&dll101&&pack101&&selection101&&cache101))std::cerr<<"Test101 rollback detail: stage="<<interruption<<" threw="<<failed<<" reason="<<failure101<<" dll="<<dll101<<" pack="<<pack101<<" selection="<<selection101<<" cache="<<cache101<<" actual_dll_bytes="<<actual_dll101.size()<<" expected_dll_bytes="<<previous101_loader.size()<<'\n';
        require(failed&&dll101&&pack101&&selection101&&cache101,"Test101 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test101 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous101_loader&&read_bytes(pack+L".possession-previous")==previous101_pack,"Test101 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous101_loader&&read_bytes(pack+L".possession-previous")==previous101_pack,"Repeat install rotated Test101 backup");
    std::cout<<"PASS: exact Test101 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.
    { // Release this predecessor payload before the next upgrade case.
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
    } // End predecessor payload scope.
    { // Release this predecessor payload before the next upgrade case.
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
    } // End predecessor payload scope.
    { // Release this predecessor payload before the next upgrade case.
    auto previous104_loader=read_bytes(L"../autoload-test104-baseline/dinput8.dll"),previous104_pack=read_bytes(L"../autoload-test104-baseline/marker.pack");
    require(!previous104_loader.empty()&&!previous104_pack.empty(),"Test104 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test104 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test104 fixture flag write");}
        atomic_bytes(dll,previous104_loader);atomic_bytes(pack,previous104_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous104_loader&&read_bytes(pack)==previous104_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test104 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test104 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous104_loader&&read_bytes(pack+L".possession-previous")==previous104_pack,"Test104 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous104_loader&&read_bytes(pack+L".possession-previous")==previous104_pack,"Repeat install rotated Test104 backup");
    std::cout<<"PASS: exact Test104 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.
    { // Release this predecessor payload before the next upgrade case.
    auto previous105_loader=read_bytes(L"../autoload-test105-baseline/dinput8.dll"),previous105_pack=read_bytes(L"../autoload-test105-baseline/marker.pack");
    require(!previous105_loader.empty()&&!previous105_pack.empty(),"Test105 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test105 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test105 fixture flag write");}
        atomic_bytes(dll,previous105_loader);atomic_bytes(pack,previous105_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous105_loader&&read_bytes(pack)==previous105_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test105 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test105 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous105_loader&&read_bytes(pack+L".possession-previous")==previous105_pack,"Test105 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous105_loader&&read_bytes(pack+L".possession-previous")==previous105_pack,"Repeat install rotated Test105 backup");
    std::cout<<"PASS: exact Test105 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.
    { // Release this predecessor payload before the next upgrade case.
    auto previous106_loader=read_bytes(L"../autoload-test106-baseline/dinput8.dll"),previous106_pack=read_bytes(L"../autoload-test106-baseline/marker.pack");
    require(!previous106_loader.empty()&&!previous106_pack.empty(),"Test106 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test106 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test106 fixture flag write");}
        atomic_bytes(dll,previous106_loader);atomic_bytes(pack,previous106_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous106_loader&&read_bytes(pack)==previous106_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test106 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test106 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous106_loader&&read_bytes(pack+L".possession-previous")==previous106_pack,"Test106 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous106_loader&&read_bytes(pack+L".possession-previous")==previous106_pack,"Repeat install rotated Test106 backup");
    std::cout<<"PASS: exact Test106 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.
    { // Release this predecessor payload before the next upgrade case.
    auto previous107_loader=read_bytes(L"../autoload-test107-baseline/dinput8.dll"),previous107_pack=read_bytes(L"../autoload-test107-baseline/marker.pack");
    require(!previous107_loader.empty()&&!previous107_pack.empty(),"Test107 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test107 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test107 fixture flag write");}
        atomic_bytes(dll,previous107_loader);atomic_bytes(pack,previous107_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous107_loader&&read_bytes(pack)==previous107_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test107 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test107 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous107_loader&&read_bytes(pack+L".possession-previous")==previous107_pack,"Test107 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous107_loader&&read_bytes(pack+L".possession-previous")==previous107_pack,"Repeat install rotated Test107 backup");
    std::cout<<"PASS: exact Test107 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto published017_loader=read_bytes(L"../autoload-v017-baseline/dinput8.dll"),published017_pack=read_bytes(L"../autoload-v017-baseline/marker.pack");
    require(!published017_loader.empty()&&!published017_pack.empty(),"published v0.17 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"published v0.17 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"published v0.17 fixture flag write");}
        atomic_bytes(dll,published017_loader);atomic_bytes(pack,published017_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==published017_loader&&read_bytes(pack)==published017_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"published v0.17 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"published v0.17 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==published017_loader&&read_bytes(pack+L".possession-previous")==published017_pack,"published v0.17 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==published017_loader&&read_bytes(pack+L".possession-previous")==published017_pack,"Repeat install rotated published v0.17 backup");
    std::cout<<"PASS: exact published v0.17 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previous109_loader=read_bytes(L"../autoload-test109-baseline/dinput8.dll"),previous109_pack=read_bytes(L"../autoload-test109-baseline/marker.pack");
    require(!previous109_loader.empty()&&!previous109_pack.empty(),"Test109 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test109 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test109 fixture flag write");}
        atomic_bytes(dll,previous109_loader);atomic_bytes(pack,previous109_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous109_loader&&read_bytes(pack)==previous109_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test109 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test109 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous109_loader&&read_bytes(pack+L".possession-previous")==previous109_pack,"Test109 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous109_loader&&read_bytes(pack+L".possession-previous")==previous109_pack,"Repeat install rotated Test109 backup");
    std::cout<<"PASS: exact Test109 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previous113_loader=read_bytes(L"../autoload-test113-baseline/dinput8.dll"),previous113_pack=read_bytes(L"../autoload-test113-baseline/marker.pack");
    require(!previous113_loader.empty()&&!previous113_pack.empty(),"Test113 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test113 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test113 fixture flag write");}
        atomic_bytes(dll,previous113_loader);atomic_bytes(pack,previous113_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous113_loader&&read_bytes(pack)==previous113_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test113 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test113 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous113_loader&&read_bytes(pack+L".possession-previous")==previous113_pack,"Test113 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous113_loader&&read_bytes(pack+L".possession-previous")==previous113_pack,"Repeat install rotated Test113 backup");
    std::cout<<"PASS: exact Test113 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previous114_loader=read_bytes(L"../autoload-test114-baseline/dinput8.dll"),previous114_pack=read_bytes(L"../autoload-test114-baseline/marker.pack");
    require(!previous114_loader.empty()&&!previous114_pack.empty(),"Test114 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test114 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test114 fixture flag write");}
        atomic_bytes(dll,previous114_loader);atomic_bytes(pack,previous114_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous114_loader&&read_bytes(pack)==previous114_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test114 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test114 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous114_loader&&read_bytes(pack+L".possession-previous")==previous114_pack,"Test114 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous114_loader&&read_bytes(pack+L".possession-previous")==previous114_pack,"Repeat install rotated Test114 backup");
    std::cout<<"PASS: exact Test114 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previous115_loader=read_bytes(L"../autoload-test115-baseline/dinput8.dll"),previous115_pack=read_bytes(L"../autoload-test115-baseline/marker.pack");
    require(!previous115_loader.empty()&&!previous115_pack.empty(),"Test115 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test115 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test115 fixture flag write");}
        atomic_bytes(dll,previous115_loader);atomic_bytes(pack,previous115_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous115_loader&&read_bytes(pack)==previous115_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test115 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test115 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous115_loader&&read_bytes(pack+L".possession-previous")==previous115_pack,"Test115 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous115_loader&&read_bytes(pack+L".possession-previous")==previous115_pack,"Repeat install rotated Test115 backup");
    std::cout<<"PASS: exact Test115 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previous116_loader=read_bytes(L"../autoload-test116-baseline/dinput8.dll"),previous116_pack=read_bytes(L"../autoload-test116-baseline/marker.pack");
    require(!previous116_loader.empty()&&!previous116_pack.empty(),"Test116 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test116 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test116 fixture flag write");}
        atomic_bytes(dll,previous116_loader);atomic_bytes(pack,previous116_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous116_loader&&read_bytes(pack)==previous116_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test116 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test116 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous116_loader&&read_bytes(pack+L".possession-previous")==previous116_pack,"Test116 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous116_loader&&read_bytes(pack+L".possession-previous")==previous116_pack,"Repeat install rotated Test116 backup");
    std::cout<<"PASS: exact Test116 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previous019_loader=read_bytes(L"../autoload-v019-baseline/dinput8.dll"),previous019_pack=read_bytes(L"../autoload-v019-baseline/marker.pack");
    require(!previous019_loader.empty()&&!previous019_pack.empty(),"local v0.19 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"local v0.19 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"local v0.19 fixture flag write");}
        atomic_bytes(dll,previous019_loader);atomic_bytes(pack,previous019_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous019_loader&&read_bytes(pack)==previous019_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"local v0.19 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"local v0.19 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous019_loader&&read_bytes(pack+L".possession-previous")==previous019_pack,"local v0.19 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous019_loader&&read_bytes(pack+L".possession-previous")==previous019_pack,"Repeat install rotated local v0.19 backup");
    std::cout<<"PASS: exact local v0.19 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto published018_loader=read_bytes(L"../autoload-v018-baseline/dinput8.dll"),published018_pack=read_bytes(L"../autoload-v018-baseline/marker.pack");
    require(!published018_loader.empty()&&!published018_pack.empty(),"published v0.18 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"published v0.18 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"published v0.18 fixture flag write");}
        atomic_bytes(dll,published018_loader);atomic_bytes(pack,published018_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==published018_loader&&read_bytes(pack)==published018_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"published v0.18 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"published v0.18 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==published018_loader&&read_bytes(pack+L".possession-previous")==published018_pack,"published v0.18 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==published018_loader&&read_bytes(pack+L".possession-previous")==published018_pack,"Repeat install rotated published v0.18 backup");
    std::cout<<"PASS: exact published v0.18 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previous122_loader=read_bytes(L"../autoload-test122-baseline/dinput8.dll"),previous122_pack=read_bytes(L"../autoload-test122-baseline/marker.pack");
    require(!previous122_loader.empty()&&!previous122_pack.empty(),"Test122 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test122 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test122 fixture flag write");}
        atomic_bytes(dll,previous122_loader);atomic_bytes(pack,previous122_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous122_loader&&read_bytes(pack)==previous122_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test122 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test122 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous122_loader&&read_bytes(pack+L".possession-previous")==previous122_pack,"Test122 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous122_loader&&read_bytes(pack+L".possession-previous")==previous122_pack,"Repeat install rotated Test122 backup");
    std::cout<<"PASS: exact Test122 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previous020_loader=read_bytes(L"../autoload-v020-baseline/dinput8.dll"),previous020_pack=read_bytes(L"../autoload-v020-baseline/marker.pack");
    require(!previous020_loader.empty()&&!previous020_pack.empty(),"published v0.20 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"published v0.20 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"published v0.20 fixture flag write");}
        atomic_bytes(dll,previous020_loader);atomic_bytes(pack,previous020_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previous020_loader&&read_bytes(pack)==previous020_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"published v0.20 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"published v0.20 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previous020_loader&&read_bytes(pack+L".possession-previous")==previous020_pack,"published v0.20 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previous020_loader&&read_bytes(pack+L".possession-previous")==previous020_pack,"Repeat install rotated published v0.20 backup");
    std::cout<<"PASS: exact published v0.20 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest124_loader=read_bytes(L"../autoload-test124-baseline/dinput8.dll"),previoustest124_pack=read_bytes(L"../autoload-test124-baseline/marker.pack");
    require(!previoustest124_loader.empty()&&!previoustest124_pack.empty(),"Test124 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test124 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test124 fixture flag write");}
        atomic_bytes(dll,previoustest124_loader);atomic_bytes(pack,previoustest124_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest124_loader&&read_bytes(pack)==previoustest124_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test124 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test124 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest124_loader&&read_bytes(pack+L".possession-previous")==previoustest124_pack,"Test124 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest124_loader&&read_bytes(pack+L".possession-previous")==previoustest124_pack,"Repeat install rotated Test124 backup");
    std::cout<<"PASS: exact Test124 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previousv021_loader=read_bytes(L"../autoload-v021-baseline/dinput8.dll"),previousv021_pack=read_bytes(L"../autoload-v021-baseline/marker.pack");
    require(!previousv021_loader.empty()&&!previousv021_pack.empty(),"published v0.21 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"published v0.21 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"published v0.21 fixture flag write");}
        atomic_bytes(dll,previousv021_loader);atomic_bytes(pack,previousv021_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previousv021_loader&&read_bytes(pack)==previousv021_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"published v0.21 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"published v0.21 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previousv021_loader&&read_bytes(pack+L".possession-previous")==previousv021_pack,"published v0.21 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previousv021_loader&&read_bytes(pack+L".possession-previous")==previousv021_pack,"Repeat install rotated published v0.21 backup");
    std::cout<<"PASS: exact published v0.21 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest125_loader=read_bytes(L"../autoload-test125-baseline/dinput8.dll"),previoustest125_pack=read_bytes(L"../autoload-test125-baseline/marker.pack");
    require(!previoustest125_loader.empty()&&!previoustest125_pack.empty(),"Test125 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test125 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test125 fixture flag write");}
        atomic_bytes(dll,previoustest125_loader);atomic_bytes(pack,previoustest125_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest125_loader&&read_bytes(pack)==previoustest125_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test125 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test125 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest125_loader&&read_bytes(pack+L".possession-previous")==previoustest125_pack,"Test125 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest125_loader&&read_bytes(pack+L".possession-previous")==previoustest125_pack,"Repeat install rotated Test125 backup");
    std::cout<<"PASS: exact Test125 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest126_loader=read_bytes(L"../autoload-test126-baseline/dinput8.dll"),previoustest126_pack=read_bytes(L"../autoload-test126-baseline/marker.pack");
    require(!previoustest126_loader.empty()&&!previoustest126_pack.empty(),"Test126 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test126 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test126 fixture flag write");}
        atomic_bytes(dll,previoustest126_loader);atomic_bytes(pack,previoustest126_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest126_loader&&read_bytes(pack)==previoustest126_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test126 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test126 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest126_loader&&read_bytes(pack+L".possession-previous")==previoustest126_pack,"Test126 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest126_loader&&read_bytes(pack+L".possession-previous")==previoustest126_pack,"Repeat install rotated Test126 backup");
    std::cout<<"PASS: exact Test126 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest127_loader=read_bytes(L"../autoload-test127-baseline/dinput8.dll"),previoustest127_pack=read_bytes(L"../autoload-test127-baseline/marker.pack");
    require(!previoustest127_loader.empty()&&!previoustest127_pack.empty(),"Test127 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test127 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test127 fixture flag write");}
        atomic_bytes(dll,previoustest127_loader);atomic_bytes(pack,previoustest127_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest127_loader&&read_bytes(pack)==previoustest127_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test127 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test127 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest127_loader&&read_bytes(pack+L".possession-previous")==previoustest127_pack,"Test127 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest127_loader&&read_bytes(pack+L".possession-previous")==previoustest127_pack,"Repeat install rotated Test127 backup");
    std::cout<<"PASS: exact Test127 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest128_loader=read_bytes(L"../autoload-test128-baseline/dinput8.dll"),previoustest128_pack=read_bytes(L"../autoload-test128-baseline/marker.pack");
    require(!previoustest128_loader.empty()&&!previoustest128_pack.empty(),"Test128 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test128 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test128 fixture flag write");}
        atomic_bytes(dll,previoustest128_loader);atomic_bytes(pack,previoustest128_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest128_loader&&read_bytes(pack)==previoustest128_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test128 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test128 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest128_loader&&read_bytes(pack+L".possession-previous")==previoustest128_pack,"Test128 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest128_loader&&read_bytes(pack+L".possession-previous")==previoustest128_pack,"Repeat install rotated Test128 backup");
    std::cout<<"PASS: exact Test128 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest129_loader=read_bytes(L"../autoload-test129-baseline/dinput8.dll"),previoustest129_pack=read_bytes(L"../autoload-test129-baseline/marker.pack");
    require(!previoustest129_loader.empty()&&!previoustest129_pack.empty(),"Test129 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test129 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test129 fixture flag write");}
        atomic_bytes(dll,previoustest129_loader);atomic_bytes(pack,previoustest129_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest129_loader&&read_bytes(pack)==previoustest129_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test129 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test129 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest129_loader&&read_bytes(pack+L".possession-previous")==previoustest129_pack,"Test129 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest129_loader&&read_bytes(pack+L".possession-previous")==previoustest129_pack,"Repeat install rotated Test129 backup");
    std::cout<<"PASS: exact Test129 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest130_loader=read_bytes(L"../autoload-test130-baseline/dinput8.dll"),previoustest130_pack=read_bytes(L"../autoload-test130-baseline/marker.pack");
    require(!previoustest130_loader.empty()&&!previoustest130_pack.empty(),"Test130 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test130 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test130 fixture flag write");}
        atomic_bytes(dll,previoustest130_loader);atomic_bytes(pack,previoustest130_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest130_loader&&read_bytes(pack)==previoustest130_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test130 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test130 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest130_loader&&read_bytes(pack+L".possession-previous")==previoustest130_pack,"Test130 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest130_loader&&read_bytes(pack+L".possession-previous")==previoustest130_pack,"Repeat install rotated Test130 backup");
    std::cout<<"PASS: exact Test130 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest131_loader=read_bytes(L"../autoload-test131-baseline/dinput8.dll"),previoustest131_pack=read_bytes(L"../autoload-test131-baseline/marker.pack");
    require(!previoustest131_loader.empty()&&!previoustest131_pack.empty(),"Test131 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test131 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test131 fixture flag write");}
        atomic_bytes(dll,previoustest131_loader);atomic_bytes(pack,previoustest131_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest131_loader&&read_bytes(pack)==previoustest131_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test131 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test131 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest131_loader&&read_bytes(pack+L".possession-previous")==previoustest131_pack,"Test131 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest131_loader&&read_bytes(pack+L".possession-previous")==previoustest131_pack,"Repeat install rotated Test131 backup");
    std::cout<<"PASS: exact Test131 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    }
    { // Release this predecessor payload before the next upgrade case.
    auto previoustest132_loader=read_bytes(L"../autoload-test132-baseline/dinput8.dll"),previoustest132_pack=read_bytes(L"../autoload-test132-baseline/marker.pack");
    require(!previoustest132_loader.empty()&&!previoustest132_pack.empty(),"Test132 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test132 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test132 fixture flag write");}
        atomic_bytes(dll,previoustest132_loader);atomic_bytes(pack,previoustest132_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest132_loader&&read_bytes(pack)==previoustest132_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test132 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test132 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest132_loader&&read_bytes(pack+L".possession-previous")==previoustest132_pack,"Test132 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest132_loader&&read_bytes(pack+L".possession-previous")==previoustest132_pack,"Repeat install rotated Test132 backup");
    std::cout<<"PASS: exact Test132 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    }
    { // Release this predecessor payload before the next upgrade case.
    auto previoustest133_loader=read_bytes(L"../autoload-test133-baseline/dinput8.dll"),previoustest133_pack=read_bytes(L"../autoload-test133-baseline/marker.pack");
    require(!previoustest133_loader.empty()&&!previoustest133_pack.empty(),"Test133 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test133 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test133 fixture flag write");}
        atomic_bytes(dll,previoustest133_loader);atomic_bytes(pack,previoustest133_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest133_loader&&read_bytes(pack)==previoustest133_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test133 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test133 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest133_loader&&read_bytes(pack+L".possession-previous")==previoustest133_pack,"Test133 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest133_loader&&read_bytes(pack+L".possession-previous")==previoustest133_pack,"Repeat install rotated Test133 backup");
    std::cout<<"PASS: exact Test133 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.
    { // Release this predecessor payload before the next upgrade case.
    auto previoustest134_loader=read_bytes(L"../autoload-test134-baseline/dinput8.dll"),previoustest134_pack=read_bytes(L"../autoload-test134-baseline/marker.pack");
    require(!previoustest134_loader.empty()&&!previoustest134_pack.empty(),"Test134 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test134 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test134 fixture flag write");}
        atomic_bytes(dll,previoustest134_loader);atomic_bytes(pack,previoustest134_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest134_loader&&read_bytes(pack)==previoustest134_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test134 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test134 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest134_loader&&read_bytes(pack+L".possession-previous")==previoustest134_pack,"Test134 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest134_loader&&read_bytes(pack+L".possession-previous")==previoustest134_pack,"Repeat install rotated Test134 backup");
    std::cout<<"PASS: exact Test134 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.
    { // Release this predecessor payload before the next upgrade case.
    auto previoustest135_loader=read_bytes(L"../autoload-test135-baseline/dinput8.dll"),previoustest135_pack=read_bytes(L"../autoload-test135-baseline/marker.pack");
    require(!previoustest135_loader.empty()&&!previoustest135_pack.empty(),"Test135 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test135 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test135 fixture flag write");}
        atomic_bytes(dll,previoustest135_loader);atomic_bytes(pack,previoustest135_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest135_loader&&read_bytes(pack)==previoustest135_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test135 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test135 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest135_loader&&read_bytes(pack+L".possession-previous")==previoustest135_pack,"Test135 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest135_loader&&read_bytes(pack+L".possession-previous")==previoustest135_pack,"Repeat install rotated Test135 backup");
    std::cout<<"PASS: exact Test135 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest136_loader=read_bytes(L"../autoload-test136-baseline/dinput8.dll"),previoustest136_pack=read_bytes(L"../autoload-test136-baseline/marker.pack");
    require(!previoustest136_loader.empty()&&!previoustest136_pack.empty(),"Test136 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test136 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test136 fixture flag write");}
        atomic_bytes(dll,previoustest136_loader);atomic_bytes(pack,previoustest136_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest136_loader&&read_bytes(pack)==previoustest136_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test136 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test136 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest136_loader&&read_bytes(pack+L".possession-previous")==previoustest136_pack,"Test136 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest136_loader&&read_bytes(pack+L".possession-previous")==previoustest136_pack,"Repeat install rotated Test136 backup");
    std::cout<<"PASS: exact Test136 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest137_loader=read_bytes(L"../autoload-test137-baseline/dinput8.dll"),previoustest137_pack=read_bytes(L"../autoload-test137-baseline/marker.pack");
    require(!previoustest137_loader.empty()&&!previoustest137_pack.empty(),"Test137 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test137 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test137 fixture flag write");}
        atomic_bytes(dll,previoustest137_loader);atomic_bytes(pack,previoustest137_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest137_loader&&read_bytes(pack)==previoustest137_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test137 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test137 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest137_loader&&read_bytes(pack+L".possession-previous")==previoustest137_pack,"Test137 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest137_loader&&read_bytes(pack+L".possession-previous")==previoustest137_pack,"Repeat install rotated Test137 backup");
    std::cout<<"PASS: exact Test137 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest138_loader=read_bytes(L"../autoload-test138-baseline/dinput8.dll"),previoustest138_pack=read_bytes(L"../autoload-test138-baseline/marker.pack");
    require(!previoustest138_loader.empty()&&!previoustest138_pack.empty(),"Test138 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test138 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test138 fixture flag write");}
        atomic_bytes(dll,previoustest138_loader);atomic_bytes(pack,previoustest138_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest138_loader&&read_bytes(pack)==previoustest138_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test138 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test138 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest138_loader&&read_bytes(pack+L".possession-previous")==previoustest138_pack,"Test138 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest138_loader&&read_bytes(pack+L".possession-previous")==previoustest138_pack,"Repeat install rotated Test138 backup");
    std::cout<<"PASS: exact Test138 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest139_loader=read_bytes(L"../autoload-test139-baseline/dinput8.dll"),previoustest139_pack=read_bytes(L"../autoload-test139-baseline/marker.pack");
    require(!previoustest139_loader.empty()&&!previoustest139_pack.empty(),"Test139 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test139 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test139 fixture flag write");}
        atomic_bytes(dll,previoustest139_loader);atomic_bytes(pack,previoustest139_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest139_loader&&read_bytes(pack)==previoustest139_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test139 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test139 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest139_loader&&read_bytes(pack+L".possession-previous")==previoustest139_pack,"Test139 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest139_loader&&read_bytes(pack+L".possession-previous")==previoustest139_pack,"Repeat install rotated Test139 backup");
    std::cout<<"PASS: exact Test139 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previousv024_loader=read_bytes(L"../autoload-v024-baseline/dinput8.dll"),previousv024_pack=read_bytes(L"../autoload-v024-baseline/marker.pack");
    require(!previousv024_loader.empty()&&!previousv024_pack.empty(),"published v0.24 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"published v0.24 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"published v0.24 fixture flag write");}
        atomic_bytes(dll,previousv024_loader);atomic_bytes(pack,previousv024_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previousv024_loader&&read_bytes(pack)==previousv024_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"published v0.24 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"published v0.24 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previousv024_loader&&read_bytes(pack+L".possession-previous")==previousv024_pack,"published v0.24 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previousv024_loader&&read_bytes(pack+L".possession-previous")==previousv024_pack,"Repeat install rotated published v0.24 backup");
    std::cout<<"PASS: exact published v0.24 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest140_loader=read_bytes(L"../autoload-test140-baseline/dinput8.dll"),previoustest140_pack=read_bytes(L"../autoload-test140-baseline/marker.pack");
    require(!previoustest140_loader.empty()&&!previoustest140_pack.empty(),"Test140 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test140 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test140 fixture flag write");}
        atomic_bytes(dll,previoustest140_loader);atomic_bytes(pack,previoustest140_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest140_loader&&read_bytes(pack)==previoustest140_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test140 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test140 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest140_loader&&read_bytes(pack+L".possession-previous")==previoustest140_pack,"Test140 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest140_loader&&read_bytes(pack+L".possession-previous")==previoustest140_pack,"Repeat install rotated Test140 backup");
    std::cout<<"PASS: exact Test140 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest142_loader=read_bytes(L"../autoload-test142-baseline/dinput8.dll"),previoustest142_pack=read_bytes(L"../autoload-test142-baseline/marker.pack");
    require(!previoustest142_loader.empty()&&!previoustest142_pack.empty(),"Test142 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test142 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test142 fixture flag write");}
        atomic_bytes(dll,previoustest142_loader);atomic_bytes(pack,previoustest142_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest142_loader&&read_bytes(pack)==previoustest142_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test142 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test142 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest142_loader&&read_bytes(pack+L".possession-previous")==previoustest142_pack,"Test142 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest142_loader&&read_bytes(pack+L".possession-previous")==previoustest142_pack,"Repeat install rotated Test142 backup");
    std::cout<<"PASS: exact Test142 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest143_loader=read_bytes(L"../autoload-test143-baseline/dinput8.dll"),previoustest143_pack=read_bytes(L"../autoload-test143-baseline/marker.pack");
    require(!previoustest143_loader.empty()&&!previoustest143_pack.empty(),"Test143 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test143 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test143 fixture flag write");}
        atomic_bytes(dll,previoustest143_loader);atomic_bytes(pack,previoustest143_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest143_loader&&read_bytes(pack)==previoustest143_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test143 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test143 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest143_loader&&read_bytes(pack+L".possession-previous")==previoustest143_pack,"Test143 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest143_loader&&read_bytes(pack+L".possession-previous")==previoustest143_pack,"Repeat install rotated Test143 backup");
    std::cout<<"PASS: exact Test143 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest144_loader=read_bytes(L"../autoload-test144-baseline/dinput8.dll"),previoustest144_pack=read_bytes(L"../autoload-test144-baseline/marker.pack");
    require(!previoustest144_loader.empty()&&!previoustest144_pack.empty(),"Test144 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test144 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test144 fixture flag write");}
        atomic_bytes(dll,previoustest144_loader);atomic_bytes(pack,previoustest144_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest144_loader&&read_bytes(pack)==previoustest144_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test144 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test144 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest144_loader&&read_bytes(pack+L".possession-previous")==previoustest144_pack,"Test144 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest144_loader&&read_bytes(pack+L".possession-previous")==previoustest144_pack,"Repeat install rotated Test144 backup");
    std::cout<<"PASS: exact Test144 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest145_loader=read_bytes(L"../autoload-test145-baseline/dinput8.dll"),previoustest145_pack=read_bytes(L"../autoload-test145-baseline/marker.pack");
    require(!previoustest145_loader.empty()&&!previoustest145_pack.empty(),"Test145 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test145 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test145 fixture flag write");}
        atomic_bytes(dll,previoustest145_loader);atomic_bytes(pack,previoustest145_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest145_loader&&read_bytes(pack)==previoustest145_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test145 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test145 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest145_loader&&read_bytes(pack+L".possession-previous")==previoustest145_pack,"Test145 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest145_loader&&read_bytes(pack+L".possession-previous")==previoustest145_pack,"Repeat install rotated Test145 backup");
    std::cout<<"PASS: exact Test145 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest146_loader=read_bytes(L"../autoload-test146-baseline/dinput8.dll"),previoustest146_pack=read_bytes(L"../autoload-test146-baseline/marker.pack");
    require(!previoustest146_loader.empty()&&!previoustest146_pack.empty(),"Test146 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test146 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test146 fixture flag write");}
        atomic_bytes(dll,previoustest146_loader);atomic_bytes(pack,previoustest146_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest146_loader&&read_bytes(pack)==previoustest146_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test146 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test146 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest146_loader&&read_bytes(pack+L".possession-previous")==previoustest146_pack,"Test146 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest146_loader&&read_bytes(pack+L".possession-previous")==previoustest146_pack,"Repeat install rotated Test146 backup");
    std::cout<<"PASS: exact Test146 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest147_loader=read_bytes(L"../autoload-test147-baseline/dinput8.dll"),previoustest147_pack=read_bytes(L"../autoload-test147-baseline/marker.pack");
    require(!previoustest147_loader.empty()&&!previoustest147_pack.empty(),"Test147 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test147 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test147 fixture flag write");}
        atomic_bytes(dll,previoustest147_loader);atomic_bytes(pack,previoustest147_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest147_loader&&read_bytes(pack)==previoustest147_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test147 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test147 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest147_loader&&read_bytes(pack+L".possession-previous")==previoustest147_pack,"Test147 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest147_loader&&read_bytes(pack+L".possession-previous")==previoustest147_pack,"Repeat install rotated Test147 backup");
    std::cout<<"PASS: exact Test147 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest148_loader=read_bytes(L"../autoload-test148-baseline/dinput8.dll"),previoustest148_pack=read_bytes(L"../autoload-test148-baseline/marker.pack");
    require(!previoustest148_loader.empty()&&!previoustest148_pack.empty(),"Test148 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test148 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test148 fixture flag write");}
        atomic_bytes(dll,previoustest148_loader);atomic_bytes(pack,previoustest148_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest148_loader&&read_bytes(pack)==previoustest148_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test148 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test148 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest148_loader&&read_bytes(pack+L".possession-previous")==previoustest148_pack,"Test148 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest148_loader&&read_bytes(pack+L".possession-previous")==previoustest148_pack,"Repeat install rotated Test148 backup");
    std::cout<<"PASS: exact Test148 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest149_loader=read_bytes(L"../autoload-test149-baseline/dinput8.dll"),previoustest149_pack=read_bytes(L"../autoload-test149-baseline/marker.pack");
    require(!previoustest149_loader.empty()&&!previoustest149_pack.empty(),"Test149 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test149 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test149 fixture flag write");}
        atomic_bytes(dll,previoustest149_loader);atomic_bytes(pack,previoustest149_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest149_loader&&read_bytes(pack)==previoustest149_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test149 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test149 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest149_loader&&read_bytes(pack+L".possession-previous")==previoustest149_pack,"Test149 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest149_loader&&read_bytes(pack+L".possession-previous")==previoustest149_pack,"Repeat install rotated Test149 backup");
    std::cout<<"PASS: exact Test149 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest153_loader=read_bytes(L"../autoload-test153-baseline/dinput8.dll"),previoustest153_pack=read_bytes(L"../autoload-test153-baseline/marker.pack");
    require(!previoustest153_loader.empty()&&!previoustest153_pack.empty(),"Test153 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test153 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test153 fixture flag write");}
        atomic_bytes(dll,previoustest153_loader);atomic_bytes(pack,previoustest153_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest153_loader&&read_bytes(pack)==previoustest153_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test153 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test153 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest153_loader&&read_bytes(pack+L".possession-previous")==previoustest153_pack,"Test153 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest153_loader&&read_bytes(pack+L".possession-previous")==previoustest153_pack,"Repeat install rotated Test153 backup");
    std::cout<<"PASS: exact Test153 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest157_loader=read_bytes(L"../autoload-test157-baseline/dinput8.dll"),previoustest157_pack=read_bytes(L"../autoload-test157-baseline/marker.pack");
    require(!previoustest157_loader.empty()&&!previoustest157_pack.empty(),"Test157 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test157 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test157 fixture flag write");}
        atomic_bytes(dll,previoustest157_loader);atomic_bytes(pack,previoustest157_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest157_loader&&read_bytes(pack)==previoustest157_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test157 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test157 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest157_loader&&read_bytes(pack+L".possession-previous")==previoustest157_pack,"Test157 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest157_loader&&read_bytes(pack+L".possession-previous")==previoustest157_pack,"Repeat install rotated Test157 backup");
    std::cout<<"PASS: exact Test157 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

      { // Release this predecessor payload before the next upgrade case.
    auto previoustest156_loader=read_bytes(L"../autoload-test156-baseline/dinput8.dll"),previoustest156_pack=read_bytes(L"../autoload-test156-baseline/marker.pack");
    require(!previoustest156_loader.empty()&&!previoustest156_pack.empty(),"Test156 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test156 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test156 fixture flag write");}
        atomic_bytes(dll,previoustest156_loader);atomic_bytes(pack,previoustest156_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest156_loader&&read_bytes(pack)==previoustest156_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test156 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test156 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest156_loader&&read_bytes(pack+L".possession-previous")==previoustest156_pack,"Test156 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest156_loader&&read_bytes(pack+L".possession-previous")==previoustest156_pack,"Repeat install rotated Test156 backup");
    std::cout<<"PASS: exact Test156 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

      { // Release this predecessor payload before the next upgrade case.
    auto previoustest155_loader=read_bytes(L"../autoload-test155-baseline/dinput8.dll"),previoustest155_pack=read_bytes(L"../autoload-test155-baseline/marker.pack");
    require(!previoustest155_loader.empty()&&!previoustest155_pack.empty(),"Test155 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test155 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test155 fixture flag write");}
        atomic_bytes(dll,previoustest155_loader);atomic_bytes(pack,previoustest155_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest155_loader&&read_bytes(pack)==previoustest155_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test155 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test155 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest155_loader&&read_bytes(pack+L".possession-previous")==previoustest155_pack,"Test155 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest155_loader&&read_bytes(pack+L".possession-previous")==previoustest155_pack,"Repeat install rotated Test155 backup");
    std::cout<<"PASS: exact Test155 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

      { // Release this predecessor payload before the next upgrade case.
    auto previoustest154_loader=read_bytes(L"../autoload-test154-baseline/dinput8.dll"),previoustest154_pack=read_bytes(L"../autoload-test154-baseline/marker.pack");
    require(!previoustest154_loader.empty()&&!previoustest154_pack.empty(),"Test154 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test154 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test154 fixture flag write");}
        atomic_bytes(dll,previoustest154_loader);atomic_bytes(pack,previoustest154_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest154_loader&&read_bytes(pack)==previoustest154_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test154 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test154 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest154_loader&&read_bytes(pack+L".possession-previous")==previoustest154_pack,"Test154 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest154_loader&&read_bytes(pack+L".possession-previous")==previoustest154_pack,"Repeat install rotated Test154 backup");
    std::cout<<"PASS: exact Test154 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest152_loader=read_bytes(L"../autoload-test152-baseline/dinput8.dll"),previoustest152_pack=read_bytes(L"../autoload-test152-baseline/marker.pack");
    require(!previoustest152_loader.empty()&&!previoustest152_pack.empty(),"Test152 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test152 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test152 fixture flag write");}
        atomic_bytes(dll,previoustest152_loader);atomic_bytes(pack,previoustest152_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest152_loader&&read_bytes(pack)==previoustest152_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test152 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test152 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest152_loader&&read_bytes(pack+L".possession-previous")==previoustest152_pack,"Test152 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest152_loader&&read_bytes(pack+L".possession-previous")==previoustest152_pack,"Repeat install rotated Test152 backup");
    std::cout<<"PASS: exact Test152 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest151_loader=read_bytes(L"../autoload-test151-baseline/dinput8.dll"),previoustest151_pack=read_bytes(L"../autoload-test151-baseline/marker.pack");
    require(!previoustest151_loader.empty()&&!previoustest151_pack.empty(),"Test151 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test151 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test151 fixture flag write");}
        atomic_bytes(dll,previoustest151_loader);atomic_bytes(pack,previoustest151_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest151_loader&&read_bytes(pack)==previoustest151_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test151 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test151 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest151_loader&&read_bytes(pack+L".possession-previous")==previoustest151_pack,"Test151 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest151_loader&&read_bytes(pack+L".possession-previous")==previoustest151_pack,"Repeat install rotated Test151 backup");
    std::cout<<"PASS: exact Test151 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previoustest150_loader=read_bytes(L"../autoload-test150-baseline/dinput8.dll"),previoustest150_pack=read_bytes(L"../autoload-test150-baseline/marker.pack");
    require(!previoustest150_loader.empty()&&!previoustest150_pack.empty(),"Test150 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"Test150 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"Test150 fixture flag write");}
        atomic_bytes(dll,previoustest150_loader);atomic_bytes(pack,previoustest150_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previoustest150_loader&&read_bytes(pack)==previoustest150_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"Test150 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"Test150 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previoustest150_loader&&read_bytes(pack+L".possession-previous")==previoustest150_pack,"Test150 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previoustest150_loader&&read_bytes(pack+L".possession-previous")==previoustest150_pack,"Repeat install rotated Test150 backup");
    std::cout<<"PASS: exact Test150 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previousv027_loader=read_bytes(L"../autoload-v027-baseline/dinput8.dll"),previousv027_pack=read_bytes(L"../autoload-v027-baseline/marker.pack");
    require(!previousv027_loader.empty()&&!previousv027_pack.empty(),"published v0.27 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"published v0.27 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"published v0.27 fixture flag write");}
        atomic_bytes(dll,previousv027_loader);atomic_bytes(pack,previousv027_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previousv027_loader&&read_bytes(pack)==previousv027_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"published v0.27 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"published v0.27 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previousv027_loader&&read_bytes(pack+L".possession-previous")==previousv027_pack,"published v0.27 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previousv027_loader&&read_bytes(pack+L".possession-previous")==previousv027_pack,"Repeat install rotated published v0.27 backup");
    std::cout<<"PASS: exact published v0.27 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previousv025_loader=read_bytes(L"../autoload-v025-baseline/dinput8.dll"),previousv025_pack=read_bytes(L"../autoload-v025-baseline/marker.pack");
    require(!previousv025_loader.empty()&&!previousv025_pack.empty(),"published v0.25 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"published v0.25 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"published v0.25 fixture flag write");}
        atomic_bytes(dll,previousv025_loader);atomic_bytes(pack,previousv025_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previousv025_loader&&read_bytes(pack)==previousv025_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"published v0.25 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"published v0.25 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previousv025_loader&&read_bytes(pack+L".possession-previous")==previousv025_pack,"published v0.25 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previousv025_loader&&read_bytes(pack+L".possession-previous")==previousv025_pack,"Repeat install rotated published v0.25 backup");
    std::cout<<"PASS: exact published v0.25 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previousv022_loader=read_bytes(L"../autoload-v022-baseline/dinput8.dll"),previousv022_pack=read_bytes(L"../autoload-v022-baseline/marker.pack");
    require(!previousv022_loader.empty()&&!previousv022_pack.empty(),"published v0.22 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"published v0.22 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"published v0.22 fixture flag write");}
        atomic_bytes(dll,previousv022_loader);atomic_bytes(pack,previousv022_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previousv022_loader&&read_bytes(pack)==previousv022_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"published v0.22 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"published v0.22 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previousv022_loader&&read_bytes(pack+L".possession-previous")==previousv022_pack,"published v0.22 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previousv022_loader&&read_bytes(pack+L".possession-previous")==previousv022_pack,"Repeat install rotated published v0.22 backup");
    std::cout<<"PASS: exact published v0.22 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    { // Release this predecessor payload before the next upgrade case.
    auto previousv023_loader=read_bytes(L"../autoload-v023-baseline/dinput8.dll"),previousv023_pack=read_bytes(L"../autoload-v023-baseline/marker.pack");
    require(!previousv023_loader.empty()&&!previousv023_pack.empty(),"published v0.23 predecessor fixtures missing");
    for(int interruption=0;interruption<7;++interruption){
        {flashstartup86::Key key;require(RegOpenKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,KEY_SET_VALUE|KEY_WOW64_32KEY,&key.value)==ERROR_SUCCESS,"published v0.23 fixture flag reset");DWORD zero=0;
         for(auto name:flashstartup86::names)require(RegSetValueExW(key.value,name,0,REG_DWORD,reinterpret_cast<BYTE*>(&zero),4)==ERROR_SUCCESS,"published v0.23 fixture flag write");}
        atomic_bytes(dll,previousv023_loader);atomic_bytes(pack,previousv023_pack);auto saved_selection=read_bytes(selection),saved_cache=read_bytes(cache);
        failed=false;try{install_all(game,data,{},interruption,local);}catch(...){failed=true;}
        require(failed&&read_bytes(dll)==previousv023_loader&&read_bytes(pack)==previousv023_pack&&read_bytes(selection)==saved_selection&&read_bytes(cache)==saved_cache,"published v0.23 interruption failed rollback");
    }
    install_all(game,data,{},-1,local);require(hash_file(dll)==SETUP_BOOTSTRAP_SHA&&hash_file(pack)==EMBEDDED_PACK_SHA,"published v0.23 predecessor upgrade failed");
    require(read_bytes(dll+L".possession-previous")==previousv023_loader&&read_bytes(pack+L".possession-previous")==previousv023_pack,"published v0.23 rollback backup differs");
    install_all(game,data,{},-1,local);
    require(read_bytes(dll+L".possession-previous")==previousv023_loader&&read_bytes(pack+L".possession-previous")==previousv023_pack,"Repeat install rotated published v0.23 backup");
    std::cout<<"PASS: exact published v0.23 predecessor upgrade, seven interruption rollbacks and repeat-install backup retention.\n";
    } // End predecessor payload scope.

    failed=false;try{launcher_selection("{\"unknown\":true}",game);}catch(...){failed=true;}require(failed,"Unknown cache accepted");
    std::string result="PASS: embedded payload hashes, transaction rollback, install/update/idempotency, actual checkpoint45 upgrade and interrupted-upgrade rollback, launcher activation and unrelated-entry preservation, DLL collision refusal, UTF8/UTF16 selection preservation, unknown-cache refusal. Fixture: "+utf8(root)+"\n";
    std::ofstream report(root.substr(0,root.find_last_of(L'\\'))+L"\\verification-result.txt");report<<result;report.close();std::cout<<result;return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
