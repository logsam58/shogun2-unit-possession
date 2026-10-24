#pragma once
// Owned-game input only. Offsets are uint64_t: Shogun's Lua numbers are floats.
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <set>
#include <stdexcept>
#include "../../native/common/local_assets159.h"
namespace workshop165 {
inline constexpr char root_sha[]="255b305cefb24a68cf6d2fc79d07efad7c0252acfd98ecd3ad10fa55c959cae8";
inline void check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
inline void plain(const std::filesystem::path& path){
    auto absolute=std::filesystem::absolute(path).lexically_normal();
    check(absolute.wstring().size()>3&&absolute.wstring()[1]==L':',"Use a local drive path");
    std::filesystem::path current;
    for(auto& part:absolute){current/=part;DWORD attr=GetFileAttributesW(current.c_str());
        if(attr!=INVALID_FILE_ATTRIBUTES)check(!(attr&FILE_ATTRIBUTE_REPARSE_POINT),"Reparse cache/input refused");
        else check(GetLastError()==ERROR_FILE_NOT_FOUND||GetLastError()==ERROR_PATH_NOT_FOUND,"Cannot inspect local path");}
}
inline std::vector<unsigned char> original_root(const std::filesystem::path& pack){
    plain(pack);std::ifstream in(pack,std::ios::binary);check(bool(in),"Owned data.pack is missing");
    unsigned char header[32]{};check(bool(in.read((char*)header,32)),"Truncated pack header");
    uint32_t flags=assets159::u32(header+4),count=assets159::u32(header+16),index_size=assets159::u32(header+20);
    uint64_t total=std::filesystem::file_size(pack);
    check(!memcmp(header,"PFH3",4)&&(flags==0||flags==1||flags==3)&&!assets159::u32(header+8)&&!assets159::u32(header+12)&&
          count<=100000&&index_size<=16*1024*1024&&32ull+index_size<=total,"Unsupported pack index");
    std::vector<unsigned char> index(index_size);check(bool(in.read((char*)index.data(),index.size())),"Truncated pack index");
    size_t cursor=0;uint64_t offset=32ull+index_size,root_offset=0;unsigned root_size=0;std::set<std::string> seen;
    for(unsigned i=0;i<count;++i){
        check(cursor+4<=index.size(),"Truncated pack entry");unsigned size=assets159::u32(index.data()+cursor);cursor+=4;
        auto begin=cursor;while(cursor<index.size()&&index[cursor])++cursor;check(cursor<index.size(),"Unterminated pack name");
        std::string name((char*)index.data()+begin,cursor-begin);++cursor;std::replace(name.begin(),name.end(),'\\','/');
        std::transform(name.begin(),name.end(),name.begin(),[](unsigned char c){return char(tolower(c));});
        check(!name.empty()&&name.front()!='/'&&name.find(':')==name.npos&&name.find("//")==name.npos&&name.find("../")==name.npos&&
              name.find("/..") == name.npos&&name.find("./")==name.npos&&seen.insert(name).second&&offset<=total&&size<=total-offset,"Unsafe/duplicate/out-of-range pack entry");
        if(name=="ui/frontend ui/layout_scripts/root.luac"){root_offset=offset;root_size=size;}
        offset+=size;
    }
    check(cursor==index.size()&&root_size==13739,"Owned root script is missing or changed");
    in.seekg(std::streamoff(root_offset));std::vector<unsigned char> bytes(root_size);
    check(bool(in.read((char*)bytes.data(),bytes.size())),"Truncated owned root script");
    check(assets159::sha(bytes.data(),bytes.size())==root_sha,"Owned root script fingerprint differs");return bytes;
}
inline void create_exact(const std::filesystem::path& path,const std::vector<unsigned char>& bytes){
    plain(path);
    auto matches=[&](){
        std::ifstream in(path,std::ios::binary);check(bool(in)&&std::filesystem::file_size(path)==bytes.size(),"Existing cache differs; preserved");
        std::vector<unsigned char> actual(bytes.size());check(bool(in.read((char*)actual.data(),actual.size()))&&actual==bytes,"Existing cache differs; preserved");
    };
    if(std::filesystem::exists(path)){matches();return;}
    auto staging=path.wstring()+L".pending166";plain(staging);
    if(std::filesystem::exists(staging)){
        std::ifstream in(staging,std::ios::binary);std::vector<unsigned char> previous(bytes.size()+1);
        in.read((char*)previous.data(),previous.size());previous.resize(size_t(in.gcount()));in.close();
        check(previous==bytes,"Unknown/incomplete cache staging preserved; no further copies created");
        if(!MoveFileExW(staging.c_str(),path.c_str(),MOVEFILE_WRITE_THROUGH)){matches();check(DeleteFileW(staging.c_str())!=0,"Cannot retire exact owned staging duplicate");}
        return;
    }
    HANDLE file=CreateFileW(staging.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,nullptr);
    check(file!=INVALID_HANDLE_VALUE,"Cannot create exclusive cache staging");
    DWORD written=0;bool ok=bytes.size()<=MAXDWORD&&WriteFile(file,bytes.data(),DWORD(bytes.size()),&written,nullptr)&&written==bytes.size()&&FlushFileBuffers(file);
    CloseHandle(file);
    if(!ok){DeleteFileW(staging.c_str());check(false,"Cannot finish owned cache staging");}
    if(!MoveFileExW(staging.c_str(),path.c_str(),MOVEFILE_WRITE_THROUGH)){
        try{matches();}catch(...){DeleteFileW(staging.c_str());throw;}
        check(DeleteFileW(staging.c_str())!=0,"Cannot retire exact owned staging duplicate");
    }
}
// A small live-check receipt exists only for this process. Windows removes it
// even after a crash/forced exit. Readers must use FILE_SHARE_DELETE as well as
// FILE_SHARE_READ; the receipt never gates gameplay initialization.
class LiveReceipt166 {
    HANDLE file=INVALID_HANDLE_VALUE;
public:
    LiveReceipt166()=default;
    LiveReceipt166(const LiveReceipt166&)=delete;
    ~LiveReceipt166(){if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);}
    bool write(const std::filesystem::path& path,const std::vector<unsigned char>& bytes){
        if(file!=INVALID_HANDLE_VALUE)return true;
        if(bytes.empty()||bytes.size()>4096)return false;
        plain(path);
        file=CreateFileW(path.c_str(),GENERIC_WRITE|DELETE,FILE_SHARE_READ|FILE_SHARE_DELETE,nullptr,CREATE_NEW,
            FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_DELETE_ON_CLOSE,nullptr);
        if(file==INVALID_HANDLE_VALUE)return false;
        DWORD written=0;
        if(!WriteFile(file,bytes.data(),DWORD(bytes.size()),&written,nullptr)||written!=bytes.size()||!FlushFileBuffers(file)){
            CloseHandle(file);file=INVALID_HANDLE_VALUE;return false;
        }
        return true;
    }
};
}
