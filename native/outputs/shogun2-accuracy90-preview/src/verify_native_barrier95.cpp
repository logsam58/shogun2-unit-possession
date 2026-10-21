#include "../../../common/fixture_paths.h"
#define main previous_scope95
#include "verify_ranged_scope.cpp"
#undef main
static unsigned advanced95,aborted95,completed_calls95;
static bool __fastcall no_angle_abort95(void*,void*){return false;}
static unsigned __fastcall advance95(void*,void*){++advanced95;return 0x95;}
static unsigned __fastcall abort95(void*,void*){++aborted95;return 0x96;}
static bool __fastcall animation_complete95(void* actor,void*){++completed_calls95;return field<unsigned char>(reinterpret_cast<uintptr_t>(actor),0x11f)!=0;}
int main(){unsigned char* mapped=nullptr;try{
 std::ostringstream muted95;auto* output95=std::cout.rdbuf(muted95.rdbuf());int prior95=previous_scope95();std::cout.rdbuf(output95);require(prior95==0,"Prior actual-production scope regression");
 std::wstring path=fixture_paths::engine();require(hash_file(path)==EXPECTED_HASH,"Exact native image");
 std::ifstream in(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());auto nt=reinterpret_cast<IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
 mapped=static_cast<unsigned char*>(VirtualAlloc(nullptr,nt->OptionalHeader.SizeOfImage,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(mapped!=nullptr,"Fixture image allocation");memcpy(mapped,image.data(),nt->OptionalHeader.SizeOfHeaders);
 auto sections=IMAGE_FIRST_SECTION(nt);for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto&s=sections[i];if(s.SizeOfRawData)memcpy(mapped+s.VirtualAddress,image.data()+s.PointerToRawData,s.SizeOfRawData);}
 // Only the independent angle-abort helper is stubbed to select the WAIT
 // branch. Wrapper3b2f50, thunk3d3480 and task query3d3e90 execute native bytes.
 auto patch=mapped+0x3d3300;patch[0]=0xe9;*reinterpret_cast<int32_t*>(patch+1)=int32_t(reinterpret_cast<uintptr_t>(&no_angle_abort95)-reinterpret_cast<uintptr_t>(patch+5));
 DWORD protection;require(VirtualProtect(mapped,nt->OptionalHeader.SizeOfImage,PAGE_EXECUTE_READ,&protection)!=0,"Fixture executable protection");FlushInstructionCache(GetCurrentProcess(),mapped,nt->OptionalHeader.SizeOfImage);
 uintptr_t actor_vtable[64]{},task_vtable[32]{};actor_vtable[0xf8/4]=reinterpret_cast<uintptr_t>(&animation_complete95);task_vtable[0x40/4]=reinterpret_cast<uintptr_t>(&advance95);task_vtable[0x68/4]=reinterpret_cast<uintptr_t>(&abort95);
 Actor player,peer,foreign;unit=0x100000;possessed=reinterpret_cast<uintptr_t>(&player);enabled=1;identity_valid=true;mounted_weapon=false;unsigned projectile=60;record=reinterpret_cast<uintptr_t>(&projectile);state->mode=POSSESSED;state->stop=state->release=0;
 for(auto actor:{&player,&peer,&foreign}){auto p=reinterpret_cast<uintptr_t>(actor);field<uintptr_t>(p,0)=reinterpret_cast<uintptr_t>(actor_vtable);field<uintptr_t>(p,0x214)=actor==&foreign?0x200000:unit;field<unsigned char>(p,0xf0)=1;field<unsigned>(p,0x1f0)=8;}
 Task task;task.vtable=reinterpret_cast<uintptr_t>(task_vtable);task.actor=possessed;
 using NativeWait=unsigned(__stdcall*)(void*);auto wait=reinterpret_cast<NativeWait>(mapped+0x3b2f50);auto argument=reinterpret_cast<unsigned char*>(&task)+0x1c;
 require(weapon_ready_gate(&task,nullptr)&&weapon_draw_gate(&task,nullptr)&&weapon_task_reload_gate(&task,nullptr)&&weapon_finish_gate(&task,nullptr),"Prior four bypasses");
 require(wait(argument)==0&&advanced95==0&&completed_calls95==1,"Original wrapper must reproduce wait despite four old gates");
 auto gate=mapped+0x3d3e90;unsigned char original_bytes[16];memcpy(original_bytes,gate,16);
 require(MH_Initialize()==MH_OK&&MH_CreateHook(gate,reinterpret_cast<void*>(&weapon_task_animation_gate95),reinterpret_cast<void**>(&original_task_animation_gate95))==MH_OK&&MH_EnableHook(gate)==MH_OK,"Actual production gate hook");
 unsigned calls=0;for(unsigned i=0;i<10000;++i){
  task.actor=possessed;auto before=task;auto old=completed_calls95;require(wait(argument)==0x95&&completed_calls95==old&&!memcmp(&task,&before,sizeof task),"Owned wait not bypassed or task was mutated");++calls;
  for(auto actor:{&peer,&foreign})for(unsigned char ready:{0,1}){task.actor=reinterpret_cast<uintptr_t>(actor);field<unsigned char>(task.actor,0x11f)=ready;old=completed_calls95;require(wait(argument)==(ready?0x95u:0u)&&completed_calls95==old+1,"Foreign/same-unit native animation query changed");++calls;}
 }
 task.actor=possessed;for(unsigned failure=0;failure<4;++failure){state->release=failure==0;state->stop=failure==1;identity_valid=failure!=2;state->mode=failure==3?IDLE:POSSESSED;auto old=completed_calls95;require(wait(argument)==0&&completed_calls95==old+1,"Lost ownership failed original forwarding");++calls;}
 require(aborted95==0&&MH_DisableHook(gate)==MH_OK&&!memcmp(gate,original_bytes,16),"Original bytes/ABI restoration");require(wait(argument)==0,"Restored original wait behavior");MH_Uninitialize();VirtualFree(mapped,0,MEM_RELEASE);mapped=nullptr;
 std::cout<<"PASS: native95 unmodified wait wrapper/thunk/query plus actual production hook; original four-gate stall reproduced; "<<calls<<" native owned/peer/foreign/release/disable/identity calls, exact task bytes, actor-global query forwarding, x86 RET4 ABI and hook restoration. Synthetic actors only; no game injection.\n";return 0;
 }catch(const std::exception&e){if(mapped)VirtualFree(mapped,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
