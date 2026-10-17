#include "animation_view.h"
#include <iostream>
#include <stdexcept>
static void check(bool b,const char* message){if(!b)throw std::runtime_error(message);}
int main(){try{unsigned count=0;
    for(int y=-18;y<=18;++y)for(int p=-8;p<=8;++p){
        float yaw=y*.17f,pitch=p*.16f,cp=std::cos(pitch),sp=std::sin(pitch),sy=std::sin(yaw),cy=std::cos(yaw);
        deathpose::Eye eye{{31,4,-67,31+sy*cp,4+sp,-67+cy*cp},{-sy*sp,cp,-cy*sp}};
        float actual_yaw{},actual_pitch{};check(animationview::angles(eye,actual_yaw,actual_pitch),"Native eye rejected");
        check(std::abs(std::remainder(actual_yaw-yaw,6.283185307f))<.00002f&&std::abs(actual_pitch-pitch)<.00002f,"Native face direction changed");++count;
    }
    deathpose::Eye invalid{};float yaw{},pitch{};check(!animationview::angles(invalid,yaw,pitch),"Zero native view fabricated a face");
    invalid={{0,0,0,0,0,1},{0,0,1}};check(!animationview::angles(invalid,yaw,pitch),"Parallel up/forward accepted");
    invalid.up[1]=NAN;check(!animationview::angles(invalid,yaw,pitch),"Nonfinite native view accepted");
    animationview::Handoff handoff;handoff.observe(42,9,0,7,11);check(handoff.matches(42,9,0,7),"Serial zero rejected");
    check(!handoff.matches(42,10,0,7)&&!handoff.matches(43,9,0,7)&&!handoff.matches(42,9,1,7)&&!handoff.matches(42,9,0,8),"Stale identity adopted an endpoint");
    handoff.observe(42,9,0,7,11,true);handoff.observe(42,9,0,7,11,false);check(handoff.native_playback,"Stale Facing regressed completed native playback");
    handoff.observe(42,9,0,7,12,false);check(!handoff.native_playback,"Different encounter inherited old playback ownership");
    handoff.reset();check(!handoff.pending&&!handoff.matches(42,9,0,7),"Finished endpoint adopted twice");
    check(animationview::eyelid_fraction(0)==0&&animationview::eyelid_fraction(4999)==0&&animationview::eyelid_fraction(5000)==0,"Eyes closed before five seconds");
    check(animationview::eyelid_fraction(5001)>0&&std::abs(animationview::eyelid_fraction(7500)-.5f)<.00001f&&animationview::eyelid_fraction(10000)==1&&animationview::eyelid_fraction(UINT64_MAX)==1,"Five-second eye closing timing failed");
    deathpose::Frame head{{0,0,0},{{1,0,0},{0,1,0},{0,0,1}}};deathpose::Calibration local;
    deathpose::Eye face{{0,.1f,.1f,0,.1f,1.1f},{0,1,0}};
    check(deathpose::calibrate(head,face,local),"Native head/eye calibration rejected");
    head={{4,1,7},{{0,0,-1},{0,1,0},{1,0,0}}};deathpose::Eye falling;
    check(deathpose::follow(head,local,falling)&&std::abs(falling.view[3]-falling.view[0]-1)<.00001f&&std::abs(falling.view[5]-falling.view[2])<.00001f,"Corpse eye failed to rotate with native head face");
    std::cout<<"PASS: "<<count<<" native animation eye directions, pitch/yaw and roll-compatible up validation; corrupt pose rejection, exact actor/lease/serial/unit handoff identity and once-only retirement. Numeric fixtures only.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
