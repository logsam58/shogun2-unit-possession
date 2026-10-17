#include "identity.h"
#include "mounted_blend_clock88.h"
#include "mounted_settle_geometry88.h"
#include <limits>
int main(){try{
    mountedblend88::Key ck{1,2,3,4};mountedblend88::Clock clock;clock.begin(ck,100);
    for(unsigned i=0;i<1000;++i)clock.advance(ck,100,.1f);
    require(!clock.ready(ck)&&clock.elapsed==0,"Duplicate native updates aged the blend");
    for(unsigned i=101;i<=104;++i)clock.advance(ck,i,.1f);
    require(!clock.ready(ck),"Native blend completed before authored duration");
    for(unsigned i=105;i<=125;++i)clock.advance(ck,i,0);
    require(!clock.ready(ck),"Paused ticks completed the native blend");
    clock.advance(ck,126,.1f);require(clock.ready(ck),"Unique simulation updates did not finish native blend");
    auto other=ck;++other.draw;require(!clock.ready(other),"Blend readiness leaked to another draw");
    clock.begin(other,127);require(!clock.ready(other),"New draw inherited old blend age");
    clock.advance(other,130,.1f);require(clock.elapsed==0,"Missing ticks invented elapsed time");
    clock.advance(other,129,.1f);require(!clock.valid,"Backward native tick did not retire blend");
    clock.begin(ck,0xffffffffU);clock.advance(ck,0,.1f);require(std::abs(clock.elapsed-.1f)<.00001f,"Native tick wrap broke elapsed time");
    clock.advance(ck,0,std::numeric_limits<float>::quiet_NaN());require(!clock.valid,"Nonfinite native delta preserved stale readiness");

    heldpose::Cache cache;heldpose::Acquisition88 acquisition;heldpose::Key key{1,2,3,4,5};
    float m[48*12],before[48*12];int32_t parents[48];memcpy(parents,SEATED_BOW_PARENTS,sizeof parents);
    const unsigned weapons[3]={1,2,3};
    auto fixture=[&](float transient){memcpy(m,SEATED_BOW_MODEL,sizeof m);m[3*12+7]+=transient;memcpy(before,m,sizeof m);};
    auto step=[&](unsigned tick,bool blend,bool paused=false){return heldpose::apply_settled88(cache,acquisition,key,m,48,parents,7,weapons,blend,tick,paused);};
    fixture(1.2f);require(step(1,false)==heldpose::SettleResult::Waiting&&!cache.valid&&!memcmp(m,before,sizeof m),"Transient first-held pose was cached or modified");
    fixture(.8f);require(step(2,true)==heldpose::SettleResult::Waiting&&!cache.valid,"First post-blend observation froze the palette");
    for(unsigned i=0;i<1000;++i)require(step(2,true)==heldpose::SettleResult::Waiting&&!cache.valid,"Repeated rendering certified stability");
    fixture(.4f);require(step(3,true)==heldpose::SettleResult::Waiting&&!cache.valid,"Changing palette passed stability gate");
    fixture(0);require(step(4,true)==heldpose::SettleResult::Waiting&&!cache.valid,"First settled observation froze immediately");
    for(unsigned i=5;i<20;++i)require(step(i,true,true)==heldpose::SettleResult::Waiting&&!cache.valid,"Pause/render callbacks acquired a pose");
    fixture(0);require(step(20,true)==heldpose::SettleResult::Waiting&&!cache.valid,"A native tick gap was counted as stable continuity");
    fixture(0);require(step(21,true)==heldpose::SettleResult::Waiting&&!cache.valid,"One stable comparison certified the pose");
    fixture(0);require(step(22,true)==heldpose::SettleResult::Applied&&cache.valid,"Settled native palette was not acquired");
    for(unsigned i=0;i<48*12;++i)require(std::abs(m[i]-before[i])<.00003f,"Acquisition changed the settled native palette");
    fixture(.6f);require(step(23,true)==heldpose::SettleResult::Applied,"Acquired held palette was lost");
    for(unsigned i=0;i<48*12;++i)require(std::abs(m[i]-SEATED_BOW_MODEL[i])<.00003f,"Acquired weapon jittered after settling");
    ++key.draw;fixture(.3f);require(step(24,false)==heldpose::SettleResult::Waiting&&!cache.valid&&!memcmp(m,before,sizeof m),"Next draw inherited held geometry");
    ++key.lease;fixture(0);parents[7]=999;
    require(step(25,true)==heldpose::SettleResult::Rejected&&!cache.valid&&!memcmp(m,before,sizeof m),"Malformed hierarchy wrote a pose");
    memcpy(parents,SEATED_BOW_PARENTS,sizeof parents);fixture(0);m[3]=std::numeric_limits<float>::quiet_NaN();memcpy(before,m,sizeof m);
    require(step(26,true)==heldpose::SettleResult::Rejected&&!cache.valid&&!memcmp(m,before,sizeof m),"Nonfinite pose was accepted");
    std::cout<<"PASS: native-time blend acquisition, duplicate/pause/gap/wrap guards, decoded rider palette stability, no first-held freeze, no premature cache, per-draw/lease retirement and malformed pose refusal. Offline fixture, not live animation acceptance.\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
