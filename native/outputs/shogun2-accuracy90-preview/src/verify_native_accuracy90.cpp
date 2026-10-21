#include "../../../common/fixture_paths.h"
#include "identity.h"
#include "accuracy_geometry90.h"
#include "first_person_presentation101.h"
// Execute only preserved pure math/chance bytes in this fixture process, with
// synthetic virtual providers. No DLL loading/initialization or game launch.
struct Provider90 {uintptr_t* table;float core,control,bonus,visibility,angle,distance,calibration;bool extra,cover;};
static float __fastcall p_core(void* p,void*){return static_cast<Provider90*>(p)->core;}
static float __fastcall p_control(void* p,void*){return static_cast<Provider90*>(p)->control;}
static float __fastcall p_bonus(void* p,void*){return static_cast<Provider90*>(p)->bonus;}
static float __fastcall p_visibility(void* p,void*){return static_cast<Provider90*>(p)->visibility;}
static float __fastcall p_angle(void* p,void*){return static_cast<Provider90*>(p)->angle;}
static float __fastcall p_distance(void* p,void*){return static_cast<Provider90*>(p)->distance;}
static float __fastcall p_calibration(void* p,void*){return static_cast<Provider90*>(p)->calibration;}
static unsigned __fastcall p_extra(void* p,void*){return static_cast<Provider90*>(p)->extra?1:0;}
static unsigned __fastcall p_cover(void* p,void*){return static_cast<Provider90*>(p)->cover?1:0;}
static float __fastcall p_near(void* p,void*){return *reinterpret_cast<float*>(static_cast<unsigned char*>(p)+0x34);}
static float __fastcall p_far(void*,void*){return 1000.f;}
static int __fastcall global_extra(void*,void*){return 10;}
__declspec(noinline) static float simd_atan(void* code,float value){float result;__asm {
 movss xmm0,value
 call code
 movss result,xmm0
 }return result;}
