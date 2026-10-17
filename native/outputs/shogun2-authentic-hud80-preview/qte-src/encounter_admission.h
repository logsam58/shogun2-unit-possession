#pragma once
#include "qte_pair_gate.h"
#include "qte_timeline.h"
namespace qte {
// Prompt lifetime and native encounter lifetime are different. A resolved
// prompt must not admit another until its witness is actually removed.
class EncounterAdmission {
    WitnessKey key_{};
    uint64_t battle_{},lease_{},retry_tick_{};
    bool playing_{};
public:
    bool allowed(uint64_t battle,uint64_t lease,uint64_t tick){
        if(!battle||!lease)return false;
        if(battle!=battle_||lease!=lease_){key_={};playing_=false;retry_tick_=0;battle_=battle;lease_=lease;}
        return !key_.valid()&&tick>=retry_tick_;
    }
    bool acquire(const WitnessKey& key,uint64_t tick){
        if(!key.valid()||!allowed(key.pair.battle,key.pair.lease,tick))return false;
        key_=key;playing_=false;return true;
    }
    bool matches(uint32_t address,uint64_t generation)const{return key_.valid()&&key_.address==address&&key_.generation==generation;}
    void started(uint32_t address,uint64_t generation){if(matches(address,generation))playing_=true;}
    bool removed(uint32_t address,uint64_t generation,const ClockFrame& clock){
        if(!matches(address,generation))return false;
        // Failed native starts need a bounded retry interval. Simulation ticks
        // freeze this interval while paused; wall time never advances it.
        if(!playing_&&clock.battle==battle_&&std::isfinite(clock.step)&&clock.step>=.001&&clock.step<=.2){
            uint64_t ticks=uint64_t(std::ceil(.75/clock.step));
            retry_tick_=clock.tick>UINT64_MAX-ticks?UINT64_MAX:clock.tick+ticks;
        }else retry_tick_=0;
        key_={};playing_=false;return true;
    }
    uint32_t address()const{return key_.valid()?key_.address:0;}
};
}
