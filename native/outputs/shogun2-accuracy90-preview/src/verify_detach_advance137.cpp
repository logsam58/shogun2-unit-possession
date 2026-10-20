#define main retained_commands_advance137
#include "verify_native_commands100.cpp"
#undef main
static uintptr_t advance_site137;
static uintptr_t expected_actor137;
static bool throw137;static unsigned calls137;
static unsigned __declspec(naked) query137(uintptr_t){__asm{
 push esi
 mov esi,[esp+8]
 call dword ptr [advance_site137]
 pop esi
 ret
}}
static uintptr_t fault_read137;
static int catch137(EXCEPTION_POINTERS* p){fault_read137=p->ExceptionRecord->NumberParameters>=2?p->ExceptionRecord->ExceptionInformation[1]:0;return EXCEPTION_EXECUTE_HANDLER;}
static bool reproduces137(uintptr_t actor){__try{query137(actor);return false;}__except(catch137(GetExceptionInformation())){return fault_read137==0x75c;}}
static void __fastcall task137(void* task,void*){
 auto actor=field<uintptr_t>(reinterpret_cast<uintptr_t>(task),4);require(actor==expected_actor137,"Task actor");
 require(detached_task_actor130==actor,"Advance context missing");
 require((query137(actor)&0xffu)==0,"Absent squad reached original nearby query");++calls137;
 if(throw137)throw std::runtime_error("isolated native task exception");
}
static bool __fastcall unordered137(void*,void*){return false;}
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;auto actor=possessed,peer=P::ptr(f.peer);expected_actor137=actor;
 std::vector<unsigned char> squad(0x2800),policy(8),task(0x80);auto sq=P::ptr(squad),pol=P::ptr(policy);
 uintptr_t phys[]={actor,peer},members[8]{},groups[]={sq},vt[16]{};vt[0x30/4]=reinterpret_cast<uintptr_t>(&unordered137);field<uintptr_t>(pol,0)=reinterpret_cast<uintptr_t>(vt);
 field<uintptr_t>(unit,0x14)=environment;field<unsigned>(unit,0x384)=2;field<uintptr_t>(unit,0x388)=reinterpret_cast<uintptr_t>(phys);
 field<unsigned>(unit,0xb70)=1;field<uintptr_t>(unit,0xb74)=reinterpret_cast<uintptr_t>(groups);field<uintptr_t>(unit,0xb9c)=sq;
 field<uintptr_t>(sq,0)=base+0x15bb21c;field<uintptr_t>(sq,4)=environment;field<uintptr_t>(sq,0x510)=unit;field<uintptr_t>(sq,0x624)=pol;field<unsigned>(sq,0x14)=8;field<unsigned char>(sq,0x470)=1;field<uintptr_t>(sq,0x1c)=reinterpret_cast<uintptr_t>(members);
 auto add=reinterpret_cast<MemberInsert130>(base+0x14bc50);add(reinterpret_cast<void*>(sq),actor,0);add(reinterpret_cast<void*>(sq),peer,0);
 require(MH_Initialize()==MH_OK,"Init advance compatibility");create_unit_detach130();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable advance compatibility");state->heartbeat=GetTickCount64();sync_members130(unit);
 require(detached_actor_live130(actor)&&!field<uintptr_t>(actor,0x3b4),"Actual production native detachment missing");field<uintptr_t>(P::ptr(task),4)=actor;
 auto prior=original_personal_advance137;original_personal_advance137=reinterpret_cast<UnitTick130>(&task137);
 const unsigned starts[]={0x3eba6a,0x3ec59e},returns[]={0x3eba7b,0x3ec5af};
 for(unsigned i=0;i<2;++i){
  DWORD old{};auto p=reinterpret_cast<unsigned char*>(base+returns[i]);require(VirtualProtect(p,1,PAGE_EXECUTE_READWRITE,&old)!=0,"Fixture block boundary");auto byte=*p;*p=0xc3;VirtualProtect(p,1,old,&old);FlushInstructionCache(GetCurrentProcess(),p,1);advance_site137=base+starts[i];
  detached_task_actor130=0;require(reproduces137(actor),"Exact original missing-squad read did not reproduce at75c");
  for(unsigned n=0;n<1000;++n){detached_task_actor130=0x137abcd;reinterpret_cast<UnitTick130>(base+0x3eba00)(task.data());require(detached_task_actor130==0x137abcd,"Advance context leaked");}
  state->release=1;state->heartbeat=0;personal_advance137(task.data(),nullptr);state->release=0;state->heartbeat=GetTickCount64();
  throw137=true;try{personal_advance137(task.data(),nullptr);require(false,"Missing task exception");}catch(const std::runtime_error&){}throw137=false;require(detached_task_actor130==0x137abcd,"Exceptional advance context leaked");
  field<unsigned>(sq,0x75c)=1;require((query137(peer)&0xffu)!=0,"Native squad peer query altered");
  VirtualProtect(p,1,PAGE_EXECUTE_READWRITE,&old);*p=byte;VirtualProtect(p,1,old,&old);FlushInstructionCache(GetCurrentProcess(),p,1);
 }
 original_personal_advance137=prior;detached_task_actor130=0;
 state->stop=1;require(disable_control_hooks130()==MH_OK&&field<unsigned char>(base,0x3eba00)==0xe9,"Pending rejoin lost new task compatibility");
 state->mode=IDLE;sync_members130(unit);require(!detached_count130&&field<uintptr_t>(actor,0x3b4)==sq,"Native rejoin failed");require(disable_control_hooks130()==MH_OK&&field<unsigned char>(base,0x3eba00)==0x83,"New task hook not restored after rejoin");
 MH_DisableHook(MH_ALL_HOOKS);MH_Uninitialize();
 std::cout<<"PASS: two exact original advance-task missing-squad blocks reproduce AV75c; "<<calls137<<" production scoped/native-reader calls, unchanged valid squad queries, release/pre-rejoin protection, exception/TLS cleanup, deferred-stop residency and exact hook restoration. Remaining native task body is a provider.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
