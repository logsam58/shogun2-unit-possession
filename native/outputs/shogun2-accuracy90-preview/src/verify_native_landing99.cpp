#include "adapter.cpp"
// Original native count/pose/FPS-update control flow, with synthetic cache,
// holder skin pose and renderer providers in a private relocated PE only.
static unsigned char policy98[0x300],cache98[32*80];
static float native_pose98[16],submitted98[16];static unsigned submits98;
static void* __fastcall snapshot98(void*,void*,void* out,float){memset(out,0,0x80);field<uintptr_t>(reinterpret_cast<uintptr_t>(out),0x24)=reinterpret_cast<uintptr_t>(policy98);return out;}
static void* __fastcall vector98(void*,void*,void* out){auto p=reinterpret_cast<uintptr_t>(out);field<unsigned>(p,0)=32;field<unsigned>(p,4)=0;field<uintptr_t>(p,8)=reinterpret_cast<uintptr_t>(cache98);return out;}
static void __fastcall cleanup98(void*,void*){}
static void __fastcall string98(void*,void*,const void*){}
static void* __fastcall input98(void*,void*,void* out,void*){memset(out,0,16);field<uintptr_t>(reinterpret_cast<uintptr_t>(out),12)=base+0x17c7c4c;return out;}
static void __fastcall scalar98(void*,void*,float){}
static void __fastcall submit98(void*,void*,const float* in){memcpy(submitted98,in,64);++submits98;}
static void* __fastcall holder_pose98(void*,void*,float* out){memcpy(out,native_pose98,64);return out;}
static void patch98(unsigned rva,void* fn){auto p=reinterpret_cast<unsigned char*>(base+rva);p[0]=0xe9;*reinterpret_cast<int32_t*>(p+1)=int32_t(reinterpret_cast<uintptr_t>(fn)-(base+rva+5));FlushInstructionCache(GetCurrentProcess(),p,5);}
static unsigned root_frames99,goal_reads99;
static void __fastcall root_events99(void*,void*,uintptr_t,unsigned){++root_frames99;}
static void __fastcall reset_goal99(void*,void*,unsigned){}
static void* __fastcall goal99(void*,void*,void* out){memset(out,0,20);++goal_reads99;return out;}
int main(){unsigned char* mapped=nullptr;try{
 std::wstring path=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Total War SHOGUN 2\\Empire.Retail.dll";require(hash_file(path)==EXPECTED_HASH,"Exact native fixture image");
 std::ifstream in(path,std::ios::binary);std::vector<unsigned char> image((std::istreambuf_iterator<char>(in)),{});auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());auto nt=reinterpret_cast<IMAGE_NT_HEADERS32*>(image.data()+dos->e_lfanew);
 mapped=static_cast<unsigned char*>(VirtualAlloc(nullptr,nt->OptionalHeader.SizeOfImage,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(mapped!=nullptr,"Native fixture allocation");memcpy(mapped,image.data(),nt->OptionalHeader.SizeOfHeaders);
 auto sections=IMAGE_FIRST_SECTION(nt);for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto&s=sections[i];if(s.SizeOfRawData){require(s.PointerToRawData+s.SizeOfRawData<=image.size(),"Native section bounds");memcpy(mapped+s.VirtualAddress,image.data()+s.PointerToRawData,s.SizeOfRawData);}}
 uint32_t delta=uint32_t(reinterpret_cast<uintptr_t>(mapped)-nt->OptionalHeader.ImageBase);auto reloc=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];unsigned offset=0;
 while(offset<reloc.Size){auto block=reinterpret_cast<IMAGE_BASE_RELOCATION*>(mapped+reloc.VirtualAddress+offset);require(block->SizeOfBlock>=8&&offset+block->SizeOfBlock<=reloc.Size,"Relocation bounds");auto entries=reinterpret_cast<uint16_t*>(block+1);
  for(unsigned i=0;i<(block->SizeOfBlock-8)/2;++i)if(entries[i]>>12==IMAGE_REL_BASED_HIGHLOW)*reinterpret_cast<uint32_t*>(mapped+block->VirtualAddress+(entries[i]&4095))+=delta;offset+=block->SizeOfBlock;}
 DWORD old=0;require(VirtualProtect(mapped,nt->OptionalHeader.SizeOfImage,PAGE_EXECUTE_READWRITE,&old)!=0,"Read-only native fixture image");FlushInstructionCache(GetCurrentProcess(),mapped,nt->OptionalHeader.SizeOfImage);

 base=reinterpret_cast<uintptr_t>(mapped);
 const unsigned char routing[]={0x31,0xc0,0xc3};memcpy(mapped+0x1821e0,routing,sizeof routing);
 climb97::Status publication;publication.pid=77;publication.lease=5;publication.actor=1203;publication.unit=11;publication.tick=1000;publication.mode=climb97::Climbing;
 require(climb97::valid(publication,77,5,1203,11,1000)&&!climb97::valid(publication,77,6,1203,11,1000)&&!climb97::valid(publication,77,5,1204,11,1000)&&!climb97::valid(publication,78,5,1203,11,1000)&&!climb97::valid(publication,77,5,1203,11,1501)&&!climb97::valid(publication,77,5,1203,11,999),"Numeric wall prompt stale/battle/lease identity guards");
 Status status{};state=&status;enabled=1;state->heartbeat=GetTickCount64();state->acquire=1;state->mode=ARMED;
 std::vector<unsigned char> root(0x120),env(0x28300),common(0x100),phase(0x400),manager(0x910),cam(0x200),view(0x90),holder(0x210),other_holder(0x210),selected(0x500),owner(0x120),config(0x100),actor(0x700),dead_actor(0x700),unitbuf(0x1000),pipebuf(0x100),source(0x100),definition(0x80);
 auto ptr=[](auto& v){return reinterpret_cast<uintptr_t>(v.data());};auto a=ptr(actor),u=ptr(unitbuf),v=ptr(view),pipe=ptr(pipebuf),def=ptr(definition),ph=ptr(phase),en=ptr(env);
 unsigned char nodes[]={0x0b,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0xde,0xca,0x1f,0x43,0x89,0x13,0xfe,0xc2,0x00,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0xf1,0xd9,0x31,0x43,0x3f,0xa2,0xe7,0xc2,0x00,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0xde,0xca,0x1f,0x43,0xa8,0x18,0xfe,0xc2,0x5a,0x1c,0x58,0xc2,0xf1,0xd9,0x31,0x43,0x3f,0xa2,0xe7,0xc2,0x44,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x0b,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0xf1,0xd9,0x31,0x43,0x3f,0xa2,0xe7,0xc2,0x00,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0x50,0xca,0x35,0x43,0xc9,0xa8,0xe7,0xc2,0x00,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0xf1,0xd9,0x31,0x43,0x3f,0xa2,0xe7,0xc2,0x5a,0x1c,0x58,0xc2,0x50,0xca,0x35,0x43,0xc9,0xa8,0xe7,0xc2,0x44,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x0b,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0x50,0xca,0x35,0x43,0xc9,0xa8,0xe7,0xc2,0x00,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0x1b,0xcd,0x35,0x43,0xe7,0x53,0xe4,0xc2,0x00,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0x50,0xca,0x35,0x43,0xc9,0xa8,0xe7,0xc2,0x5a,0x1c,0x58,0xc2,0x1b,0xcd,0x35,0x43,0xe7,0x53,0xe4,0xc2,0x44,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x0b,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0x1b,0xcd,0x35,0x43,0xe7,0x53,0xe4,0xc2,0x00,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0xcd,0xcc,0x34,0x43,0x00,0x00,0xe2,0xc2,0x00,0x00,0x00,0x00,0x5a,0x1c,0x58,0xc2,0x1b,0xcd,0x35,0x43,0xe7,0x53,0xe4,0xc2,0x5a,0x1c,0x58,0xc2,0xcd,0xcc,0x34,0x43,0x00,0x00,0xe2,0xc2,0x45,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00};std::array<uintptr_t,2> active_crest98{1067,ptr(config)};std::array<uintptr_t,2> holders{ptr(other_holder),ptr(holder)};std::array<uintptr_t,1> units{u},pipes{pipe};
 field<uintptr_t>(base,0x1bc8180)=ptr(root);field<uintptr_t>(ptr(root),0x110)=en;field<uintptr_t>(en,8)=ptr(common);field<uintptr_t>(ptr(common),0xb4)=ph;field<unsigned>(ph,0x14)=6;
 field<uintptr_t>(en,0x281fc)=ptr(manager);field<uintptr_t>(ptr(manager),0)=base+0x15d26fc;field<uintptr_t>(ptr(manager),0x8e0)=ptr(cam);field<uintptr_t>(ptr(manager),0x8f8)=v;field<uintptr_t>(ptr(manager),0x8fc)=a;
 field<uintptr_t>(ptr(cam),0)=base+0x15d22e8;field<uintptr_t>(ptr(cam),0x180)=ptr(manager);field<uintptr_t>(ptr(cam),0x184)=v;field<uintptr_t>(ptr(cam),0x188)=a;field<float>(ptr(cam),0x6c)=1;
 field<uintptr_t>(v,0x6c)=en;field<unsigned>(v,0x18)=2;field<uintptr_t>(v,0x1c)=reinterpret_cast<uintptr_t>(holders.data());field<uintptr_t>(ptr(holder),8)=a;field<uintptr_t>(ptr(other_holder),8)=ptr(dead_actor);
 field<uintptr_t>(en,0x2826c)=ptr(selected);field<uintptr_t>(ptr(selected),0x4a8)=ptr(owner);auto group=ptr(owner)+0xb0;field<uintptr_t>(group,0)=base+0x15c4da0;field<unsigned>(group,0x10)=field<unsigned>(group,0x14)=1;field<uintptr_t>(group,0x18)=reinterpret_cast<uintptr_t>(units.data());
 for(auto subject:{a,ptr(dead_actor)}){field<uintptr_t>(subject,0)=base+0x15dfec8;field<uintptr_t>(subject,0x214)=u;field<uintptr_t>(subject,0x20c)=ptr(config);field<uintptr_t>(subject,0x28)=en;}
 field<unsigned char>(a,0xf0)=field<unsigned char>(a,0xa1)=1;field<unsigned>(a,0x1c8)=1203;field<unsigned>(a,0x218)=17;field<unsigned>(u,4)=11;field<uintptr_t>(u,0xf9c)=ptr(config);
 field<unsigned>(ph,0x2fc)=field<unsigned>(ph,0x300)=1;field<uintptr_t>(ph,0x304)=reinterpret_cast<uintptr_t>(pipes.data());
 field<uintptr_t>(pipe,0)=base+0x15e7fd0;field<unsigned char>(pipe,4)=1;field<unsigned>(pipe,0x10)=0x854;field<uintptr_t>(pipe,0x14)=ptr(manager);field<uintptr_t>(pipe,0x30)=ptr(source);field<uintptr_t>(pipe,0x34)=def;field<unsigned>(pipe,0x40)=11;field<uintptr_t>(ptr(source),0x30)=pipe;field<unsigned>(ptr(source),0x40)=11;
 field<uintptr_t>(def,0)=base+0x15dd88c;field<float>(base+0x1a4d3a0,0x5c)=.6f;field<float>(base+0x1a4d2e0,0x5c)=2.25f;
 field<unsigned>(def,0x10)=field<unsigned>(def,0x14)=4;field<uintptr_t>(def,0x18)=reinterpret_cast<uintptr_t>(nodes);

 field<uintptr_t>(v,0)=base+0x15d3540;
 uintptr_t holder_vt98[2]={0,reinterpret_cast<uintptr_t>(&holder_pose98)};field<uintptr_t>(ptr(holder),0)=reinterpret_cast<uintptr_t>(holder_vt98);
 for(unsigned i=0;i<16;++i)native_pose98[i]=float(i+1)*.125f;
 native_pose98[15]=1;field<float>(base+0x192e380,0x5c)=0;
 patch98(0x294460,reinterpret_cast<void*>(&snapshot98));patch98(0x453650,reinterpret_cast<void*>(&vector98));
 patch98(0x27eaf0,reinterpret_cast<void*>(&cleanup98));patch98(0x27ef60,reinterpret_cast<void*>(&cleanup98));
 patch98(0xecc90,reinterpret_cast<void*>(&string98));patch98(0x97f50,reinterpret_cast<void*>(&input98));
 patch98(0x134fad0,reinterpret_cast<void*>(&submit98));patch98(0x134f1d0,reinterpret_cast<void*>(&cleanup98));patch98(0x1374c60,reinterpret_cast<void*>(&scalar98));
 field<uintptr_t>(base+0x15d22e8,0x74)=reinterpret_cast<uintptr_t>(&scalar98);
 auto count=reinterpret_cast<ViewIndex97>(base+0x2fcb10);auto pose=reinterpret_cast<ViewPose98>(base+0x341560);
 using Update=bool(__thiscall*)(void*,float,float);auto update=reinterpret_cast<Update>(base+0x325ca0);float result[16]{};
 require(count(reinterpret_cast<void*>(v))==0,"Original cached count reproduces empty aggregate despite two current holders");
 require(pose(reinterpret_cast<void*>(v),result,17,.5f)==result&&!memcmp(result,cache98,64),"Original cached pose reads aggregate rather than current holder");
 require(!update(reinterpret_cast<void*>(ptr(cam)),.1f,.5f)&&submits98==0,"Exact native FPS update rejects cached empty aggregate before Test98");
 require(adopt_possession97(a),"Full native identity adoption for current live holder");
 const unsigned hooks[]={0x2d6210,0x340d30,0x3945c0,0x3cf280,0x2fcb10,0x341560};unsigned char before[6][16];for(unsigned i=0;i<6;++i)memcpy(before[i],mapped+hooks[i],16);
 require(MH_Initialize()==MH_OK,"Camera regression hooks initialize");create_siege_hooks97();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Camera regression hooks enable");

 // Only world/root-event providers are substituted. Exact native frame-end,
 // root-clip predicate, stop, human virtual+74 and native state writes execute.
 patch98(0x3f03d0,reinterpret_cast<void*>(&root_events99));patch98(0x3e93c0,reinterpret_cast<void*>(&reset_goal99));patch98(0x3df250,reinterpret_cast<void*>(&goal99));
 std::vector<unsigned char> asset99(0x80);uintptr_t clip99[2]={1067,ptr(asset99)};
 using Native=void(__thiscall*)(void*);auto final_frame99=reinterpret_cast<Native>(base+0x3f0370);
 // Execute exact native contact flag publication, then return before collision,
 // casualty/AI world providers. Full contact physics is not certified here.
 const unsigned char contact_return99[]={0x5d,0x81,0xc4,0xe4,0,0,0,0xc3};memcpy(mapped+0x3e9e00,contact_return99,sizeof contact_return99);FlushInstructionCache(GetCurrentProcess(),mapped+0x3e9e00,sizeof contact_return99);
 auto contact99=reinterpret_cast<Native>(base+0x3e9de0);unsigned cycles99=0,air_updates99=0;
 for(unsigned clip:{1067u,1068u,1069u,1070u,1071u,1072u,1073u,1074u})for(unsigned mode:{0u,8u,72u})for(unsigned frames:{32u,47u,64u}){
  state->heartbeat=GetTickCount64();field<unsigned>(ptr(asset99),0x64)=frames;clip99[0]=clip;
  field<uintptr_t>(a,0x474)=reinterpret_cast<uintptr_t>(clip99);field<unsigned>(a,0x1d4)=clip;field<unsigned>(a,0x1d8)=frames-1;field<unsigned>(a,0x1f0)=mode;
  field<unsigned char>(a,0xa1)=field<unsigned char>(a,0xa2)=field<unsigned char>(a,0xa3)=field<unsigned char>(a,0xa4)=0;field<unsigned char>(a,0xa7)=1;
  require(followed(a,false,false),"Current native crest admitted before final frame");final_frame99(reinterpret_cast<void*>(a));
  require(field<unsigned>(a,0x1d8)==frames&&!field<uintptr_t>(a,0x474)&&field<unsigned char>(a,0xa1)==1&&field<unsigned char>(a,0xa2)==1&&!field<unsigned char>(a,0xa7)&&field<unsigned>(a,0x554)==18,"Exact original3f0370/3ca920/3e9c50 crest completion publishes native airborne and clears active clip");
  require(!root_motion_subject(a,false),"Old ground-only predicate rejects exact native cleanup state");
  // Requested ground pose after native cleanup reproduces missing wall fallback.
  field<unsigned>(a,0x1f0)=0;require(!wall_subject97(a),"Pose is no longer a wall/crest; no stale crest grace");require(native_airborne_subject99(a)&&followed(a,false,false),"Same strict lease retains current native airborne actor");
  auto original_bytes99=actor;
  for(unsigned tick=0;tick<20;++tick){state->heartbeat=GetTickCount64();state->input=127;state->aiming=state->fire_request=1;
   require(!prepare(a)&&actor==original_bytes99,"Production motion boundary forwards original physics without any actor write or player stride");
   require(native_air_active99==1&&keyboard_wall98&&!state->aiming&&!state->fire_request&&!weapon_owned(a),"Native airborne phase suppresses player weapon/ground intent");
   require(count(reinterpret_cast<void*>(v))==2&&update(reinterpret_cast<void*>(ptr(cam)),.1f,.5f),"Original native FPS update survives empty cached roster throughout airborne handoff");++air_updates99;
  }
  contact99(reinterpret_cast<void*>(a));require(field<unsigned char>(a,0xa1)==1&&!field<unsigned char>(a,0xa2)&&root_motion_subject(a,false)&&followed(a,false,false),"Exact native contact flag publication returns same lease to normal ground eligibility");
  climb_observe98(a);require(!native_air_active99,"Landing stops native-air handoff immediately");++cycles99;
 }
 require(root_frames99==72&&goal_reads99==72&&cycles99==72&&air_updates99==1440,"Native crest/air/contact lifecycle counts");
 // Native current state validity, not pose/time or an ownership grace window.
 unsigned flags99=0;for(unsigned a1:{0u,1u,2u})for(unsigned a2:{0u,1u,2u})for(unsigned a3:{0u,1u,2u})for(unsigned a4:{0u,1u,2u}){
  field<unsigned char>(a,0xa1)=static_cast<unsigned char>(a1);field<unsigned char>(a,0xa2)=static_cast<unsigned char>(a2);field<unsigned char>(a,0xa3)=static_cast<unsigned char>(a3);field<unsigned char>(a,0xa4)=static_cast<unsigned char>(a4);
  require(native_airborne_subject99(a)==(a1==1&&a2==1&&!a3&&!a4),"Only native live/free airborne flag combination admitted");++flags99;
 }
 field<unsigned char>(a,0xa1)=field<unsigned char>(a,0xa2)=1;field<unsigned char>(a,0xa3)=field<unsigned char>(a,0xa4)=0;
 field<unsigned char>(a,0xf0)=0;require(!native_airborne_subject99(a),"Dead airborne actor rejected");field<unsigned char>(a,0xf0)=1;
 field<uintptr_t>(a,0x18)=pipe;require(!native_airborne_subject99(a),"Airborne does not admit foreign pipe");field<uintptr_t>(a,0x18)=0;
 field<uintptr_t>(a,0x210)=ptr(config);require(!native_airborne_subject99(a),"Mounted actor excluded");field<uintptr_t>(a,0x210)=0;
 field<uintptr_t>(u,0xf9c)=0;require(!native_airborne_subject99(a),"Non-foot config excluded");field<uintptr_t>(u,0xf9c)=ptr(config);
 field<unsigned>(a,0x1c8)=1204;require(!followed(a,false,false)&&count(reinterpret_cast<void*>(v))==0,"Airborne actor serial reuse never retains camera");field<unsigned>(a,0x1c8)=1203;
 holders[1]=0;require(!followed(a,false,false)&&count(reinterpret_cast<void*>(v))==0,"Airborne holder removal never retains camera");holders[1]=ptr(holder);
 state->release=1;require(count(reinterpret_cast<void*>(v))==0,"Explicit release always forwards native cached policy");state->release=0;
 state->stop=1;require(count(reinterpret_cast<void*>(v))==0,"Stop forwards native cached policy");state->stop=0;
 field<unsigned>(ph,0x14)=9;require(!followed(a,false,false)&&count(reinterpret_cast<void*>(v))==0,"Battle end never retains airborne lease");field<unsigned>(ph,0x14)=6;
 field<unsigned char>(a,0xa3)=1;require(!followed(a,false,false)&&count(reinterpret_cast<void*>(v))==0,"Other physical motion class never gains airborne bypass");field<unsigned char>(a,0xa3)=0;
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Landing fixture hook disable");for(unsigned i=0;i<6;++i)require(!memcmp(before[i],mapped+hooks[i],16),"All six existing hook bytes restore; no new99 hooks");require(MH_Uninitialize()==MH_OK,"Landing fixture teardown");
 if(climb_shared97)UnmapViewOfFile(climb_shared97);if(climb_mapping97)CloseHandle(climb_mapping97);climb_shared97=nullptr;climb_mapping97=nullptr;
 VirtualFree(mapped,0,MEM_RELEASE);mapped=nullptr;std::cout<<"PASS: native landing99 72 original frame-end/crest-cleanup/human-air-state lifecycles, 1440 untouched actor/physics-forwarding and native FPS calls, exact native contact flag publication, 81 native flag combinations, strict death/pipe/mount/config/serial/holder/release/stop/phase guards; six existing hooks restored. World/root-event providers synthetic; contact tail substituted; no game initialization.\n";return 0;
 }catch(const std::exception& e){if(mapped)VirtualFree(mapped,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