int main(){unsigned char* mapped=nullptr;try{
 std::wstring path=fixture_paths::engine();require(hash_file(path)==EXPECTED_HASH,"Exact native fixture image");
 std::ifstream in(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());auto nt=reinterpret_cast<IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
 mapped=static_cast<unsigned char*>(VirtualAlloc(nullptr,nt->OptionalHeader.SizeOfImage,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(mapped!=nullptr,"Native fixture allocation");memcpy(mapped,image.data(),nt->OptionalHeader.SizeOfHeaders);
 auto sections=IMAGE_FIRST_SECTION(nt);for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto&s=sections[i];if(s.SizeOfRawData){require(s.PointerToRawData+s.SizeOfRawData<=image.size(),"Native section bounds");memcpy(mapped+s.VirtualAddress,image.data()+s.PointerToRawData,s.SizeOfRawData);}}
 uint32_t delta=uint32_t(reinterpret_cast<uintptr_t>(mapped)-nt->OptionalHeader.ImageBase);auto reloc=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];unsigned offset=0;
 while(offset<reloc.Size){auto block=reinterpret_cast<IMAGE_BASE_RELOCATION*>(mapped+reloc.VirtualAddress+offset);require(block->SizeOfBlock>=8&&offset+block->SizeOfBlock<=reloc.Size,"Relocation bounds");auto entries=reinterpret_cast<uint16_t*>(block+1);
  for(unsigned i=0;i<(block->SizeOfBlock-8)/2;++i)if(entries[i]>>12==IMAGE_REL_BASED_HIGHLOW)*reinterpret_cast<uint32_t*>(mapped+block->VirtualAddress+(entries[i]&4095))+=delta;offset+=block->SizeOfBlock;}
 DWORD old=0;require(VirtualProtect(mapped,nt->OptionalHeader.SizeOfImage,PAGE_EXECUTE_READ,&old)!=0,"Read-only native fixture image");FlushInstructionCache(GetCurrentProcess(),mapped,nt->OptionalHeader.SizeOfImage);
 for(float x:{0.f,.000001f,.01f,.1f,.5f,1.f,2.f,10.f})require(std::abs(simd_atan(mapped+0x151324b,x)-std::atan(x))<.000001f,"Native SIMD conversion is atan, not raw width");
 uintptr_t gt[16]{},at[16]{},pt[8]{};gt[13]=reinterpret_cast<uintptr_t>(&global_extra);at[7]=reinterpret_cast<uintptr_t>(&p_core);at[5]=reinterpret_cast<uintptr_t>(&p_extra);at[15]=reinterpret_cast<uintptr_t>(&p_control);
 pt[0]=reinterpret_cast<uintptr_t>(&p_distance);pt[1]=reinterpret_cast<uintptr_t>(&p_bonus);pt[3]=reinterpret_cast<uintptr_t>(&p_visibility);pt[4]=reinterpret_cast<uintptr_t>(&p_angle);pt[5]=reinterpret_cast<uintptr_t>(&p_cover);pt[6]=reinterpret_cast<uintptr_t>(&p_calibration);
 Provider90 global{gt},attacker{at},projectile{pt};projectile.distance=projectile.calibration=100;
 using Chance=float(__cdecl*)(void*,void*,void*,unsigned);auto chance=reinterpret_cast<Chance>(mapped+0xb92b60);unsigned checks=0;
 for(float core:{0.f,20.f,50.f,80.f,200.f})for(float control:{.5f,1.f,1.25f})for(float bonus:{0.f,15.f})for(float visibility:{.75f,1.f})for(bool extra:{false,true})for(bool cover:{false,true}){
  attacker.core=core;attacker.control=control;attacker.extra=extra;projectile.bonus=bonus;projectile.visibility=visibility;projectile.angle=.9f;projectile.cover=cover;
  float expected=std::clamp(std::max(0.f,core+bonus+(extra?10.f:0.f))*control*visibility*.9f*.01f-(cover?.2f:0.f),.01f,1.f);
  require(std::abs(chance(&global,&attacker,&projectile,0)-expected)<.000001f,"Original native chance-to-hit modifiers differ");++checks;
 }
 uintptr_t camera_table[64]{};camera_table[0x3c/4]=reinterpret_cast<uintptr_t>(mapped+0x340940);camera_table[0x44/4]=reinterpret_cast<uintptr_t>(&p_near);camera_table[0x48/4]=reinterpret_cast<uintptr_t>(&p_far);camera_table[0xd8/4]=reinterpret_cast<uintptr_t>(mapped+0x134eb80);
 std::array<unsigned char,0x180> camera{};*reinterpret_cast<uintptr_t*>(camera.data())=reinterpret_cast<uintptr_t>(camera_table);
 *reinterpret_cast<float*>(camera.data()+0x34)=.1f;
 using Projection=void*(__thiscall*)(void*,float*);auto projection=reinterpret_cast<Projection>(mapped+0x1367e20);
 for(float fov:{.4f,1.309f})for(float aspect:{4.f/3,16.f/9,21.f/9}){*reinterpret_cast<float*>(camera.data()+0x24)=fov;*reinterpret_cast<float*>(camera.data()+0x30)=aspect;float matrix[16]{};
  projection(camera.data(),matrix);require(std::abs(matrix[5]-1/std::tan(fov*.5f))<.00001f&&std::abs(matrix[0]*aspect-matrix[5])<.00001f,"Exact native projection wrapper/ABI and scale");}
 for(float fov:{.4f,1.309f})for(float aspect:{4.f/3,16.f/9,21.f/9}){
  auto& near_value=*reinterpret_cast<float*>(camera.data()+0x34);near_value=.15f;
  *reinterpret_cast<float*>(camera.data()+0x24)=fov;*reinterpret_cast<float*>(camera.data()+0x30)=aspect;
  float stock[16]{},close[16]{};projection(camera.data(),stock);
  firstperson101::NearClip scope;require(scope.capture(near_value,1000)&&scope.apply(near_value),"Owned near clip capture/apply");projection(camera.data(),close);
  for(float z:{.025f,.04f,.08f,.14f}){
   require(stock[10]*z+stock[11]<0,"Stock .15 plane did not reproduce clipped close geometry");
   float depth=close[10]*z+close[11],w=close[14]*z+close[15];require(depth>=0&&depth<=w,"Owned near plane still clipped close geometry");
  }
  require(stock[0]==close[0]&&stock[5]==close[5]&&scope.restore(near_value,true)&&near_value==.15f,"Near plane changed FOV/aspect or did not restore exactly");
 }
 VirtualFree(mapped,0,MEM_RELEASE);mapped=nullptr;
 std::cout<<"PASS: exact original native SIMD atan and "<<checks<<" native chance-to-hit calls with effective core, control, projectile bonus, visibility, angle, extra modifier, cover and clamping; native projection wrapper/scale, synthetic providers, immutable native image, no RNG/game initialization.\n";return 0;
 }catch(const std::exception&e){if(mapped)VirtualFree(mapped,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
