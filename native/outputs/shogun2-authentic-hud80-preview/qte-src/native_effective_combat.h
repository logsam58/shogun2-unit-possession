#pragma once
#include "qte_native_bridge.h"
namespace qte { namespace effective {
// Constructors40f080/40f0c0/b8e640 and scoreb92800 RET10 establish
// these stack object layouts. Captured copies never retain native stack addresses.
struct ActorContext {uint32_t table,actor,difficulty,flags;};
struct PairContext {uint32_t table,environment,attacker,defender,flags;};
struct Calculator {uint32_t table,rules,diagnostics;};
struct Capture {
    uint32_t witness{};uint64_t generation{};
    Calculator calculator{};ActorContext a{},b{};PairContext pair{};
    bool valid{};
};
inline bool belongs(const Reader& r,uint32_t engine,const Capture& saved){
    // prepare3e5d40 obtains the selector from witness.environment+8 -> +9c.
    // Selector433b80 passes (*selector)+8 into pair constructor40f0c0.
    // This is the common combat environment, not the enclosing battle object.
    // Keep the battle-membership guard, following the native pointer chain.
    uint32_t environment{},common{};NativeActor a,b;
    return saved.valid&&r.get(saved.witness+4,environment)&&environment&&
        r.get(environment+8,common)&&common&&common==saved.pair.environment&&
        actor_identity(r,saved.a.actor,saved.witness,engine,a,true)&&
        actor_identity(r,saved.b.actor,saved.witness,engine,b,true)&&a.address!=b.address;
}
inline bool capture(const Reader& r,uint32_t engine,uint32_t witness,uint64_t generation,uint32_t calculator,uint32_t a,uint32_t b,uint32_t pair,Capture& out){
    Capture next;next.witness=witness;next.generation=generation;
    if(!witness||!generation||!r.get(calculator,next.calculator)||!r.get(a,next.a)||!r.get(b,next.b)||!r.get(pair,next.pair)||
       next.calculator.table!=engine+0x15c5688||next.a.table!=engine+0x15e7304||next.b.table!=next.a.table||
       next.pair.table!=engine+0x15e73f4||next.pair.attacker!=next.a.actor||next.pair.defender!=next.b.actor||
       next.a.actor==next.b.actor||!next.calculator.rules)return false;
    next.valid=true;out=next;return true;
}
// A native deterministic attack-versus-defence margin includes attack, charge,
// defence, armour, fatigue, height, facing and applicable weapon/context bonuses.
// RNG/outcome distribution b95350/b91130 is NEVER called for difficulty refresh.
using Score=int(__thiscall*)(void*,void*,void*,void*,uint32_t);
inline bool scores(const Capture& saved,Score calculate,uint32_t player,uint32_t enemy,float& own,float& other){
    if(!saved.valid||!calculate||!((saved.a.actor==player&&saved.b.actor==enemy)||(saved.b.actor==player&&saved.a.actor==enemy)))return false;
    auto calculator=saved.calculator;auto a=saved.a,b=saved.b;auto pair=saved.pair;
    int first=calculate(&calculator,&a,&b,&pair,0);
    std::swap(a,b);std::swap(pair.attacker,pair.defender);
    int reverse=calculate(&calculator,&a,&b,&pair,0);
    if(first<-10000||first>10000||reverse<-10000||reverse>10000)return false;
    own=float(saved.a.actor==player?first:reverse);other=float(saved.a.actor==player?reverse:first);return true;
}
// b94f50 RET8 and b950d0 RETc are the exact defence/armour terms
// subtracted by b92800. Pair virtual+4 (42a450) classifies native facing;
// armour uses that classification, including the native flank/rear rules.
using Defence=int(__thiscall*)(void*,void*,void*);
using Armour=int(__thiscall*)(void*,void*,void*,int);
using Facing=int(__thiscall*)(void*);
struct MitigationPorts {Defence defence{};Armour armour{};Facing facing{};};
struct Decomposition {float own_margin{},enemy_margin{},own_mitigation{},enemy_mitigation{};};
inline bool powers(const Capture& saved,Score calculate,const MitigationPorts& ports,
                   uint32_t player,uint32_t enemy,float& own,float& other,Decomposition* detail=nullptr){
    if(!ports.defence||!ports.armour||!ports.facing)return false;
    float own_margin{},enemy_margin{};
    if(!scores(saved,calculate,player,enemy,own_margin,enemy_margin))return false;
    auto calculator=saved.calculator;auto a=saved.a,b=saved.b;auto pair=saved.pair;
    const int first_facing=ports.facing(&pair);
    const int first_defence=ports.defence(&calculator,&a,&b);
    const int first_armour=ports.armour(&calculator,&a,&b,first_facing);
    std::swap(a,b);std::swap(pair.attacker,pair.defender);
    const int reverse_facing=ports.facing(&pair);
    const int reverse_defence=ports.defence(&calculator,&a,&b);
    const int reverse_armour=ports.armour(&calculator,&a,&b,reverse_facing);
    for(int value:{first_defence,first_armour,reverse_defence,reverse_armour})
        if(value<-10000||value>10000)return false;
    const int64_t first_mitigation=int64_t(first_defence)+first_armour;
    const int64_t reverse_mitigation=int64_t(reverse_defence)+reverse_armour;
    // Sown=Aown-Denemy; Senemy=Aenemy-Down. Adding the SAME Dtotal
    // to both yields Pown=Aown+Down and Penemy=Aenemy+Denemy. Therefore
    // Pown-Penemy == Sown-Senemy exactly: preserve all native terms and
    // the legacy relative score while attributing defensive strength correctly.
    const double own_power=own_margin+double(first_mitigation+reverse_mitigation);
    const double enemy_power=enemy_margin+double(first_mitigation+reverse_mitigation);
    if(!std::isfinite(own_power)||!std::isfinite(enemy_power)||std::abs(own_power)>10000||std::abs(enemy_power)>10000)return false;
    const bool player_first=saved.a.actor==player;
    Decomposition next{own_margin,enemy_margin,float(player_first?reverse_mitigation:first_mitigation),float(player_first?first_mitigation:reverse_mitigation)};
    own=float(own_power);other=float(enemy_power);if(detail)*detail=next;return true;
}
} }
