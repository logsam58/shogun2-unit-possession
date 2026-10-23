#pragma once
// Narrow Steam VDF editor. Only Shogun 2's LaunchOptions value is owned.
// Preserve every other byte; refuse ambiguous syntax, wrappers and concurrent writes.
#include "../../native/outputs/shogun2-possession-cleanup-v46/src/identity.h"
#include <filesystem>
#include <optional>
namespace steam162 {
namespace fs=std::filesystem;
struct Node {std::string key,value;size_t begin{},value_begin{},end{},close{};bool object{};std::vector<Node> children;};
inline bool equal(const std::string& a,const std::string& b){return _stricmp(a.c_str(),b.c_str())==0;}
struct Parser {
    const std::string& text;size_t p{};unsigned nodes{};
    void space(){for(;;){while(p<text.size()&&isspace(static_cast<unsigned char>(text[p])))++p;
        if(text.compare(p,2,"//"))return;while(p<text.size()&&text[p]!='\n')++p;}}
    std::string token(){space();require(p<text.size(),"Truncated Steam configuration");std::string out;
        if(text[p]=='"'){++p;bool ended=false;while(p<text.size()){char c=text[p++];if(c=='"'){ended=true;break;}
                if(c=='\\'){require(p<text.size(),"Truncated Steam escape");c=text[p++];if(c=='n')c='\n';else if(c=='t')c='\t';else if(c=='r')c='\r';else require(c=='"'||c=='\\',"Unsupported Steam escape; preserved");}out+=c;}
            require(ended,"Unterminated Steam string");}
        else {size_t start=p;while(p<text.size()&&!isspace(static_cast<unsigned char>(text[p]))&&text[p]!='{'&&text[p]!='}')++p;
            require(p>start,"Invalid Steam token");out=text.substr(start,p-start);}
        require(out.find('\0')==out.npos,"Invalid Steam string");return out;
    }
    std::vector<Node> list(unsigned depth=0){require(depth<32,"Steam configuration is too deeply nested");std::vector<Node> out;
        for(;;){space();if(p==text.size()||text[p]=='}')return out;require(++nodes<250000,"Steam configuration is too large");
            Node n;n.begin=p;n.key=token();space();n.value_begin=p;
            if(p<text.size()&&text[p]=='{'){n.object=true;++p;n.children=list(depth+1);space();require(p<text.size()&&text[p]=='}',"Unclosed Steam object");n.close=p++;}
            else n.value=token();n.end=p;out.push_back(std::move(n));}
    }
    std::vector<Node> document(){require(text.size()<16*1024*1024&&text.find('\0')==text.npos,"Invalid Steam file size or encoding");
        if(text.compare(0,3,"\xef\xbb\xbf")==0)p=3;auto out=list();space();require(p==text.size(),"Unexpected Steam closing brace");return out;}
};
inline const Node* child(const std::vector<Node>& rows,const std::string& key){const Node* result=nullptr;
    for(const auto& n:rows)if(equal(n.key,key)){require(!result,"Duplicate Steam key; preserved");result=&n;}return result;}
inline std::string vdf_quote(const std::string& s){std::string out="\"";for(char c:s){switch(c){case '\\':out+="\\\\";break;case '"':out+="\\\"";break;case '\n':out+="\\n";break;case '\r':out+="\\r";break;case '\t':out+="\\t";break;default:out+=c;}}return out+'"';}
inline const std::vector<std::string> path={"UserLocalConfigStore","Software","Valve","Steam","apps","34330","LaunchOptions"};
inline std::optional<std::string> value(const std::string& text){auto tree=Parser{text}.document();const auto* rows=&tree;
    for(size_t i=0;i<path.size();++i){auto n=child(*rows,path[i]);if(!n)return {};if(i+1==path.size()){require(!n->object,"Invalid launch options object");return n->value;}
        require(n->object,"Invalid Steam object");rows=&n->children;}return {};}
inline std::string edit(const std::string& text,const std::optional<std::string>& replacement){
    auto tree=Parser{text}.document();const auto* rows=&tree;const Node* parent=nullptr;
    for(size_t i=0;i<path.size();++i){auto n=child(*rows,path[i]);
        if(!n){if(!replacement)return text;std::string nl=text.find("\r\n")!=text.npos?"\r\n":"\n",entry;
            for(size_t j=i;j+1<path.size();++j)entry+=std::string(j,'\t')+vdf_quote(path[j])+nl+std::string(j,'\t')+"{"+nl;
            entry+=std::string(path.size()-1,'\t')+vdf_quote(path.back())+"\t\t"+vdf_quote(*replacement)+nl;
            for(size_t j=path.size()-1;j>i;--j)entry+=std::string(j-1,'\t')+"}"+nl;
            auto p=parent?parent->close:text.size();return text.substr(0,p)+nl+entry+text.substr(p);}
        if(i+1==path.size()){require(!n->object,"Invalid Steam launch options");auto out=text;
            if(replacement)out.replace(n->value_begin,n->end-n->value_begin,vdf_quote(*replacement));else out.erase(n->begin,n->end-n->begin);return out;}
        require(n->object,"Invalid Steam configuration object");parent=n;rows=&n->children;
    }return text;
}
inline std::string utf8(const std::wstring& text){int n=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text.data(),int(text.size()),nullptr,0,nullptr,nullptr);require(n>0||text.empty(),"Invalid Windows path");std::string out(n,0);if(n)WideCharToMultiByte(CP_UTF8,0,text.data(),int(text.size()),out.data(),n,nullptr,nullptr);return out;}
inline std::wstring wide(const std::string& text){int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),int(text.size()),nullptr,0);require(n>0||text.empty(),"Invalid UTF-8 Steam path");std::wstring out(n,0);if(n)MultiByteToWideChar(CP_UTF8,0,text.data(),int(text.size()),out.data(),n);return out;}
inline void plain(const fs::path& path){for(auto p=fs::absolute(path);!p.empty();){auto a=GetFileAttributesW(p.c_str());require(a==INVALID_FILE_ATTRIBUTES||!(a&FILE_ATTRIBUTE_REPARSE_POINT),"Steam/configuration path is redirected; preserved");auto parent=p.parent_path();if(p==parent)break;p=parent;}}
inline std::string read(const fs::path& p){plain(p);Handle file(CreateFileW(p.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr));require(file.h!=INVALID_HANDLE_VALUE,"Cannot read Steam configuration; exit Steam and retry");
    LARGE_INTEGER n{};require(GetFileSizeEx(file.h,&n)&&n.QuadPart>=0&&n.QuadPart<16*1024*1024,"Steam configuration size invalid");std::string out(size_t(n.QuadPart),0);DWORD got{};require(ReadFile(file.h,out.data(),DWORD(out.size()),&got,nullptr)&&got==out.size(),"Steam configuration read failed");return out;}
