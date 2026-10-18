#include "identity.h"
#include "animation_look104.h"
int main(){try{
    unsigned cases=0;
    for(int yi=-12;yi<=12;++yi)for(int pi=-6;pi<=6;++pi)for(int ri=-3;ri<=3;++ri)
    for(float offset:{-3.f,-1.7f,-.6f,0.f,.6f,1.7f,3.f})for(float elevation:{-1.5f,-.5f,0.f,.7f,1.5f}){
        float yaw=yi*.26f,pitch=pi*.2f,roll=ri*.3f;
        float f[3]={std::sin(yaw)*std::cos(pitch),std::sin(pitch),std::cos(yaw)*std::cos(pitch)};
        float right[3]={std::cos(yaw),0,-std::sin(yaw)},up[3]={-std::sin(yaw)*std::sin(pitch),std::cos(pitch),-std::cos(yaw)*std::sin(pitch)};
        deathpose::Eye native{{4,10,-20,4+f[0],10+f[1],-20+f[2]},{}};
        for(unsigned k=0;k<3;++k)native.up[k]=up[k]*std::cos(roll)+right[k]*std::sin(roll);
        deathpose::Eye out{};require(look104::free_eye(native,yaw+offset,elevation,out),"Valid animation look rejected");
        float actual_yaw{},actual_pitch{};require(animationview::angles(out,actual_yaw,actual_pitch),"Animation look basis invalid");
        float expected=std::clamp(offset,-look104::yaw_limit,look104::yaw_limit);
        require(std::abs(std::remainder(actual_yaw-yaw-expected,6.283185307f))<.0002f,"Head yaw clamp or world seam differs");
        require(std::abs(actual_pitch-elevation)<.0002f,"Head pitch changed");
        require(!memcmp(native.view,out.view,12),"Head translation left native animation");
        float rotated_right[3]={std::cos(yaw+expected),0,-std::sin(yaw+expected)};
        require(std::abs(deathpose::dot(out.up,rotated_right)-std::sin(roll))<.00002f,"Native roll changed");++cases;
    }
    deathpose::Eye valid{{0,0,0,0,0,1},{0,1,0}},out{};
    require(!look104::free_eye(valid,NAN,0,out)&&!look104::free_eye(valid,0,NAN,out),"Nonfinite requested look accepted");
    valid.up[1]=NAN;require(!look104::free_eye(valid,0,0,out),"Nonfinite native basis accepted");
    require(std::abs(look104::mouse_yaw(3.13f,.1f)+3.0531853f)<.00001f,"Mouse yaw seam jumps");
    std::cout<<"PASS: animation head-look104 "<<cases<<" native eye/yaw/pitch/roll combinations; bounded left/right, elevation, seam, unchanged eye translation and invalid-basis rejection. Geometry only; no authored combat assets.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
