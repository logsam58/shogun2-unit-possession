#include "qte_core.h"
#include "qte_transport.h"
#include <iostream>
#include <stdexcept>
static void check(bool value,const char* message){if(!value)throw std::runtime_error(message);}
int main(){try{using namespace qte;unsigned cases=0;
 Pair pair{1,9,0,2,1,2};UnitState own{Required|Strength|Environment|EffectiveCombat,0,0,10,1,1,false,1,80},enemy=own;
 for(unsigned hz:{30u,60u,75u,120u,144u,165u,240u,360u})for(auto action:{Action::Attack,Action::Defend,Action::Charge})for(unsigned morale:{0u,4u,7u})for(unsigned fatigue:{0u,5u})for(float score:{-80.f,0.f,80.f}){
  auto player=own;player.morale=morale;player.fatigue=fatigue;player.combat_score=score;Event e;check(e.begin(pair,action,player,enemy,.5f),"Admission");auto start=e.snapshot();double expected=std::max(3.,2.*double(start.duration));
  unsigned count=0;while(e.snapshot().result==Result::Pending&&count++<2000){auto before=e.snapshot();e.update(pair,player,enemy,1./hz,false);auto s=e.snapshot();
   if(s.result==Result::Timeout)check(s.response_elapsed>=3.-1e-8&&s.elapsed+1e-5>=2.*s.duration,"Timed out before time/cycle minimum");
   else check(before.response_elapsed<expected+1e-5,"Pending forever");
  }
  auto s=e.snapshot();check(s.result==Result::Timeout&&std::abs(s.response_elapsed-expected)<1e-5&&response_remaining(s)==0,"Exact combined minimum expiry");check(e.press(s.serial,attacking(action)?Button::Left:Button::Right,true)==Result::Timeout,"Expired revival");++cases;
 }
 // A slow player keeps the full native-context speed, even beyond three seconds.
 Event slow;check(slow.begin(pair,Action::Attack,own,enemy,.9f),"Slow admission");auto initial=slow.snapshot();check(initial.duration>1.5f,"Slow fixture must extend budget");slow.update(pair,own,enemy,3.001,false);auto pending=slow.snapshot();check(pending.result==Result::Pending&&pending.response_elapsed>3&&response_remaining(pending)>0,"Three-second cap still cuts slow cycle");
 slow.update(pair,own,enemy,90,true);check(slow.snapshot().elapsed==pending.elapsed&&slow.snapshot().response_elapsed==pending.response_elapsed,"Pause charged minimum cycle");
 transport::Payload data{};data.pid=1;data.enabled=data.occupied=1;data.lease=9;data.prompt=pending;data.published_wall=100;data.anchor={1,100,3.001,3.101,1,true,false};Snapshot shown;Click click;
 check(transport::valid(data)&&transport::presentation(data,100.01,shown)&&shown.result==Result::Pending&&transport::click(data,100.01,true,false,true,click),"After-three-second display/input rejected before full cycle");
 slow.update(pair,own,enemy,response_remaining(pending)+.01,false);auto expired=slow.snapshot();data.prompt=expired;check(!transport::click(data,100.01,true,false,true,click),"Projected expired input revived");++cases;
 // Alternating speed refresh preserves cumulative phase, active time and pause.
 for(unsigned hz:{30u,120u,360u})for(unsigned pattern=0;pattern<20;++pattern){Event e;e.begin(pair,Action::Defend,own,enemy,.5f);unsigned frame=0;double active=0;
  while(e.snapshot().result==Result::Pending&&frame++<2000){auto before=e.snapshot();auto state=own;state.combat_score=((frame/((pattern+1)*3))%2)?-80.f:80.f;e.update(pair,state,enemy,0,false);auto refreshed=e.snapshot();check(std::abs(before.elapsed/before.duration-refreshed.elapsed/refreshed.duration)<2e-5&&before.response_elapsed==refreshed.response_elapsed,"Refresh restarted cycle/age");
   e.update(pair,state,enemy,10,true);check(e.snapshot().response_elapsed==before.response_elapsed,"Paused dynamic event advanced");
   e.update(pair,state,enemy,1./hz,false);active+=1./hz;
  }
  auto s=e.snapshot();check(s.result==Result::Timeout&&s.response_elapsed>=3.-1e-8&&s.elapsed+1e-5>=2.*s.duration&&active<=MaximumResponseSeconds95+.04,"Dynamic speed failed bounded complete cycle");++cases;
 }
 std::cout<<"PASS: Cycle95 "<<cases<<" fixed/dynamic/pause cases across30-360Hz; at least3active seconds and one outward-return cycle, slow after3s click/display admission, precise expiry, immutable outcomes and no speed reset. Numeric ABI unchanged.\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
