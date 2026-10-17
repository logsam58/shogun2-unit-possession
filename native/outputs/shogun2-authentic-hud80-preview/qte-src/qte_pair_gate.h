#pragma once
#include "qte_core.h"

namespace qte {
// The native adapter must stamp this on the simulation thread and invalidate it
// from native witness removal. A recycled address alone is never an identity.
struct WitnessKey {
    uint32_t address{};
    uint64_t generation{};
    Pair pair{};
    bool valid()const{return address&&generation&&pair.valid();}
};
inline bool same_witness(const WitnessKey& a,const WitnessKey& b){
    return a.address==b.address&&a.generation==b.generation&&same_pair(a.pair,b.pair);
}
struct Verdict {
    uint32_t outcome{},attacker{},defender{};
    bool roles_match(const Pair& p)const{
        return (attacker==p.player&&defender==p.opponent)||
               (attacker==p.opponent&&defender==p.player);
    }
};
enum class Prepare {Forward,Defer,Replay};
class PairGate {
    WitnessKey key_{};
    Verdict original_{};
    Event event_{};
    bool occupied_{},resuming_{},canceled_{};
    uint64_t last_replayed_generation_{};
    OutcomeBindings outcomes_{};
public:
    explicit PairGate(OutcomeBindings outcomes={}):outcomes_(outcomes){}
    // Invoked only after ONE native selection has supplied the roles/verdict.
    // Never calls the selector itself and never chooses or changes either role.
    bool acquire(const WitnessKey& key,const Verdict& original,
                 const UnitState& own,const UnitState& enemy,bool charge=false){
        if(!outcomes_.valid()||occupied_||!key.valid()||key.generation<=last_replayed_generation_||
           !original.roles_match(key.pair)||original.outcome>5)return false;
        const Action action=original.attacker==key.pair.player?(charge?Action::Charge:Action::Attack):Action::Defend;
        if(!event_.begin(key.pair,action,own,enemy))return false;
        key_=key;original_=original;occupied_=true;resuming_=false;canceled_=false;
        return true;
    }
    bool update(const WitnessKey& key,const UnitState& own,const UnitState& enemy,
                double simulation_dt,bool paused){
        if(!occupied_||resuming_||!same_witness(key,key_))return false;
        if(!event_.update(key.pair,own,enemy,simulation_dt,paused))canceled_=true;
        return !canceled_;
    }
    Result press(const WitnessKey& key,uint64_t serial,Button button,bool edge){
        if(!occupied_||resuming_||canceled_||!same_witness(key,key_))return Result::Canceled;
        return event_.press(serial,button,edge);
    }
    Result press_visible93(const WitnessKey& key,uint64_t serial,Button button,bool edge,const Snapshot& shown){
        if(!occupied_||resuming_||canceled_||!same_witness(key,key_))return Result::Canceled;
        return event_.press_visible93(serial,button,edge,shown);
    }
    Prepare prepare(const WitnessKey& key)const{
        if(!occupied_||resuming_||!same_witness(key,key_))return Prepare::Forward;
        return !canceled_&&event_.snapshot().result==Result::Pending?Prepare::Defer:Prepare::Replay;
    }
    bool hold_update(const WitnessKey& key)const{
        // An unprepared witness has no principal animation array. Its native
        // update must be held until the prepare replay has actually completed.
        return occupied_&&!resuming_&&same_witness(key,key_);
    }
    bool begin_replay(const WitnessKey& key,bool native_pair_still_valid,Verdict& out){
        if(!native_pair_still_valid||prepare(key)!=Prepare::Replay)return false;
        Result result=Result::Canceled;
        if(!canceled_&&!event_.consume(key.pair,result))return false;
        out=original_;
        if(!native_outcome(event_.snapshot().action,result,original_.outcome,outcomes_,out.outcome))return false;
        resuming_=true;last_replayed_generation_=key.generation;
        return true;
    }
    bool finish_replay(const WitnessKey& key){
        if(!occupied_||!resuming_||!same_witness(key,key_))return false;
        occupied_=false;resuming_=false;canceled_=false;event_.cancel();return true;
    }
    void release(){
        // A valid pending pair resumes its already-selected stock verdict.
        // Removal/teardown instead calls removed(), which discards the cache.
        if(occupied_&&!resuming_){canceled_=true;event_.cancel();}
    }
    void removed(uint32_t address,uint64_t generation){
        if(occupied_&&key_.address==address&&key_.generation==generation){
            occupied_=false;resuming_=false;canceled_=false;event_.cancel();
        }
    }
    Snapshot snapshot()const{return event_.snapshot();}
    bool occupied()const{return occupied_;}
};

// Observe every sample, including pause/loss of focus. Begin arms against the
// current button levels so a selecting click or held RMB cannot answer a QTE.
class InputEdges {
    bool left_{},right_{};
    uint64_t serial_{};
public:
    void arm(uint64_t serial,bool left,bool right){serial_=serial;left_=left;right_=right;}
    struct Edges {uint64_t serial{};bool left{},right{};};
    Edges sample(uint64_t serial,bool left,bool right,bool accept){
        const bool current=serial==serial_;
        if(!current)return {serial,false,false};
        Edges edges{serial,current&&accept&&left&&!left_,current&&accept&&right&&!right_};
        left_=left;right_=right;
        // A new event must be explicitly armed. A stale sample cannot retarget
        // itself onto the latest prompt or leak a press into the next frame.
        return edges;
    }
};
}
