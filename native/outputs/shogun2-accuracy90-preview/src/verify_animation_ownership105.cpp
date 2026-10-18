// Exact native reset/predicate/stop/clock with production possession ownership.
// Fixture substitutions: OS RMB state, projectile getter, reset side-effect
// providers and animation root-event provider. No live game or projectile pool.
#include "identity.h"
static bool held105=true;
static SHORT WINAPI keys105(int key){return held105&&key==VK_RBUTTON?SHORT(-32768):0;}
#define GetAsyncKeyState keys105
#define main retained_commands105
#include "verify_native_commands100.cpp"
#undef main
#undef GetAsyncKeyState
static uintptr_t projectile105;
static unsigned reset_side105,reset_mode105,events105;
static void* __fastcall projectile_get105(void*,void*){return reinterpret_cast<void*>(projectile105);}
static void __fastcall reset_side_effect105(void*,void*,unsigned){++reset_side105;}
static void __fastcall reset_mode_effect105(void*,void*,unsigned){++reset_mode105;}
static bool __fastcall root_event105(void*,void*,void*,unsigned){++events105;return false;}
static void patch105(NativeFixture100& f,unsigned rva,void* target){DWORD old{};
 require(VirtualProtect(reinterpret_cast<void*>(base+rva),16,PAGE_EXECUTE_READWRITE,&old)!=0,"Fixture provider protection");
 f.jump(rva,target);VirtualProtect(reinterpret_cast<void*>(base+rva),16,old,&old);FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(base+rva),16);}
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;
 patch105(f,0x14ad40,reinterpret_cast<void*>(&projectile_get105));
 patch105(f,0x3df380,reinterpret_cast<void*>(&reset_side_effect105));
 patch105(f,0x3a5f00,reinterpret_cast<void*>(&reset_mode_effect105));
 patch105(f,0x3f03d0,reinterpret_cast<void*>(&root_event105));
 unsigned char record[0x200]{},db[32]{},asset[0xa0]{};uintptr_t clips[0x4a3]{};unsigned animation[4]{};
 projectile105=reinterpret_cast<uintptr_t>(record);auto ap=reinterpret_cast<uintptr_t>(asset),cp=reinterpret_cast<uintptr_t>(animation);
 field<unsigned>(projectile105,0)=60;field<unsigned>(projectile105,0x60)=100;field<float>(projectile105,0x6c)=200;
 field<unsigned>(unit,0xf7c)=1;field<float>(unit,0xf78)=50;
 field<uintptr_t>(possessed,0x23c)=reinterpret_cast<uintptr_t>(db);field<uintptr_t>(reinterpret_cast<uintptr_t>(db),0xc)=reinterpret_cast<uintptr_t>(clips);
 animation[1]=unsigned(ap);field<unsigned>(ap,0x64)=10;clips[130]=cp;
 equipment_slot81=equipment81::Ranged;equipment_requests81={};state->aiming=1;
 auto arm=[&](unsigned clip=130,unsigned phase=WEAPON_DRAW,unsigned mode=14){
  enabled=1;held105=true;own_weapon_call=0;state->mode=POSSESSED;state->stop=state->release=0;state->aiming=1;state->aim_pose=0;state->weapon_phase=phase;
  state->lease_generation=possession_lease.generation;state->heartbeat=GetTickCount64();equipment_slot81=equipment81::Ranged;
  weapon_actor=possessed;weapon_clip=clip;animation[0]=clip;clips[clip]=cp;field<unsigned>(ap,0x64)=10;
  field<unsigned>(possessed,0x1f0)=mode;field<unsigned>(possessed,0x1d4)=clip;field<unsigned>(possessed,0x1d8)=0;
  field<uintptr_t>(possessed,0x474)=cp;field<uintptr_t>(possessed,0x688)=0;
 };
 auto reset=reinterpret_cast<ChangeState>(base+0x3e93c0); // Actual ABI is thiscall + one unsigned.
 using Reset105=void(__thiscall*)(void*,unsigned);auto native_reset=reinterpret_cast<Reset105>(reset);
 using Clock105=void(__thiscall*)(void*);auto clock=reinterpret_cast<Clock105>(base+0x3f0370);
 arm();require(weapon_owned(possessed)&&weapon_ready_held(),"Full production weapon ownership fixture");
 native_reset(f.actor.data(),0);require(!field<uintptr_t>(possessed,0x474),"Original frame-zero reset did not reproduce recorded failure");
 unsigned char original_bytes[16];memcpy(original_bytes,reinterpret_cast<void*>(base+0x3ca920),16);
 require(MH_Initialize()==MH_OK&&MH_CreateHook(reinterpret_cast<void*>(base+0x3ca920),reinterpret_cast<void*>(&weapon_animation_stop_hook82),reinterpret_cast<void**>(&original_animation_stop82))==MH_OK&&MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Production animation stop hook");
 unsigned cases=0;
 for(const auto& item:PERSONAL_WEAPONS){
  auto length=unsigned(wcslen(item.key));field<unsigned>(projectile105,0)=700;field<unsigned>(projectile105,4)=field<unsigned>(projectile105,8)=length;
  field<uintptr_t>(projectile105,0xc)=reinterpret_cast<uintptr_t>(item.key);field<unsigned>(projectile105,0x60)=150;field<float>(projectile105,0x6c)=200;
  require(native_personal_kind(projectile105)!=PersonalWeaponKind::Unsupported,"Catalog fixture record rejected");
  for(unsigned mode:{14u,15u})for(unsigned phase:{unsigned(WEAPON_DRAW),unsigned(WEAPON_FIRED)}){
   arm(mode==14?130:132,phase,mode);unsigned side=reset_side105,mode_calls=reset_mode105;
   auto actor_before=f.actor;
   for(unsigned frame=0;frame<10;++frame){
    require(field<unsigned>(possessed,0x1d8)==frame,"Native animation frame drift");
    native_reset(f.actor.data(),0);require(field<uintptr_t>(possessed,0x474)==cp,"Owned firing animation cancelled at native reset");
    clock(f.actor.data());
   }
   require(!field<uintptr_t>(possessed,0x474)&&field<unsigned>(possessed,0x1d8)==10,"Actual native completion failed to clear animation");
   require(reset_side105==side+10&&reset_mode105==mode_calls+10,"Guard skipped native reset side effects");
   // Only the original clock/stop may change native pose/animation fields.
   field<unsigned>(P::ptr(actor_before),0x1d8)=10;field<unsigned>(P::ptr(actor_before),0x1d0)=33;field<uintptr_t>(P::ptr(actor_before),0x474)=0;
   require(actor_before==f.actor,"Guard wrote unrelated actor fields");++cases;
  }
 }
 for(unsigned clip:{124u,127u,126u}){
  arm(clip,WEAPON_IDLE,clip==127?15:14);state->aim_pose=clip==126?AIM_BOW_DRAWING:AIM_READY;
  native_reset(f.actor.data(),0);require(field<uintptr_t>(possessed,0x474)==cp,"Owned ready/draw animation reset");
 }
 // Each guard is restored before the next mutation; invalid ownership never
 // gains permission merely because the address or animation still matches.
 for(unsigned guard=0;guard<18;++guard){arm();auto actor=f.actor,cam=f.camera,holder=f.holder;unsigned generation=state->lease_generation;
  switch(guard){
   case 0:held105=false;break;case 1:enabled=0;break;case 2:state->release=1;break;case 3:state->stop=1;break;
   case 4:state->mode=IDLE;break;case 5:++field<unsigned>(possessed,0x1c8);break;case 6:field<unsigned char>(possessed,0xf0)=0;break;
   case 7:field<uintptr_t>(possessed,0x688)=1;break;case 8:field<unsigned>(possessed,0x1f0)=12;break;
   case 9:equipment_slot81=equipment81::Melee;break;case 10:own_weapon_call=1;break;
   case 11:field<uintptr_t>(P::ptr(f.camera),0x188)=P::ptr(f.peer);break;
   case 12:field<uintptr_t>(P::ptr(f.holder),8)=P::ptr(f.peer);break;
   case 13:state->weapon_phase=WEAPON_RELOAD;break;case 14:clips[130]=0;break;
   case 15:field<unsigned>(possessed,0x1d4)=124;break;case 16:field<unsigned>(possessed,0x1d8)=10;break;
   case 17:state->aiming=0;break;
  }
  native_reset(f.actor.data(),0);require(!field<uintptr_t>(possessed,0x474),"Invalid state suppressed native cancellation");
  f.actor=actor;f.camera=cam;f.holder=holder;state->lease_generation=generation;clips[130]=cp;
 }
 arm();reinterpret_cast<RangedAction>(base+0x3ca920)(f.actor.data());require(!field<uintptr_t>(possessed,0x474),"Explicit native stop suppressed");
 for(unsigned i=0;i<1000;++i){arm();field<uintptr_t>(P::ptr(f.peer),0x474)=cp;field<unsigned>(P::ptr(f.peer),0x1d4)=130;
  native_reset(f.peer.data(),0);require(!field<uintptr_t>(P::ptr(f.peer),0x474)&&field<uintptr_t>(possessed,0x474)==cp,"Other actor reset intercepted");}
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&!memcmp(original_bytes,reinterpret_cast<void*>(base+0x3ca920),16),"Exact stop bytes not restored");MH_Uninitialize();
 std::cout<<"PASS: original frame-zero reset reproduced; "<<cases<<" catalog/standing/kneeling/draw/fired lifecycles; original clock completion, reset side effects, current identity/recovery/input/db guards, 1000 peer resets and exact hook restoration. Projectile pool and real assets not simulated.\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
