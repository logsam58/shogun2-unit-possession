// Isolated original commit; allocator/registry providers only. Original selection-view callback retained.
#define wmain older_ui223
#include "verify_native_ui185.cpp"
#undef wmain
#include "../../native/deck/formation218.h"
#include <unordered_set>
static std::unordered_set<void*> allocations;static unsigned registry[5]{};static unsigned key223=0x12345678;
static void* __cdecl allocate223(unsigned bytes,void*){void* p=calloc(1,bytes);assert(p);allocations.insert(p);return p;}
static void __cdecl free223(void* p){if(p){assert(allocations.erase(p)==1);free(p);}}
static void* __cdecl registry223(unsigned key){assert(key==key223||key==key223+1);return registry;}
static void put223(uintptr_t p,uintptr_t v){memcpy(reinterpret_cast<void*>(p),&v,4);}
int wmain(int argc,wchar_t** argv){assert(argc==3);SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);unsigned cases=0;
for(int f=1;f<argc;++f){auto image=LoadLibraryExW(argv[f],nullptr,DONT_RESOLVE_DLL_REFERENCES);assert(image);auto b=reinterpret_cast<uintptr_t>(image);native214::admitted_image=0;assert(native214::admit(b));{
std::vector<unsigned char> root(0xe00),env(0x28400),ui(0x328),widget(0x328),ctrl(0x60),owner(0x200),selected(0x500),queue(0x5010),unit1(0xf00),unit2(0xf00);
auto r=reinterpret_cast<uintptr_t>(root.data()),e=reinterpret_cast<uintptr_t>(env.data()),u=reinterpret_cast<uintptr_t>(ui.data()),w=reinterpret_cast<uintptr_t>(widget.data()),c=reinterpret_cast<uintptr_t>(ctrl.data()),o=reinterpret_cast<uintptr_t>(owner.data()),s=reinterpret_cast<uintptr_t>(selected.data()),q=reinterpret_cast<uintptr_t>(queue.data()),a=reinterpret_cast<uintptr_t>(unit1.data()),z=reinterpret_cast<uintptr_t>(unit2.data());uintptr_t children[]{w},units[]{a,z};
put223(r+0x110,e);put223(r+0x5b8,e);put223(r+0x5b4,o);put223(r+0x10c,b+0x170d304);put223(e+0x2826c,s);put223(s+0x4a8,o);put223(e+0x28048,q);
put223(u,b+0x1704cdc);put223(u+0x9c,1);put223(u+0xa0,reinterpret_cast<uintptr_t>(children));put223(w,b+0x1704cdc);put223(w+0x94,u);put223(w+0x318,c);put223(c,b+0x1709cc8);put223(c+4,w);put223(c+0x40,165);ctrl[0x50]=1;
put223(o+0xb0,b+0x15c4da0);put223(o+0xc0,2);put223(o+0xc4,2);put223(o+0xc8,reinterpret_cast<uintptr_t>(units));put223(a,b+0x15c4da0);put223(z,b+0x15c4da0);put223(a+4,11);put223(z+4,22);
unsigned side_key=key223+1;Patch185 sideGlobal(b+0x192f634,&side_key,4);
Patch185 rootGlobal(b+0x1bc8180,&r,4),uiGlobal(b+0x1bbe364,&u,4),keyGlobal(b+0x192d7c0,&key223,4);
auto ap=jump185(b+0xf0510,reinterpret_cast<void*>(&allocate223)),fp=jump185(b+0xf04d0,reinterpret_cast<void*>(&free223)),rp=jump185(b+0x207490,reinterpret_cast<void*>(&registry223));
Patch185 allocator(b+0xf0510,ap.data(),5),deallocator(b+0xf04d0,fp.data(),5),reg(b+0x207490,rp.data(),5);
formation218::FormationPad pad;auto current=[]{return true;};
for(auto d:std::vector<deck174::Vec2>{{0,1},{0,-1},{1,0},{-1,0}}){for(int i=0;i<30;++i){deck216::FineIntent in;in.began=in.active=true;in.move=d;assert(pad.sample(b,r,e,in,current));root[0x598]=1;in.began=in.active=false;in.released=true;assert(pad.sample(b,r,e,in,current));assert(allocations.empty());put223(q+4+0x5000,0);++cases;}}

// Reproduce the exact live selection counts: one native group, no loose units,
 // and one entry in the fifth UI selection vector. Use the original queue writer.
 std::vector<unsigned char> group(0x500);
 auto g=reinterpret_cast<uintptr_t>(group.data());
 uintptr_t group_list[]{g};
 put223(g,b+0x15c4da0);put223(g+0x7c,31);
 put223(g+0x20,1);put223(g+0x24,reinterpret_cast<uintptr_t>(units));put223(g+0x48,e);
 put223(o+0xb4,1);put223(o+0xb8,1);put223(o+0xbc,reinterpret_cast<uintptr_t>(group_list));
 put223(o+0xc4,0);put223(o+0xe8,1);put223(o+0xec,1);put223(o+0xf0,reinterpret_cast<uintptr_t>(group_list));
 for(auto d:std::vector<deck174::Vec2>{{0,1},{0,-1},{-1,0},{1,0}}) {
  deck216::FineIntent in;in.began=in.active=true;in.move=d;
  assert(pad.sample(b,r,e,in,current));root[0x598]=1;
  in.began=in.active=false;in.released=true;
  assert(pad.sample(b,r,e,in,current)&&allocations.empty());
  put223(q+4+0x5000,0);++cases;
 }
 put223(o+0xb8,0);put223(o+0xc4,2);put223(o+0xec,0);
 for(unsigned used:{0x4fe0u,0x4ff0u,0x5000u,0x5001u}){
 deck216::FineIntent in;in.began=in.active=true;in.move={0,1};assert(pad.sample(b,r,e,in,current));root[0x598]=1;put223(q+4+0x5000,used);
 auto before=queue;in.began=in.active=false;in.released=true;assert(!pad.sample(b,r,e,in,current));assert(queue==before&&allocations.empty()&&*reinterpret_cast<unsigned*>(ctrl.data()+0x40)==165);put223(q+4+0x5000,0);++cases;
}
}assert(FreeLibrary(image));}
printf("PASS original commit223: %u complete release/dispatcher/constructor/vector-copy/serializer/destructor cases; allocator and registry are providers; no gameplay\n",cases);
}
