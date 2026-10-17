#include "qte_transport.h"
#include <iostream>
#include <stdexcept>
static void check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
static bool approximately(double a,double b){return std::abs(a-b)<2e-6;}
int main(){try{using namespace qte;
    const Pair pair{1,1,0,2,1,2};
    UnitState player{Required|Strength|Environment|EffectiveCombat,2,2,10,.5f,.8f,false,0,0},enemy=player;
    MeterDifficulty base;check(meter_difficulty(player,enemy,base),"Base facts rejected");
    for(unsigned i=0;i<1000;++i){
        auto own=player,other=enemy;own.combat_score=-40.f+float(i)*.08f;
        MeterDifficulty changed;check(meter_difficulty(own,enemy,changed)&&changed.width==base.width,"Player score resized enemy zone");
        other.combat_score=own.combat_score;
        check(meter_difficulty(player,other,changed)&&changed.player.duration==base.player.duration,"Enemy score changed player speed");
        own=player;own.morale=i%8;own.fatigue=i%6;own.general_support=float(i%101)/100;own.strength=float(i%101)/100;own.environment_support=float(i%201)/100-1;
        check(meter_difficulty(own,enemy,changed)&&changed.width==base.width,"Player unit facts leaked into width");
        other=own;check(meter_difficulty(player,other,changed)&&changed.player.duration==base.player.duration,"Enemy unit facts leaked into speed");
    }
    // Preserve every existing fallback factor and effective-score substitution.
    for(unsigned fact:{Fatigue,Morale,General,Strength,Environment,EffectiveCombat}){
        auto worse=player,better=player;
        if(fact!=EffectiveCombat)worse.known=better.known=Required|Strength|Environment;
        if(fact==Fatigue){worse.fatigue=5;better.fatigue=0;}
        if(fact==Morale){worse.morale=5;better.morale=0;}
        if(fact==General){worse.general_support=0;better.general_support=1;}
        if(fact==Strength){worse.strength=.1f;better.strength=1;}
        if(fact==Environment){worse.environment_support=-1;better.environment_support=1;}
        if(fact==EffectiveCombat){worse.combat_score=-30;better.combat_score=30;}
        MeterDifficulty easy,hard;
        check(meter_difficulty(better,worse,easy)&&meter_difficulty(worse,better,hard),"Modifier facts rejected");
        check(easy.player.duration>hard.player.duration&&easy.width>hard.width,"Existing factor did not preserve difficulty direction");
    }
    auto own=player,other=enemy;own.known=other.known=Required|Strength|Environment;
    own.height+=2;MeterDifficulty uphill,downhill;
    check(meter_difficulty(own,other,uphill)&&meter_difficulty(other,own,downhill)&&uphill.player.duration>downhill.player.duration&&uphill.width>downhill.width,"Fallback elevation lost");
    other=enemy;other.routing=true;MeterDifficulty routed,steady;
    check(meter_difficulty(player,other,routed)&&meter_difficulty(player,enemy,steady)&&routed.width>steady.width&&routed.player.duration==steady.player.duration,"Enemy routing affected wrong property");
    unsigned loops=0;float first=-1;bool varied=false;
    for(unsigned i=0;i<1000;++i){Event random;check(random.begin(pair,Action::Attack,player,enemy),"Random begin failed");auto s=random.snapshot();
        if(first<0)first=s.placement;else varied|=s.placement!=first;
        check(s.duration==base.player.duration&&s.zone_width==base.width,"Entropy changed difficulty");}
    check(varied,"Zone did not randomize");
    for(float placement:{0.f,.01f,.25f,.5f,.75f,.99f,1.f})for(unsigned morale=0;morale<8;++morale)for(unsigned fatigue=0;fatigue<6;++fatigue){
        own=player;own.morale=morale;own.fatigue=fatigue;
        for(float score:{-40.f,0.f,40.f}){
            other=enemy;other.combat_score=score;Event event;
            check(event.begin(pair,Action::Defend,own,other,placement),"Extreme begin failed");auto start=event.snapshot();
            check(zone_left(start)>=0&&zone_left(start)+start.zone_width<=1.0000001f,"Zone outside meter");
            check(event.update(pair,own,other,std::min(double(start.duration)*.25,double(start.ideal)*.5),false)&&event.snapshot().result==Result::Pending,"First-pass opportunity vanished");
            auto s=event.snapshot();const double return_center=2.*s.duration-s.ideal;
            const double target=return_center<response_deadline(s)?return_center:s.ideal;
            // Seek from current bounded phase to the return-leg center.
            double dt=target-s.elapsed;if(dt<0)dt+=2.*s.duration;
            check(event.update(pair,own,other,dt,false)&&in_success_zone(event.snapshot()),"Reachable pass centre missed");
            check(event.press(s.serial,Button::Right,true)==Result::Success,"Reachable pass input not successful");
            const auto stopped=event.snapshot();event.update(pair,own,other,100,false);
            check(event.snapshot().elapsed==stopped.elapsed&&event.snapshot().result==Result::Success,"Resolved reticle moved");
            ++loops;
        }
    }
    // State refresh preserves phase/direction and fixed placement; pause freezes.
    Event dynamic;dynamic.begin(pair,Action::Attack,player,enemy,.3f);auto s=dynamic.snapshot();
    dynamic.update(pair,player,enemy,s.duration*1.25,false);auto before=dynamic.snapshot();own=player;own.morale=5;other=enemy;other.combat_score=30;
    dynamic.update(pair,own,other,0,false);auto after=dynamic.snapshot();
    check(approximately(reticle_position(before),reticle_position(after))&&after.placement==before.placement&&after.duration<before.duration&&after.zone_width<before.zone_width,"Refresh jumped phase or rerolled placement");
    dynamic.update(pair,own,other,100,true);check(dynamic.snapshot().elapsed==after.elapsed,"Pause advanced reticle");
    check(dynamic.press(after.serial,Button::Left,true)==Result::Pending,"Paused click accepted");
    // Display projection and timestamped evaluation must agree across bounces,
    // every placement and difficulty, not just at one scripted ideal instant.
    unsigned synchronized=0;
    for(float placement:{0.f,.2f,.5f,.8f,1.f})for(unsigned morale:{0u,4u,5u,7u})for(unsigned frame=0;frame<1200;++frame){
        own=player;own.morale=morale;Event event;event.begin(pair,Action::Attack,own,enemy,placement);
        double now=double(frame)/120.;event.update(pair,own,enemy,now,false);
        transport::Payload data{};data.pid=1;data.enabled=data.occupied=1;data.lease=1;data.published_wall=100;data.prompt=event.snapshot();
        data.anchor={1,100,now,now+.1,1,true,false};Snapshot shown;Click click;
        check(transport::presentation(data,100.007,shown),"Meter projection unavailable");
        if(shown.result==Result::Timeout){check(!transport::click(data,100.007,true,false,true,click),"Expired projected meter accepted a click");continue;}
        check(transport::click(data,100.007,true,false,true,click),"Unexpired meter click unavailable");
        const bool success=in_success_zone(shown);event.update(pair,own,enemy,click.simulation-now,false);
        check(approximately(reticle_position(shown),reticle_position(event.snapshot())),"Rendered/input reticle positions diverged");
        check((event.press(shown.serial,Button::Left,true)==Result::Success)==success,"Rendered zone and input result diverged");++synchronized;
    }
    Snapshot boundary{};boundary.duration=1;boundary.zone_width=.5f;boundary.placement=.5f;
    boundary.elapsed=.25f;check(in_success_zone(boundary),"Left boundary excluded");boundary.elapsed=.75f;check(in_success_zone(boundary),"Right boundary excluded");
    boundary.elapsed=1.25f;check(in_success_zone(boundary),"Return right boundary excluded");boundary.elapsed=1.75f;check(in_success_zone(boundary),"Return left boundary excluded");
    std::cout<<"PASS: independent contextual player-speed/enemy-width mappings, existing modifiers, entropy isolated to placement, "<<loops<<" repeating/extreme-state events, "<<synchronized<<" display/click comparisons through bounces, pause, phase refresh, fixed outcome and inclusive boundaries. No game attached.\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
