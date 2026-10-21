// Original native action/ability constructor/serializer, explicit allocator,
// descriptors/writers and eligibility provider inherited from production100.
#define main retained_commands_mount154
#include "verify_native_commands100.cpp"
#undef main
static bool mount_eligible154=true;
static bool __fastcall mount_eligibility154(void*,void*,unsigned id){require(id==0x13,"M asked for unrelated native ability");return mount_eligible154;}
static unsigned descriptor_mount154[5]{};
static void* __cdecl descriptor_mount_lookup154(unsigned key){require(key==0x154154u,"Wrong native ability descriptor");return descriptor_mount154;}
static bool __cdecl keyboard_provider154(){return true;}
int main(){SetUnhandledExceptionFilter(fault100);try{
 NativeFixture100 f;using P=NativeFixture100;state->heartbeat=GetTickCount64();state->window_thread=GetCurrentThreadId();camera_callback_depth=1;DWORD old{};VirtualProtect(f.image,0x1580000,PAGE_EXECUTE_READWRITE,&old);
 f.jump(0x15be80,reinterpret_cast<void*>(mount_eligibility154));f.jump(0x207490,reinterpret_cast<void*>(descriptor_mount_lookup154));
 field<uintptr_t>(P::ptr(f.root),0x5b4)=P::ptr(f.owner);field<uintptr_t>(P::ptr(f.root),0x5b8)=environment;field<unsigned char>(P::ptr(f.root),0x598)=1;
 unsigned char descriptors154[7*28]{};memcpy(descriptors154,f.descriptors,sizeof f.descriptors);field<unsigned>(base,0x192ee34)=0x154154u;
 field<unsigned>(uintptr_t(descriptors154),6*28)=0x154154u;field<uintptr_t>(base,0x18d9000)=uintptr_t(descriptors154);field<unsigned>(base,0x18d9004)=7;
 // Physical horse data is verified separately by real followed()/dismount143.
 // This command test uses the already-proved live foot identity for native68.
 auto request=state->lease_generation;f.clear_queue();mount_request154=request;
 camera_callback_depth=0;mount_prepare154(P::ptr(f.camera));require(mount_request154==LONG(request)&&packets100.empty(),"Outside native camera callback consumed M");camera_callback_depth=1;
 ++state->window_thread;mount_prepare154(P::ptr(f.camera));require(mount_request154==LONG(request)&&packets100.empty(),"Foreign thread issued or consumed M");--state->window_thread;
 mount_prepare154(P::ptr(f.camera)+4);require(mount_request154==LONG(request)&&packets100.empty(),"Unrelated camera issued or consumed M");
 mount_prepare154(P::ptr(f.camera));
 require(packets100.size()==1&&packets100[0].key==0x154154u,"M failed to use original native UI ability queue");
 require(packets100[0].integer_count>=1&&packets100[0].integers[0]==0x13,"M queued another native ability");
 auto calls154=mount_orders154;mount_prepare154(P::ptr(f.camera));require(mount_orders154==calls154&&packets100.size()==1,"M repeated without an edge");
 mount_eligible154=false;mount_request154=request;mount_prepare154(P::ptr(f.camera));require(packets100.size()==1,"Unavailable native mount queued anyway");mount_eligible154=true;
 mount_request154=request+1;mount_prepare154(P::ptr(f.camera));require(packets100.size()==1,"Stale generation mounted a new soldier");
 original_keyboard_poll84=keyboard_provider154;auto current=reinterpret_cast<unsigned char*>(base+0x1b30f58),previous=reinterpret_cast<unsigned char*>(base+0x1b31058);
 memset(current,0,256);memset(previous,0,256);mount_key_held154=false;current[0x32]=0x80;keyboard_poll84();
 require(!current[0x32]&&!previous[0x32]&&mount_request154==LONG(request),"M edge not captured or native M input leaked");mount_request154=0;
 for(unsigned i=0;i<1000;++i){current[0x32]=0x80;keyboard_poll84();require(!mount_request154,"Held M repeated native command");}
 keyboard_poll84();state->mode=IDLE;current[0x32]=0x80;keyboard_poll84();require(current[0x32]==0x80&&!mount_request154,"RTS M was intercepted");state->mode=POSSESSED;
 std::cout<<"PASS: native M ability13 queue, unchanged native availability/foreign generation guards and1000 held edges. Same-soldier horse links/camera and both transition directions verified in dismount143.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
