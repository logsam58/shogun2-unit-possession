#define main bridge_fixture_main
#include "verify_native_bridge.cpp"
#undef main
#include "qte_session.h"
#include "encounter_admission.h"
#include "melee_commitment.h"
struct SessionFixture {
    World world; qte::Session* session{};
    bool facing_ready_now=true,routed=false;
    static bool routing(void* opaque,uint32_t,bool& value){value=static_cast<SessionFixture*>(opaque)->routed;return true;}
    static bool facing(void* opaque,uint32_t player,uint32_t enemy){
        auto& f=*static_cast<SessionFixture*>(opaque);check(player==World::player&&enemy==World::enemy,"Facing callback changed roles");return f.facing_ready_now;
    }
    static uint32_t prepare(void* opaque,uint32_t address){
        auto& f=*static_cast<SessionFixture*>(opaque);++f.world.prepares;
        if(address!=World::witness)return 0xa5a5007f;
        qte::NativeVerdict selected;
        if(!f.session->try_replay(address,0x3e5f7c,World::context,address+0x10,address+0x18,0,0,selected))
            check(World::select(&f.world,World::context,address+0x10,address+0x18,0,0,selected),"Fixture stock select failed");
        f.world.committed=selected;f.world.put(address+0xc,uint8_t(1));
        f.world.put(World::player+0x688,address);f.world.put(World::enemy+0x688,address);return 0;
    }
    static uint32_t update(void* opaque,uint32_t address){return World::update(&static_cast<SessionFixture*>(opaque)->world,address);}
    qte::BridgeCalls calls(){return {this,prepare,update,routing};}
};
#ifndef QTE_SESSION_FIXTURE_ONLY
int main(){try{using namespace qte;unsigned cases=0;
    // Timestamped queue evaluates a return-leg click exactly as displayed,
    // while the native pair remains held through the one allowed traversal.
    for(bool attack:{false,true}){
        SessionFixture f;f.world.setup(attack,5);Session session(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true);f.session=&session;
        auto owner=f.world.owner();session.prepare(World::witness,1,owner,{1,0,200,.1,1},true);
        bool clicked=false;
        for(unsigned frame=1;frame<2400&&session.occupied();++frame){
            double simulation=double(frame)/120.,wall=200+simulation;
            ClockFrame clock{1,uint64_t(std::floor(simulation/.1+1e-8)),wall,.1,1};session.poll_clock(clock);
            auto data=session.publication(99,wall);Snapshot shown;
            check(transport::presentation(data,wall,shown),"Return-leg session publication vanished");
            if(!clicked&&shown.result==Result::Pending&&shown.elapsed>shown.duration&&in_success_zone(shown)){
                Click click;check(transport::click(data,wall,attack,!attack,true,click),"Return-leg click unavailable");
                session.enqueue(click);session.levels(attack,!attack);clicked=true;
            }
            session.prepare(World::witness,1,owner,clock,true,false);
        }
        check(clicked&&!session.occupied()&&f.world.rolls==1&&f.world.prepares==1&&f.world.committed.outcome==(attack?5u:0u),"Multi-cycle return-leg replay incorrect");
        check(session.publication(99,210).error==0,"Return-leg queue reported timing error");++cases;
    }
    {SessionFixture f;f.world.setup(true,5);f.facing_ready_now=false;auto ports=f.calls();ports.facing_ready=SessionFixture::facing;
     Session s(f.world.reader(),f.world.selection(),ports,World::engine,{0,5,true},true);f.session=&s;auto owner=f.world.owner();
     s.prepare(World::witness,1,owner,{1,0,200,.1,1},true);
     auto first=s.publication(99,200);check(first.prompt.aligning&&!first.prompt.paused&&first.prompt.elapsed==0,"Facing introduced a startup grace period");
     Snapshot projected;check(transport::presentation(first,200.05,projected)&&projected.elapsed>.049f,"First visible prompt did not immediately oscillate");
     f.routed=true; // Routing may not abandon an already admitted event.
     for(uint64_t tick=1;tick<=3;++tick)s.prepare(World::witness,1,owner,{1,tick,200+double(tick)*.1,.1,1},true,false);
     auto held=s.publication(99,200.3);check(s.occupied()&&held.prompt.result==Result::Pending&&held.prompt.elapsed>0&&f.world.prepares==0,"Facing wait froze or abandoned the active meter");
     f.facing_ready_now=true;s.levels(false,false);s.prepare(World::witness,1,owner,{1,4,200.4,.1,1},true,false);
     auto ready=s.publication(99,200.4);check(!ready.prompt.aligning&&!ready.prompt.paused&&ready.prompt.elapsed>.11f,"Facing completion reset the active timing window");
     s.release();s.prepare(World::witness,1,owner,{1,5,200.5,.1,1},true,false);check(f.world.prepares==1&&!s.occupied(),"RTS release failed while facing");
     f.world.setup(true,5);s.prepare(World::witness,2,owner,{1,6,200.6,.1,1},true);
     check(!s.occupied()&&f.world.prepares==1,"Routing did not reject a NEW encounter");}
    {EncounterAdmission admission;WitnessKey key{123,7,{1,9,0,1,1,2}};
     check(admission.acquire(key,100),"Encounter not admitted");
     // Answer resolution does not release this gate. Competing witnesses
     // remain blocked during both alignment and animation playback.
     for(uint64_t tick=100;tick<150;++tick)check(!admission.allowed(1,9,tick),"Premature encounter admission");
     admission.started(123,7);check(!admission.allowed(1,9,150),"Playing encounter released early");
     check(!admission.removed(123,6,{1,150,200,.1,1}),"Recycled-generation removal released encounter");
     check(admission.removed(123,7,{1,150,200,.1,1})&&admission.allowed(1,9,150),"Finished encounter stuck");
     key.generation=8;check(admission.acquire(key,150),"Next encounter not admitted");
     check(admission.removed(123,8,{1,151,200.1,.1,1}),"Failed-start removal not processed");
     for(unsigned poll=0;poll<100;++poll)check(!admission.allowed(1,9,151),"Pause advanced failed-start retry");
     check(!admission.allowed(1,9,158)&&admission.allowed(1,9,159),"Failed-start retry interval wrong");
     key.generation=9;check(admission.acquire(key,159),"Retry did not become eligible");
     check(admission.allowed(1,10,0),"FPS actor lease change retained old encounter");
     check(!admission.removed(123,9,{1,0,201,.1,1}),"Old lease removal affected new actor");
     key.pair.lease=10;check(admission.acquire(key,0)&&admission.allowed(2,10,0),"New battle retained old lock");}
    {SessionFixture f;f.world.setup(true,5);Session s(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true);f.session=&s;
     // Denying a new prompt forwards ONE untouched original preparation;
     // it must not preview selection or reroll the native result.
     check(s.prepare(World::witness,1,f.world.owner(),{1,0,200,.1,1},true,false)==0,"Blocked admission altered native return");
     check(!s.occupied()&&f.world.rolls==1&&f.world.prepares==1&&f.world.sorts==0&&f.world.committed.outcome==5,"Blocked admission duplicated native selection");
     check(s.prepare(World::witness+0x100,2,f.world.owner(),{1,1,200.1,.1,1},true,false)==0xa5a5007f,"Blocked admission changed foreign EAX");}
    for(bool initial_zero:{false,true})for(bool attack:{false,true})for(bool success:{false,true})for(bool pause:{false,true}){
        SessionFixture f;f.world.setup(attack,5);
        if(initial_zero){f.world.put(World::player+0x688,0u);f.world.put(World::enemy+0x688,0u);}
        Session session(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true);f.session=&session;
        auto owner=f.world.owner();double elapsed=0,wall=200,paused_wall=0;uint64_t tick=100;
        session.levels(false,false);check(session.prepare(World::witness,1,owner,{1,tick,wall,.1,1},true)==0,"Acquire return differs");
        auto start=session.publication(99,wall);check(start.prompt.active&&start.enabled&&transport::valid(start),"Publication not usable");
        transport::Publication channel;transport::publish(channel,start);transport::Payload copy;
        check(transport::read(channel,copy)&&copy.prompt.serial==start.prompt.serial,"Transport changed serial");
        const double click_at=success?start.prompt.ideal:.05;bool clicked=false;
        for(unsigned frame=1;frame<600&&session.occupied();++frame){
            wall=200+double(frame)/120.;
            bool held_pause=pause&&frame>=20&&frame<80;
            if(held_pause)paused_wall+=1./120.;
            elapsed=std::max(0.,double(frame)/120.-paused_wall);tick=100+uint64_t(std::floor(elapsed/.1+1e-8));
            ClockFrame clock{1,tick,wall,.1,held_pause?0.:1.};session.poll_clock(clock);
            auto value=session.publication(99,wall);Snapshot shown;double projected{};
            check(transport::presentation(value,wall,shown,&projected),"Current active publication vanished");
            if(held_pause){check(shown.paused,"Paused circle not marked");Click click;
                check(transport::click(value,wall,true,true,true,click)&&click.paused,"Paused click not tagged");
                session.enqueue(click);session.levels(false,false);
            }else if(!clicked&&(success?in_success_zone(shown):shown.elapsed>=click_at)){Click click;
                check(transport::click(value,wall,success?attack:!attack,success?!attack:attack,true,click),"Visible click not timestamped");
                session.enqueue(click);session.levels(success?attack:!attack,success?!attack:attack);clicked=true;
            }
            // Once admitted, the full-encounter gate denies NEW prompts.
            // This must not block the existing prompt's native replay.
            session.prepare(World::witness,1,owner,clock,true,false);
        }
        check(clicked&&!session.occupied()&&f.world.rolls==1&&f.world.prepares==1,"Held pair stuck or native RNG duplicated");
        check(f.world.committed.outcome==((attack==success)?5u:0u),"Native QTE outcome differs");
        check(session.publication(99,wall).error==0,"Session error after valid timing");++cases;
        // PairGate clears/cancels its UI event after successful native replay.
        // Its default Canceled publication is not a canceled live encounter.
        auto cleared=session.publication(99,wall);
        check(!cleared.occupied&&!cleared.prompt.active&&cleared.prompt.result==Result::Canceled,"Fixture did not reach real post-replay publication");
        for(uint32_t phase:{uint32_t(melee60::Aligning),uint32_t(melee60::NativeCombat)}){
            for(unsigned repeat=0;repeat<20;++repeat){
                auto next=melee60::preparation_phase(phase,cleared.prompt.result==Result::Canceled,
                    session.occupied()&&cleared.prompt.serial==start.prompt.serial);
                check(next==phase,"Cleared resolved prompt canceled native encounter alignment/playback");
            }
        }
    }
    {SessionFixture f;f.world.setup(true,5);Session s(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true);f.session=&s;
     auto owner=f.world.owner();s.prepare(World::witness,1,owner,{1,0,200,.1,1},true);s.release();owner.verified_melee=false;
     auto canceled=s.publication(99,200);
     check(canceled.occupied&&canceled.prompt.result==Result::Canceled&&
        melee60::preparation_phase(melee60::Aligning,true,s.occupied())==melee60::Off,"Explicit RTS cancellation failed to release commitment");
     s.prepare(World::witness,1,owner,{1,1,200.1,.1,1},true);
     check(!s.occupied()&&f.world.rolls==1&&f.world.committed.outcome==5,"F10 did not replay stock once");}
    // Every legal generated timing window must survive publication validation,
    // including extreme wavering/exhaustion and both native-score extremes.
    {const uint32_t known=Required|Strength|Environment|EffectiveCombat;
     for(uint32_t morale=0;morale<=7;++morale)for(uint32_t fatigue=0;fatigue<=5;++fatigue){
         for(float native_score:{-40.f,0.f,40.f}){
             UnitState own{},enemy{};own.known=enemy.known=known;
             own.morale=morale;own.fatigue=fatigue;own.combat_score=native_score;
             enemy.morale=0;enemy.fatigue=0;enemy.combat_score=0;
             Event generated;Pair pair{1,1,1,2,1,2};
             check(generated.begin(pair,Action::Defend,own,enemy),"Generated native context rejected");
             transport::Payload data{};data.pid=99;data.enabled=1;data.occupied=1;
             data.lease=1;data.published_wall=200;
             data.anchor={1,200,0,.1,1,true,false};
             data.prompt.active=true;data.prompt.serial=1;data.prompt.opponent_unit=2;
             data.prompt.action=Action::Defend;data.prompt.result=Result::Pending;
             data.prompt=generated.snapshot();
             check(transport::valid(data),"Generated QTE impossible to present");++cases;
         }
     }}
    // READ ONLY mapping: the reader must not perform an atomic write to fetch
    // the sequence. This catches the old access-violation class of HUD bugs.
    {HANDLE map=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(transport::Publication),nullptr);
     check(map!=nullptr,"Fixture mapping failed");auto w=static_cast<transport::Publication*>(MapViewOfFile(map,FILE_MAP_WRITE,0,0,0));
     auto r=static_cast<const transport::Publication*>(MapViewOfFile(map,FILE_MAP_READ,0,0,0));
     new(w) transport::Publication;transport::Payload data;data.pid=99;data.published_wall=200;transport::publish(*w,data);
     transport::Payload read;check(transport::read(*r,read)&&read.pid==99,"Read-only mapping failed");
     w->sequence=1;check(!transport::read(*r,read),"Torn publication accepted");
     UnmapViewOfFile(r);UnmapViewOfFile(w);CloseHandle(map);}
    for(uint32_t original:{1u,2u,3u,4u}){
        SessionFixture f;f.world.setup(true,original);
        Session s(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true,true);f.session=&s;
        s.prepare(World::witness,1,f.world.owner(),{1,0,200,.1,1},true);
        check(!s.occupied()&&f.world.rolls==1&&f.world.committed.outcome==original,"Special stock outcome overridden");
    }
    {SessionFixture f;f.world.setup(true,5);f.world.put(World::enemy+0x21c,0x12345678u);
     Session s(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true,true);f.session=&s;
     s.prepare(World::witness,1,f.world.owner(),{1,0,200,.1,1},true);
     check(!s.occupied()&&f.world.rolls==1&&f.world.committed.outcome==5,"Mounted enemy acquired infantry QTE");}
    std::cout<<"PASS: "<<cases<<" complete session/transport attack-defend timing scenarios, 640 post-replay publication observations preserve alignment/playback, explicit RTS cancellation releases control, full-encounter admission through removal, failed-start retries, lease/battle reset, pause/resume, read-only mapping, exact selector replay, RTS stock restoration. Native calls are fixtures.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
#endif
