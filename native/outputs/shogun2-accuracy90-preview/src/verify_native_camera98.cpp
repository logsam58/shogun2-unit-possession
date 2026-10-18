#include "adapter.cpp"
#include <thread>
// Original native count/pose/FPS-update control flow, with synthetic cache,
// holder skin pose and renderer providers in a private relocated PE only.
static unsigned char policy98[0x300],cache98[32*80];
static float native_pose98[16],submitted98[16];static unsigned submits98;
static unsigned cached_count104;
static void* __fastcall snapshot98(void*,void*,void* out,float){memset(out,0,0x80);field<uintptr_t>(reinterpret_cast<uintptr_t>(out),0x24)=reinterpret_cast<uintptr_t>(policy98);return out;}
static void* __fastcall vector98(void*,void*,void* out){auto p=reinterpret_cast<uintptr_t>(out);field<unsigned>(p,0)=32;field<unsigned>(p,4)=cached_count104;field<uintptr_t>(p,8)=reinterpret_cast<uintptr_t>(cache98);return out;}
static void __fastcall cleanup98(void*,void*){}
static void __fastcall string98(void*,void*,const void*){}
static void* __fastcall input98(void*,void*,void* out,void*){memset(out,0,16);field<uintptr_t>(reinterpret_cast<uintptr_t>(out),12)=base+0x17c7c4c;return out;}
static void __fastcall scalar98(void*,void*,float){}
static void __fastcall submit98(void*,void*,const float* in){memcpy(submitted98,in,64);++submits98;}
static void* __fastcall holder_pose98(void*,void*,float* out){memcpy(out,native_pose98,64);return out;}
static void patch98(unsigned rva,void* fn){auto p=reinterpret_cast<unsigned char*>(base+rva);p[0]=0xe9;*reinterpret_cast<int32_t*>(p+1)=int32_t(reinterpret_cast<uintptr_t>(fn)-(base+rva+5));FlushInstructionCache(GetCurrentProcess(),p,5);}
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
 // Reproduce a casualty roster exposing a different holder with the same index.
 field<unsigned>(ptr(dead_actor),0x218)=17;holders={ptr(other_holder),ptr(holder)};
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
 const unsigned hooks[]={0x2d6210,0x340d30,0x3945c0,0x3cf280,0x2fcb10,0x341560,0x325ca0,0x321d70};unsigned char before[8][16];for(unsigned i=0;i<8;++i)memcpy(before[i],mapped+hooks[i],16);
 require(MH_Initialize()==MH_OK,"Camera regression hooks initialize");create_siege_hooks97();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Camera regression hooks enable");
 for(unsigned i=0;i<10000;++i){state->heartbeat=GetTickCount64();cached_count104=i&1;
   // Current roster compaction and the cached native count vary independently.
   if(i%4<2){holders={ptr(other_holder),ptr(holder)};field<unsigned>(v,0x18)=2;}
   else {holders={ptr(holder),ptr(other_holder)};field<unsigned>(v,0x18)=1;}
   require(count(reinterpret_cast<void*>(v))==cached_count104,"Non-camera roster count must remain native during possession");
   require(pose(reinterpret_cast<void*>(v),result,17,.5f)==result&&!memcmp(result,cache98,64),"Non-camera pose must remain native during possession");
   require(update(reinterpret_cast<void*>(ptr(cam)),.1f,.5f)&&field<uintptr_t>(ptr(cam),0x188)==a&&field<unsigned>(ptr(cam),0x1a4)==17,"Exact native FPS update retains selected soldier and AL success");
   require(!memcmp(submitted98,native_pose98,64)&&camera_scope104==0,"Native FPS renderer gets exact current holder pose and leaves no scope");
 }
 require(view_count_bypasses98==10000&&view_zero_count98==5000&&view_native_count98==1&&view_pose_bypasses98==10000&&submits98==10000,"Private scope diagnostics count actual native zero/nonzero-cache update paths");
 cached_count104=0;holders={ptr(other_holder),ptr(holder)};field<unsigned>(v,0x18)=2;
 // Foreign index, released lease, dead actor, serial reuse, changed selection,
 // camera, heartbeat, phase and battle identity all retain original policy.
 require(pose(reinterpret_cast<void*>(v),result,18,.5f)==result&&!memcmp(result,cache98,64),"Foreign logical index keeps original cached pose");
 auto raw_actor=reinterpret_cast<ViewActor97>(base+0x340cf0);
 require(raw_actor(reinterpret_cast<void*>(v),17)==ptr(dead_actor),"Non-camera duplicate-index lookup lost native ordering");
 ++camera_scope104;require(raw_actor(reinterpret_cast<void*>(v),17)==a,"Scoped native lookup did not resolve exact current holder");
 unsigned other_thread=99;std::thread outsider([&]{other_thread=count(reinterpret_cast<void*>(v));});outsider.join();
 require(other_thread==0,"Camera roster scope leaked to another native thread");--camera_scope104;
 melee60::Request pair104{};pair104.pid=GetCurrentProcessId();pair104.phase=melee60::NativeCombat;pair104.lease=possession_lease.generation;
 pair104.player=1203;pair104.player_unit=11;pair104.opponent=9;pair104.opponent_unit=12;pair104.serial=1;pair104.heartbeat=GetTickCount64();
 require(PossessionMeleeCommitment63(&pair104)==63,"Camera pair fixture publish");field<unsigned char>(a,0xa2)=1;state->heartbeat=GetTickCount64();
 require(followed(a,false)&&update(reinterpret_cast<void*>(ptr(cam)),.1f,.5f)&&field<uintptr_t>(ptr(cam),0x188)==a,"Current committed actor lost its camera during non-ground paired motion");
 melee60::Request off104{};PossessionMeleeCommitment63(&off104);field<unsigned char>(a,0xa2)=0;
 unsigned guards=0;
 auto denied=[&](){++camera_scope104;require(count(reinterpret_cast<void*>(v))==0,"Invalid ownership keeps original count policy");require(pose(reinterpret_cast<void*>(v),result,17,.5f)==result&&!memcmp(result,cache98,64),"Invalid ownership keeps original cached pose policy");--camera_scope104;++guards;};
 state->release=1;denied();state->release=0;
 state->stop=1;denied();state->stop=0;
 enabled=0;denied();enabled=1;
 field<unsigned char>(a,0xf0)=0;denied();field<unsigned char>(a,0xf0)=1;
 field<unsigned>(a,0x1c8)=1204;denied();field<unsigned>(a,0x1c8)=1203;
 units[0]=0;denied();units[0]=u;
 field<uintptr_t>(ptr(manager),0x8e0)=0;denied();field<uintptr_t>(ptr(manager),0x8e0)=ptr(cam);
 state->heartbeat=GetTickCount64()-501;denied();state->heartbeat=GetTickCount64();
 field<unsigned>(ph,0x14)=9;denied();field<unsigned>(ph,0x14)=6;
 field<uintptr_t>(ptr(root),0x110)=0;denied();field<uintptr_t>(ptr(root),0x110)=en;
 state->mode=ARMED;possessed=0;possession_lease={};denied();
 Choice97 c{en,ph,ptr(manager),v,u,ptr(holder),a,1203,17};choice97=c;
 require(count(reinterpret_cast<void*>(v))==2,"Fully validated TLS acquisition scope works before FPS adoption");choice97={};denied();
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Camera regression disable");for(unsigned i=0;i<8;++i)require(!memcmp(before[i],mapped+hooks[i],16),"All six camera regression hooks restore exact bytes");require(MH_Uninitialize()==MH_OK,"Camera regression teardown");
 if(climb_shared97)UnmapViewOfFile(climb_shared97);if(climb_mapping97)CloseHandle(climb_mapping97);climb_shared97=nullptr;climb_mapping97=nullptr;
 VirtualFree(mapped,0,MEM_RELEASE);mapped=nullptr;std::cout<<"PASS: native camera98 original cached-count/pose and FPS325ca0 reproduction, 10000 original FPS updates/current-holder pose calls; duplicate casualty index and non-camera/thread native preservation, "<<guards<<" strict ownership guards, eight exact hook restorations. Synthetic cache/skin/input/render providers; no game initialization.\n";return 0;
 }catch(const std::exception& e){if(mapped)VirtualFree(mapped,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
