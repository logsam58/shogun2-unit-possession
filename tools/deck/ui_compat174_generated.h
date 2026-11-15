#pragma once
#include "pack174_generated.h"
#include <cwctype>
#include <regex>
#include <string_view>

namespace workshop168 {
inline std::wstring lower(std::wstring s){for(auto& c:s)c=wchar_t(towlower(c));return s;}
struct Directive {std::wstring key,value;};
// Parse directives rather than stripping // inside quoted Windows paths.
inline std::vector<Directive> selection(const std::wstring& text){
    workshop165::check(text.size()<=524288,"Mod selection exceeds bound");
    std::vector<Directive> out;size_t p=0;
    auto white=[&](){
        for(;;){
            while(p<text.size()&&iswspace(text[p]))++p;
            if(p<text.size()&&(text[p]==L'#'||text.compare(p,2,L"//")==0)){
                while(p<text.size()&&text[p]!=L'\n')++p;continue;
            }
            if(text.compare(p,2,L"/*")==0){auto end=text.find(L"*/",p+2);workshop165::check(end!=text.npos,"Unclosed selection comment");p=end+2;continue;}
            break;
        }
    };
    while(p<text.size()){
        white();if(p==text.size())break;
        auto begin=p;while(p<text.size()&&(iswalnum(text[p])||text[p]==L'_'))++p;
        workshop165::check(p>begin,"Unsupported mod selection syntax");
        auto key=lower(text.substr(begin,p-begin));white();std::wstring value;
        if(p<text.size()&&text[p]==L'\"'){
            begin=++p;while(p<text.size()&&text[p]!=L'\"'&&text[p]!=L'\r'&&text[p]!=L'\n'&&text[p])++p;
            workshop165::check(p<text.size()&&text[p]==L'\"',"Unclosed mod selection path");value=text.substr(begin,p-begin);++p;
        }else{
            begin=p;while(p<text.size()&&text[p]!=L';'&&!iswspace(text[p]))++p;value=text.substr(begin,p-begin);
        }
        white();workshop165::check(p<text.size()&&text[p]==L';',"Mod selection directive lacks semicolon");++p;
        if(key==L"mod"||key==L"add_working_directory")out.push_back({key,value});
        workshop165::check(out.size()<=512,"Too many active mod directives");
    }
    return out;
}
inline bool own_root169(const std::vector<unsigned char>& root,const std::string& metadata,const std::string& native_sha){
    static const std::regex family(R"("first_party_bootstrap"\s*:\s*true\b)");
    static const std::regex fingerprint(R"sha("native_sha256"\s*:\s*"([a-f0-9]{64})")sha");
    const bool related=std::regex_search(metadata,family);
    const std::string_view script(reinterpret_cast<const char*>(root.data()),root.size());
    constexpr std::string_view prefix="/S2UP-Workshop165-";
    constexpr size_t hash_size=64;
    bool same=false,foreign=false;
    for(auto at=script.find(prefix);at!=script.npos;at=script.find(prefix,at+prefix.size())){
        const auto hash=script.substr(at+prefix.size(),hash_size);
        if(hash.size()!=hash_size||hash.find_first_not_of("0123456789abcdef")!=hash.npos||
           script.substr(at+prefix.size()+hash_size,4)!=".dll")continue;
        if(hash==native_sha)same=true;
        else foreign=true;
    }
    if(!related&&!same&&!foreign)return false;
    // Prevent delegation back into this bootstrap, even if its pack was renamed.
    // A different/old possession runtime must never be treated as a menu addon.
    std::smatch match;std::sregex_iterator first(metadata.begin(),metadata.end(),fingerprint),last;
    workshop165::check(related&&same&&!foreign&&std::distance(first,last)==1&&std::regex_search(metadata,match,fingerprint)&&match[1]==native_sha,
        "Another, older or incomplete Unit Possession build is enabled; keep one current copy active");
    return true;
}
inline std::vector<unsigned char> selected_root(const std::filesystem::path& game,const std::wstring& text,const std::string& native_sha="",const char* target_root="ui/frontend ui/layout_scripts/root.luac",unsigned expected_size=13739,const char* expected_sha=workshop165::root_sha){
    auto directives=selection(text);
    std::vector<std::filesystem::path> folders={(game/L"data").lexically_normal()};
    std::set<std::wstring> search_paths;
    for(const auto& d:directives)if(d.key==L"add_working_directory"){
        std::filesystem::path p=d.value;workshop165::check(p.is_absolute(),"Relative mod search directory is unsupported");
        auto normalized=p.lexically_normal();
        if(!search_paths.insert(normalized.wstring()).second)continue;
        deck174::paths::read_directory(p);if(normalized!=folders.front()&&std::filesystem::is_directory(p))folders.push_back(std::move(normalized));
    }
    // Deduplicate exact directives while retaining case-distinct host paths.
    std::set<std::wstring> selected_names,visited;
    std::vector<unsigned char> chosen;
    std::filesystem::path chosen_pack;
    for(const auto& d:directives)if(d.key==L"mod"){
        std::filesystem::path name=d.value;
        workshop165::check(name==name.filename()&&lower(name.extension().wstring())==L".pack","Unsupported active mod filename");
        if(!selected_names.insert(name.wstring()).second)continue;
        std::vector<unsigned char> candidate;
        bool any_match=false,any_plain=false,any_self=false,any_external=false;
        for(const auto& folder:folders){auto p=(folder/name).lexically_normal();
            if(!std::filesystem::is_regular_file(p))continue;
            // Case-distinct paths may be different files, even in a Windows process.
            if(!visited.insert(p.wstring()).second)continue;
            any_match=true;
            try{
                std::string metadata;
                auto root=workshop165::ui_root168(p,false,&metadata,target_root,expected_size,expected_sha);
                if(root.empty()){any_plain=true;continue;}
                if(own_root169(root,metadata,native_sha)){any_self=true;continue;}
                any_external=true;
                workshop165::check(root.size()<=262144,"Menu addon root exceeds the 256 KiB handoff bound");
                if(candidate.empty())candidate=std::move(root);
                else workshop165::check(root==candidate,"Different UI roots in multiple search folders; load order is ambiguous");
            }catch(const std::exception& error){
                throw std::runtime_error("Enabled pack "+name.u8string()+": "+error.what());
            }
        }
        if(!any_match)continue;
        if(any_self){
            if(any_external||any_plain)
                throw std::runtime_error("Enabled pack "+name.u8string()+": filename is shared with another mod; keep one current Unit Possession copy active");
            continue;
        }
        // Duplicate database-only names do not create a menu-script conflict.
        if(!any_external)continue;
        if(any_plain)throw std::runtime_error("Enabled pack "+name.u8string()+": resolves to both a UI and a non-UI pack; load order is ambiguous");
        if(chosen.empty()){
            chosen=std::move(candidate);
            chosen_pack=name;
        }else if(candidate!=chosen){
            throw std::runtime_error("Enabled packs "+chosen_pack.u8string()+" and "+name.u8string()+
                " replace the same UI root with different scripts");
        }
    }
    return chosen.empty()?workshop165::ui_root168(game/L"data/data.pack",true,nullptr,target_root,expected_size,expected_sha):chosen;
}
}
