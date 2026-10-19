#define main retained_lifetime_combat130
#include "verify_native_commands100.cpp"
#undef main
static bool __fastcall unordered_lifetime130(void*,void*){return false;}
static uintptr_t pose_site130,heading_site130,combat_actor130,combat_squad130;
static unsigned pose_cases130,forward_pairs130;
static unsigned xmm_input130[32],xmm_output130[32],heading_flags_before130,heading_flags_after130;
static uintptr_t __declspec(naked) read_pose_block130(){__asm{
 xor ecx,ecx
 call dword ptr [pose_site130]
 ret
}}
static unsigned __declspec(naked) read_heading_block130(uintptr_t,uintptr_t){
 __asm{
  mov edx,[esp+4]
  mov eax,[esp+8]
  movdqu xmm0,[xmm_input130+0]
  movdqu xmm1,[xmm_input130+16]
  movdqu xmm2,[xmm_input130+32]
  movdqu xmm3,[xmm_input130+48]
  movdqu xmm4,[xmm_input130+64]
  movdqu xmm5,[xmm_input130+80]
  movdqu xmm6,[xmm_input130+96]
  movdqu xmm7,[xmm_input130+112]
  cmp edx,edx
  pushfd
  pop ecx
  mov heading_flags_before130,ecx
  call dword ptr [heading_site130]
  pushfd
  pop ecx
  mov heading_flags_after130,ecx
  movdqu [xmm_output130+0],xmm0
  movdqu [xmm_output130+16],xmm1
  movdqu [xmm_output130+32],xmm2
  movdqu [xmm_output130+48],xmm3
  movdqu [xmm_output130+64],xmm4
  movdqu [xmm_output130+80],xmm5
  movdqu [xmm_output130+96],xmm6
  movdqu [xmm_output130+112],xmm7
  ret
 }
}
static void __fastcall pair_provider130(void*,void*,uintptr_t,uintptr_t,unsigned){++forward_pairs130;}
static bool throw_task130;
static void __fastcall personal_provider130(void* task,void*){
 require(detached_task_actor130==field<uintptr_t>(reinterpret_cast<uintptr_t>(task),4),"Native task context not scoped to detached actor");
 if(throw_task130)throw std::runtime_error("isolated task exception");
 auto position=read_pose_block130();float out[3]{};
 reinterpret_cast<uintptr_t(__thiscall*)(void*,void*)>(base+0x399f80)(reinterpret_cast<void*>(position),out);
 for(unsigned i=0;i<3;++i)require(out[i]==field<float>(combat_actor130,0x48+i*4),"Original native absolute-position read differs");
 auto value=read_heading_block130(combat_actor130,0);
 require((value&0xffff)==field<unsigned short>(combat_actor130,0x64),"Native heading read did not use detached actor orientation");
 require(!memcmp(xmm_input130,xmm_output130,sizeof xmm_input130)&&((heading_flags_before130^heading_flags_after130)&0x8d5)==0,"Native heading compatibility clobbered SIMD values or flags");
 reinterpret_cast<NativePair130>(base+0x14ad80)(reinterpret_cast<void*>(0x74c),combat_actor130,combat_actor130,0);
 require(forward_pairs130==0,"Detached actor reached absent squad bookkeeping");
 require(!reinterpret_cast<NativeNearby130>(base+0x17bee0)(reinterpret_cast<void*>(0x74c)),"Detached actor queried absent squad bookkeeping");
 ++pose_cases130;
}
static unsigned char swap_byte130(uintptr_t address,unsigned char value){DWORD old{};require(VirtualProtect(reinterpret_cast<void*>(address),1,PAGE_EXECUTE_READWRITE,&old)!=0,"Native fixture protection");auto prior=field<unsigned char>(address,0);field<unsigned char>(address,0)=value;VirtualProtect(reinterpret_cast<void*>(address),1,old,&old);FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(address),1);return prior;}
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;auto actor=P::ptr(f.actor),peer=P::ptr(f.peer);combat_actor130=actor;
 std::vector<unsigned char> squad(0x2800),policy(8),task(0x80),nearby(0x20);auto sq=P::ptr(squad),pol=P::ptr(policy);combat_squad130=sq;
 uintptr_t phys[]={actor,peer},members[8]{},groups[]={sq},policy_vt[16]{};
 policy_vt[0x30/4]=reinterpret_cast<uintptr_t>(&unordered_lifetime130);field<uintptr_t>(pol,0)=reinterpret_cast<uintptr_t>(policy_vt);
 field<uintptr_t>(unit,0x14)=environment;field<unsigned>(unit,0x384)=2;field<uintptr_t>(unit,0x388)=reinterpret_cast<uintptr_t>(phys);
 field<unsigned>(unit,0xb70)=1;field<uintptr_t>(unit,0xb74)=reinterpret_cast<uintptr_t>(groups);field<uintptr_t>(unit,0xb9c)=sq;
 field<uintptr_t>(sq,0)=base+0x15bb21c;field<uintptr_t>(sq,4)=environment;field<uintptr_t>(sq,0x510)=unit;field<uintptr_t>(sq,0x624)=pol;field<unsigned>(sq,0x14)=8;field<unsigned char>(sq,0x470)=1;field<uintptr_t>(sq,0x1c)=reinterpret_cast<uintptr_t>(members);
 auto add=reinterpret_cast<MemberInsert130>(base+0x14bc50);add(reinterpret_cast<void*>(sq),actor,0);add(reinterpret_cast<void*>(sq),peer,0);
 require(MH_Initialize()==MH_OK,"MinHook combat init");create_unit_detach130();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable combat compatibility");state->heartbeat=GetTickCount64();sync_members130(unit);
 require(detached_actor_live130(actor)&&!detached_actor_live130(peer),"Live detached physical identity guard");
 uintptr_t unrelated=actor+16;while(detached_bucket130(unrelated)==detached_bucket130(actor))unrelated+=16;
 auto proofs=detached_proofs130;for(unsigned i=0;i<100000;++i)require(!detached_actor_live130(unrelated),"Unrelated actor passed negative filter");
 require(detached_proofs130==proofs,"Unrelated crowd entered locked native identity scan");
 auto pair_original=original_squad_pair130;original_squad_pair130=reinterpret_cast<NativePair130>(&pair_provider130);
 auto idle_original=original_personal_idle130,move_original=original_personal_move130;original_personal_idle130=original_personal_move130=reinterpret_cast<UnitTick130>(&personal_provider130);
 field<uintptr_t>(P::ptr(task),4)=actor;
 for(unsigned i=0;i<32;++i)xmm_input130[i]=0x12340000+i;
 for(unsigned i=0;i<3;++i)field<float>(actor,0x48+i*4)=float(i*13+5);
 field<unsigned short>(actor,0x64)=0x9012;field<unsigned short>(sq,0x64c)=0x3456;
 unsigned positions[]={0x3edc85,0x3ee732},returns[]={0x3edc90,0x3ee73d},headings[]={0x3edcba,0x3ee767},resumes[]={0x3edcc1,0x3ee76e};
 for(unsigned i=0;i<2;++i){
  auto pbyte=swap_byte130(base+returns[i],0xc3),hbyte=swap_byte130(base+resumes[i],0xc3);
  pose_site130=base+positions[i];heading_site130=base+headings[i];detached_task_actor130=0x130abc;
  if(i)personal_move130(task.data(),nullptr);else personal_idle130(task.data(),nullptr);
  require(detached_task_actor130==0x130abc,"Native personal task context leaked");
  auto ordinary=read_heading_block130(peer,sq);require((ordinary&0xffff)==0x3456&&!memcmp(xmm_input130,xmm_output130,sizeof xmm_input130),"Native peer heading altered");
  // Control may end between unit callbacks. Keep personal native consumers safe
  // for the still-detached, currently proved surviving actor until actual rejoin.
  state->release=1;state->heartbeat=0;
  if(i)personal_move130(task.data(),nullptr);else personal_idle130(task.data(),nullptr);
  state->release=0;state->heartbeat=GetTickCount64();
  throw_task130=true;try{personal_idle130(task.data(),nullptr);require(false,"Fixture exception missing");}catch(const std::runtime_error&){}
  require(detached_task_actor130==0x130abc,"Native task exception leaked context");throw_task130=false;
  swap_byte130(base+returns[i],pbyte);swap_byte130(base+resumes[i],hbyte);
 }
 detached_task_actor130=actor;field<unsigned>(P::ptr(nearby),0x10)=1;
 require(reinterpret_cast<NativeNearby130>(base+0x17bee0)(nearby.data()),"Ordinary squad-nearby result altered");
 reinterpret_cast<NativePair130>(base+0x14ad80)(reinterpret_cast<void*>(sq+0x74c),peer,actor,1);require(forward_pairs130==1,"Valid peer bookkeeping not forwarded");
 auto serial=field<unsigned>(actor,0x1c8);++field<unsigned>(actor,0x1c8);require(!detached_actor_live130(actor),"Reused serial granted combat compatibility");field<unsigned>(actor,0x1c8)=serial;
 original_squad_pair130=pair_original;original_personal_idle130=idle_original;original_personal_move130=move_original;
 state->stop=1;require(disable_control_hooks130()==MH_OK,"Stop control hook policy failed");
 require(detached_count130==1&&field<unsigned char>(base,0x1b7ed0)==0xe9&&field<unsigned char>(base,0x3ed950)==0xe9&&field<unsigned char>(base,0x3edcba)==0xe9,"Pending native rejoin lost callback/combat compatibility");
 require(field<unsigned char>(base,0x14bc50)==0x56&&field<unsigned char>(base,0x1afed6)==0x83,"Stop left reassignment guards active");
 detached_task_actor130=0;state->mode=IDLE;sync_members130(unit);require(!detached_count130&&field<uintptr_t>(actor,0x3b4)==sq&&!detached_actor_live130(actor),"Native rejoin did not revoke compatibility");
 require(disable_control_hooks130()==MH_OK,"Completed rejoin hook shutdown failed");
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Disable combat compatibility");MH_Uninitialize();
 require(field<unsigned char>(base,0x3edcba)==0x66&&field<unsigned char>(base,0x3ee767)==0x66,"Native heading instructions not restored");
 std::cout<<"PASS: detached personal idle/move context; "<<pose_cases130<<" original native position/heading blocks; all SIMD registers and flags; native peer reads; post-control pre-rejoin interval; missing-squad bookkeeping isolation; valid native nearby result; exceptions, serial reuse, deferred-stop compatibility residency, 100000 cheap unrelated context rejections and exact rejoin cleanup. Full task bodies are fixture providers.\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