inline void replace(const fs::path& p,const std::string& before,const std::string& after){
    plain(p);auto temp=p;temp+=L".unit-possession162.new";
    Handle stage(CreateFileW(temp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));require(stage.h!=INVALID_HANDLE_VALUE,"An interrupted Steam update exists; files preserved");
    try{DWORD n{};require(WriteFile(stage.h,after.data(),DWORD(after.size()),&n,nullptr)&&n==after.size()&&FlushFileBuffers(stage.h),"Cannot stage Steam configuration");CloseHandle(stage.h);stage.h=nullptr;
        require(read(p)==before,"Steam configuration changed during setup; preserved");
        require(ReplaceFileW(p.c_str(),temp.c_str(),nullptr,0,nullptr,nullptr)!=0,"Cannot replace Steam configuration; exit Steam and retry");
    }catch(...){DeleteFileW(temp.c_str());throw;}
}
inline void fresh(const fs::path& p,const std::string& bytes){plain(p);Handle f(CreateFileW(p.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr));require(f.h!=INVALID_HANDLE_VALUE,"Existing Steam setup record preserved");DWORD n{};require(WriteFile(f.h,bytes.data(),DWORD(bytes.size()),&n,nullptr)&&n==bytes.size()&&FlushFileBuffers(f.h),"Cannot save Steam rollback record");}
struct Plan {std::string root,user,launcher;std::optional<std::string> original,before;std::string installed;};
// Steam may keep an empty LaunchOptions key after its UI's Clear action.
inline bool equivalent(const std::optional<std::string>& a,const std::optional<std::string>& b){return a==b||(a.value_or("").empty()&&b.value_or("").empty());}
inline std::string pack(const Plan& p){std::string out="S2STEAM162\n";auto put=[&](const std::string& s){out+=std::to_string(s.size())+":"+s;};
    for(auto& s:{p.root,p.user,p.launcher,p.installed})put(s);put(p.original?"1":"0");put(p.original.value_or(""));put(p.before?"1":"0");put(p.before.value_or(""));return out;}
inline Plan unpack(const std::string& s){require(s.compare(0,11,"S2STEAM162\n")==0,"Unknown Steam ownership record");size_t p=11;
    auto get=[&](){auto colon=s.find(':',p);require(colon!=s.npos&&colon-p>0&&colon-p<=8,"Invalid Steam record");size_t n=0;for(size_t i=p;i<colon;++i){require(s[i]>='0'&&s[i]<='9',"Invalid Steam record length");n=n*10+s[i]-'0';}p=colon+1;require(n<=s.size()-p,"Truncated Steam record");auto out=s.substr(p,n);p+=n;return out;};
    Plan plan;plan.root=get();plan.user=get();plan.launcher=get();plan.installed=get();auto exists=get();auto original=get();require(exists=="0"||exists=="1","Invalid original Steam record");if(exists=="1")plan.original=original;exists=get();original=get();require(exists=="0"||exists=="1","Invalid transaction Steam record");if(exists=="1")plan.before=original;require(p==s.size(),"Trailing Steam ownership data");return plan;}
