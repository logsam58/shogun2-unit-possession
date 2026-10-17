#include "identity.h"
#include "aim_geometry.h"
#include "manual_dispersion.h"
#include "../vendor/include/MinHook.h"
struct MockLaunch {
    unsigned char native_header[0x50]{};float velocity[3]{0,0,200};
    uint32_t calls=0;float horizontal=0,vertical=0,multiplier=0,deviation=0;
    __declspec(noinline) void scatter(uint32_t* rng,float h,float v,float m,float d){
        horizontal=h;vertical=v;multiplier=m;deviation=d;++calls;++*rng;++*rng;
    }
};
template<class T>static void* address(T m){static_assert(sizeof(m)==4,"x86 member ABI");void* p;memcpy(&p,&m,4);return p;}
static void* scatter_target;
static MockLaunch thread_launch;
static DWORD WINAPI other_thread(void*){
    uint32_t rng=0;reinterpret_cast<Dispersion>(scatter_target)(&thread_launch,&rng,0.3f,0.4f,1.25f,2.0f);
    return rng==2&&thread_launch.horizontal==0.3f&&thread_launch.vertical==0.4f&&thread_launch.deviation==2.0f?0:1;
}
int main(){try{
    float eye[3]={10,2,30},target[3]={10,2,31};ViewRay ray{};float aim[3];
    require(camera_ray(eye,target,ray),"camera forward");crosshair_target(ray,aim,80);
    require(aim[0]==10&&aim[1]==2&&aim[2]==110,"center ray +Z");
    target[0]=11;target[1]=3;target[2]=30;
    require(camera_ray(eye,target,ray),"pitched rotated camera");crosshair_target(ray,aim,80);
    require(std::abs(aim[0]-aim[1]-8)<0.0001f&&aim[2]==30,"pitched camera target");
    float muzzle[3]={10.3f,1.6f,30.7f},velocity[3],length=0;
    for(unsigned i=0;i<3;++i){velocity[i]=aim[i]-muzzle[i];length+=velocity[i]*velocity[i];}
    length=std::sqrt(length);
    for(unsigned i=0;i<3;++i){velocity[i]=velocity[i]/length*200;require(std::abs(muzzle[i]+velocity[i]*length/200-aim[i])<0.0001f,"muzzle convergence on camera ray");}
    require(!camera_ray(eye,eye,ray),"degenerate ray rejected");target[0]=NAN;require(!camera_ray(eye,target,ray),"invalid camera rejected");
    target[0]=10;target[1]=2;target[2]=31;require(camera_ray(eye,target,ray),"rifle camera");
    crosshair_target(ray,aim,crosshair_distance(60));
    length=0;for(unsigned i=0;i<3;++i)length+=(aim[i]-muzzle[i])*(aim[i]-muzzle[i]);
    require(length>=25000000&&crosshair_distance(3)==6000,"native direct-fire distance gate for both weapons");
    for(unsigned record:{3u,60u})for(float yaw:{-3.0f,0.0f,1.7f})for(float pitch:{-LOOK_PITCH_LIMIT,0.0f,LOOK_PITCH_LIMIT}){
        ray.origin[0]=eye[0];ray.origin[1]=eye[1];ray.origin[2]=eye[2];
        ray.direction[0]=std::sin(yaw)*std::cos(pitch);ray.direction[1]=std::sin(pitch);ray.direction[2]=std::cos(yaw)*std::cos(pitch);
        crosshair_target(ray,aim,crosshair_distance(record));float n=0,dot=0;
        for(unsigned i=0;i<3;++i){velocity[i]=aim[i]-muzzle[i];n+=velocity[i]*velocity[i];}
        require(n>=25000000,"direct gate at high pitch");n=std::sqrt(n);
        for(unsigned i=0;i<3;++i)dot+=velocity[i]/n*ray.direction[i];
        require(dot>0.999999f,"manual bow/matchlock launch ray convergence");
    }
    for(float yaw:{-3.0f,0.0f,1.7f})for(float pitch:{-LOOK_PITCH_LIMIT,0.0f,LOOK_PITCH_LIMIT}){
        float up[3];ray.direction[0]=std::sin(yaw)*std::cos(pitch);ray.direction[1]=std::sin(pitch);ray.direction[2]=std::cos(yaw)*std::cos(pitch);
        require(level_camera_up(ray,up),"full pitch camera up");
        float dot=0,norm=0;for(unsigned i=0;i<3;++i){dot+=up[i]*ray.direction[i];norm+=up[i]*up[i];}
        require(std::abs(dot)<0.000001f&&std::abs(norm-1)<0.000001f&&up[1]>0,"orthonormal camera without roll or pole flip");
    }
    scatter_target=address(&MockLaunch::scatter);unsigned char saved[16];memcpy(saved,scatter_target,16);
    require(MH_Initialize()==MH_OK,"init");
    require(MH_CreateHook(scatter_target,reinterpret_cast<void*>(&dispersion_hook),reinterpret_cast<void**>(&original_dispersion))==MH_OK,"dispersion hook");
    require(MH_EnableHook(scatter_target)==MH_OK,"enable");
    MockLaunch manual,ai;uint32_t rng=0;
    for(unsigned i=0;i<10000;++i){
        manual.velocity[0]=manual.velocity[1]=0;manual.velocity[2]=200;
        manual_dispersion_launch=&manual;
        reinterpret_cast<Dispersion>(scatter_target)(&manual,&rng,0.3f,0.4f,1.25f,2.0f);
        require(manual.horizontal==.3f&&manual.vertical==.4f&&manual.multiplier==1.25f&&manual.deviation==2,"native accuracy inputs preserved");
        require(std::abs(std::sqrt(manual.velocity[0]*manual.velocity[0]+manual.velocity[1]*manual.velocity[1]+manual.velocity[2]*manual.velocity[2])-200)<.0001f,"Cone keeps native projectile speed");
        require(manual.velocity[2]/200>=std::cos(.5f)-.00001f,"Manual velocity stays in accuracy cone");
        reinterpret_cast<Dispersion>(scatter_target)(&ai,&rng,0.3f,0.4f,1.25f,2.0f);
        require(ai.horizontal==0.3f&&ai.vertical==0.4f&&ai.multiplier==1.25f&&ai.deviation==2.0f,"other launch unchanged");
        manual_dispersion_launch=nullptr;
    }
    require(rng==40000&&manual.calls==10000&&ai.calls==10000&&manual_spread_removed==0&&cone_metrics.shots==10000&&cone_metrics.errors==0,"exact forwarding and RNG calls");
    manual_dispersion_launch=&thread_launch;
    Handle worker(CreateThread(nullptr,0,other_thread,nullptr,0,nullptr));require(worker.h!=nullptr,"thread");
    require(WaitForSingleObject(worker.h,5000)==WAIT_OBJECT_0,"thread completion");DWORD code=1;
    require(GetExitCodeThread(worker.h,&code)&&code==0,"manual scope stays on originating thread");manual_dispersion_launch=nullptr;
    require(MH_DisableHook(scatter_target)==MH_OK&&memcmp(saved,scatter_target,16)==0,"dispersion restoration");
    reinterpret_cast<Dispersion>(scatter_target)(&ai,&rng,0.7f,0.8f,1,3);
    require(ai.horizontal==0.7f&&ai.deviation==3,"native call after disable");
    std::cout<<"PASS: camera/muzzle convergence, invalid ray rejection, 20000 dispersion ABI calls, native accuracy inputs preserved, manual-only conical velocities, exact RNG forwarding, thread isolation and restored bytes.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
