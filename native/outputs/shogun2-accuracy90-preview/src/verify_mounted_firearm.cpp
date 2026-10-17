#include "identity.h"
#include "mounted_firearm_geometry.h"
#include "captured_gun_fixture69.h"
#include <limits>
int main(){try{
    float world[12]={1,0,0,30,0,1,0,20,0,0,1,300},point[3];
    require(mounted_carbine_muzzle_world(MOUNTED_CARBINE_FIRE_BONE,world,point),"Authored carbine anchor rejected");
    for(unsigned r=0;r<3;++r)require(std::abs(point[r]-world[r*4+3]-MOUNTED_CARBINE_FIRE_POINT[r])<.00004f,"Carbine-local anchor does not reproduce decoded FIRE_POSITION");
    const unsigned weapons[3]={1,2,3};
    // Reproduce the old torso snap independently of native animation timing.
    // The same native palette and camera pitch must produce identical torso,
    // neck and hidden-head geometry on both sides of a rifle RMB transition.
    for(float yaw:{-1.2f,0.f,1.2f})for(float pitch:{-.6f,0.f,.6f}){
        float idle[48*12],aim[48*12],old_idle[48*12];
        memcpy(idle,SEATED_BOW_MODEL,sizeof idle);memcpy(aim,idle,sizeof aim);memcpy(old_idle,idle,sizeof old_idle);
        require(pose_rider_matrices(old_idle,48,SEATED_BOW_PARENTS,7,15,18,21,17,weapons,yaw,pitch),"Old idle fixture rejected");
        require(pose_mounted_firearm(idle,48,SEATED_BOW_PARENTS,7,15,18,21,17,weapons,world,yaw,pitch,0),"Rifle idle look rejected");
        require(pose_mounted_firearm(aim,48,SEATED_BOW_PARENTS,7,15,18,21,17,weapons,world,yaw,pitch),"Rifle aimed look rejected");
        if(std::abs(pitch)>.5f)require(std::abs(old_idle[7*12+5]-aim[7*12+5])>.1f||std::abs(old_idle[7*12+6]-aim[7*12+6])>.1f,"Fixture did not expose previous torso pitch discontinuity");
        for(unsigned i=0;i<48;++i){
            bool arm=bone_descends(i,15,SEATED_BOW_PARENTS,48)||bone_descends(i,18,SEATED_BOW_PARENTS,48)||(i>=1&&i<=3);
            if(!arm)require(!memcmp(idle+i*12,aim+i*12,48),"Rifle RMB changed torso/neck/head/pelvis/legs");
            else for(unsigned k=0;k<12;++k){float expected=k==7?MOUNTED_GUN_RAISE:0.f;
                require(std::abs(aim[i*12+k]-idle[i*12+k]-expected)<.00001f,"Idle/aim difference escaped established arm lift");}
        }
        for(unsigned i:{0u,4u,5u,6u,8u,9u,10u,12u,13u})require(!memcmp(idle+i*12,SEATED_BOW_MODEL+i*12,48),"Rifle idle changed native saddle or legs");
    }
    for(float yaw:{-1.2f,0.f,1.2f})for(float pitch:{-.6f,0.f,.6f})for(float slope:{-.25f,0.f,.25f}){
        float m[48*12],before[48*12],transform[12];memcpy(m,SEATED_BOW_MODEL,sizeof m);memcpy(before,m,sizeof m);
        float c=std::cos(slope),s=std::sin(slope);world[5]=c;world[6]=s;world[9]=-s;world[10]=c;
        require(mounted_firearm_transform(m,world,7,15,18,yaw,pitch,transform),"Mounted firearm transform rejected");
        require(pose_mounted_firearm(m,48,SEATED_BOW_PARENTS,7,15,18,21,17,weapons,world,yaw,pitch),"Mounted firearm pose rejected");
        for(unsigned i:{0u,4u,5u,6u,8u,9u,10u,12u,13u})
            require(!memcmp(m+i*12,before+i*12,48),"Gun aim changed native pelvis or legs");
        float expected[12];affine_product(transform,before+12,expected);
        for(unsigned k=0;k<12;++k)require(std::abs(m[12+k]-expected[k])<.0001f,"Fallback muzzle correction differs from rendered gun transform");
        float actual[3];require(mounted_carbine_muzzle_world(m+12,world,actual),"Posed gun anchor rejected");
        float local[3];for(unsigned r=0;r<3;++r){local[r]=expected[r*4+3];for(unsigned k=0;k<3;++k)local[r]+=expected[r*4+k]*MOUNTED_CARBINE_MUZZLE[k];}
        for(unsigned r=0;r<3;++r){float v=world[r*4+3];for(unsigned k=0;k<3;++k)v+=world[r*4+k]*local[k];require(std::abs(v-actual[r])<.0001f,"World barrel origin diverged from gun attachment");}
    }
    float m[48*12],before[48*12];
    for(float pitch:{-.35f,0.f,.35f}){
        memcpy(m,SEATED_BOW_MODEL,sizeof m);
        memcpy(m+12,CAPTURED_GUN_WEAPON,48);memcpy(m+15*12,CAPTURED_GUN_LEFT,48);memcpy(m+18*12,CAPTURED_GUN_RIGHT,48);memcpy(before,m,sizeof m);
        float target[3]={CAPTURED_GUN_RAY[0]*std::cos(pitch),std::sin(pitch),CAPTURED_GUN_RAY[2]*std::cos(pitch)};
        require(align_mounted_carbine(m,48,SEATED_BOW_PARENTS,15,18,weapons,CAPTURED_GUN_WORLD,target),"Recorded Imperial Guard aim correction rejected");
        float axis[3]{},length=0;
        for(unsigned r=0;r<3;++r){for(unsigned c=0;c<3;++c)axis[r]-=CAPTURED_GUN_WORLD[r*4+c]*m[12+c*4];length+=axis[r]*axis[r];}
        for(unsigned r=0;r<3;++r)require(std::abs(axis[r]/std::sqrt(length)-target[r])<.0001f,"Recorded gun bore is not aligned to the requested ray");
        for(unsigned i:{0u,4u,5u,6u,7u,8u,9u,10u,11u,12u,13u,14u,17u,21u})require(!memcmp(m+i*12,before+i*12,48),"Bore alignment changed torso/head/seat/legs");
    }
    float opposite[3];for(unsigned r=0;r<3;++r){opposite[r]=0;for(unsigned c=0;c<3;++c)opposite[r]+=CAPTURED_GUN_WORLD[r*4+c]*m[12+c*4];}
    memcpy(before,m,sizeof m);require(!align_mounted_carbine(m,48,SEATED_BOW_PARENTS,15,18,weapons,CAPTURED_GUN_WORLD,opposite)&&!memcmp(m,before,sizeof m),"Reversed view twisted the rig");
    memcpy(m,SEATED_BOW_MODEL,sizeof m);memcpy(before,m,sizeof m);
    world[0]=0;require(!pose_mounted_firearm(m,48,SEATED_BOW_PARENTS,7,15,18,21,17,weapons,world,0,0)&&!memcmp(m,before,sizeof m),"Invalid world partially changed rig");
    world[0]=1;m[0]=std::numeric_limits<float>::quiet_NaN();memcpy(before,m,sizeof m);
    require(!pose_mounted_firearm(m,48,SEATED_BOW_PARENTS,7,15,18,21,17,weapons,world,0,0)&&!memcmp(m,before,sizeof m),"Nonfinite rig partially changed");
    std::cout<<"PASS: mounted firearm shoulder pitch and world-up aim lift preserve seated lower body; slope/yaw/pitch attachment and fallback transform agreement; decoded carbine FIRE_POSITION roundtrip; actual Imperial Guard capture aligns bore for level/up/down rays without changing torso/head/seat; reversed-view and invalid/nonfinite transactional rejection.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
