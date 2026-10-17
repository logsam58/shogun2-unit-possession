#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "hud_state89.h"
int main(){try{using namespace hudstate89;
 Input s{true,true,true,false,true,1,0,1,1,0,20,0};
 auto a=resolve(s);require(a.fire_prompt&&a.reticle==Reticle::Drop,"Ready bow missing drop reticle");
 for(unsigned kind:{1u,3u})for(unsigned icon:{1u,3u,4u,5u,7u,8u,9u}){s.kind=kind;s.icon=icon;require(resolve(s).reticle==Reticle::Drop,"Nonfirearm projectile lost descending marks");}
 s.kind=2;require(resolve(s).reticle==Reticle::Firearm,"Firearm changed reticle");
 s.pending=true;require(!resolve(s).fire_prompt&&resolve(s).reticle==Reticle::None,"Pending switch looked confirmed");s.pending=false;
 s.ammunition=0;require(!resolve(s).fire_prompt,"Empty ammo looked ready");s.ammunition=20;
 s.phase=4;require(!resolve(s).fire_prompt,"Reload looked ready");s.phase=0;
 s.aim_pose=2;require(!resolve(s).fire_prompt,"Drawing looked drawn");s.aim_pose=1;
 s.aiming=false;require(!resolve(s).fire_prompt,"Lowered weapon looked raised");s.aiming=true;
 s.ammo_known=false;require(resolve(s).reticle==Reticle::None,"Unknown ammo had confident reticle");s.ammo_known=true;
 s.equipment=false;require(resolve(s).reticle==Reticle::None,"Foreign/stale equipment had a reticle");
 for(unsigned i=1;i<4;++i)require(drop_y[i]>drop_y[i-1]&&drop_width[i]<drop_width[i-1],"Descending reticle geometry changed");
 std::cout<<"PASS: native weapon kinds, nonfirearm descending reticles, firearm retention, pending/empty/reload/draw/lower/unavailable presentation.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
