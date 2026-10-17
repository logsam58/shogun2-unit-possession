#include "adapter.cpp"
static uintptr_t fixture_unit90,fixture_record90;static float fixture_control90=1;
static unsigned difficulty_calls90,chance_calls90;static uint32_t fixture_rng90=0x12345678;
static void __fastcall difficulty90(void*,void*,void*,unsigned char* human,uint32_t* difficulty){++difficulty_calls90;*human=1;*difficulty=0;}
static void* __fastcall unit90(void*,void*){return reinterpret_cast<void*>(fixture_unit90);}
static unsigned __fastcall kind90(void*,void*){return 1;}
static float __fastcall core90(void*,void*){return float(field<unsigned>(fixture_unit90,0x234));}
static float __fastcall control90(void*,void*){return fixture_control90;}
static float __cdecl chance90(void*,void* attacker,void* projectile,unsigned debug){
 ++chance_calls90;require(debug==0,"Accuracy preview must not enable native tracing");auto a=static_cast<AccuracyProvider90*>(attacker),p=static_cast<AccuracyProvider90*>(projectile);
 require(a->vtable==base+0x15e736c&&p->vtable==base+0x15e73bc&&p->human==1,"Exact native provider layouts");
 require(field<float>(p->object,0x68)==field<float>(fixture_record90,0xe8),"Aim marker distance leaked into accuracy");
 return std::clamp(float(field<unsigned>(fixture_unit90,0x234))*.01f*fixture_control90,.01f,1.f);
}
static void jump90(unsigned rva,void* target){auto p=reinterpret_cast<unsigned char*>(base+rva);p[0]=0xe9;auto delta=int32_t(reinterpret_cast<uintptr_t>(target)-reinterpret_cast<uintptr_t>(p+5));memcpy(p+1,&delta,4);}
int main(){try{
 base=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,0x1d88000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));require(base!=0,"Fixture image allocation");
 std::array<unsigned char,0x1000> common{},pool{},actor{},unit_data{},record_data{};std::array<unsigned char,0x80> launch{};
 fixture_unit90=reinterpret_cast<uintptr_t>(unit_data.data());fixture_record90=reinterpret_cast<uintptr_t>(record_data.data());
 auto a=reinterpret_cast<uintptr_t>(actor.data()),c=reinterpret_cast<uintptr_t>(common.data()),l=reinterpret_cast<uintptr_t>(launch.data());
 field<uintptr_t>(c,0x9c)=reinterpret_cast<uintptr_t>(pool.data());field<uintptr_t>(a,0)=base+0x15dfec8;field<uintptr_t>(base+0x15dfec8,0xa8)=reinterpret_cast<uintptr_t>(&unit90);
 field<float>(fixture_record90,0xe8)=100;field<float>(fixture_record90,0xec)=16;field<float>(base,0x179af38)=1;field<float>(base,0x179af3c)=.707106769f;field<unsigned>(fixture_unit90,0x748)=14;
 field<float>(l,0x68)=6000;field<unsigned>(fixture_unit90,0x234)=20;
 jump90(0x4324f0,reinterpret_cast<void*>(&difficulty90));jump90(0xb92b60,reinterpret_cast<void*>(&chance90));jump90(0x458e10,reinterpret_cast<void*>(&kind90));jump90(0x43d500,reinterpret_cast<void*>(&core90));jump90(0x43d250,reinterpret_cast<void*>(&control90));
 accuracy90::Envelope low{},high{},debuff{},rank_extra{};
 require(measure_accuracy90(a,c,fixture_record90,launch.data(),low),"Lower accuracy native provider measurement");
 field<unsigned>(fixture_unit90,0x234)=80;require(measure_accuracy90(a,c,fixture_record90,launch.data(),high),"Higher effective accuracy");
 require(high.half_angle<low.half_angle&&high.core==80&&low.core==20,"Effective rank/buff stat changes narrow the cone");
 fixture_control90=.5f;require(measure_accuracy90(a,c,fixture_record90,launch.data(),debuff)&&debuff.half_angle>high.half_angle,"Native control/debuff factor widens spread");
 fixture_control90=1;field<unsigned>(l,0x44)=5;require(measure_accuracy90(a,c,fixture_record90,launch.data(),rank_extra)&&rank_extra.half_angle<high.half_angle,"Native launch variance adjustment preserved");
 require(fixture_rng90==0x12345678&&difficulty_calls90==4&&chance_calls90==4,"Preview must not consume RNG or launch projectiles");
 accuracy90::Envelope e{};require(accuracy90::envelope(16,.8f,100,1,1,e)&&std::abs(e.half_angle-std::atan(std::sqrt(20.f)/200))<.000001f,"Native full widths converted to HALF cone angle");
 float pixels=0;require(accuracy90::projected_radius(e.half_angle,1.3f,1080,pixels)&&std::abs(pixels-702*std::tan(e.half_angle))<.00001f,"Exact native projection radius");
 float higher=0;require(accuracy90::projected_radius(low.half_angle,1.3f,1080,higher)&&higher>pixels,"Poor accuracy separates firearm bars");
 require(!accuracy90::envelope(NAN,.5f,100,1,1,e)&&!accuracy90::envelope(10,.5f,0,1,1,e),"Unknown/invalid data cannot imply pinpoint accuracy");
 accuracy90::Status s{};s.magic=accuracy90::Magic;s.version=90;s.size=sizeof s;s.pid=7;s.lease=2;s.actor=3;s.unit=4;s.valid=1;s.tick=100;s.half_angle=.02f;s.projection_y=1.3f;
 require(accuracy90::valid(s,7,2,3,4,150)&&!accuracy90::valid(s,7,3,3,4,150)&&!accuracy90::valid(s,7,2,3,4,1100),"Stale or foreign reticle sample rejected");
 s.simulation_tick=9;require(accuracy90::valid(s,7,2,3,4,5000,9)&&!accuracy90::valid(s,7,2,3,4,5000,10),"Paused simulation retains its last native sample, advancing stale simulation rejects");
 static_assert(sizeof(accuracy90::Status)==112,"Accuracy channel ABI");
 VirtualFree(reinterpret_cast<void*>(base),0,MEM_RELEASE);std::cout<<"PASS: production native-provider ABI, current effective stats/control, calibration distance independence, launch variance, zero preview RNG/shot effects, half-angle conversion, projected reticle radius and stale sample guards.\n";return 0;
 }catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
