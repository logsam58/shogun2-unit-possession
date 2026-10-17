#include "playable_runtime.cpp"
static unsigned forwarded_updates,forwarded_prepares,forwarded_scores,forwarded_targets;
static uint32_t __fastcall update_fake(void*,void*){++forwarded_updates;return 0xcafe006f;}
static uint32_t __fastcall prepare_fake(void*,void*){++forwarded_prepares;return 0xcafe0000;}
static uint32_t __fastcall target_fake(void*,void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t){++forwarded_targets;return 0xcafe0000;}
static int __fastcall score_fake(void*,void*,void* a,void* b,void* p,uint32_t debug){++forwarded_scores;auto ca=static_cast<effective::ActorContext*>(a),cb=static_cast<effective::ActorContext*>(b);auto pair=static_cast<effective::PairContext*>(p);
 require(debug==0&&pair->attacker==ca->actor&&pair->defender==cb->actor,"Score context/ABI differs");int av{},bv{};reader().get(ca->actor+0x20,av);reader().get(cb->actor+0x20,bv);return av-bv;
}
static int __fastcall defence_fake(void*,void*,void*,void* b){int value{};reader().get(static_cast<effective::ActorContext*>(b)->actor+0x20,value);return value;}
static int __fastcall armour_fake(void*,void*,void*,void*,int){return 0;}
static int __fastcall facing_fake(void*,void*){return 0;}
int main(){void* memory=nullptr;try{
 memory=VirtualAlloc(nullptr,0x1bc9000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory,"Fixture allocate");engine=uint32_t(memory);
 std::vector<uint8_t> root(0x120),env(0x28300),common(0x100),manager(0x900),camera(0x200),p(0x700),e(0x700),pu(0x1000),eu(0x1000),w(0x110),foreign(0x110);
 auto ptr=[](auto& x){return uint32_t(x.data());};auto put=[](uint32_t address,auto value){memcpy(reinterpret_cast<void*>(address),&value,sizeof value);};
 put(engine+0x1bc8180,ptr(root));put(ptr(root)+0x110,ptr(env));put(ptr(env)+0x281fc,ptr(manager));put(ptr(manager)+0x8e0,ptr(camera));put(ptr(camera)+0x188,ptr(p));
 for(auto actor:{ptr(p),ptr(e)}){put(actor,engine+0x15dfec8);put(actor+0xf0,uint8_t(1));put(actor+0x688,ptr(w));}
 put(ptr(p)+0x214,ptr(pu));put(ptr(e)+0x214,ptr(eu));put(ptr(pu)+4,7u);put(ptr(eu)+4,8u);put(ptr(p)+0x1c8,0u);put(ptr(e)+0x1c8,9u);
 observation::PossessionPrefix lease{};lease.magic=0x31504353;lease.version=46;lease.pid=GetCurrentProcessId();lease.ready=1;lease.mode=2;lease.actor_id=0;lease.unit_id=7;lease.lease_generation=1;lease.heartbeat=GetTickCount64();possession=&lease;
 Status telemetry{};state=&telemetry;enabled=1;coordinator=1;native_update=reinterpret_cast<NativeWitnessCall>(update_fake);native_prepare=reinterpret_cast<NativeWitnessCall>(prepare_fake);native_target=reinterpret_cast<NativeTarget>(target_fake);
 require(fps_actor()==ptr(p)&&scoped(ptr(w)),"Fresh FPS identity not recognized");
 require(dispatch(ptr(w),1,true)==0&&forwarded_updates==0,"Busy native worker forwarded owned melee");
 require(dispatch(ptr(foreign),2,true)==0xcafe006f&&forwarded_updates==1,"Busy foreign native combat did not forward exactly once");
 require(update_port(nullptr,ptr(w))==1&&forwarded_updates==1,"Stock preexisting update bypassed QTE");
 require(prepare_port(nullptr,ptr(w))==1&&forwarded_prepares==0,"Unapproved original preparation forwarded");
 require(target_hook(w.data(),nullptr,ptr(p),0,0,0,0,0,0,0,0,0,0,0,0)==0&&forwarded_targets==0,"Unapproved target registration forwarded");
 NativeActor player{ptr(p),0,ptr(pu),7,0,0},enemy{ptr(e),9,ptr(eu),8,1,0};WitnessKey key{ptr(w),1,{1,1,0,9,7,8}};
 require(!authorize(key,1,Result::Canceled,player,enemy)&&!authorize(key,0,Result::Success,player,enemy),"Invisible/canceled prompt authorized melee");
 require(authorize(key,1,Result::Late,player,enemy)&&!approved(ptr(w),1),"Unsealed native variant authorized playback");
 NativeVerdict allowed{0,ptr(p),ptr(e)};put(ptr(w)+0x80,allowed);put(ptr(w)+0x98,123u);
 require(seal_authorization(ptr(w),1,allowed)&&approved(ptr(w),1)&&!approved(ptr(w),2),"Answered failure/generation authorization differs");
 require(update_port(nullptr,ptr(w))==0xcafe006f&&forwarded_updates==2,"Approved native playback did not forward once");
 put(ptr(w)+0x80,5u);require(update_port(nullptr,ptr(w))==1&&forwarded_updates==2,"Unprompted native outcome mutation authorized a casualty");put(ptr(w)+0x80,0u);
 lease.lease_generation=2;require(update_port(nullptr,ptr(w))==1&&forwarded_updates==2,"Old lease authorized current FPS actor");lease.lease_generation=1;
 retire_authorization(ptr(w),1);require(!approved(ptr(w),1),"Removed witness retained melee permission");
 // Exercise reconstructed deterministic score contexts and the actual x86 port.
 effective::Calculator calculator{engine+0x15c5688,engine+0x1000,0};effective::ActorContext a{engine+0x15e7304,ptr(p),0,1},b{engine+0x15e7304,ptr(e),0,1};put(ptr(env)+8,ptr(common));
 effective::PairContext pair{engine+0x15e73f4,ptr(common),ptr(p),ptr(e),0};
 effective::Capture saved;require(effective::capture(reader(),engine,ptr(w),1,uint32_t(&calculator),uint32_t(&a),uint32_t(&b),uint32_t(&pair),saved),"Native stack objects not copied");
 native_score=reinterpret_cast<effective::Score>(score_fake);
 native_mitigation={reinterpret_cast<effective::Defence>(defence_fake),reinterpret_cast<effective::Armour>(armour_fake),reinterpret_cast<effective::Facing>(facing_fake)};
 put(ptr(p)+0x20,30);put(ptr(e)+0x20,10);float own{},other{};
 uintptr_t before{},after{};__asm mov before,esp
 for(unsigned i=0;i<1000;++i)require(effective::scores(saved,native_score,ptr(p),ptr(e),own,other)&&own==20&&other==-20,"Relative effective score incorrect");
 __asm mov after,esp
 require(before==after&&forwarded_scores==2000,"Native score RET10/ECX stack ABI or once-only calls differ");
 put(ptr(e)+0x20,50);require(effective::scores(saved,native_score,ptr(p),ptr(e),own,other)&&own==-20&&other==20,"Enemy active native modifier did not refresh");
 std::vector<uint8_t> foreign_a(0x700),foreign_b(0x700);
 effective::ActorContext fa{engine+0x15e7304,ptr(foreign_a),0,1},fb{engine+0x15e7304,ptr(foreign_b),0,1};
 effective::PairContext fp{engine+0x15e73f4,ptr(common),fa.actor,fb.actor,0};
 put(ptr(w)+4,ptr(env));
 combat_capture={};candidate_witness=ptr(w);candidate_generation=1;owns_coordinator=true;
 score_hook(&calculator,nullptr,&a,&b,&pair,0);
 require(combat_capture.a.actor==a.actor&&combat_capture.b.actor==b.actor,"Actual held pair score was not captured");
 require(combat_capture.valid&&combat_capture.pair.environment==ptr(common)&&score_capture_rejected==0,"Native common environment capture rejected");
 require(effective::belongs(reader(),engine,combat_capture),"Fresh captured pair lost battle membership");
 auto wrong_environment=combat_capture;wrong_environment.pair.environment=ptr(env);
 require(!effective::belongs(reader(),engine,wrong_environment),"Enclosing battle object accepted as common score environment");
 put(ptr(env)+8,0u);require(!effective::belongs(reader(),engine,combat_capture),"Missing common environment accepted");put(ptr(env)+8,ptr(common));
 for(unsigned i=0;i<500;++i)score_hook(&calculator,nullptr,&fa,&fb,&fp,0);
 require(combat_capture.a.actor==saved.a.actor&&combat_capture.b.actor==saved.b.actor,"Foreign battle score overwrote held QTE calculator capture");
 require(effective_pair(nullptr,ptr(p),ptr(e),own,other),"Pending QTE canceled after a foreign melee score");
 require(score_capture_rejected==500&&effective_score_rejected==0,"Foreign score capture rejection diagnostic differs");
 const auto old_generation=1ull;candidate_generation=old_generation+1;
 require(!effective_pair(nullptr,ptr(p),ptr(e),own,other),"Recycled witness used old score data");candidate_generation=old_generation;
 coordinator=1;auto prior_witness=candidate_witness;
 require(dispatch(ptr(foreign),2,true)==0xcafe006f&&candidate_witness==prior_witness&&candidate_generation==old_generation,"Nested foreign callback lost parent scope");
 owns_coordinator=false;
 pair.attacker=0;a.actor=0;require(effective::scores(saved,native_score,ptr(p),ptr(e),own,other),"Native stack lifetime retained");
 require(!effective::scores(saved,native_score,ptr(p)+4,ptr(e),own,other),"Foreign actor used captured native score");
 lease.release=1;require(!fps_actor()&&!scoped(ptr(w)),"RTS mode retained strict melee scope");
 VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
 std::cout<<"PASS: actual production owned/foreign busy/update/prepare/target ports; shown-answer/generation/lease authorization and removal; reconstructed native score x86 ECX/RET10 ABI, two directional margins, dynamic modifiers and stack lifetime isolation. Synthetic process only; game untouched.\n";return 0;
}catch(const std::exception& error){if(memory)VirtualFree(memory,0,MEM_RELEASE);std::cerr<<error.what()<<'\n';return 1;}}
