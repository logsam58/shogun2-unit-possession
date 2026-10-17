#pragma once
namespace qte {
// Keep timestamped edge sampling at 1ms, while servicing ownership/publication
// at the previous bounded 8ms cadence. Urgent stop/focus/stale-frame work wins.
class ServiceCadence94 {
    double next_{};
public:
    static constexpr double period=.008;
    bool due(double now,bool urgent=false){
        if(!urgent&&now<next_)return false;
        next_=now+period;return true; // No catch-up burst after scheduling stalls.
    }
};
}
