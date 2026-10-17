#pragma once
#include <algorithm>
// Parameters are native live _kv_fatigue values, not guessed player constants.
inline int personal_fatigue_cost(bool moving,bool running,bool weapon_active,
                                 int native_action,int idle,int walking,int run){
    int locomotion=moving?(running?run:walking):idle;
    return weapon_active?std::max(native_action,locomotion):locomotion;
}
