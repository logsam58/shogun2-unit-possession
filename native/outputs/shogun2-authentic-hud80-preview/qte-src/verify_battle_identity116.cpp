#define QTE_SESSION_FIXTURE_ONLY
#include "verify_session.cpp"
#ifndef QTE_CLOCK_IDENTITY_HEADER116
#define QTE_CLOCK_IDENTITY_HEADER116 "native_clock_identity116.h"
#endif
#include QTE_CLOCK_IDENTITY_HEADER116
int main(){try{using namespace qte;unsigned cases=0;
    // Recorded consecutive battles reused environment821051112 and
    // phase1329217544, but common changed614640200->614650864 and tick reset.
    // Exercise the production session as well as the native-clock identity.
    for(bool attack:{false,true}){
        BattleClockIdentity116 identity;
        NativeClock old{821051112,614640200,1329217544,3802,.1f,1.f};
        const auto old_battle=identity.observe(old);
        SessionFixture f;f.world.setup(attack,5);
        Session s(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true);f.session=&s;
        auto owner=f.world.owner();owner.battle=old_battle;
        s.prepare(World::witness,1,owner,{old_battle,3802,200,.1,1},true);
        check(s.occupied(),"Previous-battle fixture failed to acquire");
        s.release();s.prepare(World::witness,1,owner,{old_battle,3803,200.1,.1,1},true,false);
        check(!s.occupied(),"Previous-battle fixture failed to release");
        s.removed(World::witness,1);
        f.world.setup(attack,5);
        NativeClock next=old;next.common=614650864;next.tick=127;
        const auto next_battle=identity.observe(next);
        owner=f.world.owner();owner.battle=next_battle;++owner.lease;
        s.prepare(World::witness,2,owner,{next_battle,127,230,.1,1},true);
        const auto shown=s.publication(99,230);
        check(shown.prompt.active&&!shown.prompt.paused&&s.occupied(),"Reused environment/phase left next battle QTE ineligible");
        check(next_battle!=old_battle&&shown.anchor.battle==next_battle&&shown.anchor.simulation==0,"Next battle inherited the previous QTE clock");
        check(f.world.rolls==1&&f.world.prepares==0,"Next battle duplicated selection or prepared before answer");
        check(s.enqueue({shown.prompt.serial,old_battle,.05,true,true,true,false}),"Stale-click fixture enqueue failed");
        s.prepare(World::witness,2,owner,{next_battle,128,230.1,.1,1},true,false);
        check(s.publication(99,230.1).prompt.result==Result::Pending,"Old battle click answered new QTE");
        ++cases;
    }
    for(unsigned changed=0;changed<3;++changed){
        BattleClockIdentity116 identity;NativeClock c{0x10000,0x20000,0x30000,3802,.1f,1.f};
        const auto original=identity.observe(c);check(original!=0,"Valid clock has no identity");
        Timeline t;check(t.observe({original,c.tick,200,.1,1}).valid,"Clock fixture failed to initialize");
        for(unsigned i=0;i<100;++i){
            c.speed=i&1?0.f:1.f;
            check(identity.observe(c)==original,"Pause/rate poll changed battle identity");
        }
        c.tick=127;check(identity.observe(c)==original,"Backward tick invented a new battle");
        check(!t.observe({original,c.tick,201,.1,1}).valid,"Same-battle rollback guard was relaxed");
        if(changed==0)c.environment+=4;else if(changed==1)c.common+=4;else c.phase+=4;
        const auto next=identity.observe(c);
        check(next!=original&&t.observe({next,c.tick,202,.1,1}).valid,"Native context change failed to reset clock");
        for(unsigned bad=0;bad<3;++bad){auto invalid=c;
            if(bad==0)invalid.environment=0;else if(bad==1)invalid.common=0;else invalid.phase=0;
            check(!identity.observe(invalid)&&identity.observe(c)==next,"Invalid clock consumed an identity");
        }
        ++cases;
    }
    std::cout<<"PASS: "<<cases<<" native battle-identity/session regressions; reused environment/phase, attack/defend reacquisition, stale clicks, context changes, pause/rate polling and same-battle rollback. Native calls use existing fixtures.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
