#pragma once
#include "qte_native_bridge.h"
namespace qte { namespace reservation {
inline bool owns(const Reader& r,uint32_t engine,uint32_t held,const NativeActor& identity){
    NativeActor current;
    NativeActor rider;
    return held&&identity.address&&combat_identity(r,identity.address,engine,current)&&same_actor(current,identity)&&
        (current.kind!=Horse||horse_rider(r,engine,current,rider));
}
// A callback-local address filter, never an ownership proof. A possible
// rider still goes through owns()/horse_rider(), including reciprocal links.
struct AliasFilter120 {
    const NativeActor& identity;bool sampled{},known{};uint32_t rider{};
    bool possible(const Reader& r,uint32_t actor){
        if(actor==identity.address)return true;
        if(identity.kind==Human)return false;
        if(identity.kind!=Horse)return true;
        if(!sampled){
            sampled=true;
            if(!identity.address||identity.address>UINT32_MAX-0x700){known=true;return false;}
            uint32_t count{},links{};std::array<uint32_t,2> seats{};
            if(r.get(identity.address+0x6cc,seats)){count=seats[0];links=seats[1];}
            else if(!r.get(identity.address+0x6cc,count)||!r.get(identity.address+0x6d0,links))return true;
            if(count!=1||!links||links>UINT32_MAX-4){known=true;return false;}
            known=r.get(links,rider);
        }
        return !known||actor==rider;
    }
};
inline bool member_proved120(const Reader& r,uint32_t engine,uint32_t held,uint32_t actor,const NativeActor& identity){
    if(!owns(r,engine,held,identity))return false;
    if(actor==identity.address)return true;
    NativeActor rider;
    return identity.kind==Horse&&horse_rider(r,engine,identity,rider)&&actor==rider.address;
}
inline bool member(const Reader& r,uint32_t engine,uint32_t held,uint32_t actor,const NativeActor& identity){
    AliasFilter120 filter{identity};
    return filter.possible(r,actor)&&member_proved120(r,engine,held,actor,identity);
}
inline bool preserve_release(const Reader& r,uint32_t engine,uint32_t held,uint32_t releasing,uint32_t actor,const NativeActor& player,const NativeActor& enemy){
    if(!held||releasing==held)return false;
    return member(r,engine,held,actor,player)||member(r,engine,held,actor,enemy);
}
inline bool overlaps(const Reader& r,uint32_t engine,uint32_t held,uint32_t candidate,const NativeActor& player,const NativeActor& enemy){
    if(!held||held==candidate||!candidate||candidate>UINT32_MAX-0x100)return false;
    // Snapshot only the bounded fields used by these native layouts.
    // A short/failed read retains the original individual-field path.
    // This is participant data, never an actor identity proof.
    std::array<uint32_t,0x50/4> snapshot{};const bool sampled=r.get(candidate,snapshot);
    auto field=[&](unsigned offset,uint32_t& value){
        if(sampled){value=snapshot[offset/4];return true;}
        return r.get(candidate+offset,value);
    };
    uint32_t table{},target{};const bool contact=field(0,table)&&table==engine+0x15e1510;
    AliasFilter120 pfilter{player},efilter{enemy};
    auto matches=[&](uint32_t actor){
        return (pfilter.possible(r,actor)&&member_proved120(r,engine,held,actor,player))||
               (efilter.possible(r,actor)&&member_proved120(r,engine,held,actor,enemy));
    };
    // Contact objects end at +0x54: their later fields are not participant
    // arrays. Only matched witnesses carry principals and extras there.
    for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
        if(contact&&offsets.first!=0x10)continue;
        uint32_t count{};if(!field(offsets.second,count)||count>5)continue;
        for(uint32_t i=0;i<count;++i){uint32_t actor{};
            if(!field(offsets.first+i*4,actor))break;
            if(matches(actor))return true;
        }
    }
    if(contact&&field(0x4c,target)&&
       matches(target))return true;
    return false;
}
} }
