#define QTE_SESSION_FIXTURE_ONLY
#include "verify_session.cpp"
#undef QTE_SESSION_FIXTURE_ONLY
#include <limits>
int main(){try{using namespace qte;unsigned cases=0;
 Pair pair{1,9,0,2,1,2};UnitState own{Required|Strength|Environment|EffectiveCombat,0,0,10,.5f,.8f,false,0,0},enemy=own;
 for(auto action:{Action::Attack,Action::Defend,Action::Charge})for(unsigned morale=0;morale<8;++morale)for(unsigned fatigue=0;fatigue<6;++fatigue)for(float score:{-40.f,0.f,40.f}){
  auto player=own;player.morale=morale;player.fatigue=fatigue;player.combat_score=score;
  Event e;check(e.begin(pair,action,player,enemy,.5f),"Timed event admission");
  e.update(pair,player,enemy,2.8,false);auto before=e.snapshot();
  check(before.result==Result::Pending&&std::abs(before.response_elapsed-2.8)<1e-10&&response_deadline(before)>=3.,"Fast or slow marker shortened response window");
  auto changed=player;changed.morale=(morale+3)%8;changed.fatigue=(fatigue+2)%6;changed.combat_score=-score;
  e.update(pair,changed,enemy,0,false);auto after=e.snapshot();
  check(after.response_elapsed==before.response_elapsed&&std::abs(reticle_position(before)-reticle_position(after))<2e-6,"Stat refresh reset age or jumped marker");
  e.update(pair,changed,enemy,100,true);check(e.snapshot().response_elapsed==after.response_elapsed,"Pause charged event time");
  e.update(pair,changed,enemy,.199,false);check(e.snapshot().result==Result::Pending,"Timed event ended early");
  e.update(pair,changed,enemy,.0011,false);
  if(e.snapshot().result==Result::Pending)e.update(pair,changed,enemy,10,false);
  auto expired=e.snapshot();
  check(expired.result==Result::Timeout&&expired.response_elapsed>=3.&&expired.elapsed/expired.duration>=2.-1e-5&&response_remaining(expired)<1e-5,"Timed event failed minimum time/cycle expiry");
  e.update(pair,own,enemy,10,false);check(e.snapshot().elapsed==expired.elapsed,"Expired marker moved");
  ++cases;
 }
 // The formerly shortest event now permits successful input after several full cycles.
 auto hard=own;hard.morale=7;hard.fatigue=5;hard.combat_score=-40;
 for(auto action:{Action::Attack,Action::Defend,Action::Charge}){
  Event e;e.begin(pair,action,hard,enemy,.5f);auto initial=e.snapshot();
  const double centre=6.*initial.duration+initial.ideal;
  check(centre<3.,"Fixture must reach a later cycle inside response window");
  e.update(pair,hard,enemy,centre,false);auto s=e.snapshot();
  check(s.elapsed>6*s.duration&&s.result==Result::Pending&&in_success_zone(s),"Later-cycle opportunity removed");
  check(e.press(s.serial,attacking(action)?Button::Left:Button::Right,true)==Result::Success,"Later-cycle successful click rejected");++cases;
 }
 // Display/queued input reject the exact deadline independently of cycle phase.
 Event e;e.begin(pair,Action::Attack,hard,enemy,.5f);e.update(pair,hard,enemy,2.99,false);
 transport::Payload payload{};payload.pid=1;payload.enabled=payload.occupied=1;payload.lease=9;payload.prompt=e.snapshot();payload.published_wall=100;payload.anchor={1,100,2.99,3.1,1,true,false};
 Snapshot shown;Click click;check(transport::presentation(payload,100.02,shown)&&shown.result==Result::Timeout&&shown.response_elapsed==3.,"Projected event missed time expiry");
 check(!transport::click(payload,100.02,true,false,true,click),"Projected deadline accepted click");
 payload.prompt.response_elapsed=std::numeric_limits<double>::quiet_NaN();check(!transport::valid(payload),"Nonfinite age accepted");
 payload.prompt=e.snapshot();payload.prompt.response_elapsed=-1;check(!transport::valid(payload),"Negative age accepted");
 payload.prompt.response_elapsed=3.1;check(!transport::valid(payload),"Over-budget age accepted");
 transport::Publication publication;transport::Payload output;publication.version=79;check(!transport::read(publication,output),"Old phase-budget publication accepted by Test91");
 std::cout<<"PASS: "<<cases<<" minimum-time/cycle state/action cases, later-cycle success, independent phase/age refresh, pause, precise expiry, frozen outcomes, display/input agreement and old-protocol rejection.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
