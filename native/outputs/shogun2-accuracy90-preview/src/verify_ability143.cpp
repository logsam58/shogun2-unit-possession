// Full production crosshair target -> original unit-reference writers ->
// original2ab840 targeted-order consumer. Primitive context IO and final
// native ability scheduler are explicit providers; no native effect claim.
#define main retained_ability143_main
#include "verify_native_commands100.cpp"
#undef main
static uintptr_t ally143;static unsigned scheduled143;static std::vector<unsigned> unit_ids143;
static bool __fastcall eligible143(void*,void*,unsigned id){return id==35;}
static void* __fastcall unit_bytes143(void* context,void*,unsigned tag,const void* data,unsigned bytes){
 require(tag==0x27&&bytes==4,"Native unit-reference wire tag changed");unit_ids143.push_back(*static_cast<const unsigned*>(data));append100(static_cast<Writer100*>(context),data,bytes);return context;
}
struct Reader143 {unsigned pad{},error{};const unsigned char* bytes{};unsigned at{};};
static void* __cdecl read_unit143(void* context,uintptr_t* out){auto r=static_cast<Reader143*>(context);unsigned id{};memcpy(&id,r->bytes+r->at,4);r->at+=4;*out=id==7?unit:id==8?ally143:0;return context;}
static void* __cdecl read_id143(void* context,unsigned* out){auto r=static_cast<Reader143*>(context);memcpy(out,r->bytes+r->at,4);r->at+=4;return context;}
static void* __cdecl read_byte143(void* context,unsigned char* out){auto r=static_cast<Reader143*>(context);*out=r->bytes[r->at++];return context;}
static void __fastcall read_point143(void* point,void*,void* context){auto r=static_cast<Reader143*>(context);memcpy(point,r->bytes+r->at,12);r->at+=16;}
static void __fastcall schedule143(void* source,void*,unsigned queued,const unsigned* order,unsigned append){
 require(reinterpret_cast<uintptr_t>(source)==unit&&queued==0&&append==0&&order[0]==35&&order[1]==ally143,"Original Inspire consumer lost source/target/mode");++scheduled143;
}
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;state->heartbeat=GetTickCount64();
 auto patch=[&](unsigned rva,void* fn){DWORD old{};VirtualProtect(reinterpret_cast<void*>(base+rva),16,PAGE_EXECUTE_READWRITE,&old);f.jump(rva,fn);VirtualProtect(reinterpret_cast<void*>(base+rva),16,old,&old);};
 patch(0x15be80,reinterpret_cast<void*>(&eligible143));patch(0x3428c0,reinterpret_cast<void*>(&unit_bytes143));
 patch(0x285730,reinterpret_cast<void*>(&read_unit143));patch(0x284b80,reinterpret_cast<void*>(&read_id143));patch(0x285930,reinterpret_cast<void*>(&read_byte143));patch(0x359110,reinterpret_cast<void*>(&read_point143));patch(0x1872a0,reinterpret_cast<void*>(&schedule143));
 unsigned char descriptors[7*28]{};memcpy(descriptors,f.descriptors,sizeof f.descriptors);unsigned key=0x1430035;memcpy(descriptors+6*28,&key,4);descriptors[6*28+16]=3;
 field<unsigned>(base,0x192da98)=key;field<uintptr_t>(base,0x18d9000)=reinterpret_cast<uintptr_t>(descriptors);field<unsigned>(base,0x18d9004)=7;
 std::vector<unsigned char> ally(0x1800),soldier(0x700),side(0x20),other_side(0x20);ally143=P::ptr(ally);auto a=P::ptr(soldier);
 field<unsigned>(ally143,4)=8;field<uintptr_t>(ally143,0x14)=environment;field<uintptr_t>(ally143,0)=field<uintptr_t>(unit,0);
 field<uintptr_t>(a,0)=base+0x15dfec8;field<uintptr_t>(a,0x214)=ally143;field<uintptr_t>(a,0x28)=environment;field<unsigned char>(a,0xf0)=1;
 std::array<uintptr_t,1> members{a};field<unsigned>(ally143,0x384)=1;field<uintptr_t>(ally143,0x388)=reinterpret_cast<uintptr_t>(members.data());
 field<unsigned>(P::ptr(side),0x10)=field<unsigned>(P::ptr(other_side),0x10)=3;field<uintptr_t>(possessed,0x44)=P::ptr(side);field<uintptr_t>(a,0x44)=P::ptr(other_side);
 require(ability_friendly143(ally143)&&!ability_friendly143(unit),"Ally-team/self proof failed");
 HWND window=CreateWindowExW(0,L"STATIC",L"Ability143",WS_POPUP,0,0,1280,720,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);require(window!=nullptr,"Hidden fixture window");input_window=window;native_window_proc=&stock_window100;
 uintptr_t target[4]={ally143};field<uintptr_t>(reinterpret_cast<uintptr_t>(native_cursor_hit100),0x20)=reinterpret_cast<uintptr_t>(target);field<uintptr_t>(P::ptr(f.root),0x130)=reinterpret_cast<uintptr_t>(native_cursor_hit100);
 for(unsigned i=0;i<1000;++i){
  f.clear_queue();unit_ids143.clear();require(ability_aim_friend143()==ally143&&!ability_target_query143,"Production crosshair target/cleanup failed");
  require(ability_inspire143(ally143)&&unit_ids143==std::vector<unsigned>({7,8}),"Original native source/target references not serialized");
  Reader143 reader{};reader.bytes=f.queue.data()+4;reader.at=7;reinterpret_cast<void(__cdecl*)(void*)>(base+0x2ab840)(&reader);
  require(scheduled143==i+1,"Original targeted-ability dispatch failed");
 }
 f.clear_queue();field<unsigned>(P::ptr(other_side),0x10)=4;require(!ability_inspire143(ally143)&&packets100.empty(),"Enemy accepted as friendly target");field<unsigned>(P::ptr(other_side),0x10)=3;
 field<uintptr_t>(a,0x214)=unit;require(!ability_friendly143(ally143),"Foreign actor backlink accepted");field<uintptr_t>(a,0x214)=ally143;
 f.clear_queue();field<unsigned>(P::ptr(f.queue)+4,QUEUE_CAPACITY)=QUEUE_CAPACITY-1;require(!ability_inspire143(ally143)&&field<unsigned>(P::ptr(f.queue)+4,QUEUE_CAPACITY)==QUEUE_CAPACITY-1,"Full queue partially published targeted ability");
 input_window=nullptr;native_window_proc=nullptr;DestroyWindow(window);
 std::cout<<"PASS:1000 production ally crosshair queries, original unit-reference writers and original targeted ability consumer; same/allied-team proof, source/target fields, scope cleanup, enemy/self/stale roster/full-queue rejection. Final effects remain native scheduler authority.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
