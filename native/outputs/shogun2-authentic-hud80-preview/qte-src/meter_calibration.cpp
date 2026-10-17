#include "qte_core.h"
#include <iostream>
#include <iomanip>
int main(){using namespace qte;
    std::cout<<"player_morale,player_fatigue,player_power,enemy_power,sweep_seconds,zone_fraction,passage_ms,frames_at_60hz\n"<<std::fixed<<std::setprecision(6);
    // Representative samples of the existing /40 score mapping, including
    // a value beyond its original clamp. Samples are not tuning coefficients.
    for(unsigned morale:{0u,2u,4u,5u,7u})for(unsigned fatigue:{0u,2u,5u})for(float player_score:{0.f,20.f,40.f})for(float score:{0.f,20.f,40.f,60.f}){
        UnitState own{Required|Strength|Environment|EffectiveCombat,fatigue,morale,10,.5f,.8f,false,0,0},enemy=own;
        own.combat_score=player_score;enemy.morale=2;enemy.fatigue=2;enemy.combat_score=score;
        MeterDifficulty d;if(!meter_difficulty(own,enemy,d))return 1;
        double passage=d.player.duration*d.width;
        std::cout<<morale<<','<<fatigue<<','<<player_score<<','<<score<<','<<d.player.duration<<','<<d.width<<','<<passage*1000<<','<<passage*60<<'\n';
    }
    return 0;
}
