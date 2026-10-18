#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "qte_core.h"
namespace qte {
struct ClockFrame {
    uint64_t battle{},tick{};
    double wall{},step{},speed{};
};
struct TimePoint {
    uint64_t battle{};double wall{},simulation{},horizon{},speed{};
    bool valid{},paused{};
};
class Timeline {
    uint64_t battle_{},origin_tick_{},last_tick_{};
    double step_{},last_wall_{},anchor_wall_{},anchor_sim_{},last_progress_{},time_{},speed_{};
    bool initialized_{};
public:
    TimePoint observe(const ClockFrame& f){
        if(!f.battle||!std::isfinite(f.wall)||!std::isfinite(f.step)||f.step<.001||f.step>.2||
           !std::isfinite(f.speed)||f.speed<0||f.speed>16)return {};
        if(!initialized_||f.battle!=battle_){
            battle_=f.battle;origin_tick_=last_tick_=f.tick;step_=f.step;
            last_wall_=anchor_wall_=last_progress_=f.wall;anchor_sim_=time_=0;speed_=f.speed;initialized_=true;
        }
        if(f.wall<last_wall_||f.tick<last_tick_||std::abs(f.step-step_)>1e-8)return {};
        const double floor=double(f.tick-origin_tick_)*step_;
        const double horizon=floor+step_;
        if(f.tick!=last_tick_){
            last_tick_=f.tick;last_progress_=f.wall;
            // Reconcile prediction at each actual model step. A permanent
            // battle-start wall anchor accumulates even tiny scheduling drift
            // until every displayed frame is clamped to the tick horizon.
            // Keep already observed/clicked time monotonic and retain the
            // same one-step native horizon, pause and stale-clock limits.
            anchor_sim_=std::max(time_,floor);anchor_wall_=f.wall;
        }
        // Pause and rate transitions start at the last displayed value. They
        // never charge wall time spent paused or retrospectively change clicks.
        if(f.speed!=speed_){anchor_sim_=time_;anchor_wall_=f.wall;speed_=f.speed;}
        if(speed_==0){anchor_sim_=time_;anchor_wall_=last_progress_=f.wall;}
        else if(f.wall-last_progress_>std::max(.5,2*step_/speed_+.1))return {};
        double forecast=anchor_sim_+std::max(0.,f.wall-anchor_wall_)*speed_;
        time_=std::max(time_,std::min(horizon,std::max(floor,forecast)));
        last_wall_=f.wall;
        return {battle_,f.wall,time_,horizon,speed_,true,speed_==0};
    }
    void reset(){initialized_=false;}
};
inline double interpolate(const TimePoint& point,double wall){
    if(!point.valid||point.paused||!std::isfinite(wall)||wall<point.wall)return point.simulation;
    return std::max(point.simulation,std::min(point.horizon,point.simulation+(wall-point.wall)*point.speed));
}
struct Click {
    uint64_t serial{},battle{};
    double simulation{};
    bool left{},right{},focused{},paused{};
    bool visible93{};Snapshot shown93{};
};
// External synchronization belongs to the adapter. On overflow, the prompt
// must be canceled rather than accepting incomplete button/timing history.
template<unsigned Capacity=64>class ClickQueue {
    static_assert(Capacity>0,"The input queue needs storage");
    Click values_[Capacity]{};uint64_t begin_{},end_{};bool overflow_{};
public:
    bool push(const Click& click){
        if(end_-begin_==Capacity){overflow_=true;return false;}
        values_[end_++%Capacity]=click;return true;
    }
    bool pop(Click& click){if(begin_==end_)return false;click=values_[begin_++%Capacity];return true;}
    bool overflowed()const{return overflow_;}
    void clear(){begin_=end_=0;overflow_=false;}
};
}
