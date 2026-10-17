#define NOMINMAX
#include <cwchar>
#include "../../shogun2-possession-cleanup-v46/src/protocol.h"
#include "native_animation_snapshot.h"
#include "possession_observation.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstddef>
static void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
using namespace qte::observation;
static_assert(offsetof(PossessionPrefix,sequence)==offsetof(Status,sequence),"Possession sequence offset");
static_assert(offsetof(PossessionPrefix,heartbeat)==offsetof(Status,heartbeat),"Heartbeat offset");
static_assert(offsetof(PossessionPrefix,unit_id)==offsetof(Status,unit_id),"Unit serial offset");
static_assert(offsetof(PossessionPrefix,actor_id)==offsetof(Status,actor_id),"Actor serial offset");
static_assert(offsetof(PossessionPrefix,lease_generation)==offsetof(Status,lease_generation),"Lease offset");
struct Memory {
    static constexpr uint32_t start=0x10000,witness=0x11000,matched=0x12000;
    static constexpr uint32_t actor=0x13000,unit=0x14000,record=0x15000,engine=0x10000000;
    std::vector<unsigned char> bytes=std::vector<unsigned char>(0x6000);
    uint32_t denied{};unsigned reads{};
    template<class T>void put(uint32_t address,T value){
        check(address>=start&&address+sizeof value<=start+bytes.size(),"Fixture bound");
        memcpy(bytes.data()+address-start,&value,sizeof value);
    }
    static bool read(void* ctx,uint32_t address,void* output,size_t size){
        auto& m=*static_cast<Memory*>(ctx);++m.reads;
        if(address==m.denied||address<start||size>m.bytes.size()||address-start>m.bytes.size()-size)return false;
        memcpy(output,m.bytes.data()+address-start,size);return true;
    }
    qte::Reader reader(){return {read,this};}
    void setup(uint32_t outcome){
        put(witness+0x80,outcome);put(witness+0x84,actor);put(witness+0x88,actor+0x800);
        put(witness+0x78,2u);put(witness+0x98,matched);put(witness+0xb4,0u);put(witness+0x94,0u);
        for(uint32_t i=0;i<2;++i){
            uint32_t end=i&&outcome==5?54u:18u;
            put(witness+0x58+i*16,actor+i*0x800);put(witness+0x5c+i*16,18u);
            put(witness+0x60+i*16,end);put(witness+0x64+i*16,uint8_t(i==0));
            put(matched+4+i*20,18u);put(matched+12+i*20,end);put(matched+16+i*20,i);
            put(matched+0x68+i*4,record+i*0x100);put(record+i*0x100+8,i&&outcome==5?1u:0u);
            put(record+i*0x100+12,1u);
        }
        put(matched+0x64,2u);
        put(actor,engine+0x15dfec8);put(actor+0x1c8,0u);put(actor+0x214,unit);put(unit+4,11u);
        put(actor+0x44,0x12345678u);put(actor+0xf0,uint8_t(1));put(actor+0x688,witness);
        put(actor+0x1d0,18u);put(actor+0x21c,0u);
    }
};
int main(){try{
    for(uint32_t outcome: {0u,5u}){
        Memory memory;memory.setup(outcome);auto before=memory.bytes;
        AnimationSnapshot animation;ActorSnapshot actor;
        check(animation_snapshot(memory.reader(),Memory::witness,animation),"Valid animation layout refused");
        check(animation.valid&&animation.outcome==outcome&&animation.request_count==2&&animation.match_count==2&&
            animation.attacker==Memory::actor&&animation.defender==Memory::actor+0x800&&
            animation.requests[0].attacks==1&&animation.requests[1].attacks==0&&
            animation.requests[1].end==(outcome==5?54u:18u)&&
            animation.matches[1].record_end==(outcome==5?1u:0u),"Animation fields confused");
        check(actor_snapshot(memory.reader(),Memory::actor,Memory::engine,actor)&&actor.valid&&actor.serial==0&&
            actor.witness==Memory::witness&&actor.unit_serial==11&&actor.alive,"Actor serial/backlink mismatch");
        memory.put(Memory::actor+0xf0,uint8_t(0));
        check(actor_snapshot(memory.reader(),Memory::actor,Memory::engine,actor)&&!actor.alive,"Death discarded instead of observed");
        memory.bytes=before;
        memory.put(Memory::witness+0x98,0u);memory.put(Memory::witness+0xb4,Memory::matched);
        check(animation_snapshot(memory.reader(),Memory::witness,animation)&&!animation.match_count&&animation.fallback==Memory::matched,"Fallback observation refused");
    }
    for(unsigned fault=0;fault<8;++fault){
        Memory memory;memory.setup(0);auto reader=memory.reader();AnimationSnapshot prior;prior.valid=77;
        if(fault==0)memory.put(Memory::witness+0x80,6u);
        if(fault==1)memory.put(Memory::witness+0x78,3u);
        if(fault==2)memory.put(Memory::witness+0x98,UINT32_MAX-4);
        if(fault==3)memory.put(Memory::matched+0x64,3u);
        if(fault==4)memory.put(Memory::matched+0x68,0u);
        if(fault==5)memory.put(Memory::matched+0x68,UINT32_MAX-4);
        if(fault==6)memory.put(Memory::witness+0x64,uint8_t(2));
        if(fault==7)memory.denied=Memory::record+8;
        auto before=memory.bytes;
        check(!animation_snapshot(reader,Memory::witness,prior)&&prior.valid==77,"Failed observation published partial fields");
        check(memory.bytes==before,"Observer wrote native memory");
    }
    Memory memory;memory.setup(0);ActorSnapshot prior;prior.valid=77;
    memory.put(Memory::actor,0u);
    check(!actor_snapshot(memory.reader(),Memory::actor,Memory::engine,prior)&&prior.valid==77,"Foreign actor vtable accepted");
    auto reads=memory.reads;
    check(!actor_snapshot(memory.reader(),UINT32_MAX-1,Memory::engine,prior)&&memory.reads==reads,"Overflow caused a read");
    AnimationSnapshot invalid;
    check(!animation_snapshot(memory.reader(),UINT32_MAX-1,invalid)&&memory.reads==reads,"Witness overflow caused a read");
    std::cout<<"PASS: paired-animation/request/backlink read-only fixtures, fallback, actor serial zero/death, eight corrupt/short-read guards and exact version46 status offsets. Native visual block remains unverified.\n";
    return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
