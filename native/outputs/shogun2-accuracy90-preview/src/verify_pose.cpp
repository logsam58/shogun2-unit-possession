#include "identity.h"
#include "pose_geometry.h"
#include "../vendor/include/MinHook.h"
static volatile LONG calls;
using Callback=void(__thiscall*)(void*,void*);
struct Receiver {void* received;unsigned marker;};
static __declspec(noinline) void __fastcall native_callback(void* self,void*,void* matrices){
    auto* r=static_cast<Receiver*>(self);r->received=matrices;++calls;
}
static Callback original_callback;
static void __fastcall detour(void* self,void*,void* matrices){original_callback(self,matrices);}
int main(){try{
    for(unsigned clip:{124u,126u,130u})for(float pitch:{-1.2f,0.0f,1.2f}){
        require(std::abs(weapon_pose_pitch(3,clip,pitch)+BOW_NATIVE_AIM_PITCH-pitch)<0.000001f,"Bow native aim bias not canceled");
        require(weapon_pose_pitch(60,clip,pitch)==pitch,"Matchlock pose changed");
    }
    for(unsigned clip:{132u,133u,134u,141u})require(weapon_pose_pitch(3,clip,0.4f)==0.4f,"Non-25-degree bow clip changed");
    int32_t parents[8]={-1,0,0,0,3,-1,-1,-1};unsigned weapons[3]={5,6,7};
    float m[96]{},before[96];for(unsigned i=0;i<8;++i){m[i*12]=m[i*12+5]=m[i*12+10]=1;m[i*12+7]=1;m[i*12+11]=2;}
    m[15]=-1;m[27]=1; // Shoulder positions straddle the pivot.
    memcpy(before,m,sizeof(m));require(pose_owned_matrices(m,8,parents,1,2,4,3,weapons,0.5f,true),"Pose rejected");
    require(memcmp(m,before,12*sizeof(float))==0,"Hips changed");
    require(memcmp(m+36,before+36,12*sizeof(float))==0,"Neck changed");
    for(unsigned i:{1u,2u,5u,6u,7u}){
        require(std::abs(m[i*12+6]-std::sin(0.5f))<1e-6f,"Pitch sign or weapon rotation differs");
        require(std::abs(m[i*12+5]*m[i*12+5]+m[i*12+6]*m[i*12+6]-1)<1e-6f,"Rotation scale differs");
    }
    require(m[48]==0.0001f&&m[55]==before[43],"Head hiding differs");
    require(!bone_descends(0,4,parents,8)&&bone_descends(4,3,parents,8),"Hierarchy traversal differs");
    int32_t cycle[2]={1,0};require(!bone_descends(0,4,cycle,2),"Cycle did not terminate");
    memcpy(m,before,sizeof(m));require(pose_owned_matrices(m,8,parents,1,2,4,3,weapons,-0.8f,false),"Idle pose rejected");
    require(memcmp(m+12,before+12,24*sizeof(float))==0,"Idle arms changed");
    m[0]=NAN;float invalid[96];memcpy(invalid,m,sizeof(m));require(!pose_owned_matrices(m,8,parents,1,2,4,3,weapons,0,true)&&memcmp(m,invalid,sizeof(m))==0,"Invalid pose partially changed");
    for(int yaw_step=-20;yaw_step<=20;++yaw_step)for(int pitch_step=-15;pitch_step<=15;++pitch_step){
        float yaw=yaw_step*0.16f,pitch=pitch_step*0.1f,c=std::cos(yaw),s=std::sin(yaw);
        float world[12]={c,0,s,42,0,1,0,10,-s,0,c,-300},pivot[3]={0,0.4f,0};
        float bone[12]={1,0,0,0.3f,0,1,0,0.45f,0,0,1,0.8f},origin[3],expected[3];
        for(unsigned row=0;row<3;++row){origin[row]=world[row*4+3];for(unsigned col=0;col<3;++col)origin[row]+=world[row*4+col]*bone[col*4+3];}
        pitch_matrix(bone,pivot,pitch);
        for(unsigned row=0;row<3;++row){expected[row]=world[row*4+3];for(unsigned col=0;col<3;++col)expected[row]+=world[row*4+col]*bone[col*4+3];}
        require(pitch_world_point(origin,pivot,world,pitch),"World muzzle rejected");
        for(unsigned k=0;k<3;++k)require(std::abs(origin[k]-expected[k])<0.0001f,"Rendered weapon and muzzle pitch disagree");
    }
    auto target=reinterpret_cast<void*>(&native_callback);unsigned char saved[16];memcpy(saved,target,16);
    require(MH_Initialize()==MH_OK,"Hook init");require(MH_CreateHook(target,reinterpret_cast<void*>(&detour),reinterpret_cast<void**>(&original_callback))==MH_OK,"Hook create");require(MH_EnableHook(target)==MH_OK,"Hook enable");
    Receiver r{};r.marker=0xabcdef12;auto callback=reinterpret_cast<Callback>(target);
    for(unsigned i=0;i<50000;++i)callback(&r,before);
    require(calls==50000&&r.received==before&&r.marker==0xabcdef12,"Skin callback ABI differs");
    require(MH_DisableHook(target)==MH_OK&&memcmp(saved,target,16)==0,"Skin callback restoration differs");MH_Uninitialize();
    std::cout<<"PASS: arm/weapon pitch, hierarchy bounds, head hiding, idle preservation, 50000 skin callback ABI calls and hook restoration\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
