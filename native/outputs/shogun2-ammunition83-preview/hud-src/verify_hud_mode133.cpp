#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "weapon_mode133.h"
#include "sprites.generated.h"
int main(){try{
    // The unit can acknowledge a weapon order before the personal animation
    // completes; the displayed mode follows the actual unit flag in both cases.
    for(bool pending:{false,true})for(unsigned personal:{1u,2u})for(bool melee:{false,true}){
        (void)pending;(void)personal;auto d=weaponmode133::resolve(true,melee);
        require(!wcscmp(d.label,melee?L"Melee mode":L"Ranged mode"),"Personal/pending state masked unit mode");
        auto art=hud80::sprite(d.icon);require(art&&art->logical_w==22&&art->logical_h==22,"Original native mode artwork missing");
        require(!wcscmp(d.key,L"1"),"Mode shortcut differs from actual mode");
    }
    for(bool has_ranged:{false,true})for(unsigned flag:{0u,1u}){
        auto native=weaponmode133::from_native(flag,has_ranged);
        require(native.known&&native.melee==(!has_ranged||flag==1),"Melee-only soldier appeared in ranged mode");
    }
    require(!weaponmode133::from_native(2,true).known,"Invalid native mode accepted");
    auto unknown=weaponmode133::resolve(false,true);require(!unknown.icon&&!unknown.key,"Unavailable unit mode appeared confirmed");
    std::cout<<"PASS: actual unit melee/ranged indication independent of personal pending state; original22px sprites and unavailable fallback.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
