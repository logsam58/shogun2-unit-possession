#include "../../shogun2-possession-cleanup-v46/src/friendly_fire_protocol109.h"
#include <iostream>
#include <stdexcept>
static void check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
int main(){try{
    friendly109::Status s{};s.magic=friendly109::magic;s.version=109;s.size=sizeof s;s.pid=7;s.lease=2;s.actor=0;s.unit=9;s.event=1;s.victim_unit=8;s.tick=100;
    friendly109::Consumer c;check(c.take(s,7,2,0,9,110,true),"Current allied hit not consumed");
    check(!c.take(s,7,2,0,9,111,true),"Allied hit replayed");
    ++s.event;check(!c.take(s,7,2,0,9,112,false)&&!c.take(s,7,2,0,9,113,true),"Inactive hit replayed on return");
    ++s.event;check(!c.take(s,7,3,0,9,114,true),"Old possession gained playback");
    ++s.event;check(!c.take(s,7,2,1,9,115,true),"Foreign actor gained playback");
    ++s.event;check(!c.take(s,7,2,0,10,116,true),"Foreign unit gained playback");
    ++s.event;check(!c.take(s,7,2,0,9,1200,true),"Stale notification played");
    ++s.event;check(!c.take(s,7,2,0,9,99,true),"Future notification played");
    ++s.event;s.magic=0;check(!c.take(s,7,2,0,9,120,true),"Invalid notification played");
    auto resource=FindResourceW(nullptr,MAKEINTRESOURCEW(301),L"WAVE");check(resource!=nullptr,"Cropped voice resource missing");
    auto bytes=static_cast<const unsigned char*>(LockResource(LoadResource(nullptr,resource)));auto size=SizeofResource(nullptr,resource);
    check(bytes&&size==136754&&!memcmp(bytes,"RIFF",4)&&!memcmp(bytes+8,"WAVE",4),"Voice resource is not the pinned cropped WAV");
    bool format=false,data=false;for(unsigned i=12;i+8<=size;){uint32_t n=0;memcpy(&n,bytes+i+4,4);check(n<=size-i-8,"Invalid WAV chunk");
        if(!memcmp(bytes+i,"fmt ",4)){check(n>=16,"Truncated audio format");uint16_t codec=0,channels=0,bits=0;uint32_t rate=0;
            memcpy(&codec,bytes+i+8,2);memcpy(&channels,bytes+i+10,2);memcpy(&rate,bytes+i+12,4);memcpy(&bits,bytes+i+22,2);
            check(codec==1&&channels==1&&rate==44100&&bits==16,"Voice format differs");format=true;}
        if(!memcmp(bytes+i,"data",4)){check(n==136710,"Routing lead-in retained or crop changed");data=true;}i+=8+n+(n&1);}
    check(format&&data,"Voice resource lacks PCM samples");
    std::cout<<"PASS: current-identity audio events, once-only/inactive/stale rejection, exact 1.55-second cropped PCM resource. No sound played.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
