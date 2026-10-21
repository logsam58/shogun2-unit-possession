#include "../../../common/fixture_paths.h"
#define main previous_scope96
#include "verify_ranged_scope.cpp"
#undef main
static unsigned advances96;
static unsigned __fastcall advance96(void*,void*){++advances96;return 0x96;}
int main(){unsigned char* mapped=nullptr;try{
 std::ostringstream mute;auto* output=std::cout.rdbuf(mute.rdbuf());int prior=previous_scope96();std::cout.rdbuf(output);require(prior==0,"Production family/scope regression");
 std::wstring path=fixture_paths::engine();require(hash_file(path)==EXPECTED_HASH,"Exact native image");
 std::ifstream in(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());auto nt=reinterpret_cast<IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
 mapped=static_cast<unsigned char*>(VirtualAlloc(nullptr,nt->OptionalHeader.SizeOfImage,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(mapped!=nullptr,"Fixture image allocation");memcpy(mapped,image.data(),nt->OptionalHeader.SizeOfHeaders);
 auto sections=IMAGE_FIRST_SECTION(nt);for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto&s=sections[i];if(s.SizeOfRawData)memcpy(mapped+s.VirtualAddress,image.data()+s.PointerToRawData,s.SizeOfRawData);}

 DWORD protect;require(VirtualProtect(mapped,nt->OptionalHeader.SizeOfImage,PAGE_EXECUTE_READ,&protect)!=0,"Executable original image");FlushInstructionCache(GetCurrentProcess(),mapped,nt->OptionalHeader.SizeOfImage);
 uintptr_t task_vtable[32]{};task_vtable[0x4c/4]=task_vtable[0x68/4]=reinterpret_cast<uintptr_t>(&advance96);
 Actor player,peer,foreign;unit=0x100000;possessed=reinterpret_cast<uintptr_t>(&player);enabled=1;identity_valid=true;unsigned projectile=3;record=reinterpret_cast<uintptr_t>(&projectile);state->mode=POSSESSED;state->stop=state->release=0;
 for(auto actor:{&player,&peer,&foreign}){auto a=reinterpret_cast<uintptr_t>(actor);field<uintptr_t>(a,0x214)=actor==&foreign?0x200000:unit;field<unsigned char>(a,0xf0)=1;field<unsigned>(a,0x1f0)=8;field<uintptr_t>(a,0x474)=1;}
 Task task;task.vtable=reinterpret_cast<uintptr_t>(task_vtable);task.actor=possessed;using NativeWait=unsigned(__stdcall*)(void*);
 auto argument=reinterpret_cast<unsigned char*>(&task)+0x1c;NativeWait waits[2]={reinterpret_cast<NativeWait>(mapped+0x3b2e50),reinterpret_cast<NativeWait>(mapped+0x3b2ad0)};
 for(auto wait:waits)require(wait(argument)==0,"Original active bow animation must stall both newly covered native waits");
 auto lower=mapped+0x3d3e60,idle=mapped+0x3d3fc0;unsigned char lower_bytes[16],idle_bytes[16];memcpy(lower_bytes,lower,16);memcpy(idle_bytes,idle,16);
 require(MH_Initialize()==MH_OK&&MH_CreateHook(lower,reinterpret_cast<void*>(&weapon_task_lower_gate96),reinterpret_cast<void**>(&original_task_lower_gate96))==MH_OK&&MH_CreateHook(idle,reinterpret_cast<void*>(&weapon_task_idle_gate96),reinterpret_cast<void**>(&original_task_idle_gate96))==MH_OK&&MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Production96 completion hooks");
 unsigned calls=0;
 for(unsigned family:{3u,60u,4u,15u,47u,82u})for(bool mounted:{false,true})for(unsigned mode=0;mode<(mounted?5u:18u);++mode){
  projectile=family;mounted_weapon=mounted;field<unsigned>(possessed,0x1f0)=mounted?29+mode:mode;
  for(unsigned repeat=0;repeat<100;++repeat)for(auto wait:waits){
   task.actor=possessed;auto before=task;require(wait(argument)==0x96&&!memcmp(&task,&before,sizeof task),"Owned native ranged wait stalled/mutated task");++calls;
   for(auto ai:{&peer,&foreign})for(bool animation:{false,true}){task.actor=reinterpret_cast<uintptr_t>(ai);field<uintptr_t>(task.actor,0x474)=animation?1:0;before=task;require(wait(argument)==(animation?0u:0x96u)&&!memcmp(&task,&before,sizeof task),"Peer/foreign animation completion changed");++calls;}
  }
 }
 task.actor=possessed;field<uintptr_t>(possessed,0x474)=1;
 for(unsigned fail=0;fail<7;++fail){state->stop=fail==0;state->release=fail==1;enabled=fail!=2;identity_valid=fail!=3;state->mode=fail==4?IDLE:POSSESSED;field<unsigned char>(possessed,0xf0)=fail==5?0:1;record=fail==6?0:reinterpret_cast<uintptr_t>(&projectile);for(auto wait:waits){require(wait(argument)==0,"Invalid ownership bypassed native wait");++calls;}}
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&!memcmp(lower,lower_bytes,16)&&!memcmp(idle,idle_bytes,16),"Exact96 hook restoration");
 MH_Uninitialize();VirtualFree(mapped,0,MEM_RELEASE);mapped=nullptr;
 std::cout<<"PASS: native96 two unmodified wrappers, task queries and actor animation predicate; "<<calls<<" calls across six synthetic projectile records, all18 infantry/five mounted modes, exact peer/foreign behavior, ownership loss, task bytes, x86 RET4 and restoration. No injected game.\n";return 0;
}catch(const std::exception&e){if(mapped)VirtualFree(mapped,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
