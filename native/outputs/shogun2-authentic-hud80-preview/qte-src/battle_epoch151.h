#pragma once
#include "native_battle_clock.h"
#include "qte_timeline.h"
namespace qte {
// Process-local identities for native battle CLOCK lifetimes. Environment and
// phase addresses are reused by the engine; they are not lifetime serials.
// Called only under the existing runtime coordinator, with fresh memory reads.
class BattleEpoch151 {
    uint32_t root_{},environment_{},common_{},phase_{},tick_{};
    uint64_t epoch_{},restarts_{};double wall_{};
public:
    uint64_t epoch()const{return epoch_;}
    uint64_t restarts()const{return restarts_;}
    bool sample(const Reader& r,uint32_t engine,uint32_t expected_environment,double wall,ClockFrame& out){
        if(!engine||engine>UINT32_MAX-0x1bc8184||!std::isfinite(wall)||wall<wall_)return false;
        uint32_t root{},env{};NativeClock c;
        if(!r.get(engine+0x1bc8180,root)||!root||root>UINT32_MAX-0x114||
           !r.get(root+0x110,env)||!env||(expected_environment&&env!=expected_environment)||
           !battle_clock(r,env,engine,c))return false;
        bool changed=!epoch_||root!=root_||env!=environment_||c.common!=common_||c.phase!=phase_;
        if(!changed&&c.tick<tick_){
            // A copied/out-of-order ClockFrame NEVER resets Timeline. Re-read
            // the live native clock to prove an actual model counter reset.
            NativeClock confirmed;
            if(!battle_clock(r,env,engine,confirmed)||confirmed.common!=c.common||confirmed.phase!=c.phase||
               confirmed.step!=c.step||confirmed.tick<c.tick||confirmed.tick>=tick_)return false;
            c=confirmed;changed=true;
        }
        uint32_t current_root{},current_env{};
        if(!r.get(engine+0x1bc8180,current_root)||current_root!=root||
           !r.get(root+0x110,current_env)||current_env!=env)return false;
        if(changed){if(epoch_)++restarts_;++epoch_;}
        root_=root;environment_=env;common_=c.common;phase_=c.phase;tick_=c.tick;wall_=wall;
        out={epoch_,c.tick,wall,c.step,c.speed};return true;
    }
};
}
