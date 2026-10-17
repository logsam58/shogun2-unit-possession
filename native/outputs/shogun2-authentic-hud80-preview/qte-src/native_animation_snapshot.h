#pragma once
#include "native_unit_context.h"
namespace qte { namespace observation {
struct ActorSnapshot {
    uint32_t valid{},address{},serial{},unit{},unit_serial{},side{},alive{},witness{},state{},mount{};
};
struct RequestSnapshot {uint32_t actor{},start{},end{},attacks{};};
struct MatchSnapshot {uint32_t start{},end{},alliance{},record{},record_end{},record_type{};};
struct AnimationSnapshot {
    uint32_t valid{},outcome{},attacker{},defender{},request_count{},matched{},fallback{},rotation{},match_count{};
    RequestSnapshot requests[2]{};MatchSnapshot matches[2]{};
};
inline bool actor_snapshot(const Reader& reader,uint32_t actor,uint32_t engine,ActorSnapshot& out){
    if(!actor||actor>UINT32_MAX-0x700)return false;
    ActorSnapshot next;next.address=actor;uint32_t vtable{};uint8_t alive{};
    if(!reader.get(actor,vtable)||vtable!=engine+0x15dfec8||
       !reader.get(actor+0x1c8,next.serial)||!reader.get(actor+0x214,next.unit)||
       !next.unit||next.unit>UINT32_MAX-8||!reader.get(next.unit+4,next.unit_serial)||
       !reader.get(actor+0x44,next.side)||!reader.get(actor+0xf0,alive)||alive>1||
       !reader.get(actor+0x688,next.witness)||!reader.get(actor+0x1d0,next.state)||
       !reader.get(actor+0x21c,next.mount))return false;
    next.alive=alive;next.valid=1;out=next;return true;
}
inline bool animation_snapshot(const Reader& reader,uint32_t witness,AnimationSnapshot& out){
    if(!witness||witness>UINT32_MAX-0x100)return false;
    AnimationSnapshot next;
    if(!reader.get(witness+0x80,next.outcome)||next.outcome>5||
       !reader.get(witness+0x84,next.attacker)||!reader.get(witness+0x88,next.defender)||
       !reader.get(witness+0x78,next.request_count)||next.request_count>2||
       !reader.get(witness+0x98,next.matched)||!reader.get(witness+0xb4,next.fallback)||
       !reader.get(witness+0x94,next.rotation))return false;
    for(uint32_t i=0;i<next.request_count;++i){auto& item=next.requests[i];uint8_t attacks{};
        const uint32_t address=witness+0x58+i*16;
        if(!reader.get(address,item.actor)||!reader.get(address+4,item.start)||
           !reader.get(address+8,item.end)||!reader.get(address+12,attacks)||attacks>1)return false;
        item.attacks=attacks;
    }
    if(next.matched){
        if(next.matched>UINT32_MAX-0x100||!reader.get(next.matched+0x64,next.match_count)||next.match_count!=2)return false;
        for(uint32_t i=0;i<next.match_count;++i){auto& item=next.matches[i];
            // Native matcher3cb3e0 compares each 20-byte definition's start,
            // end and alliance with the 16-byte preparation request. It then
            // validates the definition's loaded animation record (+68 array).
            if(!reader.get(next.matched+4+i*20,item.start)||
               !reader.get(next.matched+12+i*20,item.end)||
               !reader.get(next.matched+16+i*20,item.alliance)||
               !reader.get(next.matched+0x68+i*4,item.record)||!item.record||item.record>UINT32_MAX-16||
               !reader.get(item.record+8,item.record_end)||!reader.get(item.record+12,item.record_type))return false;
        }
    }
    next.valid=1;out=next;return true;
}
} }
