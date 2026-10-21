#pragma once
// Included after the parser, atomic writer, SavedFile and Flash snapshot.
// Immutable snapshots + flushed intent precede every managed mutation. Recovery
// touches only the five caller-derived targets and two fixed registry values.
namespace durable {
inline std::function<void(int)> checkpoint=[](int){};
struct Lock {
    Handle handle{CreateMutexW(nullptr,FALSE,L"Local\\Shogun2PossessionInstallerTransaction-v1")};
    bool held{};
    Lock(){require(handle.h!=nullptr,"Cannot lock installation");auto r=WaitForSingleObject(handle.h,0);
        held=r==WAIT_OBJECT_0||r==WAIT_ABANDONED;require(held,"Another possession installer is running");}
    ~Lock(){if(held)ReleaseMutex(handle.h);}
};
inline std::wstring absolute(const std::wstring& path){wchar_t buffer[32768];auto n=GetFullPathNameW(path.c_str(),32768,buffer,nullptr);
    require(n&&n<32768,"Invalid installation path");std::wstring out(buffer);while(out.size()>3&&out.back()==L'\\')out.pop_back();return out;}
inline std::wstring wide(const std::string& text){int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),int(text.size()),nullptr,0);
    require(n>0,"Invalid transaction text");std::wstring out(n,L'\0');require(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),int(text.size()),out.data(),n)==n,"Invalid transaction text");return out;}
inline bool exists(const std::wstring& path){auto a=GetFileAttributesW(path.c_str());if(a==INVALID_FILE_ATTRIBUTES){auto e=GetLastError();require(e==ERROR_FILE_NOT_FOUND||e==ERROR_PATH_NOT_FOUND,"Cannot inspect transaction file");return false;}
    require(!(a&(FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_DIRECTORY)),"Non-regular transaction target; preserved");return true;}
inline std::string digest(const std::wstring& path){return exists(path)?hash_file(path):"absent";}
inline std::string field(const Json& object,const char* key){auto it=object.fields.find(key);require(it!=object.fields.end()&&it->second.kind=='"',"Invalid transaction field");return it->second.value;}
inline unsigned number(const Json& object,const char* key){auto it=object.fields.find(key);require(it!=object.fields.end(),"Missing transaction number");auto& s=it->second.value;
    require(!s.empty()&&s.size()<=10&&std::all_of(s.begin(),s.end(),[](char c){return c>='0'&&c<='9';}),"Invalid transaction number");
    auto value=std::stoull(s);require(value<=0xffffffffull,"Transaction number overflow");return unsigned(value);}
inline bool flag(const Json& object,const char* key){auto i=object.fields.find(key);require(i!=object.fields.end()&&(i->second.value=="true"||i->second.value=="false"),"Invalid transaction flag");return i->second.value=="true";}
inline const char* boolean(bool value){return value?"true":"false";}

