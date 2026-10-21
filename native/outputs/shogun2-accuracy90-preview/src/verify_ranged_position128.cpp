#include "../../../common/fixture_paths.h"
// Runs the original unit aggregate, actor predicate, personal fire wait and
// fire-permission routine. Synthetic rosters/task vtable stand in for the game;
// native order scheduling, projectile dispatch and a live battle are not mocked
// into a claim of end-to-end firing success.
#define main previous_scope128
#include "verify_ranged_scope.cpp"
#undef main
static unsigned advances128;
static unsigned __fastcall advance128(void*,void*){++advances128;return 0x128;}
int main(){unsigned char* mapped=nullptr;try{
 std::wstring path=fixture_paths::engine();
 require(hash_file(path)==EXPECTED_HASH,"Exact native engine");
 std::ifstream in(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});
 auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());auto nt=reinterpret_cast<IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
 mapped=static_cast<unsigned char*>(VirtualAlloc(nullptr,nt->OptionalHeader.SizeOfImage,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(mapped!=nullptr,"Image allocation");
 memcpy(mapped,image.data(),nt->OptionalHeader.SizeOfHeaders);auto sections=IMAGE_FIRST_SECTION(nt);
 for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto&s=sections[i];if(s.SizeOfRawData)memcpy(mapped+s.VirtualAddress,image.data()+s.PointerToRawData,s.SizeOfRawData);}
 DWORD protect;require(VirtualProtect(mapped,nt->OptionalHeader.SizeOfImage,PAGE_EXECUTE_READ,&protect)!=0,"Executable image");FlushInstructionCache(GetCurrentProcess(),mapped,nt->OptionalHeader.SizeOfImage);
 base=reinterpret_cast<uintptr_t>(mapped);Actor player,peer,foreign;unit=0x100000;possessed=reinterpret_cast<uintptr_t>(&player);unsigned projectile=3;record=reinterpret_cast<uintptr_t>(&projectile);
 state->mode=POSSESSED;state->stop=state->release=0;enabled=1;identity_valid=true;
 for(auto a:{&player,&peer,&foreign}){auto x=reinterpret_cast<uintptr_t>(a);field<uintptr_t>(x,0x214)=a==&foreign?0x200000:unit;field<unsigned char>(x,0xf0)=1;field<unsigned>(x,0x1f0)=0;field<unsigned char>(x,0x11f)=1;}
 unsigned char view[0x40]{},aggregate[0x170]{};uintptr_t actors[1000]{};actors[0]=possessed;actors[1]=reinterpret_cast<uintptr_t>(&peer);
 field<unsigned>(reinterpret_cast<uintptr_t>(view),0x18)=2;field<uintptr_t>(reinterpret_cast<uintptr_t>(view),0x1c)=reinterpret_cast<uintptr_t>(actors);field<uintptr_t>(reinterpret_cast<uintptr_t>(aggregate),4)=reinterpret_cast<uintptr_t>(view);
 using Aggregate=bool(__thiscall*)(void*);auto blocked=reinterpret_cast<Aggregate>(mapped+0x1aef50);
 using Wait=unsigned(__stdcall*)(void*);auto wait=reinterpret_cast<Wait>(mapped+0x3b2d90);
 uintptr_t table[32]{};table[0x40/4]=reinterpret_cast<uintptr_t>(&advance128);Task task;task.vtable=reinterpret_cast<uintptr_t>(table);task.actor=possessed;auto argument=reinterpret_cast<unsigned char*>(&task)+0x1c;
 auto predicate=reinterpret_cast<RangedSettled128>(mapped+0x3cfa60);
 require(!blocked(aggregate)&&predicate(possessed)&&wait(argument)==0x128,"Native settled controls");
 field<unsigned char>(possessed,0x11f)=0;
 require(blocked(aggregate)&&!predicate(possessed)&&wait(argument)==0,"Original native unit/personal firing waits reproduced");
 // The original manual-intent API selects the native early-return branch.
 // With no AI destination completion, that branch preserves the prior settled
 // flag, including while standing still. This does not emulate the full tick.
 using Intent128=void(__thiscall*)(void*,const float*,unsigned);
 auto intent=reinterpret_cast<Intent128>(mapped+0x3936f0);auto drive=reinterpret_cast<void(__thiscall*)(void*)>(mapped+0x3aa090);
 for(unsigned settled:{0u,1u})for(unsigned step=0;step<1000;++step){field<unsigned char>(possessed,0x11f)=static_cast<unsigned char>(settled);float point[2]={float(step%2),0};intent(&player,point,0);drive(&player);require(field<unsigned char>(possessed,0x198)==1&&field<unsigned char>(possessed,0x11f)==settled&&blocked(aggregate)==!bool(settled),"Native manual-intent readiness preservation changed");}
 field<unsigned char>(possessed,0x11f)=0;
 unsigned char old_predicate[16],old_task[16];memcpy(old_predicate,mapped+0x3cfa60,16);memcpy(old_task,mapped+0x3d41d0,16);
 require(MH_Initialize()==MH_OK,"MinHook init");create_ranged_position128();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable production hooks");
 unsigned cases=0;
 for(unsigned family:{3u,60u,4u,15u,47u,82u})for(bool mounted:{false,true})for(unsigned mode=0;mode<(mounted?5u:18u);++mode){
  projectile=family;mounted_weapon=mounted;field<unsigned>(possessed,0x1f0)=mounted?29+mode:mode;
  for(unsigned count:{2u,90u,240u,1000u}){field<unsigned>(reinterpret_cast<uintptr_t>(view),0x18)=count;for(unsigned i=1;i<count;++i)actors[i]=reinterpret_cast<uintptr_t>(&peer);
   for(unsigned owned_index:{0u,count-1}){std::swap(actors[0],actors[owned_index]);
    for(unsigned repeat=0;repeat<20;++repeat){state->heartbeat=GetTickCount64();auto actor_before=player;auto task_before=task;task.actor=possessed;
     require(!blocked(aggregate)&&wait(argument)==0x128,"Owned unsettled soldier blocks native firing readiness");
     require(!predicate(possessed),"Non-firing caller incorrectly sees settled actor");
     require(!memcmp(&player,&actor_before,sizeof player)&&!memcmp(&task,&task_before,sizeof task),"Actor/task mutated");
     field<unsigned char>(reinterpret_cast<uintptr_t>(&peer),0x11f)=0;
     require(blocked(aggregate),"Unsettled AI peer was ignored");task.actor=reinterpret_cast<uintptr_t>(&peer);require(wait(argument)==0,"AI personal wait bypassed");
     task.actor=reinterpret_cast<uintptr_t>(&foreign);field<unsigned char>(task.actor,0x11f)=0;require(wait(argument)==0,"Foreign wait bypassed");
     task.actor=possessed;field<unsigned char>(reinterpret_cast<uintptr_t>(&peer),0x11f)=1;++cases;
    }std::swap(actors[0],actors[owned_index]);
   }
  }
 }
 // Exact reciprocal owned horse principal is exempt; unrelated horses and
 // broken passenger links retain the original native wait.
 unsigned char horse[0x700]{};auto h=reinterpret_cast<uintptr_t>(horse);uintptr_t passenger=possessed;
 mounted_weapon=true;field<unsigned>(possessed,0x1f0)=29;field<uintptr_t>(possessed,0x210)=h;field<uintptr_t>(h,0)=base+0x15e8524;field<unsigned char>(h,0xf0)=1;field<uintptr_t>(h,0x214)=unit;field<unsigned>(h,0x6cc)=1;field<uintptr_t>(h,0x6d0)=reinterpret_cast<uintptr_t>(&passenger);
 field<unsigned>(reinterpret_cast<uintptr_t>(view),0x18)=2;actors[0]=h;actors[1]=reinterpret_cast<uintptr_t>(&peer);task.actor=h;state->heartbeat=GetTickCount64();
 require(!blocked(aggregate)&&wait(argument)==0x128&&!predicate(h),"Owned horse principal or caller isolation failed");passenger=reinterpret_cast<uintptr_t>(&peer);require(blocked(aggregate)&&wait(argument)==0,"Broken horse passenger proof admitted");passenger=possessed;field<uintptr_t>(h,0x214)=unit+4;require(blocked(aggregate)&&wait(argument)==0,"Foreign horse unit admitted");field<uintptr_t>(possessed,0x210)=0;
 mounted_weapon=false;field<unsigned>(possessed,0x1f0)=0;field<unsigned>(reinterpret_cast<uintptr_t>(view),0x18)=2;actors[0]=possessed;actors[1]=reinterpret_cast<uintptr_t>(&peer);task.actor=possessed;
 for(unsigned fail=0;fail<9;++fail){state->heartbeat=GetTickCount64()-(fail==8?501:0);enabled=fail!=0;identity_valid=fail!=1;state->stop=fail==2;state->release=fail==3;state->mode=fail==4?IDLE:POSSESSED;field<unsigned char>(possessed,0xf0)=fail==5?0:1;record=fail==6?0:reinterpret_cast<uintptr_t>(&projectile);field<unsigned>(possessed,0x1f0)=fail==7?99:0;
  require(blocked(aggregate)&&wait(argument)==0,"Invalid/released ownership bypassed native firing readiness");}
 // The actual native Fire at Will/order gate remains authoritative. The new
 // readiness hooks do not change its flag, queued-order condition or behavior.
 unsigned char native_unit[0x1000]{},selector[0x20]{};field<uintptr_t>(reinterpret_cast<uintptr_t>(selector),0x10)=reinterpret_cast<uintptr_t>(native_unit);auto fire=reinterpret_cast<Aggregate>(mapped+0x15c300);
 for(unsigned enabled_fire:{0u,1u})for(unsigned queued:{0u,1u})for(unsigned ready:{0u,1u}){field<unsigned char>(reinterpret_cast<uintptr_t>(native_unit),0xeb0)=static_cast<unsigned char>(enabled_fire);field<unsigned>(reinterpret_cast<uintptr_t>(native_unit),0x9ec)=queued;field<unsigned char>(reinterpret_cast<uintptr_t>(native_unit),0x198)=static_cast<unsigned char>(ready);auto before=std::vector<unsigned char>(native_unit,native_unit+sizeof native_unit);require(fire(selector)==bool(enabled_fire&&(!queued||ready))&&!memcmp(native_unit,before.data(),sizeof native_unit),"Native hold-fire/queued-order authority changed");}
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&!memcmp(old_predicate,mapped+0x3cfa60,16)&&!memcmp(old_task,mapped+0x3d41d0,16),"Exact native byte restoration");
 require(blocked(aggregate)&&!predicate(possessed)&&wait(argument)==0,"Original waits did not return after hook removal");MH_Uninitialize();VirtualFree(mapped,0,MEM_RELEASE);mapped=nullptr;
 std::cout<<"PASS: native firing unit aggregate, personal wait, caller isolation, unchanged actor/task bytes, "<<cases<<" roster/projectile/mode cases, peer/foreign waits, invalid ownership, native fire flags/order gate and exact restoration. No game launch.\n";return 0;
}catch(const std::exception&e){if(mapped)VirtualFree(mapped,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
