// Offline contract test, not a naval-possession implementation.
// Executes unchanged native task/navigation routines from a hash-pinned, privately mapped
// owned engine. No DllMain/import resolution, attachment, hooks in the game,
// background work, network access, ship AI, pathfinding or real combat occurs.
// Dependencies outside the dispatch contract are explicit fixture providers.
// Navigation captures the native request at submission; it does not execute pathfinding.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <bcrypt.h>
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#pragma comment(lib,"bcrypt.lib")
static_assert(sizeof(void*)==4,"Build this fixture for x86");
using Bytes=std::vector<unsigned char>;
static void require(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
template<class T> static T& at(void* p,size_t offset){return *reinterpret_cast<T*>(static_cast<unsigned char*>(p)+offset);}
static std::string sha(const unsigned char* p,size_t n){
    BCRYPT_ALG_HANDLE alg{};BCRYPT_HASH_HANDLE hash{};DWORD size{},used{};
    require(BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)==0,"SHA provider");
    require(BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&size),sizeof size,&used,0)==0,"SHA object");
    Bytes object(size);unsigned char digest[32]{};
    require(BCryptCreateHash(alg,&hash,object.data(),size,nullptr,0,0)==0,"SHA create");
    require(n<=0xffffffffu&&BCryptHashData(hash,const_cast<PUCHAR>(p),static_cast<ULONG>(n),0)==0,"SHA data");
    require(BCryptFinishHash(hash,digest,32,0)==0,"SHA finish");
    BCryptDestroyHash(hash);BCryptCloseAlgorithmProvider(alg,0);
    static const char digits[]="0123456789abcdef";std::string text;
    for(auto b:digest){text+=digits[b>>4];text+=digits[b&15];}return text;
}
struct TaskStats {unsigned starts{},ticks{},destroys{},destroy_flag{};bool finish_on_start{},finish_on_tick{};};
static TaskStats& stats(void* task){return *at<TaskStats*>(task,0x30);}
static void __fastcall start(void* task,void*){auto& s=stats(task);++s.starts;if(s.finish_on_start)at<unsigned char>(task,0x10)=1;}
static void __fastcall tick(void* task,void*){auto& s=stats(task);++s.ticks;if(s.finish_on_tick)at<unsigned char>(task,0x10)=1;}
static void __fastcall destroy(void* task,void*,unsigned flag){auto& s=stats(task);++s.destroys;s.destroy_flag=flag;}
static void* task_table[]={reinterpret_cast<void*>(&destroy),reinterpret_cast<void*>(&start),reinterpret_cast<void*>(&tick)};
struct FrameStats {unsigned lower_starts{},lower_ticks{},before{},motion{},after{},tail{},reaction{},clear_lower{},fallback{};};
// Fixture-private pointer in unused tail of its oversized synthetic actor.
static FrameStats& frame(void* actor){return *at<FrameStats*>(actor,0x700);}
static void __fastcall lower_start(void* task,void*){++frame(static_cast<unsigned char*>(task)-0x250).lower_starts;}
static void __fastcall lower_tick(void* task,void*){++frame(static_cast<unsigned char*>(task)-0x250).lower_ticks;}
static void* lower_table[]={nullptr,reinterpret_cast<void*>(&lower_start),reinterpret_cast<void*>(&lower_tick)};
static void __fastcall before(void* actor,void*){++frame(actor).before;}
static void __fastcall motion(void* actor,void*){++frame(actor).motion;}
static void __fastcall after(void* actor,void*){++frame(actor).after;}
static void __fastcall tail(void* actor,void*){++frame(actor).tail;}
static void __fastcall reaction(void* actor,void*){++frame(actor).reaction;at<unsigned>(actor,0x248)=6;}
static void __fastcall clear_lower(void* context,void*){auto actor=static_cast<unsigned char*>(context)-0x24c;++frame(actor).clear_lower;at<unsigned char>(actor,0x2e8)=0;}
static void __fastcall fallback(void* actor,void*,void*){++frame(actor).fallback;}
using ContextInit=void*(__thiscall*)(void*,void*,void*);
using ContextTick=unsigned char(__thiscall*)(void*);
using ActorTick=void(__thiscall*)(void*);
using ActorModeTick=void(__thiscall*)(void*,unsigned);
struct NavigationStats {unsigned submissions{},old_destroys{},old_flag{},result_state{};void* actor{};std::array<unsigned char,0x14> location{};std::array<unsigned char,0x20> options{};};
static NavigationStats* navigation_stats;
static void __fastcall submit_path(void* actor,void*,void* location,void* options){
    require(navigation_stats!=nullptr,"Navigation fixture provider not armed");
    ++navigation_stats->submissions;navigation_stats->actor=actor;
    memcpy(navigation_stats->location.data(),location,0x14);memcpy(navigation_stats->options.data(),options,0x20);
    at<unsigned>(actor,0x3bc)=navigation_stats->result_state;
}
static void __fastcall old_navigation_destroy(void*,void*,unsigned flag){++navigation_stats->old_destroys;navigation_stats->old_flag=flag;}
static void* old_navigation_table[]={reinterpret_cast<void*>(&old_navigation_destroy)};
using TaskRequestInit=void*(__thiscall*)(void*,const void*,unsigned,unsigned,float,unsigned short,unsigned char);
using BehaviorRequestInit=void*(__thiscall*)(void*,const void*);
using BehaviorInit=void*(__thiscall*)(void*,void*,const void*);
using InstallNavigation=void(__thiscall*)(void*,const void*);
struct Image {
    unsigned char* base{};size_t size{};std::string digest;
    ContextInit init{};ContextTick context{};ActorTick simple{};ActorModeTick ordinary{};
    struct Range {unsigned rva,length;};
    const std::array<Range,14> roots{{{0x350fe0,0x26},{0x3efc70,0x96},{0x3f04f0,0xd0},{0x3dea90,0x517},
        {0x350d10,0x8b},{0x3b91c0,0x8d},{0x3b0ac0,0x4},{0x3b9110,0xae},{0x3b87f0,0x40},
        {0x3d9320,0x43},{0x3e7f00,0x2a4},{0x3d8b20,0x11},{0x3e3d20,0x10},{0x3636f0,0x4}}};
    std::array<std::string,14> originals;
    void jump(unsigned rva,void* target){require(rva+5<size,"Fixture provider bounds");base[rva]=0xe9;at<unsigned>(base,rva+1)=static_cast<unsigned>(reinterpret_cast<uintptr_t>(target)-reinterpret_cast<uintptr_t>(base+rva+5));}
    explicit Image(const std::filesystem::path& path){
        std::ifstream input(path,std::ios::binary);require(bool(input),"Open owned engine");
        input.seekg(0,std::ios::end);auto length=input.tellg();require(length>4096&&length<64*1024*1024,"Owned image size");input.seekg(0);
        Bytes bytes(static_cast<size_t>(length));input.read(reinterpret_cast<char*>(bytes.data()),bytes.size());require(bool(input),"Read engine");digest=sha(bytes.data(),bytes.size());
        require(digest=="022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5"||digest=="c4d91d74ce9ff8338cf61d5b021b03a2776830177a590fcac205509a42239545","Unreviewed engine refused");
        auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(bytes.data());require(dos->e_magic==IMAGE_DOS_SIGNATURE&&dos->e_lfanew>0,"DOS header");
        require(static_cast<size_t>(dos->e_lfanew)+sizeof(IMAGE_NT_HEADERS32)<bytes.size(),"PE header bounds");
        auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(bytes.data()+dos->e_lfanew);
        require(nt->Signature==IMAGE_NT_SIGNATURE&&nt->FileHeader.Machine==IMAGE_FILE_MACHINE_I386&&nt->OptionalHeader.Magic==IMAGE_NT_OPTIONAL_HDR32_MAGIC,"x86 PE required");
        size=nt->OptionalHeader.SizeOfImage;require(size==0x1d88000&&nt->OptionalHeader.SizeOfHeaders<=bytes.size(),"Reviewed PE size");
        base=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));require(base!=nullptr,"Private image allocation");
        memcpy(base,bytes.data(),nt->OptionalHeader.SizeOfHeaders);
        auto sections=IMAGE_FIRST_SECTION(nt);
        require(reinterpret_cast<const unsigned char*>(sections+nt->FileHeader.NumberOfSections)<=bytes.data()+bytes.size(),"Section headers");
        for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){auto& s=sections[i];require(static_cast<uint64_t>(s.PointerToRawData)+s.SizeOfRawData<=bytes.size()&&static_cast<uint64_t>(s.VirtualAddress)+s.SizeOfRawData<=size,"Section bounds");memcpy(base+s.VirtualAddress,bytes.data()+s.PointerToRawData,s.SizeOfRawData);}
        auto reloc=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];require(static_cast<uint64_t>(reloc.VirtualAddress)+reloc.Size<=size,"Relocation directory");
        unsigned cursor=0;
        while(cursor<reloc.Size){require(reloc.Size-cursor>=8,"Relocation header");auto block=reinterpret_cast<const IMAGE_BASE_RELOCATION*>(base+reloc.VirtualAddress+cursor);require(block->SizeOfBlock>=8&&block->SizeOfBlock<=reloc.Size-cursor&&block->SizeOfBlock%2==0,"Relocation block");auto entries=reinterpret_cast<const unsigned short*>(block+1);
            for(unsigned i=0;i<(block->SizeOfBlock-8)/2;++i){auto type=entries[i]>>12;require(type==0||type==IMAGE_REL_BASED_HIGHLOW,"Relocation type");if(type){auto rva=block->VirtualAddress+(entries[i]&4095);require(rva+4<=size,"Relocation target");at<unsigned>(base,rva)+=static_cast<unsigned>(reinterpret_cast<uintptr_t>(base)-nt->OptionalHeader.ImageBase);}}
            cursor+=block->SizeOfBlock;
        }
        for(unsigned i=0;i<roots.size();++i)originals[i]=sha(base+roots[i].rva,roots[i].length);
        // Only downstream services are replaced, in this private fixture image.
        jump(0x3ad010,reinterpret_cast<void*>(&before));jump(0x3e4850,reinterpret_cast<void*>(&motion));jump(0x3f0370,reinterpret_cast<void*>(&after));jump(0x3f05c0,reinterpret_cast<void*>(&tail));jump(0x3ca8a0,reinterpret_cast<void*>(&clear_lower));
        jump(0x3df290,reinterpret_cast<void*>(&submit_path));
        for(unsigned i=0;i<roots.size();++i)require(sha(base+roots[i].rva,roots[i].length)==originals[i],"Tested native routine was modified");
        DWORD old{};require(VirtualProtect(base,size,PAGE_EXECUTE_READ,&old)!=0,"Private code protection");FlushInstructionCache(GetCurrentProcess(),base,size);
        init=reinterpret_cast<ContextInit>(base+0x350fe0);context=reinterpret_cast<ContextTick>(base+0x3efc70);simple=reinterpret_cast<ActorTick>(base+0x3f04f0);ordinary=reinterpret_cast<ActorModeTick>(base+0x3dea90);
    }
    ~Image(){if(base)VirtualFree(base,0,MEM_RELEASE);}
    Image(const Image&)=delete;Image& operator=(const Image&)=delete;
};
struct Actor {
    alignas(16) std::array<unsigned char,0x720> data{};
    std::array<void*,0x80> table{};TaskStats duty,priority;FrameStats frames;
    void* p(){return data.data();}
    explicit Actor(Image& image){
        table[0x12c/4]=reinterpret_cast<void*>(&reaction);table[0x174/4]=reinterpret_cast<void*>(&fallback);
        at<void*>(p(),0)=table.data();at<FrameStats*>(p(),0x700)=&frames;
        image.init(data.data()+0x2f0,p(),data.data()+0x24c);image.init(data.data()+0x564,p(),data.data()+0x24c);
        // Sentinel values are opaque: none may be dereferenced by tested paths.
        for(unsigned off:{0x10u,0x18u,0x98u,0x20cu,0x21cu,0x3b8u,0x600u})at<unsigned>(p(),off)=0xa5000000u+off;
        at<unsigned char>(p(),0xf0)=1;at<unsigned char>(p(),0x665)=1;at<unsigned>(p(),0x674)=2;
        task(0x2f0,duty,false);at<void*>(p(),0x250)=lower_table;at<unsigned char>(p(),0x2e8)=1;
    }
    void task(unsigned offset,TaskStats& s,bool cleanup){auto context=data.data()+offset;memset(context+8,0,0x8c);at<void*>(context,8)=task_table;at<void*>(context,16)=context;at<void*>(context,20)=data.data()+0x24c;at<TaskStats*>(context+8,0x30)=&s;context[0x94]=1;context[0x95]=0;context[0x96]=0;context[0x97]=cleanup?1:0;}
    void identity() {for(unsigned off:{0x10u,0x18u,0x98u,0x20cu,0x21cu,0x3b8u,0x600u})require(at<unsigned>(p(),off)==0xa5000000u+off,"Owner/support/station changed");require(at<unsigned char>(p(),0xf0)==1,"Alive flag changed");}
};
static unsigned cases;
static void passed(const char* name){++cases;std::cout<<"PASS "<<name<<'\n';}
static void context_tests(Image& image){
    std::array<unsigned char,0xa4> raw;raw.fill(0xa7);auto result=image.init(raw.data(),reinterpret_cast<void*>(0x1234),reinterpret_cast<void*>(0x5678));
    require(result==raw.data()&&at<unsigned>(raw.data(),0)==0x1234&&at<unsigned>(raw.data(),4)==0x5678,"Context init identity");for(unsigned i=8;i<raw.size();++i)require(raw[i]==((i>=0x94&&i<0x9c)?0:0xa7),"Context init wrote outside fields");passed("native context initializer boundaries");
    {Actor a(image);auto c=a.data.data()+0x564;at<unsigned>(c,8)=0xdeadbeef;require(image.context(c)==0,"Inactive task must not dispatch");passed("inactive task never dispatched");}
    {Actor a(image);auto c=a.data.data()+0x2f0;require(image.context(c)==1&&image.context(c)==1,"Active result");require(a.duty.starts==1&&a.duty.ticks==2&&at<unsigned>(c,0x1c)==2,"Single start and two ticks");a.identity();passed("one start, successive native ticks");}
    {Actor a(image);auto c=a.data.data()+0x2f0;c[0x18]=1;image.context(c);require(a.duty.starts==0&&a.duty.ticks==0&&a.duty.destroys==0&&c[0x94]==1,"Retained completed task");passed("completed duty retained when cleanup disabled");}
    for(bool start_finish:{false,true}){Actor a(image);a.task(0x564,a.priority,true);auto c=a.data.data()+0x564;a.priority.finish_on_start=start_finish;a.priority.finish_on_tick=!start_finish;image.context(c);require(a.priority.starts==1&&a.priority.ticks==unsigned(!start_finish)&&a.priority.destroys==1&&a.priority.destroy_flag==1&&c[0x94]==0&&c[0x95]==0,"Native completion cleanup");image.context(c);require(a.priority.destroys==1,"Duplicate cleanup");a.identity();passed(start_finish?"completion during start cleans once":"completion during tick cleans once");}
    {Actor a(image);auto c=a.data.data()+0x2f0;c[0x96]=1;image.context(c);image.context(c);require(a.frames.clear_lower==1,"Lower reset not limited to task start");a.identity();passed("lower reset happens only at first start");}
}
static void actor_tests(Image& image,bool ordinary){
    auto dispatch=[&](Actor& a){if(ordinary)image.ordinary(a.p(),0);else image.simple(a.p());};
    Actor a(image),peer(image);dispatch(a);dispatch(peer);require(a.duty.starts==1&&a.duty.ticks==1,"Initial duty");
    auto initial_peer=peer.data;a.task(0x564,a.priority,true);dispatch(a);
    require(a.priority.starts==1&&a.priority.ticks==1&&a.duty.ticks==1,"Priority did not suspend duty");require(a.frames.lower_ticks==2&&a.frames.motion==2,"Lower/motion service stopped during priority");require(peer.data==initial_peer,"Other actor modified");
    require(at<unsigned char>(a.p(),0x385)==unsigned(!ordinary),"Native duty restart policy differs");a.priority.finish_on_tick=true;dispatch(a);dispatch(a);
    require(a.duty.ticks==2&&a.duty.starts==unsigned(ordinary?2:1)&&a.priority.destroys==1,"Duty did not resume as native policy specifies");a.identity();peer.identity();
    passed(ordinary?"ordinary actor resumes by restarting existing duty":"alternate actor resumes existing duty without restart");
    for(unsigned i=0;i<100;++i){a.priority={};a.task(0x564,a.priority,true);a.priority.finish_on_tick=true;dispatch(a);dispatch(peer);dispatch(a);require(a.priority.destroys==1,"Repeated interrupt cleanup");a.identity();peer.identity();}
    require(a.duty.ticks==102&&peer.duty.ticks==101,"Repeated handoff/peer progression");passed(ordinary?"100 ordinary interruptions preserve owner and peer progress":"100 alternate interruptions preserve owner and peer progress");
    TaskStats replacement;a.priority={};a.task(0x564,a.priority,true);dispatch(a);a.task(0x2f0,replacement,false);a.priority.finish_on_tick=true;dispatch(a);dispatch(a);
    require(replacement.starts==1&&replacement.ticks==1&&a.duty.ticks==102,"Old duty restored over current task");a.identity();passed(ordinary?"ordinary dispatch resumes current replacement duty":"alternate dispatch resumes current replacement duty");
}

