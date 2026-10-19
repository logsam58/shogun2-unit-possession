#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "native_combat_actor.h"
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace qte;
static void check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
static bool reject_prefix;static uint64_t calls;
static bool read120(void*,uint32_t address,void* out,size_t size){
    ++calls;if(reject_prefix&&size==0x218)return false;
    SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,size,&got)&&got==size;
}
inline bool combat_before120(const Reader& r,uint32_t actor,uint32_t engine,NativeActor& out,bool require_alive=true){
    if(!actor||actor>UINT32_MAX-0x700)return false;
    uint32_t table{},serial{},unit{},id{},side{};uint8_t alive{};float height{};
    if(!r.get(actor,table)||(table!=actor_table(engine,Human)&&table!=actor_table(engine,Horse))||
       !r.get(actor+0xf0,alive)||alive>1||(require_alive&&alive!=1)||
       !r.get(actor+0x214,unit)||!unit||unit>UINT32_MAX-0xfd0||
       !r.get(actor+0x1c8,serial)||!r.get(unit+4,id)||!id||
       !r.get(actor+0x44,side)||!r.get(actor+0x4c,height)||!std::isfinite(height))return false;
    out={actor,serial,unit,id,side,height,table==actor_table(engine,Horse)?Horse:Human};return true;
}

int main(){try{
    constexpr uint32_t engine=0x10000000;Reader r{read120,nullptr};
    std::vector<uint8_t> actor(0x700),unit(0x1000);uint32_t a=uint32_t(actor.data()),u=uint32_t(unit.data());
    auto put=[](uint32_t address,auto value){memcpy(reinterpret_cast<void*>(address),&value,sizeof value);};
    unsigned checks=0;auto compare=[&](uint32_t address,bool alive){NativeActor before{},after{};
        memset(&before,0xa5,sizeof before);after=before;bool b=combat_before120(r,address,engine,before,alive),n=combat_identity(r,address,engine,after,alive);
        check(b==n&&!memcmp(&before,&after,sizeof before),"Identity decision/output differs from Test119");++checks;};
    for(unsigned i=0;i<8000;++i){
        put(a,i%17==0?3u:actor_table(engine,i%2));put(a+0xf0,uint8_t((i/3)%4));
        put(a+0x214,i%19==0?0u:i%23==0?0xfffffff0u:u);put(a+0x1c8,i);put(u+4,i%29==0?0u:7u);
        put(a+0x44,i%3);float h=i%31==0?std::numeric_limits<float>::infinity():i%37==0?std::numeric_limits<float>::quiet_NaN():1.25f;
        put(a+0x4c,h);reject_prefix=i%7==0;compare(a,i%5==0);
    }
    reject_prefix=false;for(uint32_t address:{0u,7u,0xfffffff0u})compare(address,true);
    SYSTEM_INFO si;GetSystemInfo(&si);auto pages=static_cast<uint8_t*>(VirtualAlloc(nullptr,si.dwPageSize*2,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));check(pages,"Page fixture");DWORD old;
    check(VirtualProtect(pages+si.dwPageSize,si.dwPageSize,PAGE_NOACCESS,&old),"Page protection");uint32_t edge=uint32_t(pages+si.dwPageSize-0x214);
    put(edge,actor_table(engine,Human));put(edge+0xf0,uint8_t(1));compare(edge,true);VirtualFree(pages,0,MEM_RELEASE);
    put(a,actor_table(engine,Human));put(a+0xf0,uint8_t(1));put(a+0x214,u);put(u+4,7u);put(a+0x4c,1.f);calls=0;NativeActor out;
    check(combat_identity(r,a,engine,out)&&calls==2,"Identity exceeds two current reads");
    std::cout<<"PASS: "<<checks<<" identity/output differential cases, forced fallback, invalid types/life/unit/height and inaccessible page; valid identity reads="<<calls<<".\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
