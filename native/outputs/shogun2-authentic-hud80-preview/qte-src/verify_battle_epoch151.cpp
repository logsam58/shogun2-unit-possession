#define QTE_SESSION_FIXTURE_ONLY
#define main retained_bridge151_main
#include "verify_session.cpp"
#undef main
#include "battle_epoch151.h"
int main(){try{using namespace qte;
    SessionFixture f;f.world.bytes.resize(0x1bd0000);f.world.setup(true,5);auto& w=f.world;
    constexpr uint32_t root=World::witness+0xe000,phase=World::witness+0xc000;
    w.put(World::engine+0x1bc8180,root);w.put(root+0x110,World::environment);
    w.put(World::common+0xb4,phase);w.put(phase+0x14,6u);w.put(phase+0x88,0u);w.put(phase+0x8c,0u);
    w.put(World::engine+0x15b2448,.1f);w.put(World::engine+0x15ac9c8,1.f);w.put(World::engine+0x15c2764,100.f);
    BattleEpoch151 epochs;ClockFrame first,old,next;w.put(World::common+0x50,473u);
    check(epochs.sample(w.reader(),World::engine,World::environment,0,first),"Initial native epoch failed");
    w.put(World::common+0x50,13669u);check(epochs.sample(w.reader(),World::engine,0,1319.6,old)&&old.battle==first.battle,"Ordinary progress changed epoch");
    // Exact observed failure: SAME env/phase, old tick13669, new battle tick4386.
    // The old pointer-key implementation cannot open a prompt after this.
    {Session legacy(w.reader(),w.selection(),f.calls(),World::engine,{0,5,true},true);f.session=&legacy;
     legacy.poll_clock(first);legacy.poll_clock(old);auto raw=old;raw.tick=4386;raw.wall=1859;
     auto owner=w.owner();owner.battle=raw.battle;
     legacy.prepare(World::witness,1,owner,raw,true);
     check(!legacy.occupied()&&legacy.acquisition_stage()==1,"Negative control no longer reproduces the saved stage1 lockout");}
    w.setup(true,5);w.put(World::common+0x50,4386u);
    check(epochs.sample(w.reader(),World::engine,World::environment,1859,next)&&next.battle!=old.battle&&epochs.restarts()==1,"Confirmed reused-address model reset retained old epoch");
    {Session s(w.reader(),w.selection(),f.calls(),World::engine,{0,5,true},true);f.session=&s;
     s.poll_clock(first);s.poll_clock(old);s.enqueue({99,old.battle,1200,true,false,true,false});
     auto owner=w.owner();owner.battle=next.battle;s.prepare(World::witness,2,owner,next,true);
     check(s.occupied()&&s.publication(99,1859).prompt.active&&s.publication(99,1859).error==0,"New-battle QTE was not admitted or inherited an old click");}
    Timeline t;check(t.observe(next).valid,"New epoch timeline unavailable");
    auto stale=next;--stale.tick;stale.wall+=.1;check(!t.observe(stale).valid,"Out-of-order copied frame reset Timeline");
    check(!epochs.sample(w.reader(),World::engine,World::environment,1858,first),"Backward wall sample mutated epoch");
    auto before=epochs.epoch();check(!epochs.sample(w.reader(),World::engine,World::environment+4,1860,first)&&epochs.epoch()==before,"Retired/foreign environment changed epoch");
    w.put(phase+0x14,4u);check(!epochs.sample(w.reader(),World::engine,0,1860,first)&&epochs.epoch()==before,"Menu/nonbattle phase advanced QTE");w.put(phase+0x14,6u);
    for(unsigned i=0;i<100;++i){
        w.put(World::common+0x50,9000u);check(epochs.sample(w.reader(),World::engine,0,2000+i*2,old),"Progress sample failed");
        w.put(World::common+0x50,10u);check(epochs.sample(w.reader(),World::engine,0,2001+i*2,next)&&next.battle==old.battle+1,"Repeated reused-address reset did not create exactly one epoch");
        w.setup(true,5);Session s(w.reader(),w.selection(),f.calls(),World::engine,{0,5,true},true);f.session=&s;
        auto owner=w.owner();owner.battle=next.battle;s.prepare(World::witness,i+3,owner,next,true);check(s.occupied(),"Repeated battle lost QTE admission");
    }
    std::cout<<"PASS: saved 13669-to-4386 pointer-reuse failure reproduced as negative control; current-root/native double-read clock epochs restore actual Session admission, 100 reused-address battles, stale-frame/wall/menu/foreign-environment rejection and old-click isolation. Native facts/selection providers explicit.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
