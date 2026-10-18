// Native combat callbacks can arrive late within a model tick. Read-only clock
// polls run between them. Test the entire Session/publication path, not just
// Timeline in isolation: the old coordinator retains an expired render horizon.
#define QTE_SESSION_FIXTURE_ONLY
#include "verify_session.cpp"
#undef QTE_SESSION_FIXTURE_ONLY
#include "qte_presentation93.h"
int main(){try{using namespace qte;unsigned cases=0;double worst_hold=0,worst_repeat=0;
 for(double speed:{1.,.5,2.})for(double phase:{.08,.05,.02})for(unsigned hz:{120u,60u,144u}){
  SessionFixture f;f.world.setup(true,5);Session session(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true,false,false,false,true);f.session=&session;auto owner=f.world.owner();
  bool started=false;double next_native=2.+phase,previous=-1,changed=0,max_hold=0;unsigned repeated=0,total=0,preview_rolls=0;
  for(unsigned frame=0;frame<unsigned(4.*hz/speed);++frame){double wall=100.+double(frame)/hz,sim=(wall-100.)*speed;ClockFrame clock{1,uint64_t(std::floor(sim/.1+1e-7)),wall,.1,speed};
   session.poll_clock(clock);
   if(sim+1e-8>=next_native){session.prepare(World::witness,1,owner,clock,true,!started);if(!started)preview_rolls=f.world.rolls;started=true;next_native+=.1;}
   if(!started)continue;auto payload=session.publication(99,wall);Snapshot shown;check(transport::presentation(payload,wall,shown),"Current publication vanished");
   check(payload.anchor.simulation<=payload.anchor.horizon+1e-8,"Publication exceeded native horizon");
   if(sim>2.3&&shown.result==Result::Pending){++total;
    if(previous>=0&&std::abs(previous-shown.response_elapsed)<1e-6){++repeated;max_hold=std::max(max_hold,wall-changed);}else changed=wall;
    check(shown.response_elapsed+1e-6>=previous,"Projected marker moved backwards between native callbacks");
    previous=shown.response_elapsed;
   }
   check(f.world.prepares==0&&f.world.rolls==preview_rolls,"Read-only timing polls committed native animation or RNG");
  }
  double ratio=double(repeated)/std::max(1u,total);worst_hold=std::max(worst_hold,max_hold);worst_repeat=std::max(worst_repeat,ratio);
  std::cout<<"PUBLICATION107 speed="<<speed<<" phase="<<phase<<" hz="<<hz<<" repeat_fraction="<<ratio<<" max_hold_ms="<<max_hold*1000<<'\n';
  check(ratio<.15&&max_hold<.035/speed,"Late native callbacks freeze the marker despite fresh clock polls");++cases;
 }
 {SessionFixture f;f.world.setup(true,5);Session s(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true,false,false,false,true);f.session=&s;auto owner=f.world.owner();
  s.prepare(World::witness,1,owner,{1,0,100,.1,1},true);s.poll_clock({1,0,100.04,.1,1});auto before=s.publication(99,100.04);Snapshot shown;check(transport::presentation(before,100.04,shown),"Pre-pause projection");double elapsed=shown.response_elapsed;
  for(unsigned n=0;n<100;++n){double wall=100.05+n*.1;s.poll_clock({1,0,wall,.1,0});auto p=s.publication(99,wall);check(transport::presentation(p,wall,shown)&&shown.paused&&std::abs(shown.response_elapsed-elapsed)<1e-6,"Pause changed displayed age");
   Click c;Frame93 image{shown,1,owner.lease,wall};check(frame_click93(p,image,wall,true,true,true,c)&&c.paused&&!c.visible93,"Pause judged a button");s.enqueue(c);s.prepare(World::witness,1,owner,{1,0,wall,.1,0},true,false);
   auto native_pause=s.publication(99,wall);check(transport::presentation(native_pause,wall,shown)&&std::abs(shown.response_elapsed-elapsed)<1e-6,"Paused native callback discarded pre-pause display time");
  }
  s.poll_clock({1,0,110.05,.1,1});auto resumed=s.publication(99,110.05);check(transport::presentation(resumed,110.05,shown)&&!shown.paused&&std::abs(shown.response_elapsed-elapsed)<1e-6,"Resume charged paused wall time or moved backwards");
  s.prepare(World::witness,1,owner,{1,0,110.05,.1,1},true,false);auto committed=s.publication(99,110.05);check(std::abs(committed.prompt.response_elapsed-elapsed)<1e-6&&committed.error==0,"Native resume clock differs from pre-pause projection");++cases;
 }
 {SessionFixture f;f.world.setup(true,5);Session s(f.world.reader(),f.world.selection(),f.calls(),World::engine,{0,5,true},true,false,false,false,true);f.session=&s;auto owner=f.world.owner();s.prepare(World::witness,1,owner,{1,0,200,.1,1},true);
  auto initial=s.publication(99,200);double deadline=response_deadline(initial.prompt);s.poll_clock({1,uint64_t(std::floor(deadline/.1)),200+deadline,.1,1});auto p=s.publication(99,200+deadline);Snapshot shown;Click c;
  check(transport::presentation(p,200+deadline,shown)&&shown.result==Result::Timeout&&std::abs(shown.response_elapsed-deadline)<1e-6,"Current-clock projection changed precise expiry");check(!transport::click(p,200+deadline,true,false,true,c),"Expired projected event accepted input");check(f.world.prepares==0,"UI clock committed timeout animation");
  s.prepare(World::witness,1,owner,{1,uint64_t(std::floor(deadline/.1)),200+deadline,.1,1},true,false);check(f.world.prepares==1&&!s.occupied()&&s.publication(99,200+deadline).error==0,"Native callback did not resolve exact timeout once");++cases;
 }
 std::cout<<"PASS: "<<cases<<" production Session/publication cases; worst repeat="<<worst_repeat<<" hold_ms="<<worst_hold*1000<<"; native horizon, pause/resume with100 paused callbacks/levels, precise expiry, monotonic projection and no render-thread native commitment.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
