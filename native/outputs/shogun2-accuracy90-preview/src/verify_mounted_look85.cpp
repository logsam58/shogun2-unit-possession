#include "identity.h"
#include "steering.h"
#include "mounted_look85.h"
#include "mounted_firearm_geometry.h"
#include "seated_bow_asset.h"
int main(){try{
    constexpr float pi=3.14159265358979323846f;
    for(unsigned bearing:{0u,1u,16384u,32767u,32768u,49152u,65535u}){
        float forward=mountedlook85::heading(bearing),yaw=forward;
        for(int i=0;i<1000;++i)yaw=mountedlook85::mouse_yaw(yaw,.08f,true,bearing);
        require(std::abs(mountedlook85::relative(yaw,bearing)-100*pi/180)<.00001f,"Right limit not 100 degrees");
        yaw=mountedlook85::mouse_yaw(yaw,-.01f,true,bearing);
        require(std::abs(mountedlook85::relative(yaw,bearing)-(100*pi/180-.01f))<.00001f,"Excess mouse input accumulated at limit");
        for(int i=0;i<1000;++i)yaw=mountedlook85::mouse_yaw(yaw,-.08f,true,bearing);
        require(std::abs(mountedlook85::relative(yaw,bearing)+100*pi/180)<.00001f,"Left limit not 100 degrees");
        yaw=mountedlook85::mouse_yaw(forward,100.f,true,bearing);
        require(std::abs(mountedlook85::relative(yaw,bearing)-100*pi/180)<.00001f,"Large mouse input wrapped around stop");
        float foot=mountedlook85::mouse_yaw(forward,pi,false,bearing);
        require(std::abs(std::abs(mountedlook85::relative(foot,bearing))-pi)<.00001f,"Infantry look incorrectly limited");
        for(int d=-360;d<=360;++d){
            float limited=mountedlook85::constrain(forward+float(d)*pi/180,bearing);
            require(std::abs(mountedlook85::relative(limited,bearing))<=100*pi/180+.00001f,"Native boundary admitted rear view");
        }
    }
    // Turning the horse moves its permitted arc through the world-angle seam.
    float yaw=mountedlook85::heading(65000);
    for(unsigned i=0;i<2000;++i){unsigned bearing=(65000+i*41)&65535;
        yaw=mountedlook85::mouse_yaw(yaw,.01f,true,bearing);
        require(std::abs(mountedlook85::relative(yaw,bearing))<=100*pi/180+.00001f,"Horse turn escaped arc");
        auto movement=steering_mount(FORWARD,bearing,.1f,10,0);
        require(std::abs(movement.x-std::sin(mountedlook85::heading(bearing)))<.00001f&&
            std::abs(movement.z-std::cos(mountedlook85::heading(bearing)))<.00001f,"Looking changed horse movement direction");
    }
    // Bow and firearm geometry use the same 100-degree bound. Native saddle
    // and legs must remain unchanged at either extreme, beyond the old 90 cap.
    const unsigned weapons[3]={1,2,3};float world[12]={1,0,0,0,0,1,0,0,0,0,1,0};
    for(float angle:{-100*pi/180,100*pi/180}){
        float bow[48*12],gun[48*12];memcpy(bow,SEATED_BOW_MODEL,sizeof bow);memcpy(gun,bow,sizeof gun);
        require(pose_rider_matrices(bow,48,SEATED_BOW_PARENTS,7,15,18,21,17,weapons,angle,0),"Bow limit pose rejected");
        require(pose_mounted_firearm(gun,48,SEATED_BOW_PARENTS,7,15,18,21,17,weapons,world,angle,0,0),"Gun limit pose rejected");
        float expected[12];memcpy(expected,SEATED_BOW_MODEL+12,48);
        float pivot[3]={SEATED_BOW_MODEL[7*12+3],SEATED_BOW_MODEL[7*12+7],SEATED_BOW_MODEL[7*12+11]};yaw_matrix(expected,pivot,angle);
        for(unsigned k=0;k<12;++k)require(std::abs(bow[12+k]-expected[k])<.00001f&&std::abs(gun[12+k]-expected[k])<.00001f,"Weapon still stops at different angle than camera");
        for(unsigned i:{0u,4u,5u,6u,8u,9u,10u,12u,13u})require(!memcmp(bow+i*12,SEATED_BOW_MODEL+i*12,48)&&!memcmp(gun+i*12,SEATED_BOW_MODEL+i*12,48),"Look limit changed saddle or legs");
    }
    std::cout<<"PASS: mounted 200-degree total arc across heading seams, saturation/reversal, large input, turning horse, independent WASD, unrestricted infantry, matching bow/firearm upper-body limits and unchanged saddle/legs.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
