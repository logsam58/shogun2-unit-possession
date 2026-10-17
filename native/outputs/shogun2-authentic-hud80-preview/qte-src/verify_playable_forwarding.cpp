// Invoke the actual production ABI wrappers with fixture engine ports.
// No process attachment, native-memory write, or live hook installation.
#include "playable_runtime.cpp"
struct Fixture {uint32_t calls[4]{};NativeVerdict alternate{};bool alternate_return{};uint32_t errors{};};
static uint32_t __fastcall fake_prepare(void* self,void*){auto& f=*static_cast<Fixture*>(self);++f.calls[0];return 0xdead0000;}
static uint32_t __fastcall fake_update(void* self,void*){auto& f=*static_cast<Fixture*>(self);++f.calls[1];return 0xbeef0001;}
static void __fastcall fake_remove(void* self,void*,void* witness){auto& f=*static_cast<Fixture*>(self);++f.calls[2];if(witness!=self)++f.errors;}
static NativeVerdict* __fastcall fake_select(void* self,void*,NativeVerdict* out,void*const* begin,
    void*const* end,uint32_t flags,uint32_t flags2){auto& f=*static_cast<Fixture*>(self);++f.calls[3];
    if(end-begin!=2||flags!=0x12340001||flags2!=0x56780002||begin[0]!=self)++f.errors;
    auto result=f.alternate_return?&f.alternate:out;*result={5,uint32_t(self),uint32_t(begin[1])};return result;}
static DWORD WINAPI exercise(void* p){auto& f=*static_cast<Fixture*>(p);void* participants[2]={p,&f.alternate};
    uintptr_t before{},after{};__asm mov before,esp
    for(unsigned i=0;i<2000;++i){f.alternate_return=bool(i&1);NativeVerdict output;
        if(prepare_hook(p,nullptr)!=0xdead0000||update_hook(p,nullptr)!=0xbeef0001)++f.errors;
        auto result=select_hook(p,nullptr,&output,participants,participants+2,0x12340001,0x56780002);
        if(result!=(f.alternate_return?&f.alternate:&output)||result->attacker_address!=uint32_t(p)||result->defender_address!=uint32_t(&f.alternate))++f.errors;
        remove_hook(p,nullptr,p);
    }
    __asm mov after,esp
    if(before!=after)++f.errors;return f.errors;
}
int main(){try{Status telemetry{};state=&telemetry;engine=0;session=nullptr;pending_address=0;
    native_prepare=reinterpret_cast<NativeWitnessCall>(fake_prepare);native_update=reinterpret_cast<NativeWitnessCall>(fake_update);
    native_remove=reinterpret_cast<NativeWitnessRemove>(fake_remove);selection_ports.select=reinterpret_cast<NativeSelector>(fake_select);
    // Exercise the disabled path, then four foreign encounters while another
    // coordinator owns the session. Nothing may dereference the null Session.
    enabled=0;coordinator=0;Fixture first;exercise(&first);require(!first.errors,"Disabled ABI differs");
    enabled=1;coordinator=1;Fixture fixtures[4];HANDLE threads[4];
    for(unsigned i=0;i<4;++i){threads[i]=CreateThread(nullptr,0,exercise,fixtures+i,0,nullptr);require(threads[i]!=nullptr,"Fixture worker failed");}
    require(WaitForMultipleObjects(4,threads,TRUE,10000)==WAIT_OBJECT_0,"Fixture callbacks did not complete");
    for(unsigned i=0;i<4;++i){CloseHandle(threads[i]);require(!fixtures[i].errors,"Foreign combat ABI differs");
        for(auto count:fixtures[i].calls)require(count==2000,"Original callback duplicated or skipped");}
    require(active_callbacks==0&&pending_address==0&&coordinator==1,"Foreign callbacks changed owned state or did not drain");
    for(auto count:telemetry.calls)require(count==10000,"Hook telemetry counts differ");
    std::cout<<"PASS: 40000 calls through playable wrappers; disabled plus four foreign workers; full EAX/ECX/RET4/RET20 and stack preservation, alternate output pointer, exactly-once forwarding, callback drain and owned-state isolation. Game untouched.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
