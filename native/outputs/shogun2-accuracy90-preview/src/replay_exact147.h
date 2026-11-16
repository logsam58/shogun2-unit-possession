#pragma once
#include "exact_replay_carrier147.h"

// The save-only view borrows stock replay metadata and keeps native command bytes intact.
namespace replay147 {
using namespace exactreplay147;
constexpr uintptr_t tick_rva=0x303430, save_rva=0xb90710;
constexpr uintptr_t assign_rva=0xb8f170, copy_rva=0xb8db10;
using NativeTick=void(__thiscall*)(void*,const unsigned char*,void*);
using NativeSave=void*(__cdecl*)(void*,const unsigned char*);
static NativeTick native_tick;
static NativeSave native_save;
using NativeCopy=void*(__thiscall*)(void*,const unsigned char*);
static NativeCopy native_assign,native_copy;
static bool installed;
static SRWLOCK lock=SRWLOCK_INIT;
static uintptr_t recording_env,recorder;
static uint32_t recording_tick;
static bool capture_failed,recording;
static std::vector<Event> captured;
struct ReplaySource {
    uintptr_t owner{},ticks{},digests{};
    uint32_t count{},last_digest{};
    bool operator==(const ReplaySource& rhs) const noexcept {
        return owner==rhs.owner&&ticks==rhs.ticks&&digests==rhs.digests&&
            count==rhs.count&&last_digest==rhs.last_digest;
    }
};
static ReplaySource copies[8];
static uintptr_t playing_env;
static volatile LONG playing_tick;
static constexpr size_t event_limit_per_tick=64;
struct Batch {
    Event events[event_limit_per_tick]{};
    size_t count{};
    bool push(Event event) noexcept {
        if(count==event_limit_per_tick)return false;
        events[count++]=event;
        return true;
    }
};
static Batch playing;
static uintptr_t playback_session;
static uint32_t last_playback_tick;
static volatile LONG playback_epoch;
static bool playback_failed;
static volatile LONG callback_tick;
struct Calls {uintptr_t object{};uint16_t count{};};
static Calls unit_calls[2048], actor_calls[64];

struct ActorScope {uintptr_t actor{};uint16_t occurrence{};};
static thread_local ActorScope actor_scope;
struct MotionWitness {uintptr_t actor{};uint32_t proof{};};
static thread_local MotionWitness motion_witness;
struct UnitScope {uintptr_t unit{};Boundary boundary{};uint16_t occurrence{};};
static thread_local UnitScope unit_scope;

struct Exclusive {
    Exclusive(){AcquireSRWLockExclusive(&lock);}
    ~Exclusive(){ReleaseSRWLockExclusive(&lock);}
    Exclusive(const Exclusive&)=delete;
    Exclusive& operator=(const Exclusive&)=delete;
};

static bool token_available(){
    const auto registry=field<uintptr_t>(base,0x18d9000);
    const auto count=field<unsigned>(base,0x18d9004);
    if(!registry||!count||count>4096)return false;
    for(unsigned i=0;i<count;++i)if(field<uint32_t>(registry,i*28)==key)return false;
    return true;
}

static bool read_tick(const unsigned char* bytes,uint32_t tick,Batch& batch) noexcept {
    if(!bytes||!tick||tick>tick_limit)return false;
    __try{
        const auto count=read<uint32_t>(bytes+0x28024);
        if(count>block_limit)return false;
        for(uint32_t i=0;i<count;++i){
            const auto* queue=bytes+4+i*0x5004;
            if(!visit(queue,read<uint32_t>(queue+0x5000),tick,[&](Event event){return batch.push(event);}))return false;
        }
        return !batch.count||token_available();
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}

static void set_playback(uintptr_t env,uint32_t tick,const Batch& batch,bool valid_batch){
    Exclusive guard;
    const bool new_session=env!=playback_session||tick<=last_playback_tick;
    const bool already_failed=playback_failed;
    if(new_session){
        InterlockedIncrement(&playback_epoch);
        playback_session=env;
        playback_failed=false;
    }else{
        for(size_t i=0;i<playing.count;++i)if(playing.events[i].actor)playback_failed=true;
    }
    last_playback_tick=tick;
    playing_env=env;
    playback_failed=playback_failed||!valid_batch;
    if(playback_failed&&!already_failed&&!new_session)InterlockedIncrement(&playback_epoch);
    playing=playback_failed?Batch{}:batch;
    InterlockedExchange(&playing_tick,playing.count?LONG(tick):0);
    InterlockedExchange(&callback_tick,playing_tick);
    memset(unit_calls,0,sizeof unit_calls);
    memset(actor_calls,0,sizeof actor_calls);
}
static void fail_playback(){
    Exclusive guard;
    if(!playback_failed)InterlockedIncrement(&playback_epoch);
    playback_failed=true;
    playing={};
    InterlockedExchange(&playing_tick,0);
    InterlockedExchange(&callback_tick,0);
}
static void start_tick(uintptr_t env,const unsigned char* bytes){
    if(!env)return;
    const auto previous=field<uint32_t>(env,0x282bc);
    const auto tick=previous+1;
    const bool playback=field<unsigned char>(env,0x28254)!=0;
    const bool solo=!field<uintptr_t>(env,0x28248);
    if(playback&&solo){
        Batch batch;
        const bool valid_batch=read_tick(bytes,tick,batch);
        set_playback(env,tick,batch,valid_batch);
        return;
    }
    if(playback_session)set_playback(0,0,Batch{},true);
    InterlockedExchange(&callback_tick,0);
    const auto source=solo?field<uintptr_t>(env,0x2825c):0;
    if(!source||!tick||tick>tick_limit)return;
    const auto previous_count=field<uint32_t>(source,0x14);
    // If the provider has already queued the next tick it is still the same
    // ordinal. In either case reject a recorder that has an unrelated origin.
    const bool aligned=previous_count==previous||previous_count==tick;
    Exclusive guard;
    if(recording_env!=env||recorder!=source||tick<recording_tick){
        captured.clear();
        for(auto& copy:copies)copy={};
        recorder=source;
        recording_env=env;
        capture_failed=!token_available();
    }
    recording_tick=tick;
    recording=aligned&&!capture_failed;
    memset(unit_calls,0,sizeof unit_calls);
    memset(actor_calls,0,sizeof actor_calls);
    InterlockedExchange(&callback_tick,recording?LONG(tick):0);
}
static void __fastcall tick_hook(void* env,void*,const unsigned char* bytes,void* context){
    __try{start_tick(reinterpret_cast<uintptr_t>(env),bytes);}
    __except(EXCEPTION_EXECUTE_HANDLER){}
    native_tick(env,bytes,context);
}

static bool infantry(uintptr_t actor,uintptr_t formation,uintptr_t env){
    return actor&&formation&&env&&(field<uintptr_t>(actor,0)==base+0x15dfec8||field<uintptr_t>(actor,0)==base+0x15dbfb4)&&
        field<unsigned char>(actor,0xf0)==1&&!field<uintptr_t>(actor,0x210)&&
        field<uintptr_t>(actor,0x214)==formation&&field<uintptr_t>(actor,0x28)==env&&
        field<uintptr_t>(formation,0x14)==env&&unit_member130(formation,actor);
}
static void append(uintptr_t env,uintptr_t owner,Event event) noexcept {
    Exclusive guard;
    if(!recording||env!=recording_env||owner!=recorder||event.tick!=recording_tick)return;
    if(!valid(event)||captured.size()==tick_limit){
        capture_failed=true;
        recording=false;
        return;
    }
    try{captured.push_back(event);}
    catch(...){capture_failed=true;recording=false;}
}
// Position and velocity come from the original integrator, and facing is
// the native 16-bit bearing. Compare their exact bits when replaying a sample.
static uint32_t motion_proof(uintptr_t actor){
    uint32_t proof=2166136261u;
    for(unsigned i=0;i<3;++i){
        proof=(proof^field<uint32_t>(actor,0x48+i*4))*16777619u;
        proof=(proof^field<uint32_t>(actor,0x8c+i*4))*16777619u;
    }
    proof=(proof^field<uint16_t>(actor,0x64))*16777619u;
    return proof?proof:1;
}
static void finish_capture(uint32_t tick,uint32_t actor,uint32_t unit_id,uint16_t occurrence,uint32_t proof){
    Exclusive guard;
    for(auto sample=captured.rbegin();sample!=captured.rend()&&sample->tick==tick;++sample){
        if(sample->actor!=actor||sample->unit!=unit_id||sample->kind!=Kind::motion||
           sample->occurrence!=occurrence||sample->proof)continue;
        sample->proof=proof;
        return;
    }
}
static void finish_live_motion(uintptr_t actor) noexcept {
    if(!installed||!callback_tick||actor!=possessed)return;
    __try{
        const auto env=field<uintptr_t>(actor,0x28),formation=field<uintptr_t>(actor,0x214);
        if(!env||!formation||field<unsigned char>(env,0x28254)||field<uintptr_t>(env,0x28248))return;
        if(!infantry(actor,formation,env))return;
        finish_capture(field<uint32_t>(env,0x282bc),field<uint32_t>(actor,0x1c8),field<uint32_t>(formation,4),
                       actor_scope.actor==actor?actor_scope.occurrence:0,motion_proof(actor));
    }__except(EXCEPTION_EXECUTE_HANDLER){}
}
static void finish_play_motion(uintptr_t actor) noexcept {
    if(motion_witness.actor!=actor)return;
    const auto expected=motion_witness.proof;
    motion_witness={};
    __try{
        if(expected&&motion_proof(actor)!=expected)fail_playback();
    }__except(EXCEPTION_EXECUTE_HANDLER){fail_playback();}
}
static void capture_motion(uintptr_t actor,float x,float z,uint16_t facing) noexcept {
    if(!installed||actor!=possessed)return;
    __try{
        const auto env=field<uintptr_t>(actor,0x28);
        if(!env||field<unsigned char>(env,0x28254)||field<uintptr_t>(env,0x28248)||
           !infantry(actor,unit,env)||field<uintptr_t>(actor,0x18))return;
        Event event{field<uint32_t>(env,0x282bc),field<uint32_t>(actor,0x1c8),
                    field<uint32_t>(unit,4),x,z,facing,Kind::motion};
        if(actor_scope.actor==actor)event.occurrence=actor_scope.occurrence;
        event.target=motion_proof(actor);
        append(env,field<uintptr_t>(env,0x2825c),event);
    }__except(EXCEPTION_EXECUTE_HANDLER){}
}

static bool take(uintptr_t env,uint32_t tick,uint32_t unit_id,uint32_t actor_id,Stream lane,
                 Boundary boundary,uint16_t occurrence,Event& event){
    Exclusive guard;
    if(playing_env!=env||uint32_t(playing_tick)!=tick)return false;
    for(size_t i=0;i<playing.count;++i){
        auto& item=playing.events[i];
        if(item.actor&&item.unit==unit_id&&(!actor_id||item.actor==actor_id)&&
           stream(item.kind)==lane&&item.boundary==boundary&&item.occurrence==occurrence){
            event=item;
            item={};
            return true;
        }
    }
    return false;
}
template<size_t Size> static uint16_t count_call(Calls (&table)[Size],uintptr_t object){
    size_t offset=((object>>4)^(object>>15))%Size;
    for(size_t i=0;i<Size;++i){
        auto& row=table[(offset+i)%Size];
        if(!row.object){row={object,1};return 1;}
        if(row.object==object&&row.count!=65535)return ++row.count;
    }
    return 0;
}
template<size_t Size>
static uint16_t scheduled_call(Calls (&table)[Size],uintptr_t object,uintptr_t env,uint32_t tick,bool playback){
    Exclusive guard;
    if(playback?(playing_env!=env||uint32_t(playing_tick)!=tick):
                (!recording||recording_env!=env||recording_tick!=tick))return 0;
    return count_call(table,object);
}
static uint16_t clock_unit(uintptr_t formation) noexcept {
    if(!callback_tick||!formation)return 0;
    uintptr_t env{};
    uint32_t tick{};
    __try{
        env=field<uintptr_t>(formation,0x14);
        if(!env)return 0;
        tick=field<uint32_t>(env,0x282bc);
    }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
    if(uint32_t(callback_tick)!=tick)return 0;
    return scheduled_call(unit_calls,formation,env,tick,playing_tick!=0);
}
static bool has_actor(uintptr_t env,uint32_t tick,uint32_t unit_id,uint32_t actor_id){
    Exclusive guard;
    if(playing_env!=env||uint32_t(playing_tick)!=tick)return false;
    for(size_t i=0;i<playing.count;++i){
        const auto& item=playing.events[i];
        if(item.actor==actor_id&&item.unit==unit_id&&stream(item.kind)!=Stream::formation)return true;
    }
    return false;
}
static bool actor_in_playback(uintptr_t actor) noexcept {
    if(!playing_tick||!actor)return false;
    __try{
        const auto env=field<uintptr_t>(actor,0x28),formation=field<uintptr_t>(actor,0x214);
        return env&&formation&&has_actor(env,field<uint32_t>(env,0x282bc),field<uint32_t>(formation,4),
                                         field<uint32_t>(actor,0x1c8));
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static uint16_t clock_actor(uintptr_t actor,bool playback) noexcept {
    if(!callback_tick||!actor||(!playback&&actor!=possessed))return 0;
    uintptr_t env{};
    uint32_t tick{};
    __try{
        env=field<uintptr_t>(actor,0x28);
        if(!env)return 0;
        tick=field<uint32_t>(env,0x282bc);
    }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
    if(uint32_t(callback_tick)!=tick)return 0;
    return scheduled_call(actor_calls,actor,env,tick,playback);
}
static void record_action(uintptr_t actor,Event event,uintptr_t expected_formation) noexcept {
    if(!installed||!callback_tick||!actor)return;
    __try{
        const auto formation=field<uintptr_t>(actor,0x214),env=field<uintptr_t>(actor,0x28);
        if(!env||!formation||formation!=expected_formation||
           field<unsigned char>(env,0x28254)||field<uintptr_t>(env,0x28248)||
           !infantry(actor,formation,env))return;
        event.tick=field<uint32_t>(env,0x282bc);
        event.unit=field<uint32_t>(formation,4);
        event.actor=field<uint32_t>(actor,0x1c8);
        append(env,field<uintptr_t>(env,0x2825c),event);
    }__except(EXCEPTION_EXECUTE_HANDLER){}
}
static void play_motion(uintptr_t actor) noexcept {
    if(!playing_tick)return;
    __try{
        const auto env=field<uintptr_t>(actor,0x28);
        const auto formation=field<uintptr_t>(actor,0x214);
        if(!env||!formation||!field<unsigned char>(env,0x28254)||field<uintptr_t>(env,0x28248))return;
        const auto tick=field<uint32_t>(env,0x282bc);
        if(tick!=uint32_t(playing_tick))return;
        const uint16_t occurrence=actor_scope.actor==actor?actor_scope.occurrence:uint16_t(0);
        Event event;
        const auto actor_id=field<uint32_t>(actor,0x1c8),unit_id=field<uint32_t>(formation,4);
        if(!take(env,tick,unit_id,actor_id,Stream::motion,Boundary::none,occurrence,event)){
            if(!occurrence||!take(env,tick,unit_id,actor_id,Stream::motion,Boundary::none,0,event))return;
        }
        if(!infantry(actor,formation,env)||field<uintptr_t>(actor,0x18)){fail_playback();return;}
        const float dx=event.target_x-field<float>(actor,0x48),dz=event.target_z-field<float>(actor,0x50);
        if(!(dx*dx+dz*dz<=4.f)||(event.target&&motion_proof(actor)!=event.target)){fail_playback();return;}
        apply_native_intent147(actor,event.target_x,event.target_z,event.facing);
        motion_witness={actor,event.proof};
    }__except(EXCEPTION_EXECUTE_HANDLER){fail_playback();}
}

static ReplaySource replay_source(uintptr_t owner) noexcept {
    __try{
        if(!owner)return {};
        const auto count=field<uint32_t>(owner,0x14);
        if(!count||count>tick_limit||field<uint32_t>(owner,0x34)!=count+1)return {};
        const auto ticks=field<uintptr_t>(owner,0x18),digests=field<uintptr_t>(owner,0x38);
        if(!ticks||!digests)return {};
        return {owner,ticks,digests,count,field<uint32_t>(digests,count*sizeof(uint32_t))};
    }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
static bool owns_source(ReplaySource source){
    if(!source.owner||capture_failed||captured.empty())return false;
    if(source.owner==recorder)return true;
    for(const auto& copy:copies)if(source==copy)return true;
    return false;
}
// 27fe50 -> 1d1740 uses assignment for the results object. A further
// construction can copy that object. The copy is still owned by the engine.
static void transfer_copy(ReplaySource source,uintptr_t target){
    if(!target||target==source.owner)return;
    const auto destination=replay_source(target);
    Exclusive guard;
    const bool owned=owns_source(source);
    ReplaySource* slot=nullptr;
    for(auto& copy:copies){
        if(copy.owner==target){copy={};slot=&copy;break;}
        if(!copy.owner&&!slot)slot=&copy;
    }
    if(owned&&slot&&destination.owner&&source.count==destination.count&&
       source.last_digest==destination.last_digest)*slot=destination;
}
static void* __fastcall assign_hook(void* target,void*,const unsigned char* source){
    const auto from=replay_source(reinterpret_cast<uintptr_t>(source));
    void* result=native_assign(target,source);
    if(result==target)transfer_copy(from,reinterpret_cast<uintptr_t>(target));
    return result;
}
static void* __fastcall copy_hook(void* target,void*,const unsigned char* source){
    const auto from=replay_source(reinterpret_cast<uintptr_t>(source));
    void* result=native_copy(target,source);
    if(result==target)transfer_copy(from,reinterpret_cast<uintptr_t>(target));
    return result;
}
static bool save_snapshot(uintptr_t source,std::vector<Event>& events){
    const auto info=replay_source(source);
    Exclusive guard;
    if(!owns_source(info)||captured.back().tick>info.count)return false;
    for(const auto& event:captured)if(event.kind==Kind::motion&&(!event.target||!event.proof))return false;
    events=captured;
    return true;
}
static bool save_prefix(const unsigned char* source,unsigned char (&copy)[0x3c]) noexcept {
    __try{
        std::memcpy(copy,source,sizeof copy);
        const auto count=read<uint32_t>(copy+0x14);
        return count&&count<=tick_limit&&read<uint32_t>(copy+0x34)==count+1;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool build_view(SaveView& view,const unsigned char* copy,const std::vector<Event>& events){
    __try{
        return view.build(read<const Tick* const*>(copy+0x18),read<uint32_t>(copy+0x14),events);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void* __cdecl save_hook(void* writer,const unsigned char* source){
    unsigned char prefix[0x3c];
    SaveView view;
    std::vector<Event> events;
    bool use_view=false;
    try{
        use_view=save_snapshot(reinterpret_cast<uintptr_t>(source),events)&&
            save_prefix(source,prefix)&&build_view(view,prefix,events);
    }catch(...){}
    if(!use_view)return native_save(writer,source);
    store(prefix+0x18,view.ticks());
    return native_save(writer,prefix);
}

static void create(){
    const unsigned char tick_bytes[]={0xb8,0x1c,0x50,0,0,0xe8,0x76,0x8d,0x1f,1};
    const unsigned char save_bytes[]={0x83,0xec,0x74,0x53,0x55,0x56,0xe8,0xe5,7,0,0};
    const unsigned char copy_bytes[]={0x83,0xec,0x10,0x53,0x56,0x8b,0xf1,0x57,0x6a,0x0c};
    const unsigned char assign_bytes[]={0x83,0xec,0x10,0x53,0x8b,0x5c,0x24,0x18,0x56,0x8b,0xf1};
    auto* tick=reinterpret_cast<void*>(base+tick_rva);
    auto* save=reinterpret_cast<void*>(base+save_rva);
    auto* copy=reinterpret_cast<void*>(base+copy_rva);
    auto* assign=reinterpret_cast<void*>(base+assign_rva);
    if(std::memcmp(tick,tick_bytes,sizeof tick_bytes)||std::memcmp(save,save_bytes,sizeof save_bytes)||
       std::memcmp(copy,copy_bytes,sizeof copy_bytes)||std::memcmp(assign,assign_bytes,sizeof assign_bytes))return;
    struct Hook {void* target;void* detour;void** original;};
    const Hook hooks[]={
        {tick,reinterpret_cast<void*>(&tick_hook),reinterpret_cast<void**>(&native_tick)},
        {save,reinterpret_cast<void*>(&save_hook),reinterpret_cast<void**>(&native_save)},
        {copy,reinterpret_cast<void*>(&copy_hook),reinterpret_cast<void**>(&native_copy)},
        {assign,reinterpret_cast<void*>(&assign_hook),reinterpret_cast<void**>(&native_assign)},
    };
    for(size_t i=0;i<4;++i){
        const auto& hook=hooks[i];
        if(MH_CreateHook(hook.target,hook.detour,hook.original)==MH_OK)continue;
        while(i)MH_RemoveHook(hooks[--i].target);
        return;
    }
    installed=true;
}

static bool disable(){
    if(!installed)return true;
    bool stopped=true;
    for(const auto rva:{tick_rva,save_rva,copy_rva,assign_rva}){
        const auto result=MH_DisableHook(reinterpret_cast<void*>(base+rva));
        stopped=(result==MH_OK||result==MH_ERROR_DISABLED)&&stopped;
    }
    return stopped;
}
}
static void replay_record_motion147(uintptr_t actor,float x,float z,uint16_t facing){
    replay147::capture_motion(actor,x,z,facing);
}


