#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "native_reservation.h"
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace qte;
static void check(bool value,const char* message){if(!value)throw std::runtime_error(message);}
struct Reads114 {
    uint64_t calls{};bool fail{};
    static bool read(void* context,uint32_t address,void* out,size_t bytes){auto& self=*static_cast<Reads114*>(context);++self.calls;if(self.fail)return false;
        SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,bytes,&got)&&got==bytes;
    }
    Reader reader(){return {read,this};}
};
template<class T>static void put(std::vector<unsigned char>& data,size_t at,T value){memcpy(data.data()+at,&value,sizeof value);}
static uint32_t address(std::vector<unsigned char>& data){return uint32_t(data.data());}
int main(){try{constexpr uint32_t engine=0x10000000;Reads114 meter;
    std::vector<unsigned char> player(0x700),enemy(0x700),other(0x700),pu(0x1000),eu(0x1000),ou(0x1000),held(0x110),foreign(0x110),contact(0x54);
    unsigned id=1;for(auto pair:{std::pair<std::vector<unsigned char>*,std::vector<unsigned char>*>{&player,&pu},{&enemy,&eu},{&other,&ou}}){
        auto& actor=*pair.first;auto& unit=*pair.second;put(actor,0,actor_table(engine,Human));put<uint8_t>(actor,0xf0,1);put(actor,0x214,address(unit));put(actor,0x1c8,id);put(unit,4,id);put(actor,0x44,id);put(actor,0x4c,1.f);put(actor,0x688,address(held));++id;
    }
    NativeActor own,opponent;check(combat_identity(meter.reader(),address(player),engine,own)&&combat_identity(meter.reader(),address(enemy),engine,opponent),"Fixture identities");
    // Authored participant groups have five, two and three inline slots.
    uint64_t worst=0;unsigned cases=0;
    for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
        const unsigned slots=(offsets.second-offsets.first)/4;
        std::fill(foreign.begin(),foreign.end(),static_cast<unsigned char>(0));put(foreign,offsets.second,slots);for(unsigned i=0;i<slots;++i)put(foreign,offsets.first+i*4,address(other));
        meter.calls=0;check(!reservation::overlaps(meter.reader(),engine,address(held),address(foreign),own,opponent),"Unrelated crowded encounter blocked");
        worst=std::max(worst,meter.calls);std::cout<<"CROWD114 group="<<offsets.first<<" reads="<<meter.calls<<'\n';
        check(meter.calls<=40,"Unrelated crowd still repeatedly validates reserved identities");++cases;
        for(unsigned slot=0;slot<slots;++slot){put(foreign,offsets.first+slot*4,address(player));
            check(reservation::overlaps(meter.reader(),engine,address(held),address(foreign),own,opponent),"Owned overlap lost");
            put(player,0x1c8,999u);check(!reservation::overlaps(meter.reader(),engine,address(held),address(foreign),own,opponent),"Reused actor kept reservation");put(player,0x1c8,own.serial);
            put(foreign,offsets.first+slot*4,address(other));cases+=2;
        }
    }
    meter.calls=0;check(!reservation::member(meter.reader(),engine,address(held),address(other),own),"Unrelated member admitted");check(meter.calls==0,"Nonmatching human performs identity reads");++cases;
    for(unsigned bad=0;bad<5;++bad){auto identity=own;
        if(bad==0)++identity.serial;if(bad==1)++identity.unit_serial;if(bad==2)++identity.side;if(bad==3)identity.kind=Horse;if(bad==4)meter.fail=true;
        check(!reservation::member(meter.reader(),engine,address(held),address(player),identity),"Matching address bypassed live identity proof");meter.fail=false;++cases;
    }
    put<uint8_t>(player,0xf0,0);check(!reservation::member(meter.reader(),engine,address(held),address(player),own),"Dead actor admitted");put<uint8_t>(player,0xf0,1);++cases;
    put(contact,0,engine+0x15e1510);put(contact,0x4c,address(player));check(reservation::overlaps(meter.reader(),engine,address(held),address(contact),own,opponent),"Native contact target lost");++cases;
    put(contact,0x4c,address(other));check(!reservation::overlaps(meter.reader(),engine,address(held),address(contact),own,opponent),"Unrelated contact target admitted");++cases;
    check(!reservation::overlaps(meter.reader(),engine,0,address(foreign),own,opponent)&&!reservation::overlaps(meter.reader(),engine,address(held),address(held),own,opponent),"Ended/self reservation changed");++cases;
    LARGE_INTEGER start,end,freq;QueryPerformanceFrequency(&freq);QueryPerformanceCounter(&start);meter.calls=0;
    for(unsigned n=0;n<10000;++n)check(!reservation::overlaps(meter.reader(),engine,address(held),address(foreign),own,opponent),"Crowd flood changed forwarding");
    QueryPerformanceCounter(&end);std::cout<<"PASS: "<<cases<<" crowd/identity checks;10000 unrelated encounters reads="<<meter.calls<<" elapsed_ms="<<double(end.QuadPart-start.QuadPart)*1000/double(freq.QuadPart)<<" max_single_reads="<<worst<<". Actual local ReadProcessMemory; wall time informational, no FPS claim.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
