#include "adapter.cpp"
static unsigned forwarded_index97,forwarded_actor97;
static unsigned __fastcall fake_index97(void*,void*){++forwarded_index97;return 999;}
static uintptr_t __fastcall fake_actor97(void*,void*,unsigned){++forwarded_actor97;return 0;}
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
 Wall97 wall;require(read_wall97(a,pipe,wall),"Recorded native wall path proof");
 original_pipe_advance97=reinterpret_cast<PipeAdvance97>(base+0x3945c0);original_pipe_mode97=reinterpret_cast<PipeMode97>(base+0x3cf280);original_view_index97=reinterpret_cast<ViewIndex97>(&fake_index97);original_view_actor97=reinterpret_cast<ViewActor97>(&fake_actor97);
 require(adopt_possession97(a)&&state->mode==POSSESSED&&!state->acquire&&possessed==a,"F9 must adopt immediately without motion callback");
 Choice97 choice{en,ph,ptr(manager),v,u,ptr(holder),a,1203,17};choice97=choice;
 require(view_index_hook97(reinterpret_cast<void*>(v),nullptr)==17,"F9 chooses eligible live actor after dead first member");
 require(view_actor_hook97(reinterpret_cast<void*>(v),nullptr,17)==a,"Scoped native logical-index lookup resolves current holder");
 choice97={};require(view_actor_hook97(reinterpret_cast<void*>(v),nullptr,18)==0&&forwarded_actor97==1,"Foreign index forwarded");
 field<unsigned>(a,0x1c8)=1204;require(!followed(a,false)&&!choice_valid97(v,choice),"ABA actor serial rejected");field<unsigned>(a,0x1c8)=1203;
 field<uintptr_t>(a,0x18)=pipe;field<unsigned>(a,0x1f0)=68;require(followed(a,false),"Same lease survives native climb attachment");
 using Point=uintptr_t(__thiscall*)(void*,float*,float);using Project=float(__thiscall*)(void*,const float*);auto point=reinterpret_cast<Point>(base+0x39a2e0);auto project=reinterpret_cast<Project>(base+0x38cc00);float length=reinterpret_cast<float(__thiscall*)(void*)>(base+0x3931f0)(reinterpret_cast<void*>(def));require(length>20&&length<40,"Original native captured wall length");
 unsigned cases=0;
 for(float p:{0.f,.001f,.1f,.4f,.7f,.9f,.999f,1.f})for(float step:{.001f,.01f,.1f})for(unsigned input:{0u,1u,2u,3u}){
   state->heartbeat=GetTickCount64();field<float>(base,0x15b2448)=1.f/60;state->input=input;field<unsigned>(a,0x1f0)=68;
   float position[4]{};point(reinterpret_cast<void*>(def),position,p);for(unsigned k=0;k<3;++k){field<float>(a,0x48+k*4)=position[k];field<float>(a,0xd8+k*4)=0;}
   field<float>(a,0x20)=field<float>(a,0x24)=p;
   ++field<unsigned>(ptr(common),0x50);pipe_advance_hook97(reinterpret_cast<void*>(a+0x14),nullptr,pipe,step);
   float budget=(input==2?1.f:.6f)*field<float>(base,0x15b2448);float expected=std::clamp(p+(input==1?budget/length:input==2?-budget/length:0),0.f,1.f);
   require(std::abs(field<float>(a,0x20)-expected)<.00001f,"Native W/S/hold progress including top/bottom");
   float targetpoint[4]{};point(reinterpret_cast<void*>(def),targetpoint,expected);
   for(unsigned k=0;k<3;++k)require(std::abs(field<float>(a,0xd8+k*4)-(targetpoint[k]-position[k]))<.0002f,"Native path intent XYZ, actor root untouched");
   require(std::abs(project(reinterpret_cast<void*>(def),position)-p)<.00001f,"Exact native projection/evaluation round trip");++cases;
 }
 for(unsigned input:{1u,2u}){
   state->heartbeat=GetTickCount64();state->input=input;float position[4]{};point(reinterpret_cast<void*>(def),position,.2f);
   for(unsigned k=0;k<3;++k){field<float>(a,0x48+k*4)=position[k];field<float>(a,0xd8+k*4)=0;}field<float>(a,0xd8)=.1f;
   ++field<unsigned>(ptr(common),0x50);pipe_advance_hook97(reinterpret_cast<void*>(a+0x14),nullptr,pipe,0);
   require(std::abs(field<float>(a,0x20)-(.2f+(input==1?.6f/60/length:-1.f/60/length)))<.00001f,"Native rate replaces stale pending root stride in both directions");
   field<float>(base,0x15b2448)=0;for(unsigned k=0;k<3;++k){field<float>(a,0x48+k*4)=position[k];field<float>(a,0xd8+k*4)=0;}field<float>(a,0xd8)=.1f;
   ++field<unsigned>(ptr(common),0x50);pipe_advance_hook97(reinterpret_cast<void*>(a+0x14),nullptr,pipe,.1f);require(std::abs(field<float>(a,0x20)-.2f)<.00001f,"Actual pause freezes owned traversal");field<float>(base,0x15b2448)=1.f/60;
 }

 unsigned rate_cases98=0;for(bool fast:{false,true})for(float normal:{.3f,.6f,.9f})for(float segment:{.5f,1.f,1.5f})for(float dt:{.01f,.05f,.1f,.2f})for(unsigned direction98:{1u,2u}){
   field<unsigned char>(u,0x294)=fast?1:0;field<float>(base+(fast?0x1a4d2e0:0x1a4d3a0),0x5c)=normal;for(unsigned n=0;n<4;++n)field<float>(reinterpret_cast<uintptr_t>(nodes)+n*72,0x40)=segment;
   state->input=direction98;state->heartbeat=GetTickCount64();field<float>(base,0x15b2448)=dt;float position[4]{};point(reinterpret_cast<void*>(def),position,.2f);
   for(unsigned k=0;k<3;++k){field<float>(a,0x48+k*4)=position[k];field<float>(a,0xd8+k*4)=3;}field<float>(a,0x20)=.2f;
   ++field<unsigned>(ptr(common),0x50);pipe_advance_hook97(reinterpret_cast<void*>(a+0x14),nullptr,pipe,100);
   require(std::abs(field<float>(a,0x20)-(.2f+(direction98==1?normal:-1.f)*segment*dt/length))<.00001f,"Live native parameter/unit capability/segment/tick rate rather than ground pending or argument");
   float progress98=field<float>(a,0x20),intent98[3];for(unsigned k=0;k<3;++k)intent98[k]=field<float>(a,0xd8+k*4);auto steps98=climb_steps97;
   for(unsigned repeat=0;repeat<10;++repeat)pipe_advance_hook97(reinterpret_cast<void*>(a+0x14),nullptr,pipe,100);
   require(field<float>(a,0x20)==progress98&&climb_steps97==steps98,"Duplicate callbacks within one simulation tick do not consume another climb budget");for(unsigned k=0;k<3;++k)require(intent98[k]==field<float>(a,0xd8+k*4),"Duplicate callbacks preserve one pending native step");++rate_cases98;
 }
 field<unsigned char>(u,0x294)=0;field<float>(base+0x1a4d3a0,0x5c)=.6f;field<float>(base+0x1a4d2e0,0x5c)=2.25f;field<float>(base,0x15b2448)=1.f/60;
 for(unsigned n=0;n<4;++n)field<float>(reinterpret_cast<uintptr_t>(nodes)+n*72,0x40)=1;
 field<unsigned>(a,0x1f0)=8;climb_adopt98(a);require(keyboard_wall98&&climb_prepare97(a),"Current registered attachment wins during old ground-pose transition; no F prompt");field<unsigned>(a,0x1f0)=68;
 state->release=1;float peer_position[4]{};point(reinterpret_cast<void*>(def),peer_position,.2f);for(unsigned k=0;k<3;++k){field<float>(a,0x48+k*4)=peer_position[k];field<float>(a,0xd8+k*4)=0;}
 auto previous_steps=climb_steps97;pipe_advance_hook97(reinterpret_cast<void*>(a+0x14),nullptr,pipe,.1f);require(climb_steps97==previous_steps&&std::abs(field<float>(a,0x20)-(.2f+.1f/length))<.00001f,"Released ownership forwards native AI progress exactly");state->release=0;
 // Native crest selector and native downward state, without replacing its RNG/playback.
 for(unsigned kind=11;kind<=14;++kind){field<unsigned>(pipe,0x40)=field<unsigned>(ptr(source),0x40)=kind;field<float>(a,0x20)=.8f;state->input=1;
   require(pipe_mode_hook97(reinterpret_cast<void*>(a),nullptr,pipe)==72+kind-11,"Original native crest selection retained");state->input=2;require(pipe_mode_hook97(reinterpret_cast<void*>(a),nullptr,pipe)==69,"Reverse traversal avoids ascending crest event");}
 field<unsigned char>(u,0x294)=1;state->input=2;require(pipe_mode_hook97(reinterpret_cast<void*>(a),nullptr,pipe)==71,"Native fast descending capability retained at crest segment");field<unsigned char>(u,0x294)=0;
 field<unsigned>(pipe,0x40)=field<unsigned>(ptr(source),0x40)=11;field<uintptr_t>(a,0x18)=0;field<unsigned>(a,0x1f0)=72;field<unsigned>(a,0x1d4)=1067;field<uintptr_t>(a,0x474)=reinterpret_cast<uintptr_t>(active_crest98.data());field<unsigned char>(a,0xa1)=0;field<unsigned char>(a,0xa7)=1;
 require(followed(a,false),"Native crest root-motion lease retained");field<unsigned>(a,0x1d4)=55;active_crest98[0]=55;require(!followed(a,false),"Unrelated root-only state rejected");field<unsigned>(a,0x1d4)=1067;active_crest98[0]=1067;field<unsigned char>(a,0xa7)=0;require(followed(a,false),"Native detach before a7 publication retains exact active crest");field<unsigned>(a,0x1f0)=8;require(followed(a,false),"Active crest survives native requested ground mode");field<unsigned>(a,0x1f0)=72;
 field<unsigned char>(a,0xf0)=0;require(!wall_subject97(a),"Dead wall actor rejected");field<unsigned char>(a,0xf0)=1;
 field<uintptr_t>(a,0x474)=0;field<uintptr_t>(a,0x18)=pipe;field<unsigned>(a,0x1f0)=68;pipes[0]=0;require(!wall_subject97(a)&&!followed(a,false),"Removed resource rejected, cached index never grants ownership");pipes[0]=pipe;
 field<uintptr_t>(ptr(source),0x30)=0;require(!wall_subject97(a),"Resource identity reused rejected");field<uintptr_t>(ptr(source),0x30)=pipe;
 field<uintptr_t>(pipe,0x30)=0;require(wall_subject97(a)&&followed(a,false),"Actual live null construction source accepted through current registry");
 field<unsigned>(ph,0x14)=9;require(!followed(a,false),"Battle phase change rejected");field<unsigned>(ph,0x14)=6;
 climb_last_direction97=1;
 for(unsigned i=0;i<10000;++i){state->input=0;field<float>(a,0x20)=.2f;require(pipe_mode_hook97(reinterpret_cast<void*>(a),nullptr,pipe)==68,"Stable native up/hold mode and ABI");}
 // Actual MinHook/trampoline ABI and exact restoration, in this fixture only.
 const unsigned hook_rvas[]={0x2d6210,0x340d30,0x3945c0,0x3cf280,0x2fcb10,0x341560};unsigned char before[6][16];for(unsigned i=0;i<6;++i)memcpy(before[i],mapped+hook_rvas[i],16);
 require(MH_Initialize()==MH_OK,"Fixture hook initialize");create_siege_hooks97();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Fixture hook enable");
 state->heartbeat=GetTickCount64();field<float>(a,0x20)=.2f;state->input=1;
 require(reinterpret_cast<ViewIndex97>(base+0x2d6210)(reinterpret_cast<void*>(v))==17,"Actual native view trampoline scoped index");
 require(reinterpret_cast<ViewActor97>(base+0x340d30)(reinterpret_cast<void*>(v),17)==a,"Actual native view trampoline actor ABI");
 for(unsigned input:{0u,1u,2u,3u})for(unsigned i=0;i<1000;++i){
   state->input=input;float position[4]{};point(reinterpret_cast<void*>(def),position,.2f);for(unsigned k=0;k<3;++k){field<float>(a,0x48+k*4)=position[k];field<float>(a,0xd8+k*4)=0;}
   ++field<unsigned>(ptr(common),0x50);reinterpret_cast<PipeAdvance97>(base+0x3945c0)(reinterpret_cast<void*>(a+0x14),pipe,.01f);
   require(std::abs(field<float>(a,0x20)-(.2f+(input==1?.6f/60/length:input==2?-1.f/60/length:0)))<.00001f,"Actual traversal trampoline RET8");
 }
 require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Fixture hook disable");for(unsigned i=0;i<6;++i)require(!memcmp(before[i],mapped+hook_rvas[i],16),"All six siege hook bytes restored");require(MH_Uninitialize()==MH_OK,"Fixture hook teardown");
 if(climb_shared97)UnmapViewOfFile(climb_shared97);if(climb_mapping97)CloseHandle(climb_mapping97);climb_shared97=nullptr;climb_mapping97=nullptr;
 VirtualFree(mapped,0,MEM_RELEASE);mapped=nullptr;std::cout<<"PASS: siege98 production immediate F9 adoption, eligible logical-index choice, strict wall/crest lease, original native geometry/stride/projection, "<<cases<<" W/S/hold endpoint cases, four native crests, 10000 mode calls, 4000 actual hooked traversal ABI calls, "<<rate_cases98<<" native parameter/segment/dt cases with "<<(rate_cases98*10)<<" duplicate callbacks, six exact hook restorations, stale/resource/phase/death guards. Synthetic world; no game initialization.\n";return 0;
}catch(const std::exception& e){if(mapped)VirtualFree(mapped,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
