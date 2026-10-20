// Execute the unchanged native animation-linked bow audio dispatcher and
// original ability sound selector against explicit device/primitive providers.
#define main retained_audio143_main
#include "verify_native_commands100.cpp"
#undef main
static std::vector<unsigned> cues143,translated145;
static void* __fastcall audio_device143(void*,void*,unsigned* result,unsigned cue,const float*){cues143.push_back(cue);result[0]=1;result[1]=2;return result;}
static unsigned __fastcall translate145(void*,void*,unsigned cue){translated145.push_back(cue);return cue+0x10000;}
static uintptr_t __fastcall audio_unit145(void*,void*){return unit;}
static unsigned __fastcall audio_voice145(void*,void*,uintptr_t){return 0;}
static const void* __fastcall audio_position145(void*,void*,void*,unsigned,uintptr_t holder){return reinterpret_cast<void*>(field<uintptr_t>(holder,8)+0x48);}
int main(){SetUnhandledExceptionFilter(&fault100);try{
 NativeFixture100 f;using P=NativeFixture100;
 std::vector<unsigned char> bus(0x1200),controller(0x2000),device(0x40),record(0x100);std::array<uintptr_t,3> n1{},n2{},end1{},end2{};
 auto b=P::ptr(bus),c=P::ptr(controller),d=P::ptr(device);field<uintptr_t>(P::ptr(f.common),0x98)=b;field<uintptr_t>(unit,0x2c)=P::ptr(record);
 auto device_vt=base+0x17f0000;field<uintptr_t>(d,0)=device_vt;field<uintptr_t>(device_vt,0x14)=field<uintptr_t>(device_vt,0x18)=reinterpret_cast<uintptr_t>(&audio_device143);

 field<uintptr_t>(c,0)=base+0x15d4bbc;field<uintptr_t>(c,0x1d4)=environment;field<uintptr_t>(c,0x1d8)=d;
 field<uintptr_t>(c,0xfc)=base+0x15d4c28;field<uintptr_t>(c,0x118)=base+0x15d4c34;
 n1={0,reinterpret_cast<uintptr_t>(end1.data()),c+0xfc};n2={0,reinterpret_cast<uintptr_t>(end2.data()),c+0x118};
 field<uintptr_t>(b,0xef8)=reinterpret_cast<uintptr_t>(n1.data());field<uintptr_t>(b,0xefc)=reinterpret_cast<uintptr_t>(end1.data());
 field<uintptr_t>(b,0xf18)=reinterpret_cast<uintptr_t>(n2.data());field<uintptr_t>(b,0xf1c)=reinterpret_cast<uintptr_t>(end2.data());
 require(native_audio_subscriber143(0xef4,0x15d4c28,0xfc)==c+0xfc&&native_audio_subscriber143(0xf14,0x15d4c34,0x118)==c+0x118,"Original current audio subscription ownership failed");
 for(const auto& ability:ability141::catalog){
  field<unsigned char>(c+0x118,0x1be)=0;unsigned event[3]={field<unsigned>(c+0x118,0x174),ability.id,unsigned(unit)};cues143.clear();
  reinterpret_cast<NativeAudio143>(base+0x2fb480)(reinterpret_cast<void*>(c+0x118),event);require(cues143.size()==1,"Original ability selector refused native ID");auto expected=cues143;
  field<unsigned char>(c+0x118,0x1be)=0;cues143.clear();ability_sound143(ability.id);require(cues143==expected,"Ability sound differs from original selector");
 }

 std::array<unsigned char,32> original_dispatch145{},original_bus145{};
 memcpy(original_dispatch145.data(),reinterpret_cast<void*>(base+0x292620),32);memcpy(original_bus145.data(),reinterpret_cast<void*>(base+0x2fb2a0),32);
 std::array<uintptr_t,16> control145{},enum_outer145{},enum_inner145{};
 field<uintptr_t>(environment,0x2804c)=reinterpret_cast<uintptr_t>(control145.data());
 control145[5]=reinterpret_cast<uintptr_t>(enum_outer145.data());enum_outer145[0]=reinterpret_cast<uintptr_t>(enum_inner145.data());enum_inner145[2]=d;
 field<uintptr_t>(device_vt,0xd8)=reinterpret_cast<uintptr_t>(&translate145);
 field<unsigned char>(c,0x24d)=field<unsigned char>(c,0x24e)=1;field<uintptr_t>(c,0x23c)=0;field<unsigned>(P::ptr(f.config),0x18)=0;
 auto provider145=[&](unsigned address,void* code){DWORD old{};require(VirtualProtect(reinterpret_cast<void*>(base+address),8,PAGE_EXECUTE_READWRITE,&old),"Fixture provider protection");f.jump(address,code);FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(base+address),8);DWORD ignored{};require(VirtualProtect(reinterpret_cast<void*>(base+address),8,old,&ignored),"Fixture provider restore protection");};
 provider145(0x3ea960,reinterpret_cast<void*>(&audio_unit145));provider145(0x2dc800,reinterpret_cast<void*>(&audio_voice145));provider145(0x2dba00,reinterpret_cast<void*>(&audio_position145));
 using Dispatch145=void(__thiscall*)(void*,unsigned,uintptr_t);auto dispatch145=reinterpret_cast<Dispatch145>(base+0x292620);
 state->aiming=1;bow_reset142();bow_family142=1;bow_sample142(false,true);cues143.clear();translated145.clear();
 for(auto holder:{P::ptr(f.holder),P::ptr(f.peer_holder)}){dispatch145(reinterpret_cast<void*>(c),0x20,holder);dispatch145(reinterpret_cast<void*>(c),0x21,holder);}
 require(translated145==std::vector<unsigned>({0xd3,0xd4,0xd3,0xd4})&&cues143==std::vector<unsigned>({0x100d3,0x100d4,0x100d3,0x100d4}),"Owned/peer authored bow audio differs from original native emitter");
 auto count145=cues143.size();InterlockedExchange(&state->fire_request,0);bow_sample142(true,true);
 for(unsigned i=0;i<1000;++i)bow_sample142(true,true);
 require(!state->fire_request&&cues143.size()==count145,"LMB hold fired early or emitted manual audio");bow_sample142(false,true);
 require(state->fire_request&&bow_release142.serial&&cues143.size()==count145,"Bow release strength lost or manual sound emitted");
 bow_sample142(true,true);bow_sample142(true,false);require(cues143.size()==count145,"Cancellation emitted manual sound");
 require(!memcmp(original_dispatch145.data(),reinterpret_cast<void*>(base+0x292620),32)&&!memcmp(original_bus145.data(),reinterpret_cast<void*>(base+0x2fb2a0),32),"Native bow audio was detoured");
 std::cout<<"PASS:42 native ability selectors; original292620/cd56e0 bow draw/release audio for possessed and peer actors unchanged;1000 LMB hold samples, release strength and cancellation emit no manual audio. Native device/unit/position/voice primitives are explicit providers. No game launched.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
