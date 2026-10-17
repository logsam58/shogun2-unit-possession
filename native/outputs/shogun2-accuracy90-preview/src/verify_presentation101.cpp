#include "identity.h"
#include "first_person_presentation101.h"
#include "self_visibility100.h"
int main(){try{
 unsigned cases=0;int32_t parents[8]={-1,0,0,0,3,-1,-1,-1};unsigned weapons[3]={5,6,7};
 for(int angle=-14;angle<=14;++angle)for(int heading=-12;heading<=12;++heading){
  float yaw=heading*.25f,pitch=angle*.1f,c=std::cos(yaw),s=std::sin(yaw);
  float world[12]={c,0,s,50,0,1,0,30,-s,0,c,-100},eye[3]={50,31.7f,-100},pivot[3]{};
  require(firstperson101::eye_in_model(world,eye,pivot),"Owned eye/model transform rejected");
  float native[96]{},posed[96]{};
  for(unsigned i=0;i<8;++i){native[i*12]=native[i*12+5]=native[i*12+10]=1;native[i*12+3]=i==1?-.2f:.2f;native[i*12+7]=1.45f;native[i*12+11]=.05f;}
  memcpy(posed,native,sizeof posed);
  require(pose_owned_matrices(posed,8,parents,1,2,4,3,weapons,pitch,true,pivot),"Eye-relative foot look rejected");
  for(unsigned bone:{1u,2u,5u,6u,7u}){
   // A point on the stock/arm stays at the same camera-space depth for every
   // elevation instead of swinging through the eye from a shoulder pivot.
   const float* before=native+bone*12;const float* after=posed+bone*12;
   float depth=std::sin(pitch)*(after[7]-pivot[1])+std::cos(pitch)*(after[11]-pivot[2]);
   require(std::abs(depth-(before[11]-pivot[2]))<.00001f,"Weapon/arm depth drifted through camera with look elevation");
   float muzzle[3];for(unsigned r=0;r<3;++r){muzzle[r]=world[r*4+3];for(unsigned col=0;col<3;++col)muzzle[r]+=world[r*4+col]*before[col*4+3];}
   require(pitch_world_point(muzzle,pivot,world,pitch),"Same eye-relative muzzle transform rejected");
   for(unsigned r=0;r<3;++r){float expected=world[r*4+3];for(unsigned col=0;col<3;++col)expected+=world[r*4+col]*after[col*4+3];require(std::abs(expected-muzzle[r])<.0001f,"Visible gun and projectile socket diverged");}
  }
  require(!memcmp(posed,native,48)&&!memcmp(posed+36,native+36,48),"Body/neck changed by weapon look");
  require(visibility100::hide(posed,native,8,parents,4,3,1,2,weapons)&&!memcmp(posed+36,native+36,48),"Self hiding collapsed the mixed-weight neck/collar");
  ++cases;
 }
 firstperson101::NearClip clip_scope;float value=.15f;require(clip_scope.capture(value,5000)&&clip_scope.apply(value)&&value==.02f,"Observed stock near-plane correction");
 require(clip_scope.apply(value,false)&&value==.15f&&clip_scope.apply(value)&&value==.02f,"Native death view clip plane not retained");
 require(clip_scope.restore(value,true)&&value==.15f,"Exact original near plane not restored");
 value=.01f;require(clip_scope.capture(value,5000)&&clip_scope.apply(value)&&value==.01f,"Existing smaller clip distance increased");clip_scope.restore(value,true);
 value=.15f;clip_scope.capture(value,5000);clip_scope.apply(value);value=.07f;require(!clip_scope.restore(value,true)&&value==.07f,"Foreign clip change overwritten");
 value=.15f;clip_scope.capture(value,5000);clip_scope.apply(value);require(!clip_scope.restore(value,false)&&value==.02f,"Stale/replaced camera written");
 require(!clip_scope.capture(NAN,5000)&&!clip_scope.capture(0,5000)&&!clip_scope.capture(.15f,.1f),"Invalid projection accepted");
 float bad[12]={},eye[3]{},pivot[3]={1,2,3};require(!firstperson101::eye_in_model(bad,eye,pivot)&&pivot[0]==1,"Invalid eye transform partially published");
 std::cout<<"PASS: "<<cases<<" eye-relative weapon/arm and muzzle cases, native body/neck preservation, exact near-plane lifecycle and stale/foreign guards. Geometry is synthetic; no claim of roster-wide visual acceptance.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
