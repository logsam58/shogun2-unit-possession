#pragma once
// Data-only, locally generated resources. No native game pointers or hooks.
#include <windows.h>
#include <bcrypt.h>
#include <cstdint>
#include <cstring>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include "local_assets159_recipe.h"
namespace assets159 {
struct Blob {const unsigned char* bytes{};uint32_t size{},width{},height{};};
inline std::string sha(const void* data,size_t size){
    BCRYPT_ALG_HANDLE a{};BCRYPT_HASH_HANDLE h{};unsigned char result[32]{};
    if(size>0xffffffffu||BCryptOpenAlgorithmProvider(&a,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0)return {};
    auto r=BCryptCreateHash(a,&h,nullptr,0,nullptr,0,0);
    if(r>=0)r=BCryptHashData(h,(PUCHAR)data,ULONG(size),0);
    if(r>=0)r=BCryptFinishHash(h,result,32,0);
    if(h)BCryptDestroyHash(h);BCryptCloseAlgorithmProvider(a,0);if(r<0)return {};
    const char* hex="0123456789abcdef";std::string out;for(auto x:result){out+=hex[x>>4];out+=hex[x&15];}return out;
}
inline uint32_t u32(const unsigned char* p){uint32_t n;memcpy(&n,p,4);return n;}
class Store {
    HANDLE file=INVALID_HANDLE_VALUE,map{};const unsigned char* data{};
    std::map<uint32_t,Blob> entries;
public:
    Store()=default;Store(const Store&)=delete;Store& operator=(const Store&)=delete;
    ~Store(){clear();}
    void clear(){entries.clear();if(data)UnmapViewOfFile(data);data=nullptr;if(map)CloseHandle(map);map=nullptr;if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);file=INVALID_HANDLE_VALUE;}
    bool load(const std::wstring& path,unsigned kind){
        clear();file=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(file==INVALID_HANDLE_VALUE)return false;LARGE_INTEGER length{};
        if(!GetFileSizeEx(file,&length)||length.QuadPart<84||length.QuadPart>128*1024*1024){clear();return false;}
        map=CreateFileMappingW(file,nullptr,PAGE_READONLY,0,0,nullptr);if(!map){clear();return false;}
        data=(const unsigned char*)MapViewOfFile(map,FILE_MAP_READ,0,0,0);if(!data){clear();return false;}
        size_t n=size_t(length.QuadPart),p=84;unsigned count=u32(data+16);
        if(memcmp(data,"S2FPAS1\0",8)||u32(data+8)!=159||u32(data+12)!=kind||count>32||count<21||memcmp(data+20,recipe,64)){clear();return false;}
        for(unsigned i=0;i<count;++i){
            if(p+84>n){clear();return false;}unsigned id=u32(data+p),w=u32(data+p+4),h=u32(data+p+8),size=u32(data+p+12),type=u32(data+p+16);
            if(size>32*1024*1024||p+84+size>n||entries.count(id)||type!=((w||h)?1u:0u)){clear();return false;}
            const auto* bytes=data+p+84;
            if((type==1&&(!w||!h||w>4096||h>4096||uint64_t(w)*h*4!=size))||sha(bytes,size)!=std::string((const char*)data+p+20,64)){clear();return false;}
            if(!((id>=101&&id<=103)||(id>=201&&id<=220)||id==301||id==430||id==431)){clear();return false;}
            entries.emplace(id,Blob{bytes,size,w,h});p+=84+size;
        }
        if(p!=n||entries.size()!=(kind==1?26u:21u)){clear();return false;}
        for(unsigned i=201;i<=220;++i)if(!get(i).bytes){clear();return false;}
        for(unsigned i=101;i<=(kind==1?103u:101u);++i)if(!get(i).width){clear();return false;}
        if(kind==1&&(!get(301).bytes||!get(430).width||!get(431).width)){clear();return false;}
        return true;
    }
    Blob get(unsigned id)const{auto it=entries.find(id);return it==entries.end()?Blob{}:it->second;}
};
inline std::wstring beside(HMODULE module,const wchar_t* filename){wchar_t path[32768];DWORD n=GetModuleFileNameW(module,path,32768);if(!n||n>=32768)return {};std::wstring s(path);auto slash=s.find_last_of(L"\\/");return slash==s.npos?L"":s.substr(0,slash+1)+filename;}
inline Blob resource(HMODULE module,unsigned id){
    static Store store;static std::mutex mutex;static bool loaded=false;
    std::lock_guard<std::mutex> guard(mutex);
    if(!loaded){
#ifdef HUD80_QTE_ONLY
        loaded=store.load(beside(module,L"qte-assets159.s2fp"),2);
#else
        loaded=store.load(beside(module,L"hud-assets159.s2fp"),1);
#endif
    }
    return loaded?store.get(id):Blob{};
}
}
