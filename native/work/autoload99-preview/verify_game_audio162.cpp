#include "game_audio162.h"
#include "../../common/local_assets159.h"
#include <stdexcept>
#include <iostream>
static void check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
int main(){try{
    auto blob=assets159::resource(nullptr,301);WAVEFORMATEX format{};const unsigned char* samples{};DWORD length{};
    check(gameaudio162::pcm(blob.bytes,blob.size,format,samples,length)&&length==136710,"Local cropped voice is not valid PCM");
    auto malformed=std::vector<unsigned char>(blob.bytes,blob.bytes+blob.size);malformed[4]^=1;
    check(!gameaudio162::pcm(malformed.data(),malformed.size(),format,samples,length),"Invalid RIFF length accepted");
    for(size_t n:{size_t(0),size_t(11),size_t(30),size_t(44)})check(!gameaudio162::pcm(blob.bytes,n,format,samples,length),"Truncated voice accepted");
    Status owner{};owner.magic=POC_MAGIC;owner.version=46;owner.size=sizeof owner;owner.pid=7;owner.mode=POSSESSED;owner.ready=1;owner.heartbeat=100;
    check(gameaudio162::active(owner,7,110,true),"Current possession muted");
    check(!gameaudio162::active(owner,7,110,false)&&!gameaudio162::active(owner,8,110,true)&&!gameaudio162::active(owner,7,1200,true),"Inactive or stale possession played");
    owner.release=1;check(!gameaudio162::active(owner,7,110,true),"Released possession played");owner.release=0;
    Status snapshot{};check(gameaudio162::snapshot(&owner,snapshot)&&snapshot.pid==7,"Stable snapshot rejected");owner.sequence=1;check(!gameaudio162::snapshot(&owner,snapshot),"Torn snapshot accepted");
    std::cout<<"PASS: game-process audio PCM/identity/lifetime guards. No clip played by this fixture.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
