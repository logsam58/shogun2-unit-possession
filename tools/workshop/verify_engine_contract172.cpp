// Offline only: owned files/images, no engine entry point or gameplay calls.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "../../native/common/engine_contract172.h"
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace engine172;
static unsigned checks172=0;
static Image read172(const std::filesystem::path& path){
    std::ifstream in(path,std::ios::binary);need(bool(in),"fixture input missing");
    Image image;image.data.assign(std::istreambuf_iterator<char>(in),{});return image;
}
static void refused172(Image image,const std::string& reason){
    Hasher hash;bool refused=false;
    try{(void)inspect(image,hash);}catch(const std::exception& e){refused=std::string(e.what()).find(reason)!=std::string::npos;}
    need(refused,"negative fixture did not reject: "+reason);++checks172;
}
int wmain(int argc,wchar_t** argv){try{
    need(argc==4,"Use owned stock, reviewed engine and a new fixture directory");
    auto root=std::filesystem::path(argv[3]);need(std::filesystem::create_directory(root),"fixture folder must be new");
    auto stock=read172(argv[1]),modified=read172(argv[2]);Hasher hash;
    need(inspect(stock,hash).fingerprint==engine170::stock_sha,"wrong stock fixture");++checks172;
    need(inspect(modified,hash).fingerprint==engine170::bugfix_sha,"wrong reviewed fixture");++checks172;
    auto metadata=stock;metadata.data[metadata.pe+8]^=1;
    need(!inspect(metadata,hash).reviewed_image,"metadata edit incorrectly used allowlist");++checks172;
    auto overlay=stock;overlay.data.insert(overlay.data.end(),1024,0x5a);
    need(!inspect(overlay,hash).reviewed_image,"unmapped overlay rejected");++checks172;
    auto bad=stock;bad.data[bad.opt+16]^=1;refused172(bad,"header/import/layout");
    bad=stock;bad.data[bad.pe+4]^=1;refused172(bad,"x86 engine family");
    bad=stock;bad.data[bad.opt+56]^=1;refused172(bad,"address layout");
    bad=stock;bad.put32(bad.opt+96+4*8,0x400);bad.put32(bad.opt+96+4*8+4,0x80);refused172(bad,"certificate");
    bad=stock;bad.data.resize(1024);refused172(bad,"truncated sections");
    bad=stock;bad.data.resize(40);refused172(bad,"size outside bounds");
    for(const auto& h:hooks){
        bad=stock;bad.data[bad.raw(h.rva)]^=1;refused172(bad,"required native region differs");
    }
    for(unsigned rv:{0x15dfec8u,0x15dbd68u,0x15e8524u,0x15d22e8u}){
        bad=stock;bad.data[bad.raw(rv)]^=1;refused172(bad,"required native region differs");
    }
    // Select a byte that is explicitly outside the dependency closure, not
    // padding or an unrecognized function. Gaps in executable contract regions
    // are generated only from byte-verified recognized instruction bodies.
    std::vector<unsigned char> pinned(stock.data.size(),0);
    for(const auto& r:regions)std::fill_n(pinned.begin()+r.raw,r.size,(unsigned char)1);
    // Never assemble a new engine from individually acceptable fragments of
    // different profiles: their protected regions must match one whole basis.
    auto mixed=modified;unsigned different=0;
    for(const auto& r:regions)if(strcmp(r.stock,r.bugfix)){
        if(!different++)std::copy_n(stock.data.begin()+r.raw,r.size,mixed.data.begin()+r.raw);
    }
    need(different>1,"fixture needs multiple reviewed differences");
    refused172(mixed,"required native region differs");
    unsigned extension=0;
    for(const auto& s:sections)if(s.flags&0x20000000){
        for(unsigned p=s.raw;p<s.raw+s.size;++p)if(!pinned[p]){extension=p;break;}
        if(extension)break;
    }
    need(extension!=0,"contract has no verified independent extension region");
    auto independent=stock;independent.data[extension]^=1;
    need(!inspect(independent,hash).reviewed_image,"unrelated routine edit did not use contract");++checks172;
    auto combined=modified;combined.data[extension]^=1;combined.data[combined.pe+8]^=2;
    need(!inspect(combined,hash).reviewed_image,"reviewed plus independent combination refused");++checks172;
    for(auto item:{std::make_pair(L"metadata.dll",&metadata),std::make_pair(L"independent.dll",&independent),std::make_pair(L"combined.dll",&combined)}){
        std::ofstream file(root/item.first,std::ios::binary);file.write((const char*)item.second->data.data(),item.second->data.size());file.close();
        need(!admit((root/item.first).wstring()).reviewed_image,"actual file gate refused safe fixture");++checks172;
    }
    bool stale=false;try{(void)admit((root/L"metadata.dll").wstring(),engine170::stock_sha);}catch(const std::exception& e){stale=std::string(e.what()).find("changed between")!=std::string::npos;}
    need(stale,"stale fingerprint accepted");++checks172;
    auto write=CreateFileW((root/L"metadata.dll").c_str(),GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,0,nullptr);
    if(write!=INVALID_HANDLE_VALUE)CloseHandle(write);
    need(write==INVALID_HANDLE_VALUE&&GetLastError()==ERROR_SHARING_VIOLATION,"accepted binding did not retain a stable file");++checks172;
    auto reservation=VirtualAlloc((void*)0x10000000,0x1d88000,MEM_RESERVE,PAGE_NOACCESS);
    auto module=LoadLibraryExW(argv[1],nullptr,DONT_RESOLVE_DLL_REFERENCES);need(module!=nullptr,"cannot map owned image");
    if(reservation)need((uintptr_t)module!=0x10000000,"owned ASLR fixture stayed at preferred base");
    const auto binding=admit(argv[1]);
    live_hooks((uintptr_t)module,1,binding);live_hooks((uintptr_t)module,2,binding);live_hooks((uintptr_t)module,3,binding);checks172+=3;
    for(unsigned mode=0;mode<3;++mode){
        auto foreign=binding;if(mode==0)foreign.version=0;else if(mode==1)foreign.contract="other";else foreign.fingerprint=engine170::bugfix_sha;
        bool refused=false;try{live_hooks((uintptr_t)module,1,foreign);}catch(...){refused=true;}
        need(refused,"stale/foreign binding reached hook installation");++checks172;
    }
    auto target=(unsigned char*)module+hooks[0].rva;unsigned char saved=*target;DWORD old=0;
    need(VirtualProtect(target,1,PAGE_EXECUTE_READWRITE,&old)!=0,"cannot stage owned conflict");*target^=1;
    bool occupied=false;try{live_hooks((uintptr_t)module,1,binding);}catch(const std::exception& e){occupied=std::string(e.what()).find("occupied/changed hook")!=std::string::npos;}
    *target=saved;DWORD ignored=0;VirtualProtect(target,1,old,&ignored);need(occupied,"occupied in-memory hook admitted");++checks172;
    live_hooks((uintptr_t)module,1,binding);FreeLibrary(module);if(reservation)VirtualFree(reservation,0,MEM_RELEASE);++checks172;
    std::cout<<"PASS "<<checks172<<" contract controls; schema "<<schema<<"; "<<std::size(regions)<<" protected regions; independent raw offset "<<at(extension)<<". No engine initialization/gameplay, external processes or recording.\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
