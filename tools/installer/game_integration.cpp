// Inno Setup owns the wizard, file copying and Windows uninstall registration.
// This small custom action only validates/snapshots the game integration and
// edits its two activation files. It never launches a game or prunes caches.
#include "install.h"

namespace standard_setup {
#ifdef S2FP_LOCAL_ASSETS
const char* owner="shogun2-first-person-inno-local159";
constexpr size_t file_count=6;
#else
const char* owner="shogun2-first-person-inno-v1";
constexpr size_t file_count=4;
#endif
std::wstring state,game,profile;
std::vector<std::wstring> targets;
std::string manifest;
std::vector<std::string> before,after,old_hash,new_hash;
std::vector<bool> existed;
void validate_standard_game(){
#ifdef S2FP_LOCAL_ASSETS
    // Exact accepted preview predecessors; unknown injectors still fail.
    auto predecessor=durable::digest(game+L"\\DINPUT8.dll");
    const char* previous_pack=predecessor=="936bb2cd09bfb57ba4a703a3dbdb7ca682d8df4e92a7feaf8b3738d8493a26c2"?"3db77b196bc9fb3ded51ac6e850a386a2b8cc30cf9e97011b1babf88d5e2753b":
        predecessor=="13e9097a3965d9a8a3fb054c1dd9dfe30c0591eba22fda678ab736b036251c44"?"6706393a2b4756765d76a894849fe34052d1ce22eabffdd35ae23965c8a587b2":
        predecessor=="583b12f04734b8e5db59cad9f909b3a2f1c32773d5f5513008e4601d9f317af3"?"024eb40e5298e80fd817a95c13cf703de396ef741df2edb2aa1c46ac2d23573e":
        predecessor=="6e2bec13bdf74795e48281bdade07fa8a2c101ca7993ea2765679d7e5abd5af0"?"21c8e04b83437b1b1f29c4c27e61b9e84d4002862e74f94296c53b95375bf235":
        predecessor=="19f7c9b197cb8128a160aa8bd7b4e930b2612f420a5468413690bea55e791ae7"?"a611fadf4369e7a415aab40099982822c32ba5107c357fea12aadc1c73e81623":
        predecessor=="9c1471e8dea6bae0f21dec152fc092e1858e680a878a74fa282d1f1b8bda1f64"?"7835ea6cab325d8823be27e1a19476e4c0cabf826db52c1258e7ce4c8dcf12ca":
        predecessor=="8ce6d69cada96e8c344e3dab06db9242fa406201e08df8c3fbf80be982b89045"?"fa250bf95991b75b5f34b33cbd572550f8bcb31e30b46763e733220dcdcd8bae":nullptr;
    if(previous_pack){
        require(hash_file(game+L"\\Empire.Retail.dll")==EXPECTED_HASH,"Unsupported game version");
        auto pack=durable::digest(game+L"\\data\\"+PACK);
        require(pack=="absent"||pack==EMBEDDED_PACK_SHA||pack==previous_pack,"Unrecognized marker pack; preserved");
        return;
    }
#endif
    validate_game(game);
}
std::wstring item(const wchar_t* stem,size_t i){return state+L"\\"+stem+std::to_wstring(i)+L".bin";}
std::string digest(const std::wstring& p){safe_target(p);return durable::digest(p);}
void erase(const std::wstring& p){safe_target(p);if(durable::exists(p))require(DeleteFileW(p.c_str())!=0,"Cannot remove owned file; close the game and retry");}
void load(){
    require(plain_ancestors(state),"Unrecognized rollback directory; preserved");
    manifest=read_bytes(state+L"\\manifest.json");
    require(read_bytes(state+L"\\ready")==hash_file(state+L"\\manifest.json"),"Rollback record failed integrity check; preserved");
    auto j=JsonReader{manifest}.document();
    require(durable::field(j,"owner")==owner,"Unrecognized rollback owner; preserved");
    require(durable::wide(durable::field(j,"game"))==game&&durable::wide(durable::field(j,"profile"))==profile,"This setup belongs to a different game or Windows profile");
    auto rows=j.fields.at("files").items;require(rows.size()==file_count,"Invalid rollback file count");
    for(size_t i=0;i<file_count;++i){
        existed.push_back(durable::flag(rows[i],"existed"));
        old_hash.push_back(durable::field(rows[i],"before"));new_hash.push_back(durable::field(rows[i],"after"));
        require(digest(item(L"before-",i))==old_hash.back()&&digest(item(L"after-",i))==new_hash.back(),"Rollback snapshot changed; preserved");
        before.push_back(read_bytes(item(L"before-",i)));after.push_back(read_bytes(item(L"after-",i)));
    }
    require(new_hash[0]==SETUP_BOOTSTRAP_SHA&&new_hash[1]==EMBEDDED_PACK_SHA,"Rollback build differs from this setup");
}
void preflight_rollback(){
    for(size_t i=0;i<file_count;++i){auto h=digest(targets[i]);require(h==new_hash[i]||h==(existed[i]?old_hash[i]:"absent"),"A managed file changed outside setup; it has been preserved. See the rollback folder.");}
}
void rollback(){
    preflight_rollback();
    for(size_t i=0;i<file_count;++i){if(digest(targets[i])==(existed[i]?old_hash[i]:"absent"))continue;
        if(existed[i])atomic_bytes(targets[i],before[i]);else erase(targets[i]);}
    erase(state+L"\\active");
}
void prepare(){
    validate_standard_game();
    require(plain_ancestors(game),"Use a normal local Steam game directory");
    for(auto& p:targets)safe_target(p);
    if(GetFileAttributesW((state+L"\\ready").c_str())!=INVALID_FILE_ATTRIBUTES){
        load();
        if(GetFileAttributesW((state+L"\\uninstalled").c_str())!=INVALID_FILE_ATTRIBUTES){
            require(digest(targets[0])=="absent"&&digest(targets[1])=="absent","Mod files reappeared after uninstall; preserved");
            auto archive=durable::absolute(state+L"-previous-"+transaction_nonce());
            auto parent=state.substr(0,state.find_last_of(L'\\'));
            require(archive.substr(0,parent.size()+1)==parent+L"\\","Rollback archive must stay inside the setup folder");
            require(MoveFileExW(state.c_str(),archive.c_str(),MOVEFILE_WRITE_THROUGH)!=0,"Cannot retain earlier rollback snapshots");
            before.clear();after.clear();old_hash.clear();new_hash.clear();existed.clear();manifest.clear();
        }else{
            preflight_rollback();
            if(GetFileAttributesW((state+L"\\active").c_str())!=INVALID_FILE_ATTRIBUTES){
                require(read_bytes(state+L"\\active")==hash_file(state+L"\\manifest.json"),"Active setup record differs; preserved");return;}
            rollback();return;
        }
    }
    // Never adopt a pre-existing unidentified directory, even an empty one.
    require(CreateDirectoryW(state.c_str(),nullptr)!=0,"Rollback folder already exists without a completed identity. It has been preserved; choose a fresh setup folder.");
    for(auto& p:targets){existed.push_back(durable::exists(p));before.push_back(read_bytes(p));}
    after={"","",selected_text(read_text(targets[2]))?before[2]:append_selection(before[2]),launcher_selection(before[3],game)};
    // Binary snapshots are supplied by the verified temporary payload. Inno
    // copies those same bytes into the game using its standard [Files] engine.
    wchar_t module[32768];require(GetModuleFileNameW(nullptr,module,32768)!=0,"Cannot locate setup custom action");
    auto source=std::wstring(module);source=source.substr(0,source.find_last_of(L"\\"));
    after[0]=read_bytes(source+L"\\dinput8.dll");after[1]=read_bytes(source+L"\\marker.pack");
#ifdef S2FP_LOCAL_ASSETS
    for(unsigned kind:{1u,2u}){
        auto filename=kind==1?L"hud-assets159.s2fp":L"qte-assets159.s2fp";
        auto path=source+L"\\local-assets\\"+filename;assets159::Store resources;
        require(resources.load(path,kind),"Prepared local game resources are missing or corrupt");
        auto bytes=read_bytes(path);size_t i=kind+3;
        require(!existed[i]||before[i]==bytes,"An unknown local resource file exists; preserved");
        after.push_back(std::move(bytes));
    }
#endif
    atomic_bytes(state+L"\\manifest.pending","Identified snapshots are being prepared; no game file has changed.\r\n");
    manifest="{\"owner\":"+json_string(owner)+",\"game\":"+json_string(utf8(game))+",\"profile\":"+json_string(utf8(profile))+",\"files\":[";
    for(size_t i=0;i<file_count;++i){atomic_bytes(item(L"before-",i),before[i]);atomic_bytes(item(L"after-",i),after[i]);
        auto a=digest(item(L"before-",i)),b=digest(item(L"after-",i));
        if(i==0)require(b==SETUP_BOOTSTRAP_SHA,"Loader payload differs");if(i==1)require(b==EMBEDDED_PACK_SHA,"Pack payload differs");
        if(i)manifest+=",";manifest+="{\"existed\":"+std::string(existed[i]?"true":"false")+",\"before\":"+json_string(a)+",\"after\":"+json_string(b)+"}";}
    manifest+="]}";atomic_bytes(state+L"\\manifest.json",manifest);atomic_bytes(state+L"\\ready",hash_file(state+L"\\manifest.json"));
}
void finish(){
    load();preflight_rollback();require(digest(targets[0])==SETUP_BOOTSTRAP_SHA&&digest(targets[1])==EMBEDDED_PACK_SHA,"Inno file copy did not complete; restoring previous installation");
    atomic_bytes(targets[2],after[2]);atomic_bytes(targets[3],after[3]);
    for(size_t i=0;i<file_count;++i)require(digest(targets[i])==new_hash[i],"Installed integration differs");
    atomic_bytes(state+L"\\active",hash_file(state+L"\\manifest.json"));
}
std::string remove_script(std::string bytes){
    bool le=bytes.size()>1&&((static_cast<unsigned char>(bytes[0])==255&&static_cast<unsigned char>(bytes[1])==254)||bytes[1]==0);
    bool be=bytes.size()>1&&static_cast<unsigned char>(bytes[0])==254&&static_cast<unsigned char>(bytes[1])==255;
    size_t width=le||be?2:1,start=(le||be)&&((static_cast<unsigned char>(bytes[0])==255)||(static_cast<unsigned char>(bytes[0])==254))?2:0;
    if(width==1&&bytes.substr(0,3)=="\xef\xbb\xbf")start=3;
    require(width==1||(bytes.size()-start)%2==0,"Invalid mod selection encoding; preserved");
    std::string flat;for(size_t i=start;i<bytes.size();i+=width){unsigned char c=bytes[i+(be?1:0)];flat+=c<128?char(c):'?';}
    std::regex directive("(^|[\\r\\n])[ \\t]*mod[ \\t]+(?:\"shogun2_possession_checkpoint45\\.pack\"|shogun2_possession_checkpoint45\\.pack)[ \\t]*;",std::regex::icase);
    std::smatch m;while(std::regex_search(flat,m,directive)){size_t p=size_t(m.position()),n=size_t(m.length());
        // Keep the preceding line break; only this mod's statement is removed.
        if(flat[p]=='\r'||flat[p]=='\n'){++p;--n;}bytes.erase(start+p*width,n*width);flat.erase(p,n);}
    return bytes;
}
std::string remove_launcher(std::string bytes){
    if(bytes.empty())return bytes;
    while(true){auto root=JsonReader{bytes}.document();require(root.kind=='[',"Unrecognized launcher settings; preserved");bool found=false;
        for(size_t i=0;i<root.items.size();++i){auto& row=root.items[i];auto id=row.fields.find("uuid"),g=row.fields.find("game");
            if(id==row.fields.end()||g==row.fields.end()||id->second.value!="shogun2_possession_checkpoint45.pack"||g->second.value!="shogun2")continue;
            size_t a=row.begin,b=row.end;if(i)a=root.items[i-1].end;else if(root.items.size()>1)b=root.items[1].begin;
            bytes.erase(a,b-a);found=true;break;}
        if(!found)return bytes;}
}
void uninstall(bool apply){
    load();
    for(size_t i=0;i<file_count;++i){if(i==2||i==3)continue;auto h=digest(targets[i]);require(h=="absent"||h==new_hash[i],"An installed mod file was replaced or changed. Uninstall stopped and preserved it.");}
    // Parse everything before removing anything. Other mods' current entries
    // survive, including entries added after this setup completed.
    auto script=read_bytes(targets[2]),launcher=read_bytes(targets[3]);
    auto clean_script=remove_script(script),clean_launcher=remove_launcher(launcher);
    if(!apply)return;
    if(clean_script!=script)atomic_bytes(targets[2],clean_script);
    if(clean_launcher!=launcher)atomic_bytes(targets[3],clean_launcher);
    for(size_t i=0;i<file_count;++i)if(i!=2&&i!=3)erase(targets[i]);
    atomic_bytes(state+L"\\uninstalled","Mod removed. Previous snapshots retained for manual recovery.\r\n");
    erase(state+L"\\active");
}
}
int wmain(int argc,wchar_t** argv){
    std::wstring report;try{
        require(argc==6,"Use game-integration <check|prepare|finish|rollback|uninstall-check|uninstall> <game> <profile> <rollback-folder> <report>");
        using namespace standard_setup;game=durable::absolute(argv[2]);profile=durable::absolute(argv[3]);state=durable::absolute(argv[4]);report=argv[5];
        require(!running(),"Close Shogun 2 completely, then run setup again.");durable::Lock lock;
        targets={game+L"\\DINPUT8.dll",game+L"\\data\\"+PACK,profile+L"\\The Creative Assembly\\Shogun2\\scripts\\user.script.txt",profile+L"\\The Creative Assembly\\Launcher\\20190104-moddata.dat"};
#ifdef S2FP_LOCAL_ASSETS
        targets.push_back(game+L"\\data\\shogun2_possession_local_assets159\\hud-assets159.s2fp");
        targets.push_back(game+L"\\data\\shogun2_possession_local_assets159\\qte-assets159.s2fp");
#endif
        std::wstring command=argv[1];
        if(command==L"check"){validate_standard_game();for(auto& p:targets)safe_target(p);launcher_selection(read_bytes(targets[3]),game);}
        else if(command==L"prepare")prepare();else if(command==L"finish")finish();else if(command==L"rollback"){load();rollback();}
        else if(command==L"uninstall-check")uninstall(false);else if(command==L"uninstall")uninstall(true);else throw std::runtime_error("Unknown integration command");
        atomic_bytes(report,"OK\r\n");return 0;
    }catch(const std::exception& e){if(!report.empty())try{atomic_bytes(report,std::string(e.what())+"\r\n");}catch(...){}std::cerr<<e.what()<<'\n';return 1;}
}
