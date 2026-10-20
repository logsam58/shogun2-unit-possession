#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "../../shogun2-accuracy90-preview/src/ability_protocol141.h"
#include "../../shogun2-accuracy90-preview/src/ability_catalog141.h"
#include "sprites.generated.h"
int main(){try{
 for(const auto& a:ability141::catalog){require(a.key&&a.label&&a.icon,"Missing original ability presentation");if(a.id!=30&&a.id!=31)require(hud80::sprite(a.icon)!=nullptr,"Original ability icon absent");}
 ability141::Status s{};s.magic=ability141::Magic;s.version=141;s.size=sizeof s;s.pid=11;s.lease=2;s.actor=3;s.unit=4;s.tick=100;s.count=1;s.items[0]={39,ability141::Available,0};
 require(ability141::valid(s,11,2,3,4,101),"Valid ability display rejected");
 require(!ability141::valid(s,11,1,3,4,101)&&!ability141::valid(s,11,2,3,4,1101),"Stale ability display accepted");s.items[0].cooldown=NAN;require(!ability141::valid(s,11,2,3,4,101),"Invalid cooldown accepted");
 std::cout<<"PASS: native ability artwork and identity/freshness/cooldown HUD validation.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
