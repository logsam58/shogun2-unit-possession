#include "../../../common/fixture_paths.h"
#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "../../shogun2-possession-cleanup-v46/vendor/include/MinHook.h"
#include "native_observer.h"
#include <thread>
#include <atomic>
using namespace qte;
static constexpr DWORD TEST_EXCEPTION=0xe0433254;
static NativeSelector native_selector;
struct Mock {
    uint32_t prepares{},updates{},removes{},selects{},flag1{},flag2{};
    bool raise{};NativeVerdict alternate{};void* removed{};
    NativeVerdict* out{};void*const* begin{};void*const* end{};
    __declspec(noinline) NativeVerdict* select(NativeVerdict* output,void*const* a,void*const* b,uint32_t flags,uint32_t flags2){
        ++selects;flag1=flags;flag2=flags2;out=output;begin=a;end=b;
        if(flags==0x80000000)RaiseException(TEST_EXCEPTION,0,0,nullptr);
        *output={flags^flags2,uint32_t(reinterpret_cast<uintptr_t>(a[0])),uint32_t(reinterpret_cast<uintptr_t>(a[1]))};
        alternate=*output;return flags2==1?&alternate:output;
    }
    __declspec(noinline) uint32_t prepare(){
        ++prepares;void* participants[3]={this,&prepares,&updates};NativeVerdict result;
        volatile NativeSelector call=native_selector;
        call(this,&result,participants,participants+3,raise?0x80000000:0x12340002,0x43210007);
        return 0xa5a5007f;
    }
    __declspec(noinline) uint32_t update(){++updates;return 0xbeef006f;}
    __declspec(noinline) void remove(void* witness){++removes;removed=witness;}
};
template<class F,class M>static F function(M method){static_assert(sizeof(F)==sizeof(M),"Simple member pointer required");F f;memcpy(&f,&method,sizeof f);return f;}
static volatile LONG entry[4],exit_count[4],errors,scoped_selects,snapshots;
static bool read(void*,uint32_t address,void* out,size_t size){__try{memcpy(out,reinterpret_cast<void*>(address),size);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}}
static bool snapshot(void*,uint32_t,observation::Event&){InterlockedIncrement(&snapshots);return true;}
static void emit(void*,const observation::Event& e){
    InterlockedIncrement(e.exit?exit_count+e.kind:entry+e.kind);
    if(e.kind==uint32_t(observation::Kind::Select)&&e.witness){
        InterlockedIncrement(&scoped_selects);if(e.arguments[0]!=e.witness||!e.generation)InterlockedIncrement(&errors);
    }
    if(e.exit&&e.completed&&e.kind==uint32_t(observation::Kind::Prepare)&&e.returned!=0xa5a5007f)InterlockedIncrement(&errors);
    if(e.exit&&e.completed&&e.kind==uint32_t(observation::Kind::Update)&&e.returned!=0xbeef006f)InterlockedIncrement(&errors);
}
static bool exception(NativeWitnessCall call,Mock* self){
    self->raise=true;
    __try {call(self);}
    __except(GetExceptionCode()==TEST_EXCEPTION?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){
        self->raise=false;return observation::parent_witness==0&&observation::parent_generation==0;
    }
    return false;
}
int main(){try{
    auto prepare=function<NativeWitnessCall>(&Mock::prepare),update=function<NativeWitnessCall>(&Mock::update);
    auto remove=function<NativeWitnessRemove>(&Mock::remove);native_selector=function<NativeSelector>(&Mock::select);
    void* targets[4]={reinterpret_cast<void*>(prepare),reinterpret_cast<void*>(update),reinterpret_cast<void*>(remove),reinterpret_cast<void*>(native_selector)};
    void* hooks[4]={reinterpret_cast<void*>(&observation::prepare_hook),reinterpret_cast<void*>(&observation::update_hook),reinterpret_cast<void*>(&observation::remove_hook),reinterpret_cast<void*>(&observation::select_hook)};
    auto& ports=observation::ports;void** originals[4]={reinterpret_cast<void**>(&ports.prepare),reinterpret_cast<void**>(&ports.update),reinterpret_cast<void**>(&ports.remove),reinterpret_cast<void**>(&ports.select)};
    ports.emit=emit;ports.read=read;ports.snapshot=snapshot;unsigned char bytes[4][16];
    require(MH_Initialize()==MH_OK,"MinHook initialization failed");
    for(unsigned i=0;i<4;++i){memcpy(bytes[i],targets[i],16);require(MH_CreateHook(targets[i],hooks[i],originals[i])==MH_OK,"Create fixture hook failed");}
    require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Enable fixture hooks failed");
    Mock self;volatile NativeWitnessCall prepare_call=prepare,update_call=update;volatile NativeWitnessRemove remove_call=remove;volatile NativeSelector select_call=native_selector;
    uintptr_t before{},after{};__asm mov before,esp
    for(unsigned i=0;i<1000;++i){require(prepare_call(&self)==0xa5a5007f&&update_call(&self)==0xbeef006f,"Full native EAX not preserved");remove_call(&self,&self);}
    __asm mov after,esp
    require(before==after&&self.prepares==1000&&self.updates==1000&&self.removes==1000&&self.selects==1000&&self.removed==&self,"Witness forwarding/RET4 balance failed");
    void* actors[4]={&self,&before,&after,&ports};NativeVerdict output;
    __asm mov before,esp
    for(unsigned i=0;i<1000;++i){auto* returned=select_call(&self,&output,actors,actors+4,0x34560003,1);
        require(returned==&self.alternate&&output.outcome==(0x34560003^1)&&self.begin==actors&&self.end==actors+4&&self.out==&output&&self.flag1==0x34560003&&self.flag2==1,"RET20 argument/returned pointer changed");}
    __asm mov after,esp
    require(before==after,"Selector stack imbalance");
    require(exception(prepare_call,&self),"Native exception masked or TLS leaked");
    require(!observation::parent_witness&&!observation::parent_generation,"TLS scope remained active");
    uint32_t address=uint32_t(reinterpret_cast<uintptr_t>(&self));auto old=observation::registry.get(address);auto newer=observation::registry.get(address,true);
    observation::registry.removed(address,old);require(newer>old&&observation::registry.get(address)==newer,"Stale removal erased recycled witness");
    std::atomic<bool> mismatch{false};std::vector<std::thread> threads;
    for(unsigned thread=0;thread<8;++thread)threads.emplace_back([&,thread]{Mock local;
        for(unsigned i=0;i<1000;++i){if(observation::registry.get(address)!=newer)mismatch=true;
            if(prepare_call(&local)!=0xa5a5007f||update_call(&local)!=0xbeef006f)mismatch=true;
            if(observation::parent_witness||observation::parent_generation)mismatch=true;}
        remove_call(&local,&local);});
    for(auto& thread:threads)thread.join();require(!mismatch&&!errors,"Worker migration/concurrent TLS corruption");
    require(entry[0]==9001&&exit_count[0]==entry[0]&&entry[1]==9000&&exit_count[1]==entry[1]&&entry[2]==1008&&exit_count[2]==entry[2]&&entry[3]==10001&&exit_count[3]==entry[3]&&scoped_selects==18002,"Exactly-once hook observation failed");
    // Every removal takes exactly one snapshot, before the native free path.
    require(snapshots==(entry[0]+entry[1]+entry[3]-1000)*2+entry[2],"Freed witness read after removal");
    require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Disable fixture hooks failed");
    for(unsigned i=0;i<4;++i)require(!memcmp(bytes[i],targets[i],16),"Fixture original bytes not restored");
    require(prepare_call(&self)==0xa5a5007f&&self.prepares==1002,"Unhooked native call failed");
    require(!observation::active_callbacks,"Exception/concurrency leaked active callback count");
    require(MH_Uninitialize()==MH_OK,"Fixture uninitialize failed");
    // Decode/patch COPIES of the original entry bytes, never execute engine
    // functions. AS_IMAGE_RESOURCE maps PE data without imports or DllMain.
    const wchar_t* engine_path=fixture_paths::engine();
    require(hash_file(engine_path)==EXPECTED_HASH,"Original engine differs");
    HMODULE data=LoadLibraryExW(engine_path,nullptr,LOAD_LIBRARY_AS_IMAGE_RESOURCE|LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE);
    require(data!=nullptr,"Original PE data mapping failed");
    uintptr_t image=reinterpret_cast<uintptr_t>(data)&~uintptr_t(3);
    const uint32_t rvas[4]={0x3e5d40,0x3eaac0,0x3cea10,0x433b80};void* copies[4]{};
    require(MH_Initialize()==MH_OK,"Original byte fixture initialize failed");
    for(unsigned i=0;i<4;++i){copies[i]=VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);require(copies[i]!=nullptr,"Entry copy allocation failed");
        memcpy(copies[i],reinterpret_cast<void*>(image+rvas[i]),96);void* trampoline{};
        require(MH_CreateHook(copies[i],hooks[i],&trampoline)==MH_OK&&trampoline,"Original entry decoder rejected");}
    require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK&&MH_DisableHook(MH_ALL_HOOKS)==MH_OK,"Copied entry patch/restore failed");
    for(unsigned i=0;i<4;++i)require(!memcmp(copies[i],reinterpret_cast<void*>(image+rvas[i]),96),"Original entry copy changed after restore");
    require(MH_Uninitialize()==MH_OK,"Original entry fixture cleanup failed");for(void* copy:copies)VirtualFree(copy,0,MEM_RELEASE);FreeLibrary(data);
    std::cout<<"PASS: production observer hooks execute against x86 fixture methods; 9,001 prepares, 9,000 updates, 1,008 removals, 10,001 selectors; full EAX/RET4/RET20 and alternate pointer preserved; eight workers, TLS exception unwind, witness reuse, no read after removal, hook restoration. No game attached.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
