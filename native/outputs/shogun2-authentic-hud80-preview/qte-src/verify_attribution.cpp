#include "native_effective_combat.h"
#include <iostream>
#include <stdexcept>
using namespace qte;
static void check(bool b,const char* message){if(!b)throw std::runtime_error(message);}
struct Facts {int attack=10,defence=5,armour=2,shield=1,bonus{},defence_divisor=1,armour_divisor=1;};
static Facts player,enemy;
static unsigned score_calls,defence_calls,armour_calls,facing_calls;
static Facts& actor(void* p){auto id=static_cast<effective::ActorContext*>(p)->actor;check(id==100||id==200,"Foreign fixture actor");return id==100?player:enemy;}
static int __fastcall defence(void*,void*,void* a,void* b){++defence_calls;return actor(b).defence/actor(a).defence_divisor;}
static int __fastcall facing(void* p,void*){++facing_calls;auto& pair=*static_cast<effective::PairContext*>(p);return int((pair.flags>>(pair.attacker==100?0:2))&3);}
static int __fastcall armour(void*,void*,void* a,void* b,int side){++armour_calls;
    auto& other=actor(b);return side==0||side==1?other.armour/actor(a).armour_divisor+other.shield:side==2?other.shield:0;
}
static int __fastcall score(void* self,void*,void* a,void* b,void* p,uint32_t debug){++score_calls;
    auto& pair=*static_cast<effective::PairContext*>(p);
    check(!debug&&pair.attacker==static_cast<effective::ActorContext*>(a)->actor&&pair.defender==static_cast<effective::ActorContext*>(b)->actor,"Native pair role or debug flag changed");
    return actor(a).attack+actor(a).bonus-defence(self,nullptr,a,b)-armour(self,nullptr,a,b,facing(p,nullptr));
}
int main(){try{
    effective::Capture capture{};capture.valid=true;capture.witness=123;capture.generation=1;
    capture.a.actor=100;capture.b.actor=200;capture.pair.attacker=100;capture.pair.defender=200;
    auto calculate=reinterpret_cast<effective::Score>(score);
    effective::MitigationPorts ports{reinterpret_cast<effective::Defence>(defence),reinterpret_cast<effective::Armour>(armour),reinterpret_cast<effective::Facing>(facing)};
    float own{},other{};unsigned cases=0;
    uintptr_t before{},after{};__asm mov before,esp
    for(unsigned modes=0;modes<16;++modes)for(int divisor:{1,2,4})for(int bonus:{-5,0,7})for(bool reverse:{false,true}){
        player={};enemy={};player.bonus=bonus;enemy.defence_divisor=divisor;player.armour_divisor=divisor;capture.pair.flags=modes;
        effective::Decomposition detail;
        check(effective::powers(capture,calculate,ports,reverse?200:100,reverse?100:200,own,other,&detail),"Attribution rejected valid contexts");
        check(own-other==detail.own_margin-detail.enemy_margin,"Legacy directional difference changed");
        const int player_defence=player.defence/enemy.defence_divisor;
        const int enemy_defence=enemy.defence/player.defence_divisor;
        const int player_side=int((modes>>2)&3),enemy_side=int(modes&3);
        const int player_armour=player_side<2?player.armour/enemy.armour_divisor+player.shield:player_side==2?player.shield:0;
        const int enemy_armour=enemy_side<2?enemy.armour/player.armour_divisor+enemy.shield:enemy_side==2?enemy.shield:0;
        const float expected_player=float(player.attack+player.bonus+player_defence+player_armour);
        const float expected_enemy=float(enemy.attack+enemy.bonus+enemy_defence+enemy_armour);
        check(own==(reverse?expected_enemy:expected_player)&&other==(reverse?expected_player:expected_enemy),"Effective strength assigned to wrong fighter");++cases;
    }
    __asm mov after,esp
    check(before==after&&score_calls==cases*2&&defence_calls==cases*4&&armour_calls==cases*4&&facing_calls==cases*4,"Native ECX/RET8/RETc/RET10 ABI or helper counts differ");
    // Regression for Test75's mapping gap: a more armoured enemy must narrow
    // the zone; it must not masquerade as worse player readiness.
    capture.pair.flags=0;player={};enemy={};effective::powers(capture,calculate,ports,100,200,own,other);
    UnitState own_state{Required|Strength|Environment|EffectiveCombat,0,1,0,.5f,1,false,0,own},enemy_state=own_state;enemy_state.combat_score=other;
    MeterDifficulty base,changed;check(meter_difficulty(own_state,enemy_state,base),"Baseline meter rejected");
    for(unsigned field=0;field<4;++field){
        enemy={};if(field==0)enemy.armour+=4;if(field==1)enemy.defence+=4;if(field==2)enemy.shield+=4;if(field==3)enemy.attack+=4;
        effective::powers(capture,calculate,ports,100,200,own,other);auto p=own_state,e=enemy_state;p.combat_score=own;e.combat_score=other;
        check(meter_difficulty(p,e,changed)&&changed.width<base.width&&changed.player.duration==base.player.duration,"Enemy attack/defence/armour/shield did not independently narrow zone");
        player=enemy;enemy={};effective::powers(capture,calculate,ports,100,200,own,other);p=own_state;e=enemy_state;p.combat_score=own;e.combat_score=other;
        check(meter_difficulty(p,e,changed)&&changed.player.duration>base.player.duration&&changed.width==base.width,"Player attack/defence/armour/shield did not independently slow reticle");player={};
    }
    auto missing=ports;missing.armour=nullptr;own=123;other=456;
    check(!effective::powers(capture,calculate,missing,100,200,own,other)&&own==123&&other==456,"Missing native port fabricated state");
    player.armour=200000;check(!effective::powers(capture,calculate,ports,100,200,own,other),"Unbounded native mitigation accepted");
    check(!effective::powers(capture,calculate,ports,999,200,own,other),"Foreign actor accepted");
    std::cout<<"PASS: "<<cases<<" native score/mitigation attribution sequences; all facing modes, native-style defence/armour divisors, contextual bonuses, role reversal, exact old score difference and x86 ECX/RET8/RETc/RET10 stack balance. Player and enemy attack/defence/armour/shield affect their intended meter property. Synthetic engine only.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
