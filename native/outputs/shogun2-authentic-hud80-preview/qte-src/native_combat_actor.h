#pragma once
#include "native_selection_prefix.h"
namespace qte {
enum CombatKind:uint32_t {Human=0,Horse=1};
struct NativeActor {
    uint32_t address{},serial{},unit{},unit_serial{},side{};
    float height{};
    uint32_t kind{};
};
inline uint32_t actor_table(uint32_t engine,uint32_t kind){
    return kind==Human?engine+0x15dfec8:kind==Horse?engine+0x15e8524:0;
}
inline bool combat_identity(const Reader& r,uint32_t actor,uint32_t engine,NativeActor& out,bool require_alive=true){
    if(!actor||actor>UINT32_MAX-0x700)return false;
    uint32_t table{},serial{},unit{},id{},side{};uint8_t alive{};float height{};
    if(!r.get(actor,table)||(table!=actor_table(engine,Human)&&table!=actor_table(engine,Horse))||
       !r.get(actor+0xf0,alive)||alive>1||(require_alive&&alive!=1)||
       !r.get(actor+0x214,unit)||!unit||unit>UINT32_MAX-0xfd0||
       !r.get(actor+0x1c8,serial)||!r.get(unit+4,id)||!id||
       !r.get(actor+0x44,side)||!r.get(actor+0x4c,height)||!std::isfinite(height))return false;
    out={actor,serial,unit,id,side,height,table==actor_table(engine,Horse)?Horse:Human};return true;
}
// Exact reciprocal link, same unit/side/environment, and a unique ordinary
// human passenger. General special models and multi-crew vehicles are not
// implicitly admitted by widening this predicate.
inline bool horse_rider(const Reader& r,uint32_t engine,const NativeActor& horse,NativeActor& rider,bool require_alive=true){
    NativeActor current;uint32_t count{},links{},environment{};
    if(horse.kind!=Horse||!combat_identity(r,horse.address,engine,current,require_alive)||
       current.serial!=horse.serial||current.unit!=horse.unit||current.unit_serial!=horse.unit_serial||
       !r.get(horse.address+0x6cc,count)||count!=1||!r.get(horse.address+0x6d0,links)||
       !links||links>UINT32_MAX-4||!r.get(horse.address+0x28,environment)||!environment)return false;
    uint32_t address{},parent{},rider_env{},word{};
    if(!r.get(links,address)||!combat_identity(r,address,engine,rider,require_alive)||rider.kind!=Human||
       rider.unit!=horse.unit||rider.unit_serial!=horse.unit_serial||rider.side!=horse.side||
       !r.get(address+0x210,parent)||parent!=horse.address||
       !r.get(address+0x28,rider_env)||rider_env!=environment||!r.get(address+0x21c,word)||word)return false;
    return true;
}
inline bool physical_actor(const Reader& r,uint32_t engine,const NativeActor& human,NativeActor& subject,bool require_alive=true){
    NativeActor current;uint32_t mount{};
    if(human.kind!=Human||!combat_identity(r,human.address,engine,current,require_alive)||
       current.serial!=human.serial||current.unit!=human.unit||current.unit_serial!=human.unit_serial||
       !r.get(human.address+0x210,mount))return false;
    if(!mount){subject=current;return true;}
    NativeActor rider;
    if(!combat_identity(r,mount,engine,subject,require_alive)||subject.kind!=Horse||
       !horse_rider(r,engine,subject,rider,require_alive)||rider.address!=human.address||rider.serial!=human.serial)return false;
    return true;
}
inline bool same_actor(const NativeActor& a,const NativeActor& b){
    return a.address==b.address&&a.serial==b.serial&&a.unit==b.unit&&a.unit_serial==b.unit_serial&&a.side==b.side&&a.kind==b.kind;
}
inline bool charge_planning(const Reader& r,uint32_t actor){
    uint32_t state{};uint8_t flags{};
    // Captured independently for infantry and horse charge actors. Neither
    // speed, a collision witness, nor a rider's empty opponent word suffices.
    return r.get(actor+0x1d0,state)&&state==16&&r.get(actor+0x119,flags)&&(flags&2)!=0;
}
}
