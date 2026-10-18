// Execute the original native selected/wavering-selected observer. State-name,
// unit-view lookup and selection notification are explicit fixture providers.
#define main retained_command100_main
#include "verify_native_commands100.cpp"
#undef main
static unsigned notifications104,lookups104;static uintptr_t presentation104;static bool wavering104;
static bool __cdecl state_name104(const void*,const char* name){return !strcmp(name,wavering104?"wavering_selected":"selected");}
static uintptr_t __fastcall lookup104(void*,void*,unsigned unit_id){require(unit_id==7,"Native UI changed unit id");++lookups104;return presentation104;}
static void __fastcall notify104(void*,void*,uintptr_t view,bool selected){require(view==presentation104&&selected,"Native selection notification changed");++notifications104;}
static void patch104(NativeFixture100& f,unsigned rva,void* fn){DWORD old{};require(VirtualProtect(reinterpret_cast<void*>(base+rva),16,PAGE_EXECUTE_READWRITE,&old)!=0,"UI fixture patch");f.jump(rva,fn);VirtualProtect(reinterpret_cast<void*>(base+rva),16,old,&old);FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(base+rva),16);}
int main(){try{
    NativeFixture100 f;using P=NativeFixture100;
    std::vector<unsigned char> widget(0xa0),argument(0x30),battle_ui(0x80),presentation(0x620),hover(0x14);uintptr_t hit=P::ptr(f.peer);
    presentation104=P::ptr(presentation);field<unsigned>(P::ptr(widget),8)=7;field<uintptr_t>(P::ptr(widget),0xc)=P::ptr(f.root);
    field<uintptr_t>(environment,0x28264)=P::ptr(battle_ui);field<uintptr_t>(P::ptr(battle_ui),0x74)=P::ptr(presentation);
    field<uintptr_t>(P::ptr(f.root),0x130)=P::ptr(hover);field<uintptr_t>(P::ptr(hover),0x10)=reinterpret_cast<uintptr_t>(&hit);
    field<unsigned>(P::ptr(f.peer),0x21c)=7;field<uintptr_t>(P::ptr(f.manager),0)=base+0x15d26fc;
    field<uintptr_t>(base+0x15d26fc,0x164)=reinterpret_cast<uintptr_t>(&notify104);
    patch104(f,0xe6680,reinterpret_cast<void*>(&state_name104));patch104(f,0x2f8160,reinterpret_cast<void*>(&lookup104));
    using Observer=void(__thiscall*)(void*,void*,unsigned);auto observe=reinterpret_cast<Observer>(base+0x1221ae0);
    const unsigned hook=0x3e5170;unsigned char original_bytes[16];memcpy(original_bytes,reinterpret_cast<void*>(base+hook),16);
    require(MH_Initialize()==MH_OK,"Selection fixture hooks initialize");create_context_hooks103();require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Selection fixture hooks enable");
    unsigned stack_before{},stack_after{},native_writes=0;
    for(unsigned i=0;i<200;++i){
        wavering104=bool(i&1);field<unsigned char>(P::ptr(widget),0x96)=0;enabled=0;
        observe(widget.data(),argument.data(),0);
        require(field<uintptr_t>(P::ptr(f.manager),0x8fc)==P::ptr(f.peer),"Original selected/wavering observer did not reproduce followed-actor overwrite");++native_writes;
        field<uintptr_t>(P::ptr(f.manager),0x8fc)=possessed;enabled=1;state->heartbeat=GetTickCount64();state->window_thread=GetCurrentThreadId();
        require(followed(possessed,false),"Selection fixture full current identity invalid");field<unsigned char>(P::ptr(widget),0x96)=0;field<unsigned char>(presentation104,0x617)=1;
        __asm mov stack_before,esp
        observe(widget.data(),argument.data(),0);
        __asm mov stack_after,esp
        require(stack_before==stack_after&&field<uintptr_t>(P::ptr(f.manager),0x8fc)==possessed&&followed(possessed,false),"Owned UI observer changed current camera identity/ABI");
        require(field<unsigned char>(presentation104,0x615)==1&&field<unsigned char>(presentation104,0x617)==0&&notifications104==2*(i+1)&&lookups104==4*(i+1),"Guard skipped native UI selection side effects");
        require(reinterpret_cast<HoverIdentity104>(base+0x3e5170)(f.peer.data())==7,"Ordinary native identity accessor was modified");
    }
    for(unsigned denied=0;denied<5;++denied){
        field<unsigned char>(P::ptr(widget),0x96)=0;unsigned serial=field<unsigned>(possessed,0x1c8);
        if(denied==0)state->release=1;if(denied==1)state->stop=1;if(denied==2)enabled=0;
        if(denied==3)field<unsigned>(possessed,0x1c8)=serial+1;if(denied==4)state->window_thread=GetCurrentThreadId()+1;
        observe(widget.data(),argument.data(),0);require(field<uintptr_t>(P::ptr(f.manager),0x8fc)==P::ptr(f.peer),"Invalid/released/foreign-thread UI observer did not retain native write");
        state->release=state->stop=0;enabled=1;state->window_thread=GetCurrentThreadId();field<unsigned>(possessed,0x1c8)=serial;field<uintptr_t>(P::ptr(f.manager),0x8fc)=possessed;
    }
    require(selection_follow_blocks104==200,"UI write guard count differs");
    require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&!memcmp(original_bytes,reinterpret_cast<void*>(base+hook),16),"UI accessor hook did not restore exact native bytes");require(MH_Uninitialize()==MH_OK,"Selection fixture hooks teardown");
    std::cout<<"PASS: selection104 "<<native_writes<<" original selected/wavering followed-actor writes, 200 fully owned pre-write blocks, retained native UI side effects, ordinary accessor identity, five strict forwarding guards and exact restoration. State parser/view/notification providers substituted; no claim of live writer attribution.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
