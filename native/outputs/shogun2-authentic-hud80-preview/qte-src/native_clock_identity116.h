#pragma once
#include "native_battle_clock.h"
namespace qte {
// Environment and phase storage can survive a battle reload. The validated
// common battle context must belong to the identity too. Called only while
// the runtime coordinator is held; no game writes or extra memory reads.
class BattleClockIdentity116 {
    uint32_t environment_{},common_{},phase_{};
    uint64_t generation_{};
public:
    uint64_t observe(const NativeClock& clock){
        if(!clock.environment||!clock.common||!clock.phase)return 0;
        if(!generation_||clock.environment!=environment_||clock.common!=common_||clock.phase!=phase_){
            if(generation_==UINT64_MAX)return 0;
            environment_=clock.environment;common_=clock.common;phase_=clock.phase;++generation_;
        }
        // A backward tick alone is not proof of a different battle. Timeline
        // retains its rollback, pause, stale-clock and interpolation guards.
        return generation_;
    }
};
}
