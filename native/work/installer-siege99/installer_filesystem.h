#pragma once
static void safe_target(const std::wstring& input){wchar_t buffer[32768];auto n=GetFullPathNameW(input.c_str(),32768,buffer,nullptr);
    require(n&&n<32768,"Invalid installation target");std::wstring path(buffer);require(path.size()>3&&path[1]==L':'&&path[2]==L'\\',"Use an absolute local installation path");
    for(size_t i=3;i<=path.size();++i)if(i==path.size()||path[i]==L'\\'){
        auto prefix=path.substr(0,i);auto a=GetFileAttributesW(prefix.c_str());
        if(a==INVALID_FILE_ATTRIBUTES){auto e=GetLastError();require(e==ERROR_FILE_NOT_FOUND||e==ERROR_PATH_NOT_FOUND,"Cannot inspect installation path");continue;}
        require(!(a&FILE_ATTRIBUTE_REPARSE_POINT),"Reparse installation path; preserved");
        require(bool(a&FILE_ATTRIBUTE_DIRECTORY)==(i!=path.size()),"Unexpected file/directory at installation target");}}
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
