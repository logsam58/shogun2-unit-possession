#include "qte_pair_gate.h"
#include <iostream>
#include <stdexcept>
static void check(bool ok,const char* reason){if(!ok)throw std::runtime_error(reason);}
int main(){try{
    using namespace qte; const OutcomeBindings fixture{0,4,true}; // synthetic result codes only
    UnitState facts{Required|Strength,2,2,10,.5f,.8f,false};
    unsigned cases=0;
    for(unsigned i=1;i<=10000;++i)for(bool attack:{false,true}){
        Pair pair{1,7,11,12,3,4};WitnessKey key{0x1234,i,pair};
        Verdict selected{5,attack?11u:12u,attack?12u:11u};PairGate gate(fixture);
        check(gate.acquire(key,selected,facts,facts),"Native selected pair rejected");
        const auto snapshot=gate.snapshot();
        check(snapshot.action==(attack?Action::Attack:Action::Defend),"Native roles changed");
        auto other=key;other.address++;auto reused=key;reused.generation++;
        check(gate.prepare(other)==Prepare::Forward&&!gate.hold_update(other),"Unrelated combat delayed");
        check(gate.prepare(reused)==Prepare::Forward&&!gate.hold_update(reused),"Recycled witness acquired");
        check(!gate.acquire(other,selected,facts,facts),"Two pairs acquired");
        check(gate.prepare(key)==Prepare::Defer&&gate.hold_update(key),"Unprepared animation update allowed");
        gate.update(key,facts,facts,5,true);check(gate.snapshot().elapsed==0,"Paused witness advanced");
        check(gate.press(other,snapshot.serial,Button::Left,true)==Result::Canceled,"Foreign click accepted");
        gate.update(key,facts,facts,snapshot.ideal,false);
        check(gate.press(key,snapshot.serial,attack?Button::Left:Button::Right,true)==Result::Success,"Timed press failed");
        Verdict resolved;check(gate.prepare(key)==Prepare::Replay&&gate.hold_update(key),"Ready pair lost before replay");
        check(!gate.begin_replay(key,false,resolved),"Invalid native pair replayed");
        check(gate.begin_replay(key,true,resolved),"Valid replay refused");
        check(resolved.attacker==selected.attacker&&resolved.defender==selected.defender,"Cached roles changed");
        check(resolved.outcome==(attack?4u:0u),"Timed verdict incorrect");
        check(!gate.begin_replay(key,true,resolved)&&!gate.hold_update(key),"Replay recursed or held native execution");
        check(gate.finish_replay(key)&&!gate.finish_replay(key),"Replay not exactly once");
        check(!gate.acquire(key,selected,facts,facts),"Witness selected twice");++cases;
    }
    Pair pair{1,7,11,12,3,4};WitnessKey key{0x1234,1,pair};Verdict selected{5,11,12};
    PairGate unverified;check(!unverified.acquire(key,selected,facts,facts),"Unknown native outcomes acquired a playable prompt");
    for(bool release:{false,true}){
        PairGate gate(fixture);check(gate.acquire(key,selected,facts,facts),"Setup failed");
        if(release)gate.release();else {auto missing=facts;missing.known&=~General;gate.update(key,missing,facts,.1,false);}
        Verdict stock;check(gate.begin_replay(key,true,stock)&&stock.outcome==selected.outcome,"Cancellation changed stock verdict");
    }
    PairGate removed(fixture);check(removed.acquire(key,selected,facts,facts),"Removal setup");
    removed.removed(key.address,key.generation+1);check(removed.occupied(),"Stale remover canceled live witness");
    removed.removed(key.address,key.generation);Verdict stock;
    check(!removed.occupied()&&!removed.begin_replay(key,true,stock),"Removed witness replayed");
    auto wrong_roles=selected;wrong_roles.defender=99;PairGate foreign(fixture);
    check(!foreign.acquire(key,wrong_roles,facts,facts),"Non-player native contest acquired");
    InputEdges edges;edges.arm(9,true,true);
    auto held=edges.sample(9,true,true,true);check(!held.left&&!held.right,"Initial selection/held input leaked");
    edges.sample(9,false,false,true);auto paused=edges.sample(9,true,true,false);
    auto resume=edges.sample(9,true,true,true);check(!paused.left&&!paused.right&&!resume.left&&!resume.right,"Pause/focus input leaked");
    edges.sample(9,false,false,true);auto pressed=edges.sample(9,true,true,true);
    check(pressed.left&&pressed.right,"Fresh click edges missing");
    edges.arm(10,false,false);auto stale=edges.sample(9,true,true,true);
    check(!stale.left&&!stale.right,"Stale prompt click accepted");
    auto fresh=edges.sample(10,true,false,true);check(fresh.left,"Stale sample contaminated current button state");
    std::cout<<"PASS: "<<cases<<" native-pair coordinator scenarios; foreign forwarding, unprepared update hold, pause/input isolation, cached role preservation, exactly-once replay, release restoration and removal/address-reuse guards. No live hooks tested.\n";
    return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