static void navigation_tests(Image& image){
    auto init=reinterpret_cast<TaskRequestInit>(image.base+0x350d10);
    auto convert=reinterpret_cast<BehaviorRequestInit>(image.base+0x3b91c0);
    auto construct=reinterpret_cast<BehaviorInit>(image.base+0x3b9110);
    auto install=reinterpret_cast<InstallNavigation>(image.base+0x3d9320);
    auto begin=reinterpret_cast<ActorTick>(image.base+0x3e7f00);
    std::array<unsigned char,0x14> location{};at<float>(location.data(),0)=12.5f;at<float>(location.data(),4)=3.75f;at<float>(location.data(),8)=-8.25f;
    at<unsigned>(location.data(),0xc)=0x13579bdf;at<unsigned>(location.data(),0x10)=0x2468ace0;
    std::array<unsigned char,0x51> task;task.fill(0xa7);
    require(init(task.data(),location.data(),2,4,0.75f,0x2345,1)==task.data(),"Native request initializer return");
    require(memcmp(task.data(),location.data(),0x14)==0,"Task location frame lost");
    require(at<unsigned>(task.data(),0x14)==2&&at<unsigned>(task.data(),0x18)==2&&at<unsigned>(task.data(),0x1c)==4&&at<float>(task.data(),0x20)==0.75f,"Task numeric fields");
    require(at<unsigned short>(task.data(),0x24)==0x2345&&task[0x26]==1&&task[0x27]==1&&task[0x28]==0&&task[0x29]==0&&task[0x2a]==0&&task[0x2b]==0xa7,"Task flags and untouched padding");
    require(at<unsigned>(task.data(),0x2c)==2&&task[0x30]==1&&task[0x31]==0xa7&&task[0x32]==0xa7&&task[0x33]==0xa7,"Task default selector vs padding");
    for(unsigned i=0x34;i<0x41;++i)require(task[i]==0,"Task default terminal fields");
    for(unsigned i=0x41;i<task.size();++i)require(task[i]==0xa7,"Task constructor overrun");
    passed("native navigation task request preserves complete framed location and boundaries");
    std::array<unsigned char,0x4d> request;request.fill(0xa7);auto task_before=task;
    require(convert(request.data(),task.data())==request.data()&&task==task_before,"Request conversion input/return");
    require(memcmp(request.data(),task.data(),0x28)==0&&request[0x28]==1&&request[0x29]==0,"Conversion first flags");
    require(request[0x2a]==0xa7&&request[0x2b]==0xa7&&at<unsigned>(request.data(),0x2c)==2,"Conversion padding and policy");
    require(at<unsigned>(request.data(),0x30)==0&&at<unsigned>(request.data(),0x34)==0&&at<unsigned>(request.data(),0x38)==0&&request[0x3c]==0,"Default conversion enables unexpected timeout/reference");
    for(unsigned i=0x3d;i<request.size();++i)require(request[i]==0xa7,"Behavior request overrun");
    passed("native conversion separates task default flag from behavior timeout selector");
    auto custom_task=task;custom_task[0x29]=1;custom_task[0x30]=0;at<unsigned>(custom_task.data(),0x34)=17;at<unsigned>(custom_task.data(),0x38)=29;at<unsigned>(custom_task.data(),0x3c)=0x76543210;custom_task[0x40]=1;
    std::array<unsigned char,0x4d> custom{};convert(custom.data(),custom_task.data());
    require(custom[0x28]==0&&custom[0x29]==1&&at<unsigned>(custom.data(),0x30)==17&&at<unsigned>(custom.data(),0x34)==29&&at<unsigned>(custom.data(),0x38)==0x76543210&&custom[0x3c]==1,"Nondefault conversion mapping");
    passed("native conversion maps terminal fields without conflating request layouts");
    {Actor a(image);std::array<unsigned char,0xa4> context{};at<void*>(context.data(),0)=a.p();at<unsigned>(context.data(),0xa0)=0x10203040;
        std::array<unsigned char,0x6d> object;object.fill(0xa7);construct(object.data(),context.data(),request.data());
        require(at<void*>(object.data(),0)==image.base+0x15e0ddc&&at<void*>(object.data(),4)==a.p(),"Navigation object identity");
        require(object[8]==0&&at<unsigned>(object.data(),0xc)==0&&object[0x10]==0&&context[0x9c]==1&&at<unsigned>(object.data(),0x14)==0x10203040,"Base behavior initializer");
        require(memcmp(object.data()+0x1c,location.data(),0x14)==0&&at<unsigned>(object.data(),0x4c)==0&&object[0x5c]==1,"Navigation object request");
        for(unsigned i=0x5d;i<object.size();++i)require(object[i]==0xa7,"Navigation object overrun");a.identity();passed("actual navigation constructors keep identity and frame and disable default timeout fallback");
        std::array<unsigned char,12> reference{};at<unsigned>(reference.data(),8)=7;auto referenced_request=request;at<void*>(referenced_request.data(),0x38)=reference.data();
        construct(object.data(),context.data(),referenced_request.data());require(at<unsigned>(reference.data(),8)==8,"Native optional reference retention");passed("native constructor retains optional request reference once");
    }
    for(bool old_active:{false,true})for(unsigned result_state:{0u,7u}){
        Actor a(image),peer(image);NavigationStats state{};state.result_state=result_state;navigation_stats=&state;
        at<void*>(a.p(),0x24c)=a.p();at<unsigned>(a.p(),0x2ec)=0x10203040;
        at<void*>(a.p(),0x250)=old_navigation_table;at<unsigned char>(a.p(),0x2e8)=old_active?1:0;
        at<unsigned>(a.p(),0x1f0)=5;at<unsigned>(a.p(),0x244)=0xabcd1234;
        auto prior=a.data;auto peer_prior=peer.data;install(a.p(),request.data());
        require(state.old_destroys==unsigned(old_active)&&(!old_active||state.old_flag==1),"Native lower behavior cleanup");
        require(at<unsigned char>(a.p(),0x2e8)==1&&at<void*>(a.p(),0x250)==image.base+0x15e0ddc,"Native navigation not installed");
        begin(a.data.data()+0x250);
        require(state.submissions==1&&state.actor==a.p()&&state.location==location,"Native start changed destination/support frame");
        require(state.options[0]==1&&at<float>(state.options.data(),4)==0.75f&&at<unsigned>(state.options.data(),8)==0&&at<unsigned short>(state.options.data(),0xe)==0x2345,"Native path options/header");
        require(state.options[0xc]==0&&state.options[0x10]==0&&state.options[0x11]==1&&state.options[0x12]==1&&state.options[0x13]==1&&at<unsigned>(state.options.data(),0x14)==2&&state.options[0x18]==0&&at<unsigned>(state.options.data(),0x1c)==0,"Native path options/flags");
        for(unsigned i=0;i<a.data.size();++i){bool permitted=(i>=0x250&&i<0x2e9)||(i>=0x3bc&&i<0x3c0)||(result_state!=7&&i>=0x244&&i<0x248);if(!permitted)require(a.data[i]==prior[i],"Navigation modified duty, priority, support, station or position");}
        require(peer.data==peer_prior,"Navigation changed another actor");a.identity();peer.identity();
        require(at<unsigned>(a.p(),0x244)==(result_state==7?0xabcd1234u:4u),"Failure/success post-submission policy");
        passed(old_active?(result_state?"replace active lower behavior and preserve duty after rejected path":"replace active lower behavior and submit unchanged native framed request"):(result_state?"install idle lower behavior and preserve duty after rejected path":"install idle lower behavior and submit unchanged native framed request"));
        navigation_stats=nullptr;
    }
}

int wmain(int argc,wchar_t** argv){try{require(argc==2,"Expected one owned Empire.Retail.dll path");Image image(argv[1]);context_tests(image);actor_tests(image,false);actor_tests(image,true);navigation_tests(image);std::cout<<"PASS "<<cases<<" native contract cases; engine="<<image.digest<<"\nFixture only: no live possession, movement, return to station or combat acceptance.\n";return 0;}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
