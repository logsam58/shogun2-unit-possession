#pragma once
#include "../../outputs/shogun2-possession-cleanup-v46/src/identity.h"
#include <shlobj.h>
#include <regex>
#include "installer_filesystem.h"
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
static std::wstring transaction_nonce(){unsigned char random[16]{};require(BCryptGenRandom(nullptr,random,sizeof random,BCRYPT_USE_SYSTEM_PREFERRED_RNG)>=0,"Cannot identify installer transaction");
    const wchar_t* digits=L"0123456789abcdef";std::wstring out;for(auto c:random){out+=digits[c>>4];out+=digits[c&15];}return out;}
inline std::function<void(const std::wstring&,const std::wstring&)> atomic_staged=[](const std::wstring&,const std::wstring&){};
static void atomic_bytes(const std::wstring& path,const std::string& bytes){safe_target(path);auto folder=path.substr(0,path.find_last_of(L"\\/"));int r=SHCreateDirectoryExW(nullptr,folder.c_str(),nullptr);require(r==ERROR_SUCCESS||r==ERROR_ALREADY_EXISTS,"Cannot create installation folder");
    // CREATE_NEW remains essential. Unknown/legacy stages are never overwritten
    // or deleted, and no fixed abandoned filename can block a later retry.
    auto tmp=path+L".possession-setup-"+transaction_nonce()+L".new";Handle f(CreateFileW(tmp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,nullptr));require(f.h!=INVALID_HANDLE_VALUE,"Cannot stage installation file");
    try{DWORD n=0;require(WriteFile(f.h,bytes.data(),DWORD(bytes.size()),&n,nullptr)&&n==bytes.size(),"Cannot write installation file");require(FlushFileBuffers(f.h)!=0,"Cannot flush installation file");CloseHandle(f.h);f.h=nullptr;atomic_staged(path,tmp);require(MoveFileExW(tmp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0,"Cannot replace installation file");}
    catch(...){if(f.h&&f.h!=INVALID_HANDLE_VALUE){CloseHandle(f.h);f.h=nullptr;}DeleteFileW(tmp.c_str());throw;}}
struct SavedFile {std::wstring path,backup;std::string bytes;bool existed{},changed{};};
