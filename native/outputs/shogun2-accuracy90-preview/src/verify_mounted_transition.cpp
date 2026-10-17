#include "identity.h"
#include "mounted_transition_geometry.h"
#include "mounted_muzzle_calibration.h"
#include "captured_gun_fixture69.h"
#include <limits>
static bool arm(unsigned i){return bone_descends(i,15,SEATED_BOW_PARENTS,48)||bone_descends(i,18,SEATED_BOW_PARENTS,48)||(i>=1&&i<=3);}
static void close(const float* a,const float* b,unsigned count,const char* why){for(unsigned i=0;i<count;++i)require(std::abs(a[i]-b[i])<.0001f,why);}
int main(){try{
    const unsigned weapons[3]={1,2,3};float native[48*12],target[48*12],m[48*12];memcpy(native,SEATED_BOW_MODEL,sizeof native);memcpy(target,native,sizeof native);
    float pivot[3]={native[7*12+3],native[7*12+7],native[7*12+11]};
    for(unsigned i=0;i<48;++i)if(arm(i)){pitch_matrix(target+i*12,pivot,.7f);yaw_matrix(target+i*12,pivot,1.1f);target[i*12+7]+=.2f;}
    mountedge::Cache cache;mountedge::Key key{1,2,3,4,5};
    auto apply=[&](const float* input,bool aim,uint64_t tick,bool paused=false){memcpy(m,input,sizeof m);require(mountedge::apply(cache,key,m,48,SEATED_BOW_PARENTS,0,15,18,weapons,aim,tick,paused),"Valid mounted transition rejected");};
    apply(native,false,1000);require(!memcmp(m,native,sizeof m),"First pose was altered");
    apply(target,true,1010);for(unsigned i=0;i<48;++i)close(m+i*12,(arm(i)?native:target)+i*12,12,"RMB edge jumped instead of retaining current visible arms");
    apply(target,true,1060);float mid[48*12];memcpy(mid,m,sizeof m);require(cache.blending&&cache.elapsed>0&&cache.elapsed<.16f,"Transition did not advance");
    for(unsigned i=0;i<48;++i){if(!arm(i))require(!memcmp(m+i*12,target+i*12,48),"Transition changed horse seat/legs/torso/head");
        else {float inverse[12];require(affine_rigid_inverse(m+i*12,inverse),"Transition sheared or shrank a rigid weapon/arm bone");}}
    apply(target,true,1110,true);close(m,mid,48*12,"Pause changed transition pose");
    apply(target,true,1160,true);close(m,mid,48*12,"Repeated pause advanced transition");
    apply(target,true,1210);require(cache.elapsed<.16f,"Resume included paused time");
    apply(target,true,1260);apply(target,true,1310);require(!cache.blending&&!memcmp(m,target,sizeof m),"Completed transition did not return exact native posed bytes");
    for(unsigned i=0;i<100;++i){apply(target,true,1311+i);require(!memcmp(m,target,sizeof m),"Steady held pose acquired smoothing drift");}
    apply(native,false,1420);close(m,target,48*12,"RMB lower edge snapped");apply(native,false,1470);memcpy(mid,m,sizeof m);
    apply(target,true,1480);close(m,mid,48*12,"Rapid RMB reversal jumped to an old pose");
    ++key.lease;apply(native,false,1490);require(!memcmp(m,native,sizeof m)&&!cache.blending,"Retired lease leaked old transition");
    cache.reset();apply(native,false,1500);float move[12]={1,0,0,.5f,0,1,0,.2f,0,0,1,-.4f},moved[48*12],expected[48*12];
    for(unsigned i=0;i<48;++i){affine_product(move,target+i*12,moved+i*12);affine_product(move,native+i*12,expected+i*12);}
    apply(moved,true,1510);for(unsigned i=0;i<48;++i)close(m+i*12,(arm(i)?expected:moved)+i*12,12,"Cached arms detached from moving native pelvis");
    float bad[48*12];memcpy(bad,moved,sizeof bad);bad[12]=std::numeric_limits<float>::quiet_NaN();memcpy(m,bad,sizeof m);
    require(!mountedge::apply(cache,key,m,48,SEATED_BOW_PARENTS,0,15,18,weapons,true,1520,false)&&!memcmp(m,bad,sizeof m)&&!cache.valid,"Invalid transition partially changed output");
    // Check shortest-path interpolation through large rotations and scale preservation.
    const float identity[12]={1,0,0,0,0,1,0,0,0,0,1,0};float b[12],out[12],inv[12],zero[3]{};
    for(float angle:{-3.14f,-2.f,0.f,2.f,3.14f}){memcpy(b,identity,48);yaw_matrix(b,zero,angle);
        for(float t:{.01f,.25f,.5f,.75f,.99f})require(mountedge::interpolate(identity,b,t,out)&&affine_rigid_inverse(out,inv),"Quaternion transition produced non-rigid large-angle pose");}
    float raw[3],raised[3];require(mounted_carbine_muzzle_world(CAPTURED_GUN_WEAPON,CAPTURED_GUN_WORLD,raw)&&calibrated_carbine_muzzle_world(CAPTURED_GUN_WEAPON,CAPTURED_GUN_WORLD,raised),"Captured carbine socket rejected");
    require(raised[1]>raw[1]+.018f&&raised[1]<raw[1]+.022f,"Calibrated carbine socket did not move up toward barrel");
    for(float pitch:{-.6f,0.f,.6f}){float gun[12];memcpy(gun,MOUNTED_CARBINE_FIRE_BONE,48);pitch_matrix(gun,zero,pitch);
        require(mounted_carbine_muzzle_world(gun,identity,raw)&&calibrated_carbine_muzzle_world(gun,identity,raised),"Pitched muzzle rejected");
        for(unsigned r=0;r<3;++r)require(std::abs(raised[r]-raw[r]+gun[r*4+1]*.02f)<.00001f,"Muzzle calibration detached from pitched barrel");}
    float before[3]={11,22,33};memcpy(raised,before,sizeof before);memcpy(b,identity,48);b[0]=std::numeric_limits<float>::quiet_NaN();
    require(!calibrated_carbine_muzzle_world(b,identity,raised)&&!memcmp(raised,before,sizeof before),"Invalid muzzle wrote output");
    std::cout<<"PASS: mounted RMB edge continuity, bounded rigid arm/equipment blending, native seat/legs/torso/head preservation, moving-pelvis attachment, pause/resume, rapid reversal, exact steady-pose bytes, retired lease and transactional rejection; captured carbine socket raised toward barrel and follows pitch.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
