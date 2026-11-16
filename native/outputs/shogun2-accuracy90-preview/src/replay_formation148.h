#pragma once

namespace replay148 {
using namespace exactreplay147;
static constexpr uintptr_t registry_banks[]{0xb64,0xb80};
static constexpr uint32_t seed=2166136261u;
static uint32_t hash(uint32_t value,uint32_t word){return (value^word)*16777619u;}
static uint32_t nonzero(uint32_t value){return value?value:1;}
static uint32_t slot_key(unsigned bank,unsigned index,unsigned kind){
    return ((bank+1u)<<24)|(kind<<16)|(index+1u);
}
static bool read_key(uint32_t key,unsigned& bank,unsigned& index,unsigned& kind){
    bank=(key>>24)-1;
    kind=(key>>16)&255;
    index=(key&65535u)-1;
    return bank<2&&index<1000;
}
static ReplaySquad148 squad(uintptr_t owner,uintptr_t env,uintptr_t subject){
    if(!subject)return {};
    for(unsigned bank=0;bank<2;++bank){
        const auto offset=registry_banks[bank];
        const auto count=field<unsigned>(owner,offset+0xc);
        const auto slots=field<uintptr_t>(owner,offset+0x10);
        if(count>1000||(!slots&&count))return {};
        for(unsigned index=0;index<count;++index){
            if(field<uintptr_t>(slots,index*sizeof(uintptr_t))!=subject)continue;
            if(!squad_owner130(subject,owner,env))return {};
            const auto kind=field<unsigned>(subject,8);
            const auto size=field<unsigned>(subject,0x18);
            const auto members=field<uintptr_t>(subject,0x1c);
            if(kind>255||size>1000||(!members&&size))return {};
            auto proof=hash(hash(seed,kind),size);
            for(unsigned i=0;i<size;++i){
                const auto member=field<uintptr_t>(members,i*sizeof(uintptr_t));
                if(!member)return {};
                proof=hash(proof,field<unsigned>(member,0x1c8));
            }
            return {slot_key(bank,index,kind),nonzero(proof)};
        }
    }
    return {};
}
static uintptr_t resolve_squad(uintptr_t owner,uintptr_t env,uint32_t key){
    unsigned bank{},index{},kind{};
    if(!read_key(key,bank,index,kind))return 0;
    const auto offset=registry_banks[bank];
    const auto count=field<unsigned>(owner,offset+0xc);
    if(count>1000||index>=count)return 0;
    const auto slots=field<uintptr_t>(owner,offset+0x10);
    const auto candidate=slots?field<uintptr_t>(slots,index*sizeof(uintptr_t)):0;
    return squad_owner130(candidate,owner,env)&&field<unsigned>(candidate,8)==kind?candidate:0;
}
static uintptr_t find_soldier(uintptr_t owner,uintptr_t env,uint32_t id){
    for(unsigned offset:{0x384u,0x2bcu}){
        const auto count=field<unsigned>(owner,offset);
        const auto roster=field<uintptr_t>(owner,offset+4);
        if(count>2000||(!roster&&count))return 0;
        for(unsigned i=0;i<count;++i){
            const auto actor=field<uintptr_t>(roster,i*sizeof(uintptr_t));
            if(actor&&field<uint32_t>(actor,0x1c8)==id&&replay147::infantry(actor,owner,env))return actor;
        }
    }
    return 0;
}
struct Detached {
    uintptr_t actor{},unit{},env{};
    LONG epoch{};
};
static SRWLOCK detached_lock=SRWLOCK_INIT;
static Detached detached[64];
static volatile LONG detached_epoch;

static bool update_detached(uintptr_t actor,uintptr_t owner,uintptr_t env,bool entering){
    AcquireSRWLockExclusive(&detached_lock);
    const LONG epoch=replay147::playback_epoch;
    if(detached_epoch!=epoch){
        memset(detached,0,sizeof detached);
        InterlockedExchange(&detached_epoch,epoch);
    }
    Detached* target=nullptr;
    for(auto& row:detached){
        if(row.actor==actor&&row.unit==owner&&row.env==env){target=&row;break;}
        if(entering&&!target&&!row.actor)target=&row;
    }
    if(target)*target=entering?Detached{actor,owner,env,epoch}:Detached{};
    ReleaseSRWLockExclusive(&detached_lock);
    return target!=nullptr;
}

static bool create_squad(uintptr_t owner,uintptr_t env,const Event& event){
    unsigned bank{},index{},kind{};
    if(!proximity_capable147(owner,env)||!read_key(event.target,bank,index,kind)||
       kind!=event.facing||bank!=(kind==2?0u:1u)||
       index!=field<unsigned>(owner,registry_banks[bank]+0xc))return false;
    uintptr_t created{};
    if(kind==2){
        using Factory=uintptr_t(__thiscall*)(void*,unsigned,uintptr_t,unsigned,unsigned);
        created=reinterpret_cast<Factory>(base+0x1668e0)(reinterpret_cast<void*>(owner+0xb64),2,owner,0,0);
    }else{
        using Policy=unsigned(__thiscall*)(void*);
        using Factory=uintptr_t(__thiscall*)(void*,unsigned);
        const auto policy=reinterpret_cast<Policy>(base+0x1672b0)(reinterpret_cast<void*>(owner));
        if(policy!=event.source)return false;
        created=reinterpret_cast<Factory>(base+0x166d60)(reinterpret_cast<void*>(owner),policy);
    }
    const auto found=squad(owner,env,created);
    return found.key==event.target&&found.proof==event.proof;
}
static bool apply_formation(uintptr_t owner,uintptr_t env,uintptr_t actor,const Event& event){
    const auto current=field<uintptr_t>(actor,0x3b4);
    switch(event.kind){
    case Kind::detach_begin: return update_detached(actor,owner,env,true);
    case Kind::detach_end: return update_detached(actor,owner,env,false);
    case Kind::create_squad: return create_squad(owner,env,event);
    case Kind::cancel_formation_task:
        if(!formation_task130(actor))return false;
        cancel_formation_task130(actor);
        return true;
    default:break;
    }
    const auto target=resolve_squad(owner,env,event.target);
    if(!target)return false;
    const auto destination=squad(owner,env,target);
    if(event.kind==Kind::transfer_member){
        const auto before=squad(owner,env,current);
        if((current&&!before.key)||before.key!=event.source||
           membership_proof148(before,destination)!=event.proof)return false;
        original_member_transfer130(reinterpret_cast<void*>(target),actor,event.facing);
        return field<uintptr_t>(actor,0x3b4)==target&&squad_matches130(target,actor)==1;
    }
    if(event.kind==Kind::reform_unit){
        ReformDestination70 order{};
        return prepare_reform70(owner,target,order)&&reform_proof148(order)==event.proof&&
               apply_reform70(env,owner,order)==(event.facing!=0);
    }
    if(destination.proof!=event.proof)return false;
    switch(event.kind){
    case Kind::retire_squad:{
        if(field<uintptr_t>(owner,0xb9c)==target||field<unsigned>(target,0x18))return false;
        using Retire=bool(__thiscall*)(void*,uintptr_t);
        return reinterpret_cast<Retire>(base+0x169130)(reinterpret_cast<void*>(owner+0xb64),target);
    }
    case Kind::insert_member:
        if(current||squad_matches130(target,actor)!=0)return false;
        original_member_insert130(reinterpret_cast<void*>(target),actor,event.facing);
        return field<uintptr_t>(actor,0x3b4)==target&&squad_matches130(target,actor)==1;
    case Kind::remove_member:
        if(current!=target||squad_matches130(target,actor)!=1)return false;
        reinterpret_cast<MemberRemove130>(base+0x1a80a0)(reinterpret_cast<void*>(target),actor);
        return !field<uintptr_t>(actor,0x3b4)&&squad_matches130(target,actor)==0;
    case Kind::reselect_main:{
        const auto old=resolve_squad(owner,env,event.source);
        if(!old||field<uintptr_t>(owner,0xb9c)!=old)return false;
        reinterpret_cast<UnitTick130>(base+0x16b180)(reinterpret_cast<void*>(owner));
        return field<uintptr_t>(owner,0xb9c)==target;
    }
    case Kind::officer_slot:
        if(field<unsigned char>(actor,0x665)!=1)return false;
        reissue_officer_slot68(actor,target,owner,env);
        return field<unsigned char>(actor,0x665)==0;
    default:return false;
    }
}
}

