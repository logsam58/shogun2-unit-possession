#include "combat_focus125.h"
#include <iostream>
#include <stdexcept>
static void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
int main(){try{
    unsigned cases=0;
    for(unsigned a:{0u,1u})for(unsigned b:{0u,1u})for(unsigned bearing=0;bearing<65536;bearing+=256){
        float yaw=float(bearing)*6.283185307f/65536.f;
        melee60::Request r{};r.serial=1;r.lease=2;r.player=3;r.player_unit=4;r.opponent=5;r.opponent_unit=6;r.player_kind=a;r.opponent_kind=b;
        look126::Pose p{0,0,bearing,1000,0,false,true},e{std::sin(yaw)*5,std::cos(yaw)*5,(bearing+32768)&65535,1000,0,false,true};
        look126::Pair pair;pair.observe(r,true,p);check(!pair.locked(r,1000),"One participant unlocked look");pair.observe(r,false,e);
        check(pair.locked(r,1500)&&!pair.locked(r,1501)&&!pair.locked(r,999),"Freshness boundary failed");
        auto wrong=r;++wrong.lease;check(!pair.locked(wrong,1000),"Wrong lease unlocked look");wrong=r;++wrong.serial;check(!pair.locked(wrong,1000),"Old event unlocked look");wrong=r;++wrong.opponent;check(!pair.locked(wrong,1000),"Other opponent unlocked look");
        e.bearing=(bearing+16384)&65535;pair.observe(r,false,e);check(!pair.locked(r,1000),"One unaligned participant unlocked look");
        p.playing=true;p.animation=42;e.playing=false;e.animation=42;pair.observe(r,true,p);pair.observe(r,false,e);check(!pair.locked(r,1000),"Unilateral playback unlocked look");
        e.playing=true;e.animation=43;pair.observe(r,false,e);check(!pair.locked(r,1000),"Different native animations unlocked look");
        e.animation=42;pair.observe(r,false,e);check(pair.locked(r,1000),"Actual matched native playback failed to unlock");
        look125::Focus focus;check(focus.begin(r)&&!focus.unlocked,"New encounter retained old free-look");check(!focus.release(false,yaw)&&!focus.unlocked,"Unproved facing released look");
        check(focus.release(true,yaw)&&focus.unlocked&&focus.yaw==yaw,"Unlock did not retain the exact native viewing direction");
        check(!focus.begin(r)&&!focus.release(true,yaw+1)&&focus.yaw==yaw,"Repeated phase recentered free-look");
        auto changed_kind=r;changed_kind.player_kind=1-r.player_kind;check(focus.begin(changed_kind)&&!focus.unlocked,"Different participant kind retained free-look authority");
        ++r.serial;check(focus.begin(r)&&!focus.unlocked,"New event did not restore initial lock");focus.reset();check(!focus.valid&&!focus.unlocked,"Release retained animation authority");++cases;
    }
    std::cout<<"PASS: "<<cases<<" native pair-facing/playback lock cases, both participant kinds, heading seams, proof/freshness/serial/lease guards, exact native-view unlock and bounded free-look lifecycle. No timer or live game.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
