// Replays saved Test153 stage21: routed living actor, valid camera/roster,
// native selection count0/capacity32. World/death rendering remains external.
#define main retained_commands_routing154
#include "verify_native_commands100.cpp"
#undef main
static bool routed154;
static bool __fastcall routing_provider154(void*,void*){return routed154;}
int main(){SetUnhandledExceptionFilter(fault100);try{
 NativeFixture100 f;using P=NativeFixture100;DWORD old{};VirtualProtect(f.image,0x1580000,PAGE_EXECUTE_READWRITE,&old);f.jump(0x1821e0,reinterpret_cast<void*>(routing_provider154));
 auto a=possessed,g=fixture_group100;auto generation=possession_lease.generation;
 field<unsigned>(g,0x10)=32;field<unsigned>(g,0x14)=0;
 require(!followed(a,false),"Empty unrelated selection retained normal possession");
 for(unsigned broken:{2u,3u}){
  field<unsigned>(unit,0xcec)=broken;field<unsigned>(unit,0xce8)=6;routed154=true;
  for(unsigned i=0;i<1000;++i){state->heartbeat=GetTickCount64();require(followed(a,false),"Saved native routing deselection still ejected living soldier");}
  require(possession_lease.generation==generation&&state->mode==POSSESSED,"Routing changed soldier or lease");
  require(!followed(a,true),"Routing empty selection granted a new acquisition");
  routed154=false;require(followed(a,false),"Rally with same empty native selection ejected retained soldier");
 }
 field<unsigned>(g,0x14)=1;f.units[0]=unit+4;require(!followed(a,false),"Foreign nonempty selection bypassed ownership");f.units[0]=unit;
 ++field<unsigned>(a,0x1c8);field<unsigned>(g,0x14)=0;routed154=true;require(!followed(a,false),"Reused soldier identity bypassed routing guard");--field<unsigned>(a,0x1c8);
 field<unsigned>(g,0x14)=1;require(followed(a,false),"Current native selection cannot recover");
 std::cout<<"PASS: saved count0/capacity32 broken/shattered release replay, 2000 living continuations, rally retention and strict acquisition/foreign selection/serial rejection. Death/paired outcome pipeline unchanged.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
