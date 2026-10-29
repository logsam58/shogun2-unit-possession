#pragma once
// One admission implementation for bridge, worker, adapter, QTE and HUD.
// No engine calls, writes, background threads, logs or executable scanning.
#include <windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "engine_profiles170.h"
#include "engine_contract172_generated.h"

namespace engine172 {
inline std::string at(unsigned rva){std::ostringstream o;o<<"0x"<<std::hex<<rva;return o.str();}
inline void need(bool ok,const std::string& why){if(!ok)throw std::runtime_error("Engine compatibility: "+why);}
struct File {
    HANDLE handle=INVALID_HANDLE_VALUE;
    explicit File(const std::wstring& path){
        handle=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        need(handle!=INVALID_HANDLE_VALUE,"cannot open engine for a stable read");
    }
    ~File(){if(handle!=INVALID_HANDLE_VALUE)CloseHandle(handle);}
    File(const File&)=delete;File& operator=(const File&)=delete;
};
struct Hasher {
    BCRYPT_ALG_HANDLE alg=nullptr;
    Hasher(){need(BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)==0,"SHA256 unavailable");}
    ~Hasher(){if(alg)BCryptCloseAlgorithmProvider(alg,0);}
    Hasher(const Hasher&)=delete;Hasher& operator=(const Hasher&)=delete;
    std::string digest(const unsigned char* p,size_t n){
        need(n<=64*1024*1024,"digest bound");
        DWORD size=0,written=0;BCRYPT_HASH_HANDLE h=nullptr;
        need(BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,(PUCHAR)&size,sizeof size,&written,0)==0&&size<=65536,"SHA256 state unavailable");
        std::vector<unsigned char> object(size);std::array<unsigned char,32> out{};
        need(BCryptCreateHash(alg,&h,object.data(),size,nullptr,0,0)==0,"SHA256 initialization failed");
        auto status=BCryptHashData(h,const_cast<PUCHAR>(p),ULONG(n),0);
        if(status==0)status=BCryptFinishHash(h,out.data(),ULONG(out.size()),0);
        BCryptDestroyHash(h);need(status==0,"SHA256 calculation failed");
        std::ostringstream s;for(auto v:out)s<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(v);return s.str();
    }
};
struct Result {
    unsigned version=schema;
    std::string contract=contract_id,fingerprint;
    bool reviewed_image=false;
    unsigned image_size=0x1d88000,preferred_base=0x10000000;
};
struct Image {
    std::vector<unsigned char> data;
    unsigned pe=0,opt=0,table=0;
    unsigned u16(size_t at)const{need(at<=data.size()&&data.size()-at>=2,"truncated PE field");return unsigned(data[at])|(unsigned(data[at+1])<<8);}
    unsigned u32(size_t at)const{need(at<=data.size()&&data.size()-at>=4,"truncated PE field");return unsigned(data[at])|(unsigned(data[at+1])<<8)|(unsigned(data[at+2])<<16)|(unsigned(data[at+3])<<24);}
    void put32(size_t pos,unsigned value){need(pos<=data.size()&&data.size()-pos>=4,"header bound");memcpy(data.data()+pos,&value,4);}
    unsigned raw(unsigned rva,unsigned length=1)const{
        for(const auto& s:sections)if(rva>=s.rva&&rva-s.rva<s.size){
            auto delta=rva-s.rva;need(length<=s.size-delta&&s.raw<=data.size()&&size_t(s.raw)+delta+length<=data.size(),"RVA range exceeds file at "+at(rva));return s.raw+delta;
        }
        throw std::runtime_error("Engine compatibility: unmapped RVA "+at(rva));
    }
    void shape(Hasher& hash){
        need(data.size()>=1024&&data.size()<=64*1024*1024,"engine size outside bounds");
        need(u16(0)==0x5a4d,"not a PE image");pe=u32(0x3c);
        need(pe>=64&&pe<=4096&&u32(pe)==0x4550,"invalid PE header");opt=pe+24;
        need(u16(pe+4)==0x14c&&u16(opt)==0x10b,"requires original x86 engine family");
        need(u16(pe+6)==std::size(sections)&&u16(pe+20)==224,"section layout differs");
        need(u32(opt+28)==0x10000000&&u32(opt+56)==0x1d88000&&u32(opt+60)==header_size,"engine address layout differs");
        table=opt+u16(pe+20);need(table+std::size(sections)*40<=header_size&&header_size<=data.size(),"section table bound");
        unsigned last=header_size;
        for(unsigned i=0;i<std::size(sections);++i){
            const auto& s=sections[i];auto p=table+i*40;
            need(!memcmp(data.data()+p,s.name,strlen(s.name))&&data[p+strlen(s.name)]==0,"section identity differs");
            bool virtual_ok=u32(p+8)==s.virtual_size;
            if(!strcmp(s.name,".rdata"))virtual_ok=u32(p+8)==rdata_virtual_sizes[0]||u32(p+8)==rdata_virtual_sizes[1];
            need(virtual_ok&&u32(p+12)==s.rva&&u32(p+16)==s.size&&u32(p+20)==s.raw&&u32(p+36)==s.flags,"section contract differs at "+at(s.rva));
            need(s.raw>=last&&s.raw<=data.size()&&s.size<=data.size()-s.raw,"overlapping/truncated sections");last=s.raw+s.size;
        }
        // Certificates are unmapped file metadata. Never permit them to point
        // inside mapped engine bytes. Overlay bytes confer no engine binding.
        auto cert=u32(opt+96+4*8),cert_size=u32(opt+96+4*8+4);
        need((!cert&&!cert_size)||(cert>=last&&!(cert&7)&&cert_size<=data.size()&&cert<=data.size()-cert_size),"invalid certificate range");
        std::vector<unsigned char> header(data.begin(),data.begin()+header_size);
        auto put=[&](size_t pos,unsigned value){need(pos<=header.size()&&header.size()-pos>=4,"normalized header bound");memcpy(header.data()+pos,&value,4);};
        put(pe+8,0);put(opt+64,0);put(opt+96+4*8,0);put(opt+96+4*8+4,0);
        for(unsigned i=0;i<std::size(sections);++i)if(!strcmp(sections[i].name,".rdata"))put(table+i*40+8,sections[i].virtual_size);
        bool valid=hash.digest(header.data(),header.size())==header_sha;
        need(valid,"PE header/import/layout contract differs");
    }
};
inline Result inspect(Image& image,Hasher& hash,const std::string& expected={}){
    Result result;result.fingerprint=hash.digest(image.data.data(),image.data.size());
    need(expected.empty()||result.fingerprint==expected,"engine changed between identity and validation reads");
    try {
    image.shape(hash);result.reviewed_image=engine170::supported(result.fingerprint);
    bool stock_match=true,bugfix_match=true;
    if(!result.reviewed_image)for(const auto& r:regions){
        auto p=image.raw(r.rva,r.size);need(p==r.raw,"protected region mapping differs");
        auto value=hash.digest(image.data.data()+p,r.size);
        stock_match=stock_match&&value==r.stock;bugfix_match=bugfix_match&&value==r.bugfix;
        need(stock_match||bugfix_match,"required native region differs at RVA "+at(r.rva)+" (contract "+std::string(contract_id,12)+", engine "+result.fingerprint.substr(0,12)+")");
    }
    } catch(const std::exception& e){
        throw std::runtime_error(std::string(e.what())+" [contract="+std::string(contract_id,12)+", engine="+result.fingerprint+"]");
    }
    return result;
}
inline Result admit(const std::wstring& path,const std::string& expected={}){
    // Successful admissions retain their read-only file handle for module
    // lifetime. No later disk replacement can invalidate a published binding.
    struct Cached {std::wstring path;Result result;std::shared_ptr<File> file;};
    static std::mutex mutex;static std::vector<Cached> cache;
    std::lock_guard<std::mutex> lock(mutex);
    for(const auto& prior:cache)if(!_wcsicmp(prior.path.c_str(),path.c_str())){
        need(expected.empty()||expected==prior.result.fingerprint,"engine changed between identity and validation reads");return prior.result;
    }
    need(cache.size()<4,"too many engine identities in this component");
    auto file=std::make_shared<File>(path);LARGE_INTEGER size{};
    need(GetFileSizeEx(file->handle,&size)&&size.QuadPart>=1024&&size.QuadPart<=64*1024*1024,"engine file size outside bounds");
    Image image;image.data.resize(size_t(size.QuadPart));size_t offset=0;
    while(offset<image.data.size()){
        DWORD read=0,request=DWORD((std::min)(size_t(1<<20),image.data.size()-offset));
        need(ReadFile(file->handle,image.data.data()+offset,request,&read,nullptr)&&read==request,"engine read incomplete");offset+=read;
    }
    Hasher hash;auto result=inspect(image,hash,expected);cache.push_back({path,result,file});return result;
}
inline void live_hooks(uintptr_t base,unsigned kind,const Result& binding){
    need(binding.version==schema&&binding.contract==contract_id&&binding.fingerprint.size()==64,"stale/foreign engine binding");
    wchar_t path[32768]{};auto n=GetModuleFileNameW(reinterpret_cast<HMODULE>(base),path,DWORD(std::size(path)));
    need(n&&n<std::size(path),"loaded engine identity unavailable");
    const auto resident=admit(path);
    need(resident.fingerprint==binding.fingerprint,"loaded engine differs from validated binding");
    Hasher hash;
    for(const auto& h:hooks){if(h.kind!=kind)continue;
        std::array<unsigned char,16> code{};SIZE_T n=0;
        need(base<=UINTPTR_MAX-h.rva&&ReadProcessMemory(GetCurrentProcess(),(const void*)(base+h.rva),code.data(),code.size(),&n)&&n==code.size(),"cannot read hook at RVA "+at(h.rva));
        // Prefixes contain no relocatable operands in this pinned family.
        need(hash.digest(code.data(),code.size())==h.sha,"occupied/changed hook at RVA "+at(h.rva)+" (contract "+std::string(contract_id,12)+", engine "+binding.fingerprint+")");
    }
}
}
