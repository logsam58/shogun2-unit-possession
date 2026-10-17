#include "identity.h"
#include "mounted_shot87.h"
#include "native_gate_fixture87.h"
#include "shot_capture87.h"
#include <limits>
int main(){try{
    auto image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x1600000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));require(image,"Native gate fixture allocation");
    memcpy(image+0x420050,NATIVE_GATE87,sizeof NATIVE_GATE87);
    for(auto offset:NATIVE_GATE_RELOC87){auto& address=*reinterpret_cast<uint32_t*>(image+0x420050+offset);address=address-0x10000000+uint32_t(image);}
    for(unsigned i=0;i<3;++i)memcpy(image+NATIVE_CONSTANT_RVA87[i],NATIVE_CONSTANT_BITS87+i,4);
    image[0x447720]=0x31;image[0x447721]=0xc0;image[0x447722]=0xc3;
    // Return success after the real actor arc gate. No production game bytes are modified.
    const unsigned char accepted[]={0xb0,1,0xe9,0xf1,0xff,0xff,0xff};memcpy(image+0x4201e6,accepted,sizeof accepted);
    using Gate=bool(__thiscall*)(void*,const float*);auto gate=reinterpret_cast<Gate>(image+0x420050);
    alignas(16) unsigned char actor[0x100]{},context[0xc0]{},debug[0x2b00]{},launch[0x80]{};
    *reinterpret_cast<void**>(context+0xb4)=debug;*reinterpret_cast<void**>(launch)=context;*reinterpret_cast<void**>(launch+0x18)=actor;
    for(int heading:{0,12000,23483,49699,65535})for(float degrees:{-179.f,-100.f,-92.58f,-90.1f,-89.5f,0.f,49.41f,89.57f,90.1f,92.58f,100.f,179.f}){
        *reinterpret_cast<uint16_t*>(actor+0x64)=uint16_t(heading);
        float a=heading*(6.28318530718f/65536.f)+degrees*.01745329252f,point[3]={std::sin(a)*6000,0,std::cos(a)*6000};
        *reinterpret_cast<float*>(launch+0x14)=mountedshot87::native_arc(false);
        require(gate(launch,point)==(std::abs(degrees)<90),"Native infantry arc behavior changed or captured failure not reproduced");
        *reinterpret_cast<float*>(launch+0x14)=mountedshot87::native_arc(true);
        require(gate(launch,point),"Mounted manual release still rejected by actor-bearing gate");
    }
    VirtualFree(image,0,MEM_RELEASE);
    const unsigned weapons[3]={1,2,3};
    for(const auto& shot:SHOTS87)for(float yaw:{-1.74532925f,-.4f,0.f,1.74532925f})for(float pitch:{-.5f,0.f,.5f}){
        float m[48*12],before[48*12],world[12]={1,0,0,0,0,1,0,0,0,0,1,0};memcpy(m,SEATED_BOW_MODEL,sizeof m);
        const float pivot[3]={0,0,0};yaw_matrix(world,pivot,yaw);pitch_matrix(world,pivot,pitch);
        float inverse[12];require(affine_pose_inverse(world,inverse),"Fixture world inverse");
        float local[3]{};for(unsigned r=0;r<3;++r)for(unsigned c=0;c<3;++c)local[r]+=inverse[r*4+c]*shot.socket[c];
        for(unsigned r=0;r<3;++r)m[36+r*4+3]=local[r];memcpy(before,m,sizeof m);
        ViewRay ray{};memcpy(ray.origin,shot.eye,12);memcpy(ray.direction,shot.direction,12);
        float correction=0;require(mountedshot87::align_bow(m,48,SEATED_BOW_PARENTS,15,18,weapons,world,ray,correction),"Captured mounted socket rejected");
        float socket[3];require(arrow_attachment_world(m+36,world,socket),"Corrected visible socket unavailable");
        float depth=0;for(unsigned i=0;i<3;++i)depth+=(socket[i]-ray.origin[i])*ray.direction[i];
        float error=0;for(unsigned i=0;i<3;++i){float d=socket[i]-ray.origin[i]-depth*ray.direction[i];error+=d*d;}
        require(error<.00000002f&&depth>=.249f&&depth<=1.501f,"Visible arrow and crosshair diverged");
        for(unsigned i=0;i<48;++i)if(!bone_descends(i,15,SEATED_BOW_PARENTS,48)&&!bone_descends(i,18,SEATED_BOW_PARENTS,48)&&i!=1&&i!=3)
            require(!memcmp(before+i*12,m+i*12,48),"Saddle/legs/torso/head/holstered sword changed");
        memcpy(before,m,sizeof m);require(mountedshot87::align_bow(m,48,SEATED_BOW_PARENTS,15,18,weapons,world,ray,correction),"Repeated correction rejected");
        for(unsigned i=0;i<48*12;++i)require(std::abs(before[i]-m[i])<.0001f,"Socket alignment accumulates");
        ViewRay invalid=ray;invalid.origin[0]+=100;memcpy(before,m,sizeof m);
        require(!mountedshot87::align_bow(m,48,SEATED_BOW_PARENTS,15,18,weapons,world,invalid,correction)&&!memcmp(before,m,sizeof m),"Excessive correction wrote owned render");
        invalid=ray;invalid.direction[0]=std::numeric_limits<float>::quiet_NaN();
        require(!mountedshot87::align_bow(m,48,SEATED_BOW_PARENTS,15,18,weapons,world,invalid,correction)&&!memcmp(before,m,sizeof m),"Invalid ray wrote owned render");
    }
    std::cout<<"PASS: original x86 native firing gate reproduces captured >90-degree failures; mounted correction passes across actor headings; infantry arc unchanged. Seven captured sockets align visible bow/arrow with reticle across 12 world rotations; lower body/sword untouched; bounds, finite and repeat guards.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
