#include "qte_core.h"
#include <iostream>
#include <stdexcept>
static void check(bool b,const char* message){if(!b)throw std::runtime_error(message);}
int main(){try{
    using namespace qte;
    UnitState balanced{Required|Strength|Environment,2,2,12,.5f,.8f,false};
    UnitState strong{Required|Strength,0,0,14,1,1,false},weak{Required|Strength,5,7,10,0,.3f,false};
    Difficulty d,e,h;check(difficulty(balanced,balanced,d)&&difficulty(strong,weak,e)&&difficulty(weak,strong,h),"Valid unit contexts rejected");
    check(e.advantage>0&&h.advantage<0&&e.duration>d.duration&&d.duration>h.duration&&e.tolerance>d.tolerance&&d.tolerance>h.tolerance,"Relative advantage did not make QTE slower/easier");
    for(unsigned factor:{Fatigue,Morale,Elevation,General,Strength,Environment}){
        auto better=balanced,worse=balanced;
        if(factor==Fatigue){better.fatigue=0;worse.fatigue=5;}if(factor==Morale){better.morale=0;worse.morale=7;}
        if(factor==Elevation){better.height+=2;worse.height-=2;}if(factor==General){better.general_support=1;worse.general_support=0;}
        if(factor==Strength){better.strength=1;worse.strength=.3f;}
        if(factor==Environment){better.environment_support=1;worse.environment_support=-1;}
        Difficulty a,b;check(difficulty(better,worse,a)&&difficulty(worse,better,b)&&a.duration>b.duration&&a.tolerance>b.tolerance,"A required relative factor did not affect timing");
    }
    Pair p{1,9,11,12,3,4};unsigned cases=0;
    auto zero_serial=p;zero_serial.player=0;Event zero_event;
    check(zero_event.begin(zero_serial,Action::Attack,balanced,balanced),"Valid native actor serial zero rejected");
    for(Action action:{Action::Attack,Action::Defend})for(unsigned i=0;i<10000;++i){
        Event event;check(event.begin(p,action,balanced,balanced),"Begin failed");auto s=event.snapshot();
        check(event.update(p,balanced,balanced,s.ideal,true)&&event.snapshot().elapsed==0,"Pause advanced time");
        check(event.press(s.serial,action==Action::Attack?Button::Left:Button::Right,true)==Result::Pending,"Paused press accepted");
        check(event.update(p,balanced,balanced,s.ideal,false),"Advance failed");
        check(std::abs(closing_radius(event.snapshot())-32)<.001f,"Closing ring does not reach target at ideal time");
        check(event.press(s.serial+1,Button::Left,true)==Result::Pending,"Stale event input accepted");
        auto expected=action==Action::Attack?Button::Left:Button::Right;
        check(event.press(s.serial,expected,false)==Result::Pending,"Held button accepted as new click");
        check(event.press(s.serial,expected,true)==Result::Success,"Correct timed input failed");
        check(event.press(s.serial,expected==Button::Left?Button::Right:Button::Left,true)==Result::Success,"Result overwritten by another click");
        Result result;check(event.consume(p,result)&&result==Result::Success&&!event.consume(p,result),"Native outcome was not exactly once");++cases;
    }
    for(Result expected:{Result::Early,Result::Late,Result::WrongButton}){
        Event event;check(event.begin(p,Action::Attack,balanced,balanced,.5f),"Begin failed");auto s=event.snapshot();
        if(expected==Result::Late)event.update(p,balanced,balanced,s.duration,false);
        if(expected==Result::Timeout)event.update(p,balanced,balanced,s.duration+.1,false);
        Result got=expected==Result::Timeout?event.snapshot().result:event.press(s.serial,expected==Result::WrongButton?Button::Right:Button::Left,true);
        check(got==expected,"Failure classification incorrect");
    }
    Event dynamic;check(dynamic.begin(p,Action::Defend,balanced,balanced),"Dynamic begin");dynamic.update(p,balanced,balanced,.5,false);auto before=dynamic.snapshot();
    dynamic.update(p,strong,weak,0,false);auto after=dynamic.snapshot();check(after.duration>before.duration&&std::abs(before.elapsed/before.duration-after.elapsed/after.duration)<.00001f,"Dynamic context jumped circle phase");
    auto changed=p;changed.lease++;check(!dynamic.update(changed,strong,weak,.1,false)&&!dynamic.snapshot().active,"Old possession pair retained");
    for(uint32_t outcome=0;outcome<=5;++outcome){
        uint32_t mapped=99;OutcomeBindings fixture{0,4,true}; // synthetic codes ONLY
        check(native_outcome(Action::Attack,Result::Success,outcome,fixture,mapped)&&mapped==4&&native_outcome(Action::Attack,Result::Early,outcome,fixture,mapped)&&mapped==0,"Attack success/failure intent wrong");
        check(native_outcome(Action::Defend,Result::Success,outcome,fixture,mapped)&&mapped==0&&native_outcome(Action::Defend,Result::Timeout,outcome,fixture,mapped)&&mapped==4,"Defense success/failure intent wrong");
        check(native_outcome(Action::Attack,Result::Canceled,outcome,{},mapped)&&mapped==outcome,"Cancellation changed stock combat");
        mapped=99;check(!native_outcome(Action::Defend,Result::Late,outcome,{},mapped)&&mapped==99,"Unknown result binding changed native outcome");
    }
    for(unsigned missing:{Fatigue,Morale,Elevation,General}){auto invalid=balanced;invalid.known&=~missing;Event event;check(!event.begin(p,Action::Attack,invalid,balanced),"Missing native context fabricated");}
    auto routing=balanced;routing.routing=true;Event event;check(!event.begin(p,Action::Attack,routing,balanced),"Routing unit acquired");
    std::cout<<"PASS: "<<cases<<" timed attack/defend events; legacy modifier regression; dynamic meter phase continuity; paused/stale/held input rejection; failure states; exactly-once native verdict consumption; original-role/outcome preservation; missing context and routing guards.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