static ReplaySquad148 capture_squad148(uintptr_t native_unit,uintptr_t env,uintptr_t squad) noexcept {
    if(!replay147::unit_scope.occurrence||replay147::unit_scope.unit!=native_unit)return {};
    __try{return replay148::squad(native_unit,env,squad);}
    __except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
static uint32_t membership_proof148(ReplaySquad148 before,ReplaySquad148 target) noexcept {
    return replay148::nonzero(replay148::hash(before.proof,target.proof));
}
static void record_formation148(uintptr_t actor,exactreplay147::Kind kind,ReplaySquad148 target,
                                uint32_t source,uint16_t argument) noexcept {
    const auto scope=replay147::unit_scope;
    if(!scope.occurrence)return;
    exactreplay147::Event event{};
    event.kind=kind;
    event.boundary=scope.boundary;
    event.occurrence=scope.occurrence;
    event.target=target.key;
    event.source=source;
    event.proof=target.proof;
    event.facing=argument;
    replay147::record_action(actor,event,scope.unit);
}
static uint16_t replay_clock_unit148(uintptr_t native_unit) noexcept {
    return replay147::clock_unit(native_unit);
}
static void replay_unit_scope148(uintptr_t native_unit,bool after,uint16_t occurrence) noexcept {
    using exactreplay147::Boundary;
    replay147::unit_scope={native_unit,after?Boundary::after_unit:Boundary::before_unit,occurrence};
}
static void replay_apply_unit148(uintptr_t native_unit,bool after,uint16_t occurrence) noexcept {
    using namespace exactreplay147;
    if(!occurrence||!replay147::playing_tick)return;
    __try{
        const auto env=field<uintptr_t>(native_unit,0x14);
        if(!env||!field<unsigned char>(env,0x28254)||field<uintptr_t>(env,0x28248))return;
        const auto tick=field<uint32_t>(env,0x282bc), owner=field<uint32_t>(native_unit,4);
        const auto boundary=after?Boundary::after_unit:Boundary::before_unit;
        Event event;
        while(replay147::take(env,tick,owner,0,Stream::formation,boundary,occurrence,event)){
            auto actor=replay148::find_soldier(native_unit,env,event.actor);
            if(actor&&replay148::apply_formation(native_unit,env,actor,event))continue;
            replay147::fail_playback();
            break;
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){replay147::fail_playback();}
}
static bool replay_detached148(uintptr_t actor,uintptr_t* owner,uintptr_t* env) noexcept {
    const LONG epoch=replay147::playback_epoch;
    if(!epoch||!replay147::playback_session||replay148::detached_epoch!=epoch||!actor)return false;
    AcquireSRWLockShared(&replay148::detached_lock);
    replay148::Detached found{};
    for(const auto& row:replay148::detached)
        if(row.actor==actor&&row.epoch==epoch){found=row;break;}
    ReleaseSRWLockShared(&replay148::detached_lock);
    __try{
        if(!found.actor||found.env!=replay147::playback_session||
           !field<unsigned char>(found.env,0x28254)||!replay147::infantry(actor,found.unit,found.env))return false;
        if(owner)*owner=found.unit;
        if(env)*env=found.env;
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}

