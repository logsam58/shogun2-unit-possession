#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "native_reservation.h"
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace qte;
static void check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
template<class T>static void put(std::vector<unsigned char>& data,size_t at,T value){memcpy(data.data()+at,&value,sizeof value);}
static uint32_t ptr(std::vector<unsigned char>& data){return uint32_t(data.data());}
struct Reads115 {
    uint64_t calls{},bytes{};bool reject_bulk{};uint32_t blocked{},change{};
    static bool read(void* context,uint32_t address,void* out,size_t size){auto& self=*static_cast<Reads115*>(context);++self.calls;self.bytes+=size;
        if((self.reject_bulk&&size==0x50)||(self.blocked&&address<=self.blocked&&uint64_t(address)+size>self.blocked))return false;
        SIZE_T got{};bool ok=ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,size,&got)&&got==size;
        if(ok&&size==0x50&&self.change){uint32_t serial=999;memcpy(reinterpret_cast<void*>(self.change),&serial,4);}
        return ok;
    }
    Reader reader(){return {read,this};}
};
// Frozen Test114 overlap traversal: the differential oracle includes its
// eager identity checks and individual native field reads.
static bool previous114(const Reader& r,uint32_t engine,uint32_t held,uint32_t candidate,const NativeActor& player,const NativeActor& enemy){
    if(!held||held==candidate||!candidate||candidate>UINT32_MAX-0x100)return false;
    bool p=reservation::owns(r,engine,held,player),e=reservation::owns(r,engine,held,enemy);if(!p&&!e)return false;
    uint32_t table{},target{};const bool contact=r.get(candidate,table)&&table==engine+0x15e1510;
    for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
        if(contact&&offsets.first!=0x10)continue;
        uint32_t count{};if(!r.get(candidate+offsets.second,count)||count>5)continue;
        for(uint32_t i=0;i<count;++i){uint32_t actor{};
            if(!r.get(candidate+offsets.first+i*4,actor))break;
            if((p&&reservation::member(r,engine,held,actor,player))||(e&&reservation::member(r,engine,held,actor,enemy)))return true;
        }
    }
    return contact&&r.get(candidate+0x4c,target)&&((p&&reservation::member(r,engine,held,target,player))||(e&&reservation::member(r,engine,held,target,enemy)));
}
int main(){try{
    constexpr uint32_t engine=0x10000000;Reads115 meter;unsigned checks=0;
    std::vector<unsigned char> player(0x700),enemy(0x700),other(0x700),pu(0x1000),eu(0x1000),ou(0x1000),held(0x110),foreign(0x110);
    unsigned serial=1;for(auto pair:{std::pair<std::vector<unsigned char>*,std::vector<unsigned char>*>{&player,&pu},{&enemy,&eu},{&other,&ou}}){
        auto& a=*pair.first;auto& u=*pair.second;put(a,0,actor_table(engine,Human));put<uint8_t>(a,0xf0,1);put(a,0x214,ptr(u));put(a,0x1c8,serial);put(u,4,serial);put(a,0x44,serial);put(a,0x4c,1.f);put(a,0x688,ptr(held));++serial;
    }
    NativeActor own,opponent;check(combat_identity(meter.reader(),ptr(player),engine,own)&&combat_identity(meter.reader(),ptr(enemy),engine,opponent),"Fixture identities");
    auto compare=[&](uint32_t candidate,const NativeActor& p,const NativeActor& e){
        bool current=reservation::overlaps(meter.reader(),engine,ptr(held),candidate,p,e);
        bool prior=previous114(meter.reader(),engine,ptr(held),candidate,p,e);check(current==prior,"Current decision differs from Test114 reference");++checks;return current;
    };
    for(auto group:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
        std::fill(foreign.begin(),foreign.end(),static_cast<unsigned char>(0));unsigned count=(group.second-group.first)/4;put(foreign,group.second,count);
        for(unsigned i=0;i<count;++i)put(foreign,group.first+4*i,ptr(other));
        meter.calls=meter.bytes=0;check(!reservation::overlaps(meter.reader(),engine,ptr(held),ptr(foreign),own,opponent),"Unrelated encounter blocked");
        std::cout<<"CROWD115 group="<<group.first<<" reads="<<meter.calls<<" bytes="<<meter.bytes<<'\n';
        check(meter.calls==1&&meter.bytes==0x50,"Unrelated encounter exceeds one bounded read");++checks;
        for(unsigned i=0;i<count;++i)for(auto actor:{ptr(player),ptr(enemy)}){
            put(foreign,group.first+4*i,actor);check(compare(ptr(foreign),own,opponent),"Current match lost");
            auto& a=actor==ptr(player)?player:enemy;auto& u=actor==ptr(player)?pu:eu;auto expected=actor==ptr(player)?own:opponent;
            put(a,0x1c8,999u);check(!compare(ptr(foreign),own,opponent),"Reused serial admitted");put(a,0x1c8,expected.serial);
            put<uint8_t>(a,0xf0,0);check(!compare(ptr(foreign),own,opponent),"Dead actor admitted");put<uint8_t>(a,0xf0,1);
            put(u,4,999u);check(!compare(ptr(foreign),own,opponent),"Reused unit admitted");put(u,4,expected.unit_serial);
            put(a,0x44,999u);check(!compare(ptr(foreign),own,opponent),"Changed side admitted");put(a,0x44,expected.side);
            meter.reject_bulk=true;check(compare(ptr(foreign),own,opponent),"Bulk failure discarded readable match");meter.reject_bulk=false;
            meter.blocked=actor+0x1c8;check(!compare(ptr(foreign),own,opponent),"Unreadable positive identity admitted");meter.blocked=0;
            put(foreign,group.first+4*i,ptr(other));
        }
    }
    // Mutation after the bulk participant snapshot must still be rejected by
    // the later current identity proof; no proof is cached across callbacks.
    std::fill(foreign.begin(),foreign.end(),static_cast<unsigned char>(0));put(foreign,0x24,1u);put(foreign,0x10,ptr(player));meter.change=ptr(player)+0x1c8;
    check(!reservation::overlaps(meter.reader(),engine,ptr(held),ptr(foreign),own,opponent),"Snapshot became a stale positive proof");++checks;meter.change=0;put(player,0x1c8,own.serial);
    // Native contact targets at +4c and malformed counts use the same oracle.
    uint32_t random=0x14725836;auto next=[&](){random^=random<<13;random^=random>>17;return random^=random<<5;};
    for(unsigned n=0;n<5000;++n){
        for(unsigned offset=0;offset<0x50;offset+=4){uint32_t value=next()%7;put(foreign,offset,value==0?ptr(player):value==1?ptr(enemy):value==2?ptr(other):value==3?0u:value);}
        put(foreign,0,n&1?engine+0x15e1510:0u);for(unsigned offset:{0x24u,0x38u,0x48u})put(foreign,offset,next()%8);
        if(n%11==0)put<uint8_t>(player,0xf0,0);if(n%13==0)put(enemy,0x1c8,999u);
        meter.reject_bulk=n%3==0;compare(ptr(foreign),own,opponent);meter.reject_bulk=false;put<uint8_t>(player,0xf0,1);put(enemy,0x1c8,opponent.serial);
    }
    // The bulk read may cross an inaccessible page even though the first
    // group is readable. The original field fallback must preserve its match.
    SYSTEM_INFO info{};GetSystemInfo(&info);auto pages=static_cast<unsigned char*>(VirtualAlloc(nullptr,info.dwPageSize*2,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));check(pages!=nullptr,"Guard fixture allocation");
    DWORD old{};check(VirtualProtect(pages+info.dwPageSize,info.dwPageSize,PAGE_NOACCESS,&old)!=0,"Guard fixture protection");
    uint32_t guarded=uint32_t(pages+info.dwPageSize-0x28),zero=0,count=1,actor=ptr(player);memcpy(reinterpret_cast<void*>(guarded),&zero,4);memcpy(reinterpret_cast<void*>(guarded+0x24),&count,4);memcpy(reinterpret_cast<void*>(guarded+0x10),&actor,4);
    check(compare(guarded,own,opponent),"Page-boundary fallback lost owned match");actor=ptr(other);memcpy(reinterpret_cast<void*>(guarded+0x10),&actor,4);check(!compare(guarded,own,opponent),"Unreadable later groups invented a match");VirtualFree(pages,0,MEM_RELEASE);
    // Preserve reciprocal mounted aliases as well as ordinary human addresses.
    std::vector<unsigned char> horse(0x700),passengers(4);auto mount=ptr(horse);put(horse,0,actor_table(engine,Horse));put<uint8_t>(horse,0xf0,1);put(horse,0x214,ptr(pu));put(horse,0x1c8,983u);put(horse,0x44,own.side);put(horse,0x4c,1.f);put(horse,0x688,ptr(held));put(horse,0x28,0x1234u);put(horse,0x6cc,1u);put(horse,0x6d0,ptr(passengers));put(passengers,0,ptr(player));put(player,0x210,mount);put(player,0x28,0x1234u);
    NativeActor mounted;check(combat_identity(meter.reader(),mount,engine,mounted),"Mounted fixture identity");
    std::fill(foreign.begin(),foreign.end(),static_cast<unsigned char>(0));put(foreign,0x24,1u);put(foreign,0x10,ptr(player));check(compare(ptr(foreign),mounted,opponent),"Reciprocal rider alias lost");
    put(player,0x210,0u);check(!compare(ptr(foreign),mounted,opponent),"Detached rider admitted");put(player,0x210,mount);put(horse,0x6cc,2u);check(!compare(ptr(foreign),mounted,opponent),"Multiple passengers admitted");put(horse,0x6cc,1u);put(foreign,0x10,mount);check(compare(ptr(foreign),mounted,opponent),"Current horse lost");put(horse,0x1c8,999u);check(!compare(ptr(foreign),mounted,opponent),"Reused horse admitted");
    std::fill(foreign.begin(),foreign.end(),static_cast<unsigned char>(0));put(foreign,0x24,5u);for(unsigned i=0;i<5;++i)put(foreign,0x10+i*4,ptr(other));meter.calls=meter.bytes=0;
    LARGE_INTEGER begin,end,freq;QueryPerformanceFrequency(&freq);QueryPerformanceCounter(&begin);
    for(unsigned n=0;n<100000;++n)check(!reservation::overlaps(meter.reader(),engine,ptr(held),ptr(foreign),own,opponent),"Unrelated flood changed forwarding");
    QueryPerformanceCounter(&end);check(meter.calls==100000&&meter.bytes==8000000,"Flood read budget changed");
    check(!reservation::overlaps(meter.reader(),engine,0,ptr(foreign),own,opponent)&&!reservation::overlaps(meter.reader(),engine,ptr(held),ptr(held),own,opponent),"Ended/self reservation changed");++checks;
    std::cout<<"PASS: "<<checks<<" differential/identity/fallback cases;100000 unrelated encounters reads="<<meter.calls<<" bytes="<<meter.bytes<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/double(freq.QuadPart)<<". Actual ReadProcessMemory; fixture time is not game FPS.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