inline fs::path target(const Plan& p){require(!p.user.empty()&&p.user.size()<=10&&p.user.find_first_not_of("0123456789")==p.user.npos,"Invalid Steam user identity");auto root=fs::path(wide(p.root));require(root.is_absolute(),"Steam root must be absolute");return root/L"userdata"/wide(p.user)/L"config/localconfig.vdf";}
inline std::string installed(const std::string& launcher,const std::optional<std::string>& old){
    require(!old||old->find('%')==old->npos,"An existing Steam wrapper or expansion is present; preserved");
    return '"'+launcher+"\" %command%"+(old&&!old->empty()?" "+*old:"");
}
inline std::string user(const fs::path& root){
    auto login=Parser{read(root/L"config/loginusers.vdf")}.document();auto users=child(login,"users");require(users&&users->object,"Sign in to Steam once, then exit Steam before setup");std::string selected;
    for(auto& u:users->children){auto recent=child(u.children,"MostRecent");if(!recent||recent->value!="1")continue;
        require(selected.empty()&&u.object&&u.key.size()==17&&u.key.find_first_not_of("0123456789")==u.key.npos,"Ambiguous Steam account; preserved");
        auto id=std::stoull(u.key);require(id>76561197960265728ull&&id<=76561202255233023ull,"Unsupported Steam account identity");selected=std::to_string(id-76561197960265728ull);}
    require(!selected.empty(),"No recent Steam account. Sign in once, exit Steam and retry setup");return selected;
}
inline void closed(){Handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0));require(snapshot.h!=INVALID_HANDLE_VALUE,"Cannot check Steam processes");PROCESSENTRY32W e{};e.dwSize=sizeof e;require(Process32FirstW(snapshot.h,&e)!=0,"Cannot check Steam processes");
    do{require(_wcsicmp(e.szExeFile,L"steam.exe")!=0,"Exit Steam using Steam > Exit, then retry setup. Setup never closes Steam for you.");}while(Process32NextW(snapshot.h,&e));}
inline fs::path registry_root(){wchar_t text[32768];DWORD bytes=sizeof text;require(RegGetValueW(HKEY_CURRENT_USER,L"Software\\Valve\\Steam",L"SteamPath",RRF_RT_REG_SZ,nullptr,text,&bytes)==ERROR_SUCCESS,"Cannot locate Steam for this Windows account");auto path=fs::absolute(text).lexically_normal();plain(path);return path;}
inline Plan prepare(const fs::path& root,const fs::path& app){
    Plan p;p.root=utf8(root.wstring());p.user=user(root);p.launcher=utf8((app/L"shogun2-unit-possession-launcher.exe").wstring());
    p.before=value(read(target(p)));auto state=app/L"steam-launch162.state";
    if(fs::exists(state)){auto previous=unpack(read(state));require(previous.root==p.root&&previous.user==p.user&&previous.launcher==p.launcher,"Steam ownership belongs to another account or installation; preserved");
        require(equivalent(p.before,previous.original)||p.before==std::optional<std::string>(previous.installed),"Launch Options were edited outside setup; preserved");p.original=previous.original;
    }else{
        p.original=p.before;
        // Migrate the exact v0.31 manual wrapper, preserving its game arguments.
        std::string prefix='"'+p.launcher+"\" %command%";
        if(p.before&&p.before->compare(0,prefix.size(),prefix)==0&&(p.before->size()==prefix.size()||(*p.before)[prefix.size()]==' ')){
            p.original=p.before->size()==prefix.size()?std::optional<std::string>{}:std::optional<std::string>(p.before->substr(prefix.size()+1));}
    }
    p.installed=installed(p.launcher,p.original);target(p);return p;
}
inline void verify(const Plan& p,const fs::path& root,const fs::path& app){require(p.root==utf8(root.wstring())&&p.launcher==utf8((app/L"shogun2-unit-possession-launcher.exe").wstring())&&p.installed==installed(p.launcher,p.original),"Steam transaction identity differs; preserved");target(p);}
inline void apply(const Plan& p,const fs::path& app){auto file=target(p);auto text=read(file);require(value(text)==p.before,"Launch Options changed during setup; preserved");
    auto state=app/L"steam-launch162.state";
    if(!fs::exists(state))fresh(state,pack(p));
    else {auto old=unpack(read(state));require(old.root==p.root&&old.user==p.user&&old.launcher==p.launcher&&old.original==p.original&&old.installed==p.installed,"Steam ownership record differs; preserved");}
    if(p.before!=std::optional<std::string>(p.installed))replace(file,text,edit(text,p.installed));
    require(value(read(file))==std::optional<std::string>(p.installed),"Steam launch configuration verification failed");}
inline void restore(const Plan& p,const std::optional<std::string>& desired){auto file=target(p);auto text=read(file);auto current=value(text);
    require(equivalent(current,desired)||current==std::optional<std::string>(p.installed),"Launch Options changed outside setup; preserved. Restore them in Steam before uninstalling.");
    if(current!=desired)replace(file,text,edit(text,desired));}
}
