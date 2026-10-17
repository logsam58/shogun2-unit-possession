#include "identity.h"
#include "personal_weapon_profile.h"
int main(){try{
    // IDs here deliberately differ from foot arrows: native unit-selected
    // rider records must remain usable without substituting a guessed ID.
    for(unsigned id:{3u,92u,123u,700u}){
        require(personal_projectile_supported(id,150,50.f,true)&&personal_bow_projectile(id,150,50.f,true),"Mounted arrow record rejected by foot ID");
    }
    require(personal_projectile_supported(3,150,50.f,false)&&personal_projectile_supported(60,100,200.f,false),"Foot bow/matchlock regression");
    require(!personal_projectile_supported(92,150,50.f,false)&&!personal_projectile_supported(60,100,200.f,true),"Unsupported foot record or mounted gun admitted");
    for(float speed:{0.f,49.f,51.f,200.f,std::numeric_limits<float>::quiet_NaN()})
        require(!personal_projectile_supported(92,150,speed,true),"Unsupported mounted projectile admitted");
    for(unsigned range:{0u,100u,149u,151u,500u})require(!personal_projectile_supported(92,range,50.f,true),"Wrong native range accepted");
    for(unsigned mode=0;mode<100;++mode){
        require(personal_weapon_mode(mode,true)==(mode>=29&&mode<=33),"Mounted weapon mode leaks into melee/death");
        require(personal_weapon_mode(mode,false)==(mode<18),"Foot weapon mode changed");
    }
    require(personal_ready_mode(true)==32&&personal_reload_mode(true)==33&&personal_lower_mode(true)==29&&
        bow_raise_clip(true)==751&&bow_hold_clip(true)==753,"Recovered rider animation/state contract changed");
    require(personal_ready_mode(false)==14&&personal_reload_mode(false)==16&&personal_lower_mode(false)==8&&
        bow_raise_clip(false)==126&&bow_hold_clip(false)==124,"Foot draw/reload profile changed");
    std::cout<<"PASS: mounted native arrow profiles preserve runtime IDs; rider state/animation contract; unsupported projectile and melee/death guards; foot bow and firearm compatibility.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
