#pragma once
#include "protocol.h"
// Local input intent only; these bits are never added to the shared movement ABI.
constexpr unsigned ChargePrimary199=1u<<16;
inline bool charge_forward_sprint199(unsigned movement){
    return (movement&(FORWARD|RUN|BACKWARD|LEFT|RIGHT))==(FORWARD|RUN);
}
inline unsigned charge_intent199(unsigned movement,bool primary,bool aiming,bool owned){
    return owned?(movement|((primary&&!aiming)?ChargePrimary199:0u)):0u;
}
struct ChargeHold199 {
    static constexpr uint64_t hold_ms=150;
    unsigned lease{};
    uint64_t started{};
    unsigned sample(unsigned input,unsigned current_lease,uint64_t now){
        if(!current_lease||!charge_forward_sprint199(input)||!(input&ChargePrimary199)){
            lease=0;
            return input&~ChargePrimary199;
        }
        if(lease!=current_lease){
            lease=current_lease;
            started=now;
        }
        return now-started>=hold_ms?input:input&~ChargePrimary199;
    }
};
struct ClimbRequest199 {
    SRWLOCK lock=SRWLOCK_INIT;unsigned generation{};uint64_t tick{};
    void set(unsigned lease,uint64_t now){AcquireSRWLockExclusive(&lock);generation=lease;tick=now;ReleaseSRWLockExclusive(&lock);}
    void clear(){set(0,0);}
    bool take(unsigned lease,uint64_t now){AcquireSRWLockExclusive(&lock);bool result=generation&&generation==lease&&now>=tick&&now-tick<=250;generation=0;tick=0;ReleaseSRWLockExclusive(&lock);return result;}
};
