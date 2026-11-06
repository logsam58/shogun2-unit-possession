// Actual native UI Resize/MoveTo/Position and OpenPanels predicate in owned
// mapped stock/Bugfix images. Font renderer and speed-command consumer are
// explicit providers; never call game entry, simulation or Steam.
#define wmain old_native_ui214
#include "verify_native_ui185.cpp"
#undef wmain
#include "../../native/deck/ui214.h"
struct Node191 {unsigned char bytes[0x328]{};};
static void __fastcall event191(void*,void*){}
static void __fastcall position_event191(Node191* self,void*,const float* p){memcpy(self->bytes+0xa4,p,8);}
static void set191(Node191& n,unsigned at,uintptr_t v){memcpy(n.bytes+at,&v,4);}
static void xy191(Node191& n,float x,float y){float p[2]{x,y};memcpy(n.bytes+0xa4,p,8);}
struct State214 {uintptr_t table{},owner{};unsigned char rest[0x118]{};};
static unsigned refreshes214{},speed_calls214{};static float speed214=1.f;
static bool __fastcall state214(State214* s,void*,float w,float h,bool images){assert(images);memcpy(reinterpret_cast<char*>(s)+0x28,&w,4);memcpy(reinterpret_cast<char*>(s)+0x2c,&h,4);return true;}
static void __fastcall font214(void* s,void*){auto p=reinterpret_cast<unsigned char*>(s);assert(*reinterpret_cast<int*>(p+0xd0)==*reinterpret_cast<int*>(p+0xe4));++refreshes214;}
static float __fastcall get_speed214(void*,void*){return speed214;}
static void __fastcall set_speed214(void*,void*,float speed,unsigned char flag){assert(!flag);speed214=speed;++speed_calls214;}
int wmain(int argc,wchar_t** argv){assert(argc==3);unsigned cases{};
 for(int index=1;index<argc;++index){
  auto image=LoadLibraryExW(argv[index],nullptr,DONT_RESOLVE_DLL_REFERENCES);assert(image);auto base=reinterpret_cast<uintptr_t>(image);
  native214::admitted_image=0;assert(native214::admit(base,127));
  // Every new dependency is rejected if occupied/changed before admission.
  for(const auto& span:native214::spans){unsigned char changed=*reinterpret_cast<unsigned char*>(base+span.rva)^1;{Patch185 bad(base+span.rva,&changed,1);native214::admitted_image=0;assert(!native214::admit(base,127));}assert(native214::admit(base,127));++cases;}
  {unsigned char manager[0x60]{};auto manager_ptr=reinterpret_cast<uintptr_t>(manager);Patch185 manager_global(base+0x1bc73e4,&manager_ptr,4);
  auto native_modal=reinterpret_cast<bool(__thiscall*)(void*)>(base+0x1096240);
  for(unsigned a:{0u,1u,32u})for(unsigned b:{0u,1u,32u}){memcpy(manager+0x30,&a,4);memcpy(manager+0x3c,&b,4);assert(native214::modal(base)==native_modal(manager));++cases;}
  memset(manager,0,sizeof manager);
  assert(deck185::install_ui185(base));
  {auto physical=jump185(base+0x13f7b40,reinterpret_cast<void*>(&physical185));Patch185 p(base+0x13f7b40,physical.data(),5);
   uintptr_t mode_ptr=reinterpret_cast<uintptr_t>(&mode185),display_ptr=reinterpret_cast<uintptr_t>(&display_info185);Patch185 mode(base+0x1754574+0x248,&mode_ptr,4),display(base+0x1754574+0x30c,&display_ptr,4);
   uintptr_t renderer[8]{};renderer[0]=base+0x1754574;
   Node191 root,corner,button;State214 root_state,corner_state,button_state;uintptr_t sizes[]{0,reinterpret_cast<uintptr_t>(&state214)};
   Node191* widgets[]{&root,&corner,&button};State214* states[]{&root_state,&corner_state,&button_state};uintptr_t state_ptrs[3]{};
   for(unsigned n=0;n<3;++n){auto w=reinterpret_cast<uintptr_t>(widgets[n]);state_ptrs[n]=reinterpret_cast<uintptr_t>(states[n]);states[n]->table=reinterpret_cast<uintptr_t>(sizes);states[n]->owner=w;set191(*widgets[n],0,base+0x1704cdc);set191(*widgets[n],0xb8,state_ptrs[n]);set191(*widgets[n],0xb0,1);set191(*widgets[n],0xb4,reinterpret_cast<uintptr_t>(&state_ptrs[n]));set191(*widgets[n],0x94,n?reinterpret_cast<uintptr_t>(n==1?&root:&corner):0);widgets[n]->bytes[0xf5]=1;
    *reinterpret_cast<int*>(reinterpret_cast<char*>(states[n])+0xe4)=12;*reinterpret_cast<int*>(reinterpret_cast<char*>(states[n])+0xd0)=12;
   }
   auto rp=reinterpret_cast<uintptr_t>(&root),cp=reinterpret_cast<uintptr_t>(&corner),bp=reinterpret_cast<uintptr_t>(&button);set191(root,8,reinterpret_cast<uintptr_t>(renderer));set191(root,0x9c,1);set191(root,0xa0,reinterpret_cast<uintptr_t>(&cp));set191(corner,0x9c,1);set191(corner,0xa0,reinterpret_cast<uintptr_t>(&bp));
   Patch185 current(base+0x1bbe364,&rp,4);auto event=jump185(base+0xf945f0,reinterpret_cast<void*>(&event191));Patch185 notify(base+0xf945f0,event.data(),5);uintptr_t position=reinterpret_cast<uintptr_t>(&position_event191);Patch185 position_cb(base+0x1704cdc+0xc,&position,4);
   auto refresh=jump185(base+0xf87480,reinterpret_cast<void*>(&font214));Patch185 font_provider(base+0xf87480,refresh.data(),5);
   auto size=[](State214& s,float w,float h){memcpy(reinterpret_cast<char*>(&s)+0x28,&w,4);memcpy(reinterpret_cast<char*>(&s)+0x2c,&h,4);};
   for(bool tooltip:{false,true})for(auto dimensions:{std::pair<unsigned,unsigned>{1280,800},{1280,720}}){
    width185=dimensions.first;height185=dimensions.second;deck185::panel_extent191_set(0,0);auto view=deck185::active_ui185();size(root_state,float(view.width),float(view.height));size(corner_state,tooltip?354.f:184.f,tooltip?311.f:204.f);size(button_state,80,80);xy191(root,0,0);xy191(corner,float(view.width)-400,float(view.height)-400);xy191(button,20,20);
    deck214::owner_root=0;auto before=deck214::Node{};assert(deck214::node(cp,before)&&deck214::original_position(before));assert(deck214::decorate(rp,cp,!tooltip));auto after=deck214::Node{};assert(deck214::node(cp,after)&&deck214::original_position(after));float factor=tooltip?1.5f:1.15f;
    assert(std::fabs(after.width-before.width*factor)<.01f&&std::fabs(after.height-before.height*factor)<.01f);assert(after.x>=8&&after.y>=8&&after.x+after.width<=view.width-7&&after.y+after.height<=view.height-7);++cases;
    auto font_size=*reinterpret_cast<int*>(reinterpret_cast<char*>(&corner_state)+0xe4);assert(font_size==int(std::lround(12*factor)));unsigned refreshes=refreshes214;
    for(unsigned n=0;n<100;++n)assert(deck214::decorate(rp,cp,!tooltip));deck214::Node again;assert(deck214::node(cp,again));assert(again.width==after.width&&again.height==after.height&&refreshes214==refreshes);++cases;
    // Unrelated root/foreign object and unbounded/cyclic children refuse.
    assert(!deck214::decorate(rp,rp,!tooltip));auto parent=cp;set191(corner,0x94,cp);assert(!deck214::decorate(rp,cp,!tooltip));set191(corner,0x94,rp);++cases;
    for(auto* s:states){*reinterpret_cast<int*>(reinterpret_cast<char*>(s)+0xe4)=12;*reinterpret_cast<int*>(reinterpret_cast<char*>(s)+0xd0)=12;}
   }
  }
  {std::vector<unsigned char> root(0xe00),env(0x28400);auto r=reinterpret_cast<uintptr_t>(root.data()),e=reinterpret_cast<uintptr_t>(env.data());memcpy(root.data()+0x110,&e,4);Patch185 global(base+0x1bc8194,&r,4);
   auto get=jump185(base+0x321880,reinterpret_cast<void*>(&get_speed214)),set=jump185(base+0x10e58a0,reinterpret_cast<void*>(&set_speed214));Patch185 getter(base+0x321880,get.data(),5),setter(base+0x10e58a0,set.data(),5);native214::BattleSpeed speed;speed214=1.f;
   assert(speed.apply(base,r,e,false)&&std::fabs(speed214-.4f)<.001);assert(speed.apply(base,r,e,true)&&speed214==0);assert(speed.apply(base,r,e,true)&&std::fabs(speed214-.4f)<.001);assert(speed.apply(base,r,e,false)&&speed214==1);cases+=4;
   unsigned calls=speed_calls214;root[0xd18]=1;assert(!speed.apply(base,r,e,true));root[0xd18]=0;env[0x281e8]=1;assert(!speed.apply(base,r,e,true));env[0x281e8]=0;env[0x28254]=1;assert(!speed.apply(base,r,e,true));env[0x28254]=0;assert(!speed.apply(base,r+4,e,true)&&speed_calls214==calls);cases+=4;
  }
  for(auto target:deck185::native_ui185.targets){assert(MH_DisableHook(target)==MH_OK);assert(MH_RemoveHook(target)==MH_OK);}deck185::native_ui185={};}native214::admitted_image=0;assert(FreeLibrary(image));
 }
 std::cout<<"PASS native214: "<<cases<<" original-image modal/geometry/guard and speed-route cases; font renderer and speed consumer are providers; no gameplay\n";
}
