#include "identity.h"
#include "look_pitch96.h"
int main(){try{
 unsigned cases=0;
 for(int yi=-12;yi<=12;++yi)for(int pi=-7;pi<=7;++pi)for(int ri=-4;ri<=4;++ri)for(float requested:{-1.5533f,-1.1f,-.4f,0.f,.6f,1.2f,1.5533f}){
  float yaw=yi*.26f,pitch=pi*.2f,roll=ri*.3f;
  float f[3]={std::sin(yaw)*std::cos(pitch),std::sin(pitch),std::cos(yaw)*std::cos(pitch)};
  float right[3]={std::cos(yaw),0,-std::sin(yaw)},up[3]={-std::sin(yaw)*std::sin(pitch),std::cos(pitch),-std::cos(yaw)*std::sin(pitch)};
  deathpose::Eye eye{{4,10,-20,4+f[0],10+f[1],-20+f[2]},{}};
  for(unsigned k=0;k<3;++k)eye.up[k]=up[k]*std::cos(roll)+right[k]*std::sin(roll);
  deathpose::Eye result;require(look96::pitched_eye(eye,requested,result),"Valid native eye rejected");float actual_yaw,actual_pitch;
  require(animationview::angles(result,actual_yaw,actual_pitch),"Output not orthogonal");
  require(!memcmp(result.view,eye.view,3*sizeof(float)),"Native QTE translation changed");
  // Near vertical, subtracting world coordinates bounds float angular precision.
  require(std::abs(actual_pitch-requested)<.00025f,"Requested vertical view lost");
  require(std::abs(std::remainder(actual_yaw-yaw,6.283185307f))<.0002f,"Native QTE yaw changed");
  float right_component=deathpose::dot(result.up,right);require(std::abs(right_component-std::sin(roll))<.00002f,"Native roll changed");++cases;
 }
 deathpose::Eye valid{{0,0,0,0,0,1},{0,1,0}},out;
 auto invalid=valid;invalid.up[1]=NAN;require(!look96::pitched_eye(invalid,0,out),"Invalid native basis accepted");
 require(!look96::pitched_eye(valid,NAN,out),"Invalid requested pitch accepted");
 for(float p:{-1.2f,0.f,1.2f})for(int dy:{-499,-1,0,1,499}){
  float q=look96::mouse_pitch(p,dy,.0025f);require(std::isfinite(q)&&std::abs(q)<=1.5533431f,"Mouse pitch clamp");
  require((dy==0&&p==q)||(dy>0&&q<=p)||(dy<0&&q>=p),"Mouse vertical direction");
 }
 std::cout<<"PASS: look96 "<<cases<<" yaw/native-pitch/roll/requested-elevation combinations, native eye translation, vertical input/clamps and invalid basis.\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
