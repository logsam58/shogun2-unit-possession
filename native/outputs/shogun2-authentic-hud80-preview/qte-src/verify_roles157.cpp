#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cstdint>
static bool sparse157;static unsigned reads157;
static BOOL WINAPI read157(HANDLE p,LPCVOID a,LPVOID out,SIZE_T size,SIZE_T* got){
    ++reads157;if(sparse157&&(size==0x218||size==0x68c)){if(got)*got=0;return FALSE;}
    return ReadProcessMemory(p,a,out,size,got);
}
#define ReadProcessMemory read157
#include "playable_runtime.cpp"
#undef ReadProcessMemory
int main(){void* memory=nullptr;try{
    memory=VirtualAlloc(nullptr,0x1bc9000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory,"Fixture engine allocation");engine=uint32_t(memory);
    std::vector<uint8_t> root(0x122),env(0x28300),manager(0x900),camera(0x200),p(0x700),pu(0x1000),held(0x110),other(0x110),horse(0x700),riders(4);
    auto ptr=[](auto& x){return uint32_t(x.data());};auto put=[](uint32_t a,auto v){memcpy(reinterpret_cast<void*>(a),&v,sizeof v);};
    const auto a=ptr(p),u=ptr(pu),h=ptr(horse);
    put(engine+0x1bc8180,ptr(root));put(ptr(root)+0x110,ptr(env));put(ptr(env)+0x281fc,ptr(manager));put(ptr(manager)+0x8e0,ptr(camera));put(ptr(camera)+0x188,a);
    put(a+0xf0,uint8_t(1));put(a+0x28,ptr(env));put(a+0x688,ptr(held));put(a+0x4c,1.f);put(a+0x214,u);put(a+0x1c8,11u);put(u+4,7u);
    put(h,engine+0x15e8524);put(h+0xf0,uint8_t(1));put(h+0x28,ptr(env));put(h+0x4c,1.f);put(h+0x214,u);put(h+0x1c8,13u);
    put(h+0x6cc,1u);put(h+0x6d0,ptr(riders));put(ptr(riders),a);
    observation::PossessionPrefix lease{};lease.magic=0x31504353;lease.version=46;lease.pid=GetCurrentProcessId();lease.ready=1;lease.mode=2;
    lease.actor_id=11;lease.unit_id=7;lease.lease_generation=1;lease.heartbeat=GetTickCount64();possession=&lease;
    Status telemetry{};state=&telemetry;enabled=1;unsigned cases=0;
    for(auto role:{0x15dfec8u,0x15dbd68u,0x15dbfb4u,0x15dc590u,0x15dc1a8u,0x15dc39cu,0x15dcf54u,0x15dc978u,0x15dcb6cu,0x15dcd60u}){
        put(a,engine+role);
        for(bool sparse:{false,true})for(bool mounted:{false,true}){
            sparse157=sparse;put(a+0x210,mounted?h:0u);lease.heartbeat=GetTickCount64();reads157=0;
            uint64_t generation{};CameraScope121 scope{};
            require(fps_camera_actor(&generation,&scope)==a&&generation==1,"Actual QTE camera rejects supported soldier role");
            if(!sparse)require(scope.valid&&reads157==7,"Role support adds camera reads");
            require(fps_actor()==(mounted?h:a),"Actual QTE physical actor rejects soldier/mount");
            require(scoped(ptr(held))&&!scoped(ptr(other)),"Role QTE witness scope changed");
            NativeActor own{};require(combat_identity(reader(),a,engine,own)&&own.kind==Human,"Role combat identity rejected");
            float position[3]{};require(refresh_committed_pose(own,position),"Role native pose refresh rejected");
            put(a+0x1c8,12u);require(!fps_camera_actor()&&!refresh_committed_pose(own,position),"Stale role serial accepted");put(a+0x1c8,11u);
            put(u+4,8u);require(!fps_camera_actor(),"Foreign unit identity accepted");put(u+4,7u);
            put(a+0xf0,uint8_t(0));require(fps_camera_actor()==a&&!combat_identity(reader(),a,engine,own),"Confirmed death camera or live-combat guard changed");put(a+0xf0,uint8_t(1));
            put(a+0xf0,uint8_t(2));require(!fps_camera_actor(),"Invalid life value accepted");put(a+0xf0,uint8_t(1));
            lease.release=1;require(!fps_camera_actor(),"Released lease accepted");lease.release=0;
            lease.pid=0;require(!fps_camera_actor(),"Foreign process lease accepted");lease.pid=GetCurrentProcessId();
            lease.heartbeat=GetTickCount64()-1001;require(!fps_camera_actor(),"Stale heartbeat accepted");lease.heartbeat=GetTickCount64();
            if(mounted){put(ptr(riders),0u);require(!fps_actor(),"Missing reciprocal rider admitted");put(ptr(riders),a);}
            ++cases;
        }
    }
    sparse157=false;put(a+0x210,0u);
    for(auto role:{0x15e65f4u,0x15e8524u,0x15d1124u,0x15d0ed8u,0x15d1370u,0x15d04c8u,0x15dc784u,0x15dbb1cu,0x15dcf58u}){
        put(a,engine+role);require(!fps_camera_actor(),"Non-soldier/intermediate/unknown role granted camera scope");
    }
    VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: 40 production QTE role/mounted/sparse camera and combat cases; current serial/unit/process/heartbeat/death/witness/reciprocal mount guards; unchanged seven-read bulk camera path. No game process opened.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';if(memory)VirtualFree(memory,0,MEM_RELEASE);return 1;}}
