#pragma once
#include "qte_native_bridge.h"
namespace qte { namespace reservation {
inline bool owns(const Reader& r,uint32_t engine,uint32_t held,const NativeActor& identity){
    NativeActor current;
    NativeActor rider;
    return held&&identity.address&&combat_identity(r,identity.address,engine,current)&&same_actor(current,identity)&&
        (current.kind!=Horse||horse_rider(r,engine,current,rider));
}
inline bool member(const Reader& r,uint32_t engine,uint32_t held,uint32_t actor,const NativeActor& identity){
    // A human has no rider alias: a different address cannot be a member.
    // This only rejects; every possible hit still proves current identity.
    if(identity.kind==Human&&actor!=identity.address)return false;
    if(!owns(r,engine,held,identity))return false;
    if(actor==identity.address)return true;
    NativeActor rider;
    return identity.kind==Horse&&horse_rider(r,engine,identity,rider)&&actor==rider.address;
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
    // Contact objects end at +0x54: their later fields are not participant
    // arrays. Only matched witnesses carry principals and extras there.
    for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
        if(contact&&offsets.first!=0x10)continue;
        uint32_t count{};if(!field(offsets.second,count)||count>5)continue;
        for(uint32_t i=0;i<count;++i){uint32_t actor{};
            if(!field(offsets.first+i*4,actor))break;
            if((member(r,engine,held,actor,player))||(member(r,engine,held,actor,enemy)))return true;
        }
    }
    if(contact&&field(0x4c,target)&&
       ((member(r,engine,held,target,player))||(member(r,engine,held,target,enemy))))return true;
    return false;
}
} }
