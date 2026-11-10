// Native camera integration regression: real vtable dispatch, real3290a0
// frame body, real1374c60 interpolation, real134fe50 basis, real render getters.
// Explicit fixture providers replace logging, altitude/environment and clock.
// These tests never attach to, patch, send input to, or start the real game.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "../../../native/outputs/shogun2-accuracy90-preview/vendor/include/MinHook.h"
#define NAVAL_REAL_HOOKS128
#define NAVAL_CAMERA_FIXTURE128
#include "verify_naval_control127.cpp"
static uintptr_t image128;
static unsigned frame_calls128,original_checks128;
static bool check_original128;
static naval127::Pose expected_native128;
static void* __fastcall log_text128(void* string,void*,const char*){
 at<unsigned>(string,0)=at<unsigned>(string,4)=0;
 at<void*>(string,8)=nullptr;at<uintptr_t>(string,12)=image128+0x17c7c4c;return string;
}
static void __fastcall log_event128(void*,void*,void*,void*){++frame_calls128;}
static void __fastcall altitude128(void*,void*,float){}
static float __fastcall clock128(void*,void*){return 0.f;}
static void __fastcall noop128(void*,void*){}
static void __fastcall terrain128(void* camera,void*){
 if(check_original128){auto p=naval127::read_pose(reinterpret_cast<uintptr_t>(camera));
  require(!memcmp(&p,&expected_native128,sizeof p),"Native terrain clamps saw possessed camera instead of preserved RTS pose");++original_checks128;}
}
using Frame128=bool(__thiscall*)(void*,float,float);
using Getter128=void(__thiscall*)(void*,float*);
static bool frame128(World127& w){
 auto v=at<uintptr_t>(w.camera.data(),0);auto update=at<Frame128>(reinterpret_cast<void*>(v),0x13c);
 unsigned before{},after{};__asm mov before,esp
 bool result=update(w.camera.data(),.016f,.5f);
 __asm mov after,esp
 require(before==after,"Native camera frame stack cleanup mismatch");return result;
}
static void getters128(World127& w,float* eye,float* target){
 auto v=reinterpret_cast<void*>(at<uintptr_t>(w.camera.data(),0));
 at<Getter128>(v,0x120)(w.camera.data(),eye);at<Getter128>(v,0x124)(w.camera.data(),target);
}
static void check_view128(World127& w){
 float eye[3]{},target[3]{};getters128(w,eye,target);
 require(!memcmp(eye,naval127::control.shown.eye,12)&&!memcmp(target,naval127::control.shown.target,12),"Rendered pose differs from possessed sailor view");
 require(!memcmp(w.camera.data()+0x184,w.camera.data()+0x190,12)&&!memcmp(w.camera.data()+0x19c,w.camera.data()+0x1a8,12),"Native smoothing destinations retained overhead pose");
 float length=0;for(unsigned n=0;n<3;++n){float f=at<float>(w.camera.data(),0x64+4*n);require(std::isfinite(f),"Native camera basis not finite");length+=f*f;}
 require(std::fabs(length-1.f)<.001f,"Native basis was not refreshed");
}
static LONG WINAPI fixture_error128(EXCEPTION_POINTERS* p){
 fprintf(stderr,"FIXTURE exception=%08lx eip=%08lx engine_rva=%08lx address=%08lx ecx=%08lx edx=%08lx\n",p->ExceptionRecord->ExceptionCode,p->ContextRecord->Eip,p->ContextRecord->Eip-image128,p->ExceptionRecord->ExceptionInformation[1],p->ContextRecord->Ecx,p->ContextRecord->Edx);return EXCEPTION_EXECUTE_HANDLER;
}
int wmain(int argc,wchar_t** argv){try{
 std::cout<<std::unitbuf;SetErrorMode(SEM_NOGPFAULTERRORBOX);SetUnhandledExceptionFilter(fixture_error128);
 require(argc==2,"Expected owned engine");Image image(argv[1]);World127 w(image);image128=base;
 image.jump(0xecc90,reinterpret_cast<void*>(&log_text128));image.jump(0x97f50,reinterpret_cast<void*>(&log_event128));
 image.jump(0x335640,reinterpret_cast<void*>(&altitude128));image.jump(0xa0b60,reinterpret_cast<void*>(&clock128));
 at<void*>(image.base,0x15d1a74+0x110)=reinterpret_cast<void*>(&noop128);
 at<void*>(image.base,0x15d1a74+0x10c)=reinterpret_cast<void*>(&terrain128);
 at<unsigned char>(w.camera.data(),0x150)=1;
 // Real pose/basis methods. World127's older direct-helper fixture stub is removed.
 at<void*>(image.base,0x15d1a74+0xf4)=image.base+0x134fe50;
 auto saved=naval127::read_pose(reinterpret_cast<uintptr_t>(w.camera.data()));expected_native128=saved;
 for(unsigned off:{0x184u,0x190u})memcpy(w.camera.data()+off,saved.eye,12);
 for(unsigned off:{0x19cu,0x1a8u})memcpy(w.camera.data()+off,saved.target,12);
 auto baseline=w.camera;check_original128=true;
 auto native_hash=sha(image.base+0x3290a0,0x163);
 require(frame128(w)&&frame_calls128==1,"Unhooked native overhead frame did not run");
 require(MH_Initialize()==MH_OK,"MinHook initialize");naval127::install();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable isolated fixture hooks");
 require(w.begin(),"Sailor acquire");naval127::sample(FORWARD,true);w.tick();
 require(frame128(w),"Owned native frame rejected view");check_view128(w);
 require(naval127::control.shown.eye[2]!=saved.eye[2],"Regression: helper was not reached by real native camera dispatch");
 passed("native overhead vtable dispatch reaches production hook and render getters follow sailor");
 for(unsigned n=0;n<120;++n){at<float>(w.sailor.p(),0x48)=n*.01f;rendered_x129=n*.01f;w.tick();naval127::look(.5f,.1f);require(frame128(w),"Follow frame");check_view128(w);}
 require(original_checks128==frame_calls128,"Some original frame missed preserved overhead state");
 passed("120 moving-camera frames preserve native RTS state and update rendered interpolation buffers");
 naval127::release();require(frame128(w),"Paused release frame");
 require(!naval127::camera_pending&&!naval127::input_active(),"Paused return left controls or camera pending");
 auto restored=naval127::read_pose(reinterpret_cast<uintptr_t>(w.camera.data()));require(!memcmp(&restored,&saved,sizeof saved),"Camera return numeric pose mismatch");
 require(!memcmp(w.camera.data()+0x184,baseline.data()+0x184,48),"Camera return smoothing state mismatch");
 w.tick();require(!naval127::active()&&naval127::control.returns==1,"Duty did not return after paused release");
 passed("release restores native camera before simulation resumes and restarts retained duty");
 for(unsigned n=0;n<100;++n){require(w.begin(),"Reacquire after camera return");naval127::sample(FORWARD,true);w.tick();frame128(w);naval127::release();w.tick();frame128(w);
  require(!naval127::active()&&!naval127::camera_pending,"Repeated return stuck");}
 passed("100 actual camera dispatch acquire-follow-release-reacquire cycles");
 require(w.begin(),"Never-rendered acquisition");w.tick();naval127::release();w.tick();
 require(w.begin(),"Unrendered previous view incorrectly blocks another acquisition");frame128(w);naval127::release();w.tick();frame128(w);
 passed("unrendered acquisition cannot leave permanent camera return barrier");
 require(w.begin(),"Retirement acquire");frame128(w);auto retired=w.camera;
 check_original128=false;at<void*>(w.root.data(),0x110)=nullptr;frame128(w);
 require(!naval127::active()&&!naval127::camera_pending,"World retirement retains lease");
 at<void*>(w.root.data(),0x110)=w.env.data();check_original128=false;
 passed("retired world drops camera ownership without old actor task writes");
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&MH_Uninitialize()==MH_OK,"Fixture hooks restore");
 require(sha(image.base+0x3290a0,0x163)==native_hash,"Camera code bytes not exactly restored");
 passed("native camera hook removal restores exact original code");
 std::cout<<"PASS "<<cases<<" camera integration cases; engine="<<image.digest<<"\nNo live gameplay proof; synthetic world with actual native frame dispatch and render getters.\n";return 0;
 }catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
