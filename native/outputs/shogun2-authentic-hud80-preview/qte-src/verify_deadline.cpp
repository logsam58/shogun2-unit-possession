#define QTE_SESSION_FIXTURE_ONLY
#include "verify_session.cpp"
#undef QTE_SESSION_FIXTURE_ONLY
int main(){try{using namespace qte;unsigned cases=0;
    Pair pair{1,9,0,2,1,2};UnitState player{Required|Strength|Environment|EffectiveCombat,0,0,10,.5f,.8f,false,0,0},enemy=player;
    for(auto action:{Action::Attack,Action::Defend,Action::Charge})for(float placement:{0.f,.5f,1.f})for(unsigned morale:{0u,4u,5u,7u})for(unsigned fatigue:{0u,5u}){
        auto own=player;own.morale=morale;own.fatigue=fatigue;Event event;
        check(event.begin(pair,action,own,enemy,placement),"Deadline event rejected");auto s=event.snapshot();double deadline=response_deadline(s);
        check(deadline==std::max(3.,2.*double(s.duration)),"Deadline must meet minimum time and cycle");
        check(event.update(pair,own,enemy,deadline-.0001,false)&&event.snapshot().result==Result::Pending,"Event expired before its time budget");
        auto paused=event.snapshot();event.update(pair,own,enemy,100,true);check(event.snapshot().elapsed==paused.elapsed&&event.snapshot().result==Result::Pending,"Pause consumed response budget");
        event.update(pair,own,enemy,.0002,false);check(event.snapshot().result==Result::Timeout&&response_remaining(event.snapshot())==0,"Unanswered event exceeded its time budget");
        auto button=attacking(action)?Button::Left:Button::Right;check(event.press(s.serial,button,true)==Result::Timeout,"Post-deadline click overwrote failure");
        uint32_t outcome=99;check(native_outcome(action,Result::Timeout,5,{0,5,true},outcome)&&outcome==(attacking(action)?0u:5u),"Timeout mapped to wrong native failure");
        Result result;check(event.consume(pair,result)&&result==Result::Timeout&&!event.consume(pair,result),"Timeout consumed more than once");++cases;
    }
    Event dynamic;dynamic.begin(pair,Action::Attack,player,enemy,.5f);auto start=dynamic.snapshot();dynamic.update(pair,player,enemy,start.duration*1.8,false);
    auto before=dynamic.snapshot();auto own=player;own.morale=5;dynamic.update(pair,own,enemy,0,false);auto after=dynamic.snapshot();
    check(std::abs(before.elapsed/before.duration-after.elapsed/after.duration)<.00001f&&after.duration<before.duration,"State refresh reset deadline progress");
    check(after.response_elapsed==before.response_elapsed&&response_remaining(after)>=3.-after.response_elapsed-1e-6,"State refresh changed consumed response time");
    dynamic.update(pair,own,enemy,.01,false);check(dynamic.snapshot().result==Result::Pending,"Faster motion ended a timed event early");
    dynamic.update(pair,own,enemy,response_remaining(dynamic.snapshot())+.001,false);check(dynamic.snapshot().result==Result::Timeout,"Timed event failed to expire");
    for(bool attack:{false,true})for(bool pause:{false,true}){
        SessionFixture f;f.world.setup(attack,5);Session s(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true);f.session=&s;auto owner=f.world.owner();
        s.prepare(World::witness,1,owner,{1,0,200,.1,1},true);auto initial=s.publication(99,200);double limit=response_deadline(initial.prompt);
        double elapsed=0,wall=200;uint64_t tick=0;bool paused_once=false;
        for(unsigned frame=1;frame<700&&s.occupied();++frame){
            if(pause&&!paused_once&&elapsed>.2){s.poll_clock({1,tick,wall,.1,0});s.prepare(World::witness,1,owner,{1,tick,wall+2,.1,0},true,false);wall+=2;paused_once=true;
                check(s.occupied()&&s.publication(99,wall).prompt.result==Result::Pending,"Paused no-input event failed");}
            elapsed+=1./120.;wall+=1./120.;tick=uint64_t(std::floor(elapsed/.1+1e-8));ClockFrame clock{1,tick,wall,.1,1};s.poll_clock(clock);
            auto data=s.publication(99,wall);Snapshot shown;check(transport::presentation(data,wall,shown),"Deadline presentation disappeared");
            if(shown.result==Result::Timeout){Click click;check(!transport::click(data,wall,attack,!attack,true,click),"Timed-out display accepted input");}
            s.prepare(World::witness,1,owner,clock,true,false);
        }
        check(!s.occupied()&&f.world.prepares==1&&f.world.rolls==1&&f.world.committed.outcome==(attack?0u:5u),"No-input timeout failed exactly-once native replay");
        check(elapsed>=limit-.11&&elapsed<limit+.21&&s.publication(99,wall).error==0,"Native deadline drifted beyond one clock horizon");++cases;
    }
    // Projection can expire before the next engine callback, but only that
    // callback commits native consequences. No expired input is enqueued.
    Event projected;projected.begin(pair,Action::Attack,player,enemy,.5f);auto p=projected.snapshot();double limit=response_deadline(p);
    projected.update(pair,player,enemy,limit-.01,false);transport::Payload data{};data.pid=1;data.enabled=data.occupied=1;data.lease=9;data.published_wall=100;data.prompt=projected.snapshot();data.anchor={1,100,1,1.1,1,true,false};
    Snapshot shown;check(transport::presentation(data,100.02,shown)&&shown.result==Result::Timeout&&response_remaining(shown)==0,"Display did not expire consistently");
    Click click;check(!transport::click(data,100.02,true,false,true,click),"Projection timeout produced click");projected.update(pair,player,enemy,.02,false);check(projected.snapshot().result==shown.result,"Display and engine timeout diverged");
    std::cout<<"PASS: "<<cases<<" finite deadline/extreme-state/native no-input replay cases; minimum-three-second/full-cycle response budget, real pause, phase-preserving refresh, projection/input expiry, immutable failure and exactly-once attack/defend/charge outcomes. Synthetic engine only.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
