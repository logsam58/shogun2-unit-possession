#pragma once
#include "qte_core.h"
#include <array>
#include <cstring>

namespace qte {
// Read callback returns false on a short read or invalid region. The live
// adapter must additionally prove unit/actor identity and sample on the game
// simulation thread. This reader never writes game memory or calls game code.
using ReadBytes=bool(*)(void*,uint32_t,void*,size_t);
struct Reader {
    ReadBytes read{};void* context{};
    template<class T>bool get(uint32_t address,T& out)const{
        return read&&address>=0x10000&&address<=UINT32_MAX-sizeof(T)&&
            read(context,address,&out,sizeof out);
    }
};
struct Reasons {
    uint64_t flags{};
    bool has(unsigned id)const{return id<48&&(flags&(uint64_t(1)<<id))!=0;}
};
inline bool morale_reasons(const Reader& r,uint32_t unit,Reasons& out){
    // UI110d9b93 passes unit+cc4 into getter16f8c0. Its begin/end are
    // +34/+38; native forward links are node+4 and reason ids are node+8.
    uint32_t begin{},end{};
    if(unit>UINT32_MAX-0xfd0||!r.get(unit+0xcf8,begin)||!r.get(unit+0xcfc,end)||!begin||!end)return false;
    std::array<uint32_t,48> seen{};unsigned count=0;uint64_t flags=0;
    while(begin!=end){
        if(count==seen.size()||(begin&3)||begin>UINT32_MAX-12)return false;
        for(unsigned i=0;i<count;++i)if(seen[i]==begin)return false;
        seen[count++]=begin;
        uint32_t next{},id{};
        if(!r.get(begin+4,next)||!r.get(begin+8,id)||id>=48||!next)return false;
        flags|=uint64_t(1)<<id;begin=next;
    }
    out.flags=flags;return true;
}
inline float environment_support(const Reasons& reasons){
    // QTE tuning weights, NOT a claim about engine morale coefficients.
    // General, fatigue, casualties and hill height are separate inputs.
    float value=0;
    if(reasons.has(1))value+=.2f; // army support
    if(reasons.has(7))value+=.2f; // secure flanks
    if(reasons.has(8))value+=.3f; // fortification
    if(reasons.has(9))value+=.2f; // fortification breach
    if(reasons.has(10))value+=.2f; // settlement
    if(reasons.has(13))value+=.2f; // overwhelming friendly units
    if(reasons.has(15)||reasons.has(16)||reasons.has(17))value+=.2f; // inspiration/rally
    if(reasons.has(26)||reasons.has(27))value-=.3f; // attacked flank/rear
    if(reasons.has(28)||reasons.has(29))value-=.2f; // exposed flanks
    if(reasons.has(39))value-=.3f; // friendly fire
    if(reasons.has(42))value-=.2f; // enemy numbers
    if(reasons.has(43))value-=.2f; // surprised
    if(reasons.has(47))value-=.2f; // unprepared night battle
    return std::clamp(value,-1.f,1.f);
}
inline bool unit_context(const Reader& r,uint32_t unit,float native_height,
                         bool native_routing,UnitState& out){
    if(!unit||unit>UINT32_MAX-0xfd0||!std::isfinite(native_height))return false;
    UnitState next{};next.height=native_height;next.routing=native_routing;
    uint8_t immune{};uint32_t fatigue{},morale{},initial{},alive{},army{};
    if(!r.get(unit+0xd3c,immune)||immune>1||!r.get(unit+0xd34,fatigue)||
       !r.get(unit+0xce8,morale)||morale>7)return false;
    next.fatigue=immune?0:fatigue;if(next.fatigue>5)return false;
    next.morale=morale;next.known=Fatigue|Morale|Elevation;
    if(r.get(unit+0xcbc,initial)&&r.get(unit+0x2bc,alive)&&initial&&initial<=100000&&alive<=initial){
        next.strength=float(alive)/float(initial);next.known|=Strength;
    }
    // Native unit:name callback1c1559 obtains army from unit+2a4, then
    // validates unit membership in army+114/+118. The army commander-alive
    // Lua callback1c2c40 reads army+1ec. Do not confuse unit+14/cc4 (env)
    // with the army pointer.
    if(!r.get(unit+0x2a4,army)||!army||army>UINT32_MAX-0x250)return false;
    uint32_t count{},members{};uint8_t commander_alive{};
    if(!r.get(army+0x114,count)||!r.get(army+0x118,members)||count==0||count>4096||
       !members||members>UINT32_MAX-count*4||!r.get(army+0x1ec,commander_alive)||commander_alive>1)return false;
    bool found=false;
    for(uint32_t i=0;i<count;++i){uint32_t member{};if(!r.get(members+i*4,member))return false;if(member==unit){found=true;break;}}
    if(!found)return false;
    Reasons reasons;if(!morale_reasons(r,unit,reasons))return false;
    next.general_support=commander_alive?.25f:0.f;
    if(commander_alive&&reasons.has(2))next.general_support=.75f;
    if(commander_alive&&reasons.has(3))next.general_support=1.f;
    next.environment_support=environment_support(reasons);
    next.known|=General|Environment;
    if(!valid(next))return false;
    out=next;return true;
}
}
