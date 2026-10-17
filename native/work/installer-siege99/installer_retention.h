#pragma once
#include "cache_catalog.h"

// Only installer-owned paths are considered. Reparse points are never followed.
struct RetentionReport { unsigned caches_removed{}, backups_removed{}, deferred{}; };
static RetentionReport retention_report;
static bool plain_path(const std::wstring& path, bool directory) {
    DWORD a=GetFileAttributesW(path.c_str());
    return a!=INVALID_FILE_ATTRIBUTES && !(a&FILE_ATTRIBUTE_REPARSE_POINT) && bool(a&FILE_ATTRIBUTE_DIRECTORY)==directory;
}
static bool plain_ancestors(std::wstring path) {
    wchar_t absolute[32768];DWORD n=GetFullPathNameW(path.c_str(),32768,absolute,nullptr);
    if(!n||n>=32768)return false;path=absolute;
    if(path.size()<3||path[1]!=L':'||path[2]!=L'\\')return false;
    for(size_t end=3;end<=path.size();++end)if(end==path.size()||path[end]==L'\\') {
        auto prefix=path.substr(0,end);if(!plain_path(prefix,true))return false;
    }
    return true;
}
static std::vector<std::wstring> children(const std::wstring& folder) {
    std::vector<std::wstring> names;WIN32_FIND_DATAW data{};
    HANDLE h=FindFirstFileW((folder+L"\\*").c_str(),&data);
    if(h==INVALID_HANDLE_VALUE)return names;
    do {std::wstring name=data.cFileName;if(name!=L"."&&name!=L"..")names.push_back(name);}while(FindNextFileW(h,&data));
    FindClose(h);return names;
}
static bool numeric_suffix(const std::wstring& name,const std::wstring& prefix) {
    return name.size()>prefix.size()&&name.compare(0,prefix.size(),prefix)==0 &&
        std::all_of(name.begin()+prefix.size(),name.end(),[](wchar_t c){return c>=L'0'&&c<=L'9';});
}
static bool owned_log(const std::wstring& name,bool private_log) {
    static const std::wregex outer(L"(?:autoload-[0-9]+\\.log|session-v[0-9]+-[0-9]+\\.jsonl)");
    static const std::wregex inner(L"qte-playable[0-9]+-[0-9]+\\.jsonl");
    return std::regex_match(name,private_log?inner:outer);
}
static void prune_legacy_backups(const std::wstring& path) {
    auto sep=path.find_last_of(L"\\/");auto folder=path.substr(0,sep),base=path.substr(sep+1);
    if(!plain_ancestors(folder)){++retention_report.deferred;return;}
    for(auto& name:children(folder))if(numeric_suffix(name,base+L".possession-before-")||numeric_suffix(name,base+L".checkpoint45-before-")) {
        auto old=folder+L"\\"+name;if(!plain_path(old,false)){++retention_report.deferred;continue;}
        if(DeleteFileW(old.c_str()))++retention_report.backups_removed;else ++retention_report.deferred;
    }
}
static void prune_runtime_caches(const std::wstring& localdata) {
    auto root=localdata+L"\\Shogun2PossessionCheckpoint45";
    if(GetFileAttributesW(root.c_str())==INVALID_FILE_ATTRIBUTES)return;
    if(!plain_ancestors(root)){++retention_report.deferred;return;}
    for(auto& name:children(root)) {
        if(name==std::wstring(L"integrated-")+EMBEDDED_BUILD)continue;
        const CacheIdentity* identity=nullptr;
        for(auto& row:CACHE_CATALOG)if(name==std::wstring(L"integrated-")+row.build){identity=&row;break;}
        if(!identity){if(std::regex_match(name,std::wregex(L"integrated-[0-9a-f]{16}")))++retention_report.deferred;continue;}
        auto folder=root+L"\\"+name;
        if(!plain_path(folder,true)){++retention_report.deferred;continue;}
        std::vector<std::wstring> files;bool safe=true;
        try {
            for(auto& item:identity->files) {
                auto path=folder+L"\\"+item.name;
                if(!plain_path(path,false)||(GetFileAttributesW(path.c_str())&FILE_ATTRIBUTE_READONLY)||hash_file(path)!=item.sha){safe=false;break;}
                files.push_back(path);
            }
            if(!safe){++retention_report.deferred;continue;}
            for(auto& child:children(folder)) {
                auto path=folder+L"\\"+child;
                if(std::find(files.begin(),files.end(),path)!=files.end())continue;
                if(child==L"private"&&plain_path(path,true)) {
                    for(auto& log:children(path)) {
                        auto logpath=path+L"\\"+log;
                        if(!owned_log(log,true)||!plain_path(logpath,false)||(GetFileAttributesW(logpath.c_str())&FILE_ATTRIBUTE_READONLY)){safe=false;break;}
                        files.push_back(logpath);
                    }
                } else if(owned_log(child,false)&&plain_path(path,false)&&!(GetFileAttributesW(path.c_str())&FILE_ATTRIBUTE_READONLY))files.push_back(path);
                else safe=false;
                if(!safe)break;
            }
        } catch(...) {safe=false;}
        if(!safe){++retention_report.deferred;continue;}
        // Probe all handles before removing anything; running/locked images defer cleanup.
        std::vector<HANDLE> handles;
        for(auto& path:files) {
            HANDLE h=CreateFileW(path.c_str(),DELETE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,FILE_FLAG_OPEN_REPARSE_POINT,nullptr);
            if(h==INVALID_HANDLE_VALUE){safe=false;break;}
            FILE_ATTRIBUTE_TAG_INFO info{};
            if(!GetFileInformationByHandleEx(h,FileAttributeTagInfo,&info,sizeof info)||(info.FileAttributes&(FILE_ATTRIBUTE_REPARSE_POINT|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_DIRECTORY)))safe=false;
            handles.push_back(h);if(!safe)break;
        }
        if(safe)for(auto h:handles) {FILE_DISPOSITION_INFO info{TRUE};if(!SetFileInformationByHandle(h,FileDispositionInfo,&info,sizeof info))safe=false;}
        for(auto h:handles)CloseHandle(h);
        if(!safe){++retention_report.deferred;continue;}
        auto private_folder=folder+L"\\private";
        if(GetFileAttributesW(private_folder.c_str())!=INVALID_FILE_ATTRIBUTES&&!RemoveDirectoryW(private_folder.c_str())){++retention_report.deferred;continue;}
        if(RemoveDirectoryW(folder.c_str()))++retention_report.caches_removed;else ++retention_report.deferred;
    }
}
