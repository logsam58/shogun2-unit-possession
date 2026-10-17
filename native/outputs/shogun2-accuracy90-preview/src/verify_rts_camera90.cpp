#include "adapter.cpp"
static unsigned toggles90,refreshes90;
static void __fastcall toggle90(void* object,void*,unsigned active){require(active==0,"Restore only exits native FPS");auto manager=reinterpret_cast<uintptr_t>(object);field<uintptr_t>(manager,0x8e0)=field<uintptr_t>(manager,0x8cc);++toggles90;}
static void __fastcall refresh90(void*,void*){++refreshes90;}
int main(){try{
 base=reinterpret_cast<uintptr_t>(VirtualAlloc(nullptr,0x1d88000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(base!=0,"Fixture image");
 Status status{};state=&status;state->window_thread=GetCurrentThreadId();state->mode=POSSESSED;
 std::array<unsigned char,0x1200> root{},common{},phase{},manager{},overhead{},soldier{};std::vector<unsigned char> env(0x29000);
 auto ptr=[](auto& a){return reinterpret_cast<uintptr_t>(a.data());};
 field<uintptr_t>(base,0x1bc8180)=ptr(root);field<uintptr_t>(ptr(root),0x110)=ptr(env);field<uintptr_t>(ptr(env),8)=ptr(common);field<uintptr_t>(ptr(common),0xb4)=ptr(phase);field<unsigned>(ptr(phase),0x14)=6;
 field<uintptr_t>(ptr(env),0x281fc)=ptr(manager);field<uintptr_t>(ptr(manager),0)=base+0x15d26fc;field<uintptr_t>(ptr(manager),0x8cc)=ptr(overhead);field<uintptr_t>(ptr(manager),0x8e0)=ptr(overhead);field<uintptr_t>(ptr(manager),0x8e4)=ptr(soldier);
 field<uintptr_t>(ptr(overhead),0)=base+0x15d1a74;field<uintptr_t>(ptr(soldier),0)=base+0x15d22e8;
 field<uintptr_t>(base+0x15d1a74,0xf4)=reinterpret_cast<uintptr_t>(&refresh90);field<uintptr_t>(base+0x15d26fc,0xf4)=reinterpret_cast<uintptr_t>(&refresh90);original_toggle=reinterpret_cast<Toggle>(&toggle90);
 rts90::Pose before{{10,60,30},{40,0,50},{0,1,0},.75f};rts90::Pose moved{{100,2,300},{101,2,301},{0,1,0},1.309f};
 write_rts_pose90(ptr(overhead),before);write_rts_pose90(ptr(manager),before);
 for(unsigned i=0;i<500;++i){
  before.eye[0]=float(i);before.target[0]=float(i+30);write_rts_pose90(ptr(overhead),before);write_rts_pose90(ptr(manager),before);
  require(save_rts_view90(ptr(env),ptr(manager)),"Capture latest RTS position/angle/FOV");
  write_rts_pose90(ptr(overhead),moved);write_rts_pose90(ptr(manager),moved);field<uintptr_t>(ptr(manager),0x8e0)=ptr(soldier);field<uint32_t>(ptr(overhead),0x190)=0xfeed1234;
  rts_pending90=1;restore_rts_on_window90();auto result=read_rts_pose90(ptr(overhead)),result_manager=read_rts_pose90(ptr(manager));
  if(memcmp(&result,&before,sizeof before)||memcmp(&result_manager,&before,sizeof before))std::cout<<"cycle="<<i<<" restores="<<rts_restores90<<" rejects="<<rts_rejects90<<" toggles="<<toggles90<<" pending="<<rts_pending90<<" eye="<<result.eye[0]<<","<<result.eye[1]<<","<<result.eye[2]<<" expected="<<before.eye[0]<<","<<before.eye[1]<<","<<before.eye[2]<<" manager="<<result_manager.eye[0]<<","<<result_manager.eye[1]<<","<<result_manager.eye[2]<<"\n";
  require(!memcmp(&result,&before,sizeof before)&&!memcmp(&result_manager,&before,sizeof before),"Exit used the unit view instead of saved RTS pose");
  require(field<uintptr_t>(ptr(manager),0x8e0)==ptr(overhead)&&field<uint32_t>(ptr(overhead),0x190)==0xfeed1234&&!rts_view90.active,"Native ownership and unrelated camera fields preserved");
 }
 // Cached native view/projection matrices must also return to the RTS pose.
 field<unsigned char>(ptr(overhead),0xa4)=1;field<float>(ptr(overhead),0x30)=16.f/9;
 for(unsigned j=0;j<16;++j){field<float>(ptr(overhead),0xa8+j*4)=float(j);field<float>(ptr(overhead),0xe8+j*4)=float(j+1);}
 require(save_rts_view90(ptr(env),ptr(manager)),"Cached camera capture");field<float>(ptr(overhead),0xa8)=999;field<float>(ptr(overhead),0x30)=21.f/9;
 field<uintptr_t>(ptr(manager),0x8e0)=ptr(soldier);rts_pending90=1;restore_rts_on_window90();
 require(field<unsigned char>(ptr(overhead),0xa4)==1&&field<float>(ptr(overhead),0xa8)==0&&std::abs(field<float>(ptr(overhead),0xe8)-16.f/21)<.000001f,"Cached view restored and resized projection aspect corrected");
 require(toggles90==501&&rts_restores90==501,"Repeated possession round trips");
 require(save_rts_view90(ptr(env),ptr(manager)),"Old request snapshot");auto old_generation=uint32_t(rts_generation90);
 require(save_rts_view90(ptr(env),ptr(manager)),"Fresh request snapshot");auto new_generation=uint32_t(rts_generation90);rts_pending90=LONG(new_generation);field<uintptr_t>(ptr(manager),0x8e0)=ptr(soldier);
 restore_rts_on_window90(old_generation);require(rts_view90.active&&uint32_t(rts_pending90)==new_generation&&rts_restores90==501,"Delayed old return message ended a fresh possession");
 restore_rts_on_window90(new_generation);require(rts_restores90==502,"Fresh return generation failed");
 require(save_rts_view90(ptr(env),ptr(manager)),"Final capture");field<uintptr_t>(ptr(common),0xb4)=ptr(phase)+16;field<uintptr_t>(ptr(manager),0x8e0)=ptr(soldier);rts_pending90=1;restore_rts_on_window90();
 require(rts_restores90==502&&rts_rejects90==1&&field<uintptr_t>(ptr(manager),0x8e0)==ptr(soldier),"Stale battle snapshot must not write a replacement camera");
 VirtualFree(reinterpret_cast<void*>(base),0,MEM_RELEASE);std::cout<<"PASS: 502 production RTS camera round trips, cached matrices, resized projection and delayed-message generations restore saved eye/target/up/FOV and native camera ownership; unrelated fields preserved; stale battle rejected.\n";return 0;
 }catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
