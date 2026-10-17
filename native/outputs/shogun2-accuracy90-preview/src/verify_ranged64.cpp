#include "identity.h"
#include "personal_weapon_catalog.h"
#include "seated_weapon_geometry.h"
int main(){try{
    for(const auto& p:PERSONAL_WEAPONS){
        unsigned length=unsigned(std::wcslen(p.key));
        require(personal_key_kind(p.key,length,length,300,200.f)==p.kind,"Installed personal projectile key rejected");
        char narrow[129]{};for(unsigned i=0;i<length;++i)narrow[i]=char(p.key[i]);
        require(personal_ascii_key_kind(narrow,length,length,300,200.f)==p.kind,"Native narrow projectile key rejected");
        require(personal_ascii_key_kind(narrow,length,length-1,300,200.f)==PersonalWeaponKind::Unsupported,"Malformed narrow key accepted");
        require(personal_key_kind(p.key,length,length-1,300,200.f)==PersonalWeaponKind::Unsupported,"Malformed key capacity accepted");
        require(personal_key_kind(p.key,length,length,300,std::numeric_limits<float>::quiet_NaN())==PersonalWeaponKind::Unsupported,"Nonfinite projectile accepted");
    }
    for(const wchar_t* k:{L"shogun_cannon",L"naval_cannon",L"unknown_mod_record",L"shogun_arrow_rider_extra"})
        require(personal_key_kind(k,unsigned(std::wcslen(k)),128,150,50)==PersonalWeaponKind::Unsupported,"Unknown/station key admitted");
    float pose[48*12];memcpy(pose,SEATED_BOW_MODEL,sizeof pose);
    // Place a native infantry upper body one metre above the seated pelvis.
    for(unsigned i=0;i<48;++i)pose[i*12+7]+=1.f;
    float arms[12];memcpy(arms,pose+12*12,48);
    require(seat_personal_weapon(pose,48,SEATED_BOW_PARENTS),"Seated remap rejected valid rig");
    for(unsigned i:{0u,5u,6u})for(unsigned k=0;k<12;++k)require(std::abs(pose[i*12+k]-SEATED_BOW_MODEL[i*12+k])<.001f,"Pelvis/legs not seated");
    for(unsigned k=0;k<12;++k)require(std::abs(pose[12*12+k]-(arms[k]-(k==7?1.f:0.f)))<.001f,"Native upper-body pose lost");
    float world[12]={0,0,1,30,0,1,0,20,-1,0,0,300};float world_pose[48*12];
    for(unsigned i=0;i<48;++i)affine_product(world,pose+i*12,world_pose+i*12);
    require(seat_personal_weapon(world_pose,48,SEATED_BOW_PARENTS,world),"World eye remap failed");
    for(unsigned i=0;i<48;++i){float expected[12];affine_product(world,pose+i*12,expected);for(unsigned k=0;k<12;++k)require(std::abs(world_pose[i*12+k]-expected[k])<.002f,"World eye differs from model-space remap");}
    auto before=world_pose[0];int32_t bad[48];memcpy(bad,SEATED_BOW_PARENTS,sizeof bad);bad[7]=1;
    require(!seat_personal_weapon(world_pose,48,bad,world)&&world_pose[0]==before,"Wrong rig mutated");
    require(!seat_personal_weapon(world_pose,47,SEATED_BOW_PARENTS,world),"Wrong bone count accepted");
    float scaled[12]={1.1f,.08f,0,30,0,.95f,.03f,20,0,0,1.05f,300},inverse[12],identity[12];
    require(affine_pose_inverse(scaled,inverse),"Bounded native affine transform rejected");
    affine_product(scaled,inverse,identity);for(unsigned r=0;r<3;++r)for(unsigned c=0;c<4;++c)
        require(std::abs(identity[r*4+c]-(r==c?1.f:0.f))<.002f,"Affine inverse does not preserve points");
    for(unsigned i=0;i<48;++i)affine_product(scaled,pose+i*12,world_pose+i*12);
    require(seat_personal_weapon(world_pose,48,SEATED_BOW_PARENTS,scaled),"Scaled native world cannot seat rider");
    for(unsigned i=0;i<48;++i){float expected[12];affine_product(scaled,pose+i*12,expected);for(unsigned k=0;k<12;++k)
        require(std::abs(world_pose[i*12+k]-expected[k])<.002f,"Scaled world remap drifts from model pose");}
    scaled[0]=0;scaled[4]=0;scaled[8]=0;unsigned failure=0;
    require(!seat_personal_weapon(world_pose,48,SEATED_BOW_PARENTS,scaled,&failure)&&failure==SeatWorld,"Singular world admitted or wrong diagnostic");
    require(!seat_personal_weapon(world_pose,47,SEATED_BOW_PARENTS,nullptr,&failure)&&failure==SeatCount,"Bone-count diagnostic missing");
    std::cout<<"PASS: all keyed native projectile families; malformed/unknown record guards; seated lower body with intact authored upper body; model/world eye agreement; rig, count and transform rejection.\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