struct Transaction {
    std::wstring folder,game,appdata;
    std::vector<SavedFile> files;
    std::vector<std::string> before_hash,after_hash;
    flashstartup86::Snapshot flash;
    std::string manifest;
    bool resolved{};
    std::wstring item(const wchar_t* prefix,size_t i)const{return folder+L"\\"+prefix+std::to_wstring(i)+L".bin";}
    static std::vector<std::wstring> targets(const std::wstring& game,const std::wstring& appdata){return {
        game+L"\\data\\"+PACK,game+L"\\DINPUT8.dll",
        appdata+L"\\The Creative Assembly\\Shogun2\\scripts\\user.script.txt",
        appdata+L"\\The Creative Assembly\\Launcher\\20190104-moddata.dat",game+L"\\possession-flash-startup-restore.reg"};}
    Transaction(const std::wstring& g,const std::wstring& a,const std::vector<SavedFile>& saved,
                const std::vector<std::string>& after,const flashstartup86::Snapshot& registry):game(absolute(g)),appdata(absolute(a)),files(saved),flash(registry){
        require(files.size()==5&&after.size()==5,"Transaction target count");auto expected=targets(game,appdata);
        require(plain_ancestors(game),"Use a regular game directory without reparse ancestors");
        for(const auto& path:expected)safe_target(path);
        folder=game+L"\\possession-transaction-"+transaction_nonce();require(CreateDirectoryW(folder.c_str(),nullptr)!=0,"Cannot create transaction journal");
        manifest="{\"owner\":\"shogun2-possession-transaction-v1\",\"game\":"+json_string(utf8(game))+",\"appdata\":"+json_string(utf8(appdata))+
            ",\"registry\":"+json_string(utf8(flashstartup86::key_path))+",\"key_existed\":"+boolean(flash.key_existed)+",\"files\":[";
        for(size_t i=0;i<5;++i){require(absolute(files[i].path)==expected[i],"Transaction path differs");files[i].path=expected[i];
            require(digest(files[i].path)==(files[i].existed?hash_original(i,files[i].bytes):"absent"),"File changed before snapshot");
            // Missing originals still get an empty, identified snapshot.
            if(!files[i].existed)atomic_bytes(item(L"before-",i),files[i].bytes);
            atomic_bytes(item(L"after-",i),after[i]);before_hash.push_back(hash_file(item(L"before-",i)));after_hash.push_back(hash_file(item(L"after-",i)));
            if(i)manifest+=",";manifest+="{\"existed\":"+std::string(boolean(files[i].existed))+",\"before\":"+json_string(before_hash.back())+",\"after\":"+json_string(after_hash.back())+"}";}
        manifest+="],\"flags\":[";
        for(unsigned i=0;i<2;++i){if(i)manifest+=",";manifest+="{\"exists\":"+std::string(boolean(flash.before[i].exists))+",\"value\":"+std::to_string(flash.before[i].value)+"}";}
        manifest+="]}";atomic_bytes(folder+L"\\manifest.json",manifest);
        atomic_bytes(folder+L"\\ready",hash_file(folder+L"\\manifest.json"));checkpoint(0);
    }
    std::string hash_original(size_t i,const std::string& bytes){atomic_bytes(item(L"before-",i),bytes);return hash_file(item(L"before-",i));}
    // Load only after the manifest/ready digest agrees. Caller supplies the exact
    // active game/profile; the journal cannot name arbitrary restoration paths.
    Transaction(const std::wstring& g,const std::wstring& a,const std::wstring& directory):folder(directory),game(absolute(g)),appdata(absolute(a)){
        require(plain_ancestors(folder),"Reparse transaction journal; preserved");
        require(read_bytes(folder+L"\\ready")==digest(folder+L"\\manifest.json"),"Incomplete transaction journal; preserved");
        manifest=read_bytes(folder+L"\\manifest.json");require(manifest.size()<=65536,"Oversized transaction journal");auto data=JsonReader{manifest}.document();
        require(data.kind=='{'&&field(data,"owner")=="shogun2-possession-transaction-v1"&&wide(field(data,"game"))==game&&wide(field(data,"appdata"))==appdata&&
            wide(field(data,"registry"))==flashstartup86::key_path,"Foreign transaction journal; preserved");
        auto& rows=data.fields.at("files");auto& values=data.fields.at("flags");require(rows.kind=='['&&rows.items.size()==5&&values.kind=='['&&values.items.size()==2,"Invalid transaction layout");
        auto paths=targets(game,appdata);for(const auto& path:paths)safe_target(path);files.resize(5);
        for(size_t i=0;i<5;++i){files[i].path=paths[i];files[i].existed=flag(rows.items[i],"existed");before_hash.push_back(field(rows.items[i],"before"));after_hash.push_back(field(rows.items[i],"after"));
            require(std::regex_match(before_hash.back(),std::regex("[a-f0-9]{64}"))&&std::regex_match(after_hash.back(),std::regex("[a-f0-9]{64}")),"Invalid snapshot fingerprint");}
        flash.key_existed=flag(data,"key_existed");for(unsigned i=0;i<2;++i)flash.before[i]={flag(values.items[i],"exists"),number(values.items[i],"value")};
    }
    void validate_registry()const{flashstartup86::Snapshot current;
        for(unsigned i=0;i<2;++i){auto b=flash.before[i],c=current.before[i];auto applied=b.exists&&b.value>=1?b.value:1;
            bool unchanged=c.exists==b.exists&&(!c.exists||c.value==b.value);require(unchanged||(c.exists&&c.value==applied),"Registry changed outside installer; transaction preserved");}}
    void restore(){
        // Preflight everything first. Never overwrite an unexpected user edit.
        for(size_t i=0;i<5;++i){safe_target(files[i].path);auto current=digest(files[i].path),original=files[i].existed?before_hash[i]:"absent";
            require(current==original||current==after_hash[i],"File changed outside installer; transaction preserved");
            if(current!=original&&files[i].existed)require(digest(item(L"before-",i))==before_hash[i],"Original snapshot missing; preserved");}
        validate_registry();
        for(size_t n=5;n>0;--n){size_t i=n-1;auto original=files[i].existed?before_hash[i]:"absent";if(digest(files[i].path)==original)continue;
            if(files[i].existed)atomic_bytes(files[i].path,read_bytes(item(L"before-",i)));else require(DeleteFileW(files[i].path.c_str())!=0,"Cannot restore absent original");}
        flash.changed=true;flash.restore();atomic_bytes(folder+L"\\resolved",read_bytes(folder+L"\\ready"));resolved=true;cleanup();
    }
    void commit(){atomic_bytes(folder+L"\\committed",read_bytes(folder+L"\\ready"));checkpoint(14);finish();}
    void apply(const std::function<void(size_t)>& progress={},const std::function<void()>& verify={},int fail_after=-1){
        bool committed=false;std::vector<std::wstring> paths;for(auto& f:files)paths.push_back(f.path);
        atomic_staged=[paths](const std::wstring& target,const std::wstring&){if(std::find(paths.begin(),paths.end(),target)!=paths.end())checkpoint(11);};
        flashstartup86::checkpoint=[](int stage){checkpoint(stage);};
        try{
            for(size_t i=0;i<5;++i){if(progress)progress(i);checkpoint(1+int(i)*2);
                require(digest(item(L"after-",i))==after_hash[i],"Prepared installer payload changed");
                auto current=digest(files[i].path),original=files[i].existed?before_hash[i]:"absent";
                require(current==original||current==after_hash[i],"Target changed during install; preserved");
                if(current!=after_hash[i])atomic_bytes(files[i].path,read_bytes(item(L"after-",i)));
                checkpoint(2+int(i)*2);require(fail_after!=int(i),"Simulated installation interruption");}
            if(verify)verify();validate_registry();
            if(!flash.skipped())flash.apply(fail_after==5?0:-1);
            require(fail_after!=6,"Simulated post-registry interruption");
            require(flashstartup86::Snapshot{}.skipped(),"Flash startup bypass verification failed");
            atomic_bytes(folder+L"\\committed",read_bytes(folder+L"\\ready"));committed=true;checkpoint(14);finish();
        }catch(...){if(!committed)restore();throw;}
    }
    void finish(){
        for(size_t i=0;i<5;++i)if(files[i].existed&&before_hash[i]!=after_hash[i]){
            auto previous=files[i].path+L".possession-previous";
            if(digest(previous)==before_hash[i])continue;
            require(digest(item(L"before-",i))==before_hash[i],"Cannot retain previous installation snapshot");
            // Preserve an unrecognized previous snapshot instead of assuming its
            // filename grants deletion authority. Repeated repair creates none.
            if(exists(previous)){auto preserved=previous+L"-preserved-"+folder.substr(folder.size()-32);
                if(!exists(preserved))require(CopyFileW(previous.c_str(),preserved.c_str(),TRUE)!=0,"Cannot preserve previous backup");
                else require(digest(preserved)==digest(previous),"Backup preservation conflict");}
            atomic_bytes(previous,read_bytes(item(L"before-",i)));
        }
        atomic_bytes(folder+L"\\resolved",read_bytes(folder+L"\\ready"));resolved=true;cleanup();
    }
    void cleanup(){
        // Enumerate an exact owned closure, with no recursive deletion. Extra
        // files remain; a resolved marker makes interrupted cleanup idempotent.
        auto expected=read_bytes(folder+L"\\ready");require(digest(folder+L"\\manifest.json")==expected,"Journal changed; preserved");
        for(size_t i=0;i<5;++i)for(auto prefix:{L"before-",L"after-"}){auto path=item(prefix,i);if(!exists(path))continue;
            auto& hash=prefix[0]==L'b'?before_hash[i]:after_hash[i];require(digest(path)==hash,"Snapshot changed; preserved");DeleteFileW(path.c_str());}
        // Keep the small immutable manifest/ready/resolved receipt. It proves
        // ownership and allows later invocations to distinguish completed work.
    }
    static void recover(const std::wstring& game,const std::wstring& appdata){unsigned count=0;
        for(auto& name:children(game)){if(!std::regex_match(name,std::wregex(L"possession-transaction-[0-9a-f]{32}")))continue;
            require(++count<=1024,"Too many transaction records; inspect before repair");auto folder=game+L"\\"+name;
            if(!plain_path(folder,true))continue;
            // No ready record means no external mutation was allowed. Preserve
            // preparatory remnants/unknown data without making retry fail.
            if(!exists(folder+L"\\ready"))continue;
            Transaction transaction(game,appdata,folder);auto ready=read_bytes(folder+L"\\ready");
            if(exists(folder+L"\\resolved")){require(read_bytes(folder+L"\\resolved")==ready,"Foreign resolution marker");transaction.cleanup();}
            else if(exists(folder+L"\\committed")){require(read_bytes(folder+L"\\committed")==ready,"Foreign commit marker");transaction.finish();}
            else transaction.restore();
        }
    }
};
}
