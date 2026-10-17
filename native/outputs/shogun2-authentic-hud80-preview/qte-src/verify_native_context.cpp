#include "native_unit_context.h"
#include <iostream>
#include <stdexcept>
#include <vector>
static void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
struct Memory {
    static constexpr uint32_t start=0x10000,unit=0x11000,army=0x13000,members=0x13400;
    static constexpr uint32_t first=0x14000,second=0x14020,end=0x14040;
    std::vector<unsigned char> bytes=std::vector<unsigned char>(0x6000);
    template<class T>void put(uint32_t address,T value){check(address>=start&&address+sizeof value<=start+bytes.size(),"Fixture bound");memcpy(bytes.data()+address-start,&value,sizeof value);}
    static bool read(void* ctx,uint32_t address,void* out,size_t size){
        auto& m=*static_cast<Memory*>(ctx);
        if(address<start||size>m.bytes.size()||address-start>m.bytes.size()-size)return false;
        memcpy(out,m.bytes.data()+address-start,size);return true;
    }
    qte::Reader reader(){return {&read,this};}
    void setup(){
        put(unit+0xd3c,uint8_t(0));put(unit+0xd34,2u);put(unit+0xce8,2u);
        put(unit+0xcbc,160u);put(unit+0x2bc,120u);put(unit+0x2a4,army);
        put(army+0x114,2u);put(army+0x118,members);put(members,unit+0x1000);put(members+4,unit);
        put(army+0x1ec,uint8_t(1));
        put(unit+0xcf8,first);put(unit+0xcfc,end);
        put(first+4,second);put(first+8,2u);put(second+4,end);put(second+8,8u);
    }
};
int main(){try{
    using namespace qte;unsigned cases=0;
    for(unsigned i=0;i<10000;++i){Memory memory;memory.setup();UnitState own;
        check(unit_context(memory.reader(),Memory::unit,12,false,own),"Valid native layout refused");
        check(own.known==(Required|Strength|Environment)&&own.fatigue==2&&own.morale==2&&own.strength==.75f&&own.height==12&&own.general_support==.75f&&own.environment_support==.3f,"Aggregate/native offsets mismatched");
        memory.put(Memory::first+8,3u);UnitState rally;
        check(unit_context(memory.reader(),Memory::unit,12,false,rally)&&rally.general_support==1,"Native rally support ignored");
        memory.put(Memory::army+0x1ec,uint8_t(0));UnitState absent;
        check(unit_context(memory.reader(),Memory::unit,12,false,absent)&&absent.general_support==0,"Dead general supplied support");
        memory.put(Memory::unit+0xd3c,uint8_t(1));memory.put(Memory::unit+0xd34,99u);
        UnitState immune;check(unit_context(memory.reader(),Memory::unit,12,false,immune)&&immune.fatigue==0,"Native fatigue immunity ignored");++cases;
    }
    for(unsigned fault=0;fault<12;++fault){Memory memory;memory.setup();
        if(fault==0)memory.put(Memory::first+4,Memory::first); // cycle
        if(fault==1)memory.put(Memory::first+8,48u); // invalid enum
        if(fault==2)memory.put(Memory::second+4,0u); // unterminated
        if(fault==3)memory.put(Memory::unit+0xcf8,UINT32_MAX-1);
        if(fault==4)memory.put(Memory::army+0x114,4097u);
        if(fault==5)memory.put(Memory::army+0x118,UINT32_MAX-4);
        if(fault==6)memory.put(Memory::members+4,0u); // wrong membership
        if(fault==7)memory.put(Memory::army+0x1ec,uint8_t(2));
        if(fault==8)memory.put(Memory::unit+0xce8,8u);
        if(fault==9)memory.put(Memory::unit+0xd34,6u);
        if(fault==10)memory.put(Memory::unit+0x2a4,0u);
        if(fault==11)memory.put(Memory::unit+0xcfc,0u);
        UnitState old;old.known=0x1234;old.height=42;
        check(!unit_context(memory.reader(),Memory::unit,12,false,old),"Corrupt native context accepted");
        check(old.known==0x1234&&old.height==42,"Failed read published partial facts");
    }
    Memory memory;memory.setup();memory.put(Memory::unit+0xcf8,Memory::end);UnitState empty;
    check(unit_context(memory.reader(),Memory::unit,12,true,empty)&&empty.routing&&empty.general_support==.25f&&empty.environment_support==0,"Empty morale list/routing mismatch");
    auto other=empty;other.routing=false;Difficulty difficulty_result;
    check(!difficulty(empty,other,difficulty_result),"Routing unit acquired QTE");
    for(unsigned id=0;id<48;++id){Reasons reasons{uint64_t(1)<<id};float support=environment_support(reasons);check(std::isfinite(support)&&support>=-1&&support<=1,"Unbounded environment tuning");}
    Reasons all{(uint64_t(1)<<48)-1};check(std::abs(environment_support(all))<=1,"Combined environmental support unbounded");
    std::cout<<"PASS: "<<cases<<" native-layout context fixtures; whole-unit fatigue/morale/strength, army membership, commander/alive and local rally reasons, environment states; corrupt/cyclic/short-read guards. Static binding plus fixture tests; no live context test.\n";
    return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
