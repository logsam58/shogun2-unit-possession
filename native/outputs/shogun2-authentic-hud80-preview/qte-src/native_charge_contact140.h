#pragma once
#include "../../shogun2-possession-cleanup-v46/src/charge_protocol146.h"
// Included at the runtime's native identity boundary. Only a fresh owned
// physical charge may retain its ORIGINAL encounter instead of acquiring a QTE.
// One bounded lifetime record; no new selector roll, outcome, clip or damage.
struct StockCharge140 {
    uint32_t address{},table{},environment{};uint64_t generation{},lease{},serial{};
    NativeActor player{};bool registered{};
};
static StockCharge140 stock_charge140;
static __declspec(thread) uint32_t stock_charge_scope140;
static volatile LONG stock_charge_prepares140,stock_charge_updates140;
static HANDLE charge_read_map146;
static const charge146::Shared* charge_read146;
static uint64_t charge_read_retry146;
static bool stock_charge_owner146(const NativeActor& actor,uint64_t lease){
    auto now=GetTickCount64();
    if(!charge_read146&&now>=charge_read_retry146){
        charge_read_retry146=now+250;wchar_t name[96];charge146::name(name,GetCurrentProcessId());
        charge_read_map146=OpenFileMappingW(FILE_MAP_READ,FALSE,name);
        if(charge_read_map146){charge_read146=static_cast<const charge146::Shared*>(MapViewOfFile(charge_read_map146,FILE_MAP_READ,0,0,sizeof(charge146::Shared)));if(!charge_read146){CloseHandle(charge_read_map146);charge_read_map146=nullptr;}}
    }
    charge146::Value v;observation::PossessionPrefix p{};
    return possession_snapshot93(p)&&charge146::read(charge_read146,v)&&
        charge146::matches(v,GetCurrentProcessId(),uint32_t(lease),p.actor_id,p.unit_id,actor.serial,unsigned(actor.kind),now);
}
static __declspec(thread) bool stock_charge_other146;
static bool stock_charge_task140(const Reader& r,uint32_t actor){
    uint32_t table{},parent{};uint8_t present{},done{};
    return r.get(actor+0x2e8,present)&&present==1&&
        r.get(actor+0x250,table)&&table==engine+0x15e0a5c&&
        r.get(actor+0x254,parent)&&parent==actor&&r.get(actor+0x258,done)&&done<=1;
    // Native3eba00 can mark the task done AFTER queueing contact but BEFORE
    // the encounter manager prepares it. Its still-current charge task/pose
    // remains evidence; task replacement or cleared charge flags ends it.
}
static bool stock_charge_jump140(const Reader& r,const NativeActor& actor){
    uint32_t task{},table{},parent{};uint8_t present{},done{};
    return actor.kind==Horse&&r.get(actor.address+0x248,task)&&task==16&&
        r.get(actor.address+0x2e8,present)&&present==1&&r.get(actor.address+0x250,table)&&table==engine+0x15e0cec&&
        r.get(actor.address+0x254,parent)&&parent==actor.address&&r.get(actor.address+0x258,done)&&!done;
}
static void stock_charge_clear140(){
    if(stock_charge140.serial&&commitment.serial==stock_charge140.serial)clear_commitment();
    stock_charge140={};
}
static bool stock_charge_current140(uint32_t address,uint64_t generation){
    if(!address||stock_charge140.address!=address||stock_charge140.generation!=generation||!generation)return false;
    uint64_t lease{};auto actor=fps_actor(&lease);auto r=reader();NativeActor current;
    uint32_t table{},environment{};
    // Prove the CURRENT camera/reciprocal mount first, never a saved actor.
    return lease==stock_charge140.lease&&actor==stock_charge140.player.address&&
        combat_identity(r,actor,engine,current,false)&&same_actor(current,stock_charge140.player)&&
        lifetimes.matches140(address,generation)&&r.get(address,table)&&table==stock_charge140.table&&
        r.get(address+4,environment)&&environment==stock_charge140.environment&&
        r.get(actor+0x28,environment)&&environment==stock_charge140.environment;
}
static bool stock_charge_pose140(){
    const auto& s=stock_charge140;auto r=reader();uint32_t count{},one{},two{},link{};uint8_t active{},playing{};
    if(!r.get(s.address+0xc,active)||active!=1||!r.get(s.address+0x38,count)||count!=2||
       !r.get(s.address+0x30,one)||!r.get(s.address+0x34,two)||
       (one!=s.player.address&&two!=s.player.address)||one==two||
       !r.get(s.address+0xd1,playing)||playing>1)return false;
    NativeActor p,e;auto other=one==s.player.address?two:one;
    if(!combat_identity(r,s.player.address,engine,p,false)||!same_actor(p,s.player)||
       !combat_identity(r,other,engine,e,false)||p.unit==e.unit||p.side==e.side||
       !r.get(p.address+0x688,link)||link!=s.address||!r.get(e.address+0x688,link)||link!=s.address)return false;
    for(auto participant:{p,e}){
        uint32_t environment{};NativeActor rider;
        if(!r.get(participant.address+0x28,environment)||environment!=s.environment||
           (participant.kind==Horse&&!horse_rider(r,engine,participant,rider,false)))return false;
    }
    melee60::Request next;next.pid=GetCurrentProcessId();next.lease=uint32_t(s.lease);next.serial=s.serial;
    next.player=p.serial;next.player_unit=p.unit_serial;next.player_kind=p.kind;
    next.opponent=e.serial;next.opponent_unit=e.unit_serial;next.opponent_kind=e.kind;
    next.phase=playing?melee60::NativeCombat:melee60::Aligning;
    if(!refresh_committed_pose(p,next.player_position)||!refresh_committed_pose(e,next.opponent_position))return false;
    memcpy(next.player_anchor,next.player_position,sizeof next.player_anchor);
    memcpy(next.opponent_anchor,next.opponent_position,sizeof next.opponent_anchor);
    // Same existing adapter handoff, with no Facing/prompt hold. Its original
    // native targets guide alignment; authored root motion then owns playback.
    commitment=next;committed_player=p;committed_enemy=e;stock_charge140.registered=true;return true;
}
static void stock_charge_heartbeat140(){
    if(!stock_charge140.address)return;
    if(!stock_charge_current140(stock_charge140.address,stock_charge140.generation)){stock_charge_clear140();return;}
    if(!stock_charge_pose140()&&commitment.serial==stock_charge140.serial)clear_commitment();
}
static void stock_charge_removed140(uint32_t address,uint64_t generation){
    if(stock_charge140.address==address&&stock_charge140.generation==generation)stock_charge_clear140();
}
static bool stock_charge_target_scope140(uint32_t address){
    if(stock_charge_scope140!=address||!owns_coordinator)return false;
    if(stock_charge_other146){uint64_t lease{};auto actor=owned_combat_actor(address,&lease);NativeActor identity;return actor&&combat_identity(reader(),actor,engine,identity)&&stock_charge_owner146(identity,lease);}
    return stock_charge_current140(address,stock_charge140.generation);
}
static bool stock_charge_dispatch140(uint32_t address,uint64_t generation,bool updating,uint32_t& answer){
    if(stock_charge140.address&&!stock_charge_current140(stock_charge140.address,stock_charge140.generation))stock_charge_clear140();
    if(stock_charge140.address&&stock_charge140.address!=address){
        // Do not start a QTE that steals this player's still-registered charge.
        if(stock_charge140.registered){
            uint64_t lease{};auto actor=owned_combat_actor(address,&lease);NativeActor identity;
            if(actor&&combat_identity(reader(),actor,engine,identity)&&stock_charge_owner146(identity,lease)){
                auto old_scope=stock_charge_scope140;bool old_other=stock_charge_other146;stock_charge_scope140=address;stock_charge_other146=true;
                __try{answer=updating?native_update(reinterpret_cast<void*>(address)):native_prepare(reinterpret_cast<void*>(address));}
                __finally{stock_charge_scope140=old_scope;stock_charge_other146=old_other;}return true;
            }
            answer=1;return true;
        }
        return false;
    }
    if(!stock_charge140.address){
        if(updating||!generation||session->occupied()||delivery_address)return false;
        uint64_t lease{};auto actor=owned_combat_actor(address,&lease);NativeActor identity;auto r=reader();
        uint32_t table{},environment{},actor_environment{};uint8_t active{},phase{};
        if(!actor||!combat_identity(r,actor,engine,identity)||
           (!stock_charge_task140(r,actor)&&!stock_charge_jump140(r,identity)&&!stock_charge_owner146(identity,lease))||
           !player_encounter(r,actor,address,engine,false)||
           !r.get(address+0xc,active)||active||!r.get(address+0xd0,phase)||phase||
           !r.get(address,table)||!r.get(address+4,environment)||!environment||
           !r.get(actor+0x28,actor_environment)||environment!=actor_environment)return false;
        stock_charge140={address,table,environment,generation,lease,generation|(uint64_t(1)<<63),identity,false};
    }
    if(!stock_charge_current140(address,generation))return false;
    const auto previous=stock_charge_scope140;stock_charge_scope140=address;bool completed=false;
    __try{
        if(updating){InterlockedIncrement(&stock_charge_updates140);answer=native_update(reinterpret_cast<void*>(address));}
        else {InterlockedIncrement(&stock_charge_prepares140);answer=native_prepare(reinterpret_cast<void*>(address));}
        if(answer&0xff)stock_charge_clear140();else stock_charge_heartbeat140();
        if(commitment.serial==stock_charge140.serial&&commitment.phase!=melee60::Off)publish_commitment();
        completed=true;
    }__finally{stock_charge_scope140=previous;if(!completed)stock_charge_clear140();}
    return true;
}
