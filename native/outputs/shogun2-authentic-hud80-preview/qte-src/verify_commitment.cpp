#include "melee_commitment.h"
#include "../../shogun2-possession-qte55-preview/src/identity.h"
#include "../../shogun2-possession-qte55-preview/src/steering.h"
#include <iostream>
int main(){try{
    melee60::Request r;r.pid=77;r.lease=3;r.serial=1;r.phase=melee60::Facing;
    r.player=0;r.player_unit=1;r.opponent=1;r.opponent_unit=2;r.heartbeat=100;
    r.opponent_position[2]=2;
    require(melee60::live(r,77,3,0,1,100),"Zero native actor serial excluded");
    require(!melee60::live(r,77,4,0,1,100)&&!melee60::live(r,77,3,1,1,100),"Foreign lease/actor accepted");
    require(!melee60::live(r,77,3,0,2,100)&&!melee60::live(r,78,3,0,1,100),"Foreign unit/process accepted");
    require(!melee60::live(r,77,3,0,1,601)&&!melee60::live(r,77,3,0,1,99),"Stale/future controller accepted");
    auto invalid=r;invalid.opponent_position[0]=NAN;require(!melee60::valid(invalid),"Invalid target accepted");
    melee60::Channel channel;melee60::publish(channel,r);melee60::Request copy;
    require(melee60::read(channel,copy)&&copy.opponent_unit==2,"Commitment publication failed");
    channel.sequence|=1;require(!melee60::read(channel,copy),"Torn commitment accepted");
    for(unsigned angle=0;angle<65536;angle+=257){
        auto heading=angle;float wanted=melee60::facing(0,0,0,2);
        for(unsigned frame=0;frame<100;++frame)heading=static_cast<unsigned short>(heading+face_view(heading,wanted,.1f,1.39626336f));
        require(melee60::angle_error(wanted,heading)<.001f,"Native-rate facing did not converge");
    }
    require(melee60::eyelid_fraction(9999)==0&&melee60::eyelid_fraction(10000)==0,"Eyes close before10seconds");
    require(melee60::eyelid_fraction(12500)>.49f&&melee60::eyelid_fraction(12500)<.51f,"Midpoint fade differs");
    require(melee60::eyelid_fraction(15000)==1&&melee60::eyelid_fraction(20000)==1,"Eyes not fully closed at15seconds");
    // The engine calls prepare again while the accepted variant is aligning.
    // A successful QTE is not evidence that the native animation has started.
    for(bool current_event:{false,true}){
        uint32_t phase=melee60::Aligning;
        for(unsigned frame=0;frame<500;++frame){
            phase=melee60::preparation_phase(phase,false,current_event);
            phase=melee60::playback_phase(phase,false);
            require(phase==melee60::Aligning,"Repeated native prepare abandoned alignment before playback");
        }
        phase=melee60::playback_phase(phase,true);
        require(phase==melee60::NativeCombat,"Observed native playback did not hand over root motion");
        require(melee60::playback_phase(phase,false)==melee60::NativeCombat,"Playback observation regressed phase");
        require(melee60::preparation_phase(phase,true,current_event)==(current_event?melee60::Off:melee60::NativeCombat),"Live cancellation or cleared prompt changed wrong phase");
    }
    float previous=0;for(uint64_t t=0;t<=15000;++t){float now=melee60::eyelid_fraction(t);require(now>=previous&&now<=1,"Death fade not bounded/monotone");previous=now;}
    std::cout<<"PASS: commitment identity/lease/process/freshness/torn-read guards, native-rate facing convergence, 1000 repeated preparation/playback observations preserve alignment until native playback, cancellation releases control, death eyes open10seconds and close by15seconds. Native camera/actor lifetime not live tested.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
