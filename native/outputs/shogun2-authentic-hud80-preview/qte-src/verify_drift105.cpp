#include "qte_timeline.h"
#include <iostream>
#include <stdexcept>
static void need105(bool value,const char* message){if(!value)throw std::runtime_error(message);}
int main(){try{using namespace qte;unsigned cases=0;double worst=0;
 // A model tick arriving 0.5 ms late accumulates a full prediction step in
 // twenty seconds if prediction stays anchored to the beginning of battle.
 // A fast renderer can then submit an unchanged marker on almost every frame.
 for(double rate:{.5,1.,2.})for(unsigned hz:{60u,120u,144u,240u}){
  Timeline t;double previous=0;unsigned capped=0,total=0;
  const double interval=.1005/rate;
  for(unsigned frame=0;frame<hz*120;++frame){
   double wall=double(frame)/hz;uint64_t tick=uint64_t(std::floor(wall/interval+1e-8));
   auto p=t.observe({1,tick,wall,.1,rate});double floor=double(tick)*.1;
   need105(p.valid&&p.simulation+1e-8>=previous&&p.simulation+1e-8>=floor&&p.simulation<=p.horizon+1e-8,"Drift violated native monotonic horizon");
   auto displayed=interpolate(p,wall+.001);
   need105(displayed>=p.simulation&&displayed<=p.horizon+1e-8,"Display/click projection exceeded native horizon");
   if(wall>30){++total;if(p.horizon-displayed<1e-6)++capped;}
   previous=p.simulation;
  }
  double ratio=double(capped)/total;worst=std::max(worst,ratio);
  std::cout<<"DRIFT105 rate="<<rate<<" hz="<<hz<<" capped_fraction="<<ratio<<'\n';
  need105(ratio<.12,"Accumulated clock drift froze the marker at the native horizon");++cases;
 }
 // Pause, resume, skipped native ticks and rate changes must retain authority
 // and never charge a pause or move a click backwards.
 Timeline t;auto p=t.observe({2,0,100,.1,1});p=t.observe({2,1,100.11,.1,1});
 auto paused=t.observe({2,1,100.12,.1,0});double frozen=paused.simulation;
 need105(t.observe({2,1,200,.1,0}).simulation==frozen,"Drift correction charged paused wall time");
 auto resumed=t.observe({2,1,200,.1,1});need105(resumed.simulation==frozen,"Resume moved marker");
 auto skipped=t.observe({2,4,200.1,.1,1});need105(skipped.valid&&skipped.simulation>=.4&&skipped.simulation<=.5,"Skipped native ticks lost authority");
 auto fast=t.observe({2,4,200.1,.1,2});need105(fast.simulation==skipped.simulation,"Rate transition jumped marker");
 need105(!t.observe({2,4,201,.1,2}).valid,"True native stall forecast forever");
 std::cout<<"PASS: "<<cases<<" two-minute model-drift/refresh scenarios; worst capped fraction="<<worst<<"; native horizon, monotonic display/click projection, pause, resume, speed, skipped ticks and stall authority retained. No scanout measurement.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
