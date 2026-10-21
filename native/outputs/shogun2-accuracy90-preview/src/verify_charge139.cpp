// Production bridge + original retail charge constructor, task entry, speed
// and moving-pose selector. Pathfinding/property/transition providers are
// explicit; this does not certify live contact, animation blending or outcomes.
#define main retained_commands139
#include "verify_native_commands100.cpp"
#undef main
static bool wants_run139=true;
static bool __fastcall run_intent139(void*,void*){return wants_run139;}
static float __fastcall property139(void*,void*){return 2.f;}
static float __fastcall align_speed140(void*,void*,unsigned){return 5.f;}
static unsigned path_calls139;
static bool path_complete139,contact_allowed139;static unsigned contacts139;
static uintptr_t contact_actor139,contact_target139;
static void __fastcall path139(void* actor,void*,const void*,const void*){++path_calls139;if(path_complete139)field<unsigned>(reinterpret_cast<uintptr_t>(actor),0x3bc)=6;}
static bool __fastcall contact_gate139(void*,void*,uintptr_t,uintptr_t){return contact_allowed139;}
static void __fastcall contact_queue139(void*,void*,uintptr_t actor,uintptr_t target){++contacts139;contact_actor139=actor;contact_target139=target;}
static void __fastcall direct_path139(void*,void*,const void*,float,unsigned){++path_calls139;}
static void __fastcall point139(void*,void*){}
static void __fastcall transition139(void*,void*){}
static unsigned transition_value139[2]={0x4a3,0};
static void* __fastcall transition_lookup139(void*,void*,unsigned,unsigned,void*){return transition_value139;}
static void* __fastcall no_squad139(void*,void*){return nullptr;}
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;auto actor=possessed,peer=P::ptr(f.peer);
 std::vector<unsigned char> enemy(0x700),enemy_unit(0x1800),own_side(0x20),enemy_side(0x20),squad(0x2800);
 auto target=P::ptr(enemy),eu=P::ptr(enemy_unit),sq=P::ptr(squad);
 uintptr_t own_roster[]={actor,peer},enemy_roster[]={target};
 field<unsigned>(unit,0x384)=2;field<uintptr_t>(unit,0x388)=reinterpret_cast<uintptr_t>(own_roster);
 field<uintptr_t>(unit,0x14)=environment;
 memcpy(enemy.data(),f.peer.data(),enemy.size());
 field<uintptr_t>(target,0x214)=eu;field<uintptr_t>(target,0x44)=P::ptr(enemy_side);field<unsigned>(target,0x1c8)=88;
 field<unsigned>(eu,4)=99;field<uintptr_t>(eu,0x14)=environment;field<unsigned>(eu,0x384)=1;field<uintptr_t>(eu,0x388)=reinterpret_cast<uintptr_t>(enemy_roster);
 field<uintptr_t>(actor,0x44)=P::ptr(own_side);field<unsigned>(P::ptr(own_side),0x10)=0;field<unsigned>(P::ptr(enemy_side),0x10)=1;
 field<float>(target,0x50)=20;field<float>(P::ptr(f.config),0x2c)=6;field<float>(unit,0x22c)=1.25f;
 field<uintptr_t>(sq,0)=base+0x15bb21c;field<uintptr_t>(sq,4)=environment;field<uintptr_t>(sq,0x510)=unit;field<uintptr_t>(peer,0x3b4)=sq;
 field<unsigned>(peer,0x244)=2;field<unsigned>(peer,0x1d0)=16;field<unsigned char>(peer,0x119)=2;
 field<unsigned char>(peer,0x2e8)=1;field<uintptr_t>(peer,0x250)=base+0x15e0a5c;field<uintptr_t>(peer,0x254)=peer;
 field<uintptr_t>(peer+0x250,0x1c)=target;field<uintptr_t>(peer+0x250,0x20)=eu;field<float>(peer+0x250,0x28)=1;
 field<uintptr_t>(actor,0x24c)=actor;field<uintptr_t>(actor,0x2ec)=environment;
 field<float>(actor,0x1ac)=4;field<float>(actor,0x150)=1;
 DWORD old{};require(VirtualProtect(f.image,0x1580000,PAGE_EXECUTE_READWRITE,&old)!=0,"Fixture protection");
 f.jump(0x18c120,reinterpret_cast<void*>(&run_intent139));f.jump(0xa0b60,reinterpret_cast<void*>(&property139));
 f.jump(0x3df290,reinterpret_cast<void*>(&path139));f.jump(0x3a8a40,reinterpret_cast<void*>(&transition_lookup139));
 f.jump(0x3de710,reinterpret_cast<void*>(&direct_path139));f.jump(0x370b40,reinterpret_cast<void*>(&point139));
 f.jump(0x3ca920,reinterpret_cast<void*>(&transition139));
 VirtualProtect(f.image,0x1580000,old,&old);FlushInstructionCache(GetCurrentProcess(),f.image,0x1580000);

 equipment_ranged81=0;equipment_slot81=equipment81::Melee;state->heartbeat=GetTickCount64();
 auto start=reinterpret_cast<UnitTick130>(base+0x3e6d50),pose=reinterpret_cast<UnitTick130>(base+0x3dd9d0);
 auto task=actor+0x250;unsigned before{},after{};
 ChargeIntent139 descriptor{target,eu,0,1,0,0,{}};
 auto order=[&](uintptr_t physical,const ChargeIntent139& value){auto fn=field<uintptr_t>(field<uintptr_t>(physical,0),0x154);reinterpret_cast<ChargeOrder139>(fn)(reinterpret_cast<void*>(physical),&value);};
 order(actor,descriptor);require(charge_native_task146(actor)&&field<uintptr_t>(task,0x1c)==target&&field<uintptr_t>(task,0x20)==eu&&field<float>(task,0x3c)==6,"Original infantry charge descriptor/speed");
 __asm mov before,esp
 start(reinterpret_cast<void*>(task));__asm mov after,esp
 require(before==after&&field<unsigned>(actor,0x244)==2&&path_calls139==1,"Original infantry charge entry ABI/gait/path");
 pose(reinterpret_cast<void*>(actor));require(field<unsigned>(actor,0x1d0)==16,"Original CHARGE moving pose");
 descriptor.special=1;order(actor,descriptor);require(field<float>(task,0x3c)==8,"Original special charge adjustment");descriptor.special=0;
 // Current native mounted principal, reciprocal rider/horse identity and a
 // mounted enemy carried by the enemy's master human roster.
 std::vector<unsigned char> horse(0x700),peer_horse(0x700),enemy_horse(0x700),horse_holder(0x210),peer_horse_holder(0x210),mount_config(0x100),record(0x100);
 auto h=P::ptr(horse),ph=P::ptr(peer_horse),eh=P::ptr(enemy_horse),mc=P::ptr(mount_config);
 uintptr_t passengers[]={actor,peer,target};uintptr_t all_holders[]={P::ptr(f.holder),P::ptr(f.peer_holder),P::ptr(horse_holder),P::ptr(peer_horse_holder)};
 field<uintptr_t>(unit,0xf9c)=mc;field<uintptr_t>(unit,0x2c)=P::ptr(record);
 for(unsigned i=0;i<3;++i){auto a=i==0?h:i==1?ph:eh;auto rider=passengers[i];auto u=i==2?eu:unit;
  field<uintptr_t>(a,0)=base+0x15e8524;field<uintptr_t>(a,0x214)=u;field<uintptr_t>(a,0x20c)=mc;field<uintptr_t>(a,0x28)=environment;
  field<unsigned char>(a,0xf0)=field<unsigned char>(a,0xa1)=1;field<unsigned>(a,0x1c8)=44+i;field<unsigned>(a,0x6cc)=1;field<uintptr_t>(a,0x6d0)=reinterpret_cast<uintptr_t>(passengers+i);
  field<uintptr_t>(a,0x44)=i==2?P::ptr(enemy_side):P::ptr(own_side);field<uintptr_t>(rider,0x210)=a;field<uintptr_t>(a,0x24c)=a;field<uintptr_t>(a,0x2ec)=environment;
 }
 field<float>(eh,0x50)=20;field<float>(mc,0x2c)=9;field<float>(mc,0x1c)=2;field<float>(mc,0x20)=7;field<float>(mc,0x6c)=1;
 field<uintptr_t>(P::ptr(horse_holder),8)=h;field<uintptr_t>(P::ptr(peer_horse_holder),8)=ph;
 field<unsigned>(P::ptr(f.view),0x18)=4;field<uintptr_t>(P::ptr(f.view),0x1c)=reinterpret_cast<uintptr_t>(all_holders);
 field<unsigned>(actor,0x1f0)=29;require(followed(actor,true),"Fresh mounted production lease");state->lease_generation=possession_lease.generation;state->heartbeat=GetTickCount64();

 descriptor={eh,eu,0,1,0,0,{}};order(h,descriptor);
 require(charge_native_task146(h)&&field<uintptr_t>(actor,0x250)==base+0x15e1b34,"Native horse virtual154 lost rider propagation");
 start(reinterpret_cast<void*>(h+0x250));require(field<unsigned>(h,0x244)==2,"Original horse charge entry gait");
 field<float>(h,0x1ac)=5;field<float>(h,0x150)=1;pose(reinterpret_cast<void*>(h));require(field<unsigned>(h,0x1d0)==16,"Original horse charge pose");
 require(VirtualProtect(f.image,0x1580000,PAGE_EXECUTE_READWRITE,&old)!=0,"Contact fixture protection");
 f.jump(0x3e53d0,reinterpret_cast<void*>(&contact_gate139));f.jump(0x3c0bd0,reinterpret_cast<void*>(&contact_queue139));
 VirtualProtect(f.image,0x1580000,old,&old);FlushInstructionCache(GetCurrentProcess(),f.image,0x1580000);
 auto update=reinterpret_cast<UnitTick130>(base+0x3eba00);
 for(bool allow:{false,true}){
  field<float>(eh,0x50)=20;path_complete139=false;field<unsigned>(h,0x3bc)=0;order(h,descriptor);start(reinterpret_cast<void*>(h+0x250));
  field<float>(eh,0x50)=1;field<float>(h+0x250,0x40)=.2f;contact_allowed139=allow;path_complete139=true;auto prior=contacts139;
  __asm mov before,esp
  update(reinterpret_cast<void*>(h+0x250));__asm mov after,esp
  require(before==after&&contacts139==prior+unsigned(allow)&&field<unsigned char>(h+0x250,8)==1,"Original native eligibility/contact/completion flow differs");
  if(allow)require(contact_actor139==h&&contact_target139==eh,"Native charge contact used wrong physical actor/target");
 }
 // Test140: execute the ORIGINAL jump factory/constructor. Asset lookup is
 // explicitly unavailable; actual clip selection/arc remain live checks.
 require(VirtualProtect(f.image,0x1580000,PAGE_EXECUTE_READWRITE,&old)!=0,"Jump fixture protection");
 f.jump(0x3cfa00,reinterpret_cast<void*>(&no_squad139));
 VirtualProtect(f.image,0x1580000,old,&old);FlushInstructionCache(GetCurrentProcess(),f.image,0x1580000);
 field<unsigned>(h,0x248)=0;field<unsigned char>(h,0x2e8)=0;float jump_vector140[3]={0,0,1};
 auto jump_factory140=reinterpret_cast<void(__thiscall*)(void*,const float*)>(base+0x3d9140);
 __asm mov before,esp
 jump_factory140(reinterpret_cast<void*>(h),jump_vector140);__asm mov after,esp
 require(before==after&&native_mounted_jump140(h)&&field<uintptr_t>(h,0x250)==base+0x15e0cec,"Original stock horse jump task / ABI not recognized");
 // An airborne flag does not sever a valid native horse/rider lease.
 field<unsigned char>(h,0xa2)=1;NativeSubject jump_subject140;
 require(native_subject(actor,unit,P::ptr(f.view),false,jump_subject140)&&jump_subject140.actor==h,"Native horse jump lost reciprocal possession identity");
 state->heartbeat=GetTickCount64();auto jump_actor_before140=horse;
 require(!prepare(h)&&!memcmp(horse.data(),jump_actor_before140.data(),horse.size()),"Manual movement overwrote native jump task/launch/landing fields");
 // A stock matched pair can be aligning while its native jump starts.
 // The native jump must win over manual target alignment, not just WASD.
 std::vector<unsigned char> jump_pair140(0x110);auto jw=P::ptr(jump_pair140);
 field<uintptr_t>(h,0x688)=field<uintptr_t>(eh,0x688)=jw;
 field<unsigned char>(jw,0xc)=1;field<unsigned>(jw,0x38)=2;
 field<uintptr_t>(jw,0x30)=h;field<uintptr_t>(jw,0x34)=eh;
 field<float>(h,0x69c)=field<float>(h,0x48)+10;field<float>(h,0x6a4)=field<float>(h,0x50);
 require(VirtualProtect(f.image,0x1580000,PAGE_EXECUTE_READWRITE,&old)!=0,"Jump alignment provider");
 f.jump(0x18b470,reinterpret_cast<void*>(&align_speed140));VirtualProtect(f.image,0x1580000,old,&old);
 FlushInstructionCache(GetCurrentProcess(),f.image,0x1580000);
 melee60::Request jump_request140;jump_request140.pid=GetCurrentProcessId();jump_request140.phase=melee60::Aligning;
 jump_request140.lease=possession_lease.generation;jump_request140.serial=9123;jump_request140.heartbeat=GetTickCount64();
 jump_request140.player=field<unsigned>(h,0x1c8);jump_request140.player_unit=field<unsigned>(unit,4);jump_request140.player_kind=1;
 jump_request140.opponent=field<unsigned>(eh,0x1c8);jump_request140.opponent_unit=field<unsigned>(eu,4);jump_request140.opponent_kind=1;
 require(PossessionMeleeCommitment63(&jump_request140)==63,"Jump alignment commitment fixture");
 jump_actor_before140=horse;state->heartbeat=GetTickCount64();
 require(!prepare(h)&&!memcmp(horse.data(),jump_actor_before140.data(),horse.size()),"Pair alignment overwrote the already-started native horse jump");
 melee60::Request off140;PossessionMeleeCommitment63(&off140);
 field<uintptr_t>(h,0x688)=field<uintptr_t>(eh,0x688)=0;
 field<unsigned char>(h+0x250,8)=1;require(!native_mounted_jump140(h),"Completed horse jump retained motion override");
 field<unsigned char>(h+0x250,8)=0;field<unsigned>(h,0x248)=0;require(!native_mounted_jump140(h),"Unrelated horse task admitted as jump");
 field<unsigned char>(h,0xa2)=0;
 std::cout<<"PASS: original infantry/horse charge virtual dispatch, rider propagation, entry gait/pose/special pace, full native contact-update eligibility/queue/completion and original native jump with paired-alignment preservation. Path/property/transition/contact/asset providers explicit. Unit targeting is separately executed in verify_proximity147; rendered outcomes require live testing.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
