#pragma once
// Bridge a CURRENT formation charge into the detached personal native task.
// No invented targets, attack results, animation clips or unit/squad writes.
// The retail constructor owns the task, the retail update owns contact and
// commitment. This header is included after the existing identity/equipment API.
using ChargeOrder139=void(__thiscall*)(void*,const void*);
struct ChargeIntent139 {uintptr_t target{},target_unit{};float x{},z{};unsigned char special{},required{};unsigned char pad[2]{};};
static_assert(sizeof(ChargeIntent139)==20,"Retail charge descriptor ABI");
struct Charge139 {
    unsigned generation{},cursor{},target_serial{},target_unit_serial{},target_index{},roster_offset{},owner_serial{};
    uintptr_t actor{},target{},target_unit{},target_config{},target_owner{},native_target{};
    float forward_x{},forward_z{};
    uint64_t searched{};bool active{},spent{};
};
static Charge139 charge139;
static volatile LONG charge_reset_request139;
static volatile LONG charge_starts139,charge_cancels139,charge_checks139,charge_errors139;
static bool charge_task139(uintptr_t actor){
    return field<unsigned char>(actor,0x2e8)==1&&field<uintptr_t>(actor,0x250)==base+0x15e0a5c&&
        field<uintptr_t>(actor,0x254)==actor&&field<unsigned char>(actor,0x258)==0;
}
static bool charge_target139(const Charge139& c){
    __try {
    if(!c.target||!c.target_unit||c.target_unit==unit||(c.roster_offset!=0x384&&c.roster_offset!=0x2bc))return false;
    // Prove current roster membership BEFORE reading the remembered actor.
    unsigned n=field<unsigned>(c.target_unit,c.roster_offset);auto list=field<uintptr_t>(c.target_unit,c.roster_offset+4);
    if(!n||n>2000||!list||c.target_index>=n||field<uintptr_t>(list,c.target_index*4)!=c.target_owner)return false;
    auto owner=c.target_owner;
    if(field<uintptr_t>(owner,0)!=base+0x15dfec8||field<unsigned>(owner,0x1c8)!=c.owner_serial||
       field<uintptr_t>(owner,0x214)!=c.target_unit||field<uintptr_t>(owner,0x28)!=environment||field<unsigned char>(owner,0xf0)!=1)return false;
    if(c.target!=owner){
        if(field<uintptr_t>(owner,0x210)!=c.target||field<uintptr_t>(c.target,0)!=base+0x15e8524||
           field<unsigned>(c.target,0x6cc)!=1)return false;
        auto links=field<uintptr_t>(c.target,0x6d0);if(!links||field<uintptr_t>(links,0)!=owner)return false;
    }else if(field<uintptr_t>(c.target,0x210))return false;
    if(field<uintptr_t>(c.target_unit,0x14)!=environment||field<unsigned>(c.target_unit,4)!=c.target_unit_serial||
       field<unsigned>(c.target,0x1c8)!=c.target_serial||
       field<uintptr_t>(c.target,0x20c)!=c.target_config||field<uintptr_t>(c.target,0x214)!=c.target_unit||
       field<uintptr_t>(c.target,0x28)!=environment||field<unsigned char>(c.target,0xf0)!=1)return false;
    auto own=field<uintptr_t>(possessed,0x44),other=field<uintptr_t>(c.target,0x44);
    if(!own||!other)return false;
    unsigned a=field<unsigned>(own,0x10),b=field<unsigned>(other,0x10);
    // Native contact/charge logic owns combat eligibility. Being engaged,
    // recovering or routing does not invalidate a live enemy's identity.
    return a<=31&&b<=31&&a!=b;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool charge_bind_target139(Charge139& c,uintptr_t target,uintptr_t target_unit,unsigned* budget=nullptr){
    if(!target_unit||target_unit==unit||field<uintptr_t>(target_unit,0x14)!=environment)return false;
    unsigned off=field<unsigned>(target_unit,0x384)?0x384:0x2bc;
    auto n=field<unsigned>(target_unit,off),list=field<uintptr_t>(target_unit,off+4);
    if(!n||n>2000||!list)return false;
    for(unsigned i=0;i<n;++i){if(budget){if(!*budget)return false;--*budget;}
      __try {auto owner=field<uintptr_t>(list,i*4);if(!owner)continue;
      if(!target||owner==target||(field<uintptr_t>(owner,0)==base+0x15dfec8&&field<uintptr_t>(owner,0x210)==target)){
        auto mount=field<uintptr_t>(owner,0x210);auto subject=target?target:(mount?mount:owner);
        c.target=subject;c.native_target=target;c.target_unit=target_unit;c.target_index=i;c.roster_offset=off;c.target_owner=owner;c.owner_serial=field<unsigned>(owner,0x1c8);
        c.target_serial=field<unsigned>(subject,0x1c8);c.target_unit_serial=field<unsigned>(target_unit,4);
        c.target_config=field<uintptr_t>(subject,0x20c);
        if(charge_target139(c))return true;
        if(target)return false;
      }
      }__except(EXCEPTION_EXECUTE_HANDLER){continue;}
    }
    return false;
}
static bool charge_direction139(uintptr_t actor,Intent movement,const Charge139& c){
    __try {
    float dx=field<float>(c.target,0x48)-field<float>(actor,0x48),dz=field<float>(c.target,0x50)-field<float>(actor,0x50);
    if(!c.native_target){dx=c.forward_x;dz=c.forward_z;}
    float distance=std::hypot(dx,dz),length=std::hypot(movement.x,movement.z);
    // Manual intent must close on the native target/direction. Turning away
    // cancels rather than steering the player or inventing a charge angle.
    return std::isfinite(distance)&&std::isfinite(length)&&distance>.001f&&length>.001f&&
        dx*movement.x+dz*movement.z>0;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool charge_capture139(uintptr_t actor,ChargeIntent139& intent,Charge139& candidate,unsigned* budget=nullptr){
    __try {
    if(!charge_task139(actor)||field<unsigned>(actor,0x244)!=2||field<unsigned>(actor,0x1d0)!=16||
       (field<unsigned char>(actor,0x119)&2)==0)return false;
    auto task=actor+0x250;
    intent={field<uintptr_t>(task,0x1c),field<uintptr_t>(task,0x20),field<float>(task,0x24),field<float>(task,0x28),field<unsigned char>(task,0x2c),field<unsigned char>(task,0x2d),{}};
    if(intent.special>1||intent.required>1||!std::isfinite(intent.x)||!std::isfinite(intent.z)||std::hypot(intent.x,intent.z)<.001f)return false;
    candidate.forward_x=intent.x;candidate.forward_z=intent.z;
    return charge_bind_target139(candidate,intent.target,intent.target_unit,budget)&&charge_task139(actor)&&
        field<uintptr_t>(task,0x1c)==intent.target&&field<uintptr_t>(task,0x20)==intent.target_unit;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool charge_peer139(uintptr_t rider,ChargeIntent139& intent,Charge139& candidate,unsigned* budget=nullptr){
    __try {
    if(!rider||rider==possessed||field<uintptr_t>(rider,0)!=base+0x15dfec8||
       field<uintptr_t>(rider,0x214)!=unit||field<uintptr_t>(rider,0x28)!=environment||
       field<unsigned char>(rider,0xf0)!=1)return false;
    auto actor=rider;
    if(possession_lease.identity.mount){
        actor=field<uintptr_t>(rider,0x210);if(!actor||actor==locomotion_actor()||field<uintptr_t>(actor,0)!=base+0x15e8524||
           field<uintptr_t>(actor,0x214)!=unit||field<uintptr_t>(actor,0x28)!=environment||field<unsigned>(actor,0x6cc)!=1)return false;
        auto links=field<uintptr_t>(actor,0x6d0);if(!links||field<uintptr_t>(links,0)!=rider)return false;
    }else if(field<uintptr_t>(rider,0x210))return false;
    auto squad=field<uintptr_t>(actor,0x3b4);
    if(!root_motion_subject(actor,false)||field<uintptr_t>(actor,0x20c)!=locomotion_config()||
       !squad_owner130(squad,unit,environment)||!charge_task139(actor)||field<unsigned>(actor,0x244)!=2||
       field<unsigned>(actor,0x1d0)!=16||(field<unsigned char>(actor,0x119)&2)==0)return false;
    return charge_capture139(actor,intent,candidate,budget);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Exit can originate on the window thread. Only the motion boundary changes
// the task record; an asynchronous reset never races a partial struct clear.
static void charge_reset139(){InterlockedExchange(&charge_reset_request139,1);}
static void charge_cancel139(uintptr_t actor){
    if(charge139.active&&charge139.actor==actor&&charge139.generation==possession_lease.generation&&
       member_owned130(actor)&&
       charge_task139(actor)&&field<uintptr_t>(actor+0x250,0x1c)==charge139.native_target&&
       !field<uintptr_t>(actor,0x18)&&!field<uintptr_t>(actor,0x688)&&!native_recovery103(actor)){
        // Replace only our still-current task through the original native
        // mode-task constructor (3d8d80 ->3b89f0); no raw task destruction.
        unsigned idle[2]{};
        reinterpret_cast<ChargeOrder139>(base+0x3d8d80)(reinterpret_cast<void*>(actor),idle);
        InterlockedIncrement(&charge_cancels139);
    }
    charge139.active=false;
}
static bool charge_allowed139(uintptr_t actor,unsigned input){
    if(!enabled||!state||state->stop||state->release||state->mode!=POSSESSED||actor!=locomotion_actor()||
       !followed(possessed,false)||!root_motion_subject(actor,false)||native_recovery103(actor)||
       wall_subject97(possessed)||native_airborne_subject99(possessed)||equipment_committed81()||
       state->aiming||state->aim_pose||state->weapon_phase||equipment_crouched81()||
       (input&(FORWARD|RUN))!=(FORWARD|RUN)||(input&BACKWARD)||native_routing(unit)||
       field<unsigned>(actor,0x1f0)>11)return false;
    if(equipment_ranged81&&(equipment_slot81!=equipment81::Melee||!field<unsigned char>(unit,0xead)))return false;
    // Original unit run intent returns bool in AL, NOT a charge enum. The
    // active native personal task on a peer provides charge authority below.
    return reinterpret_cast<UnitRunAllowed>(base+0x18c120)(reinterpret_cast<void*>(unit));
}
static void charge_prepare139(uintptr_t actor,unsigned input,Intent movement){
    __try {
        if(InterlockedExchange(&charge_reset_request139,0)||charge139.generation!=possession_lease.generation||charge139.actor!=actor){
            charge139={};charge139.generation=possession_lease.generation;charge139.actor=actor;
        }
        if(!charge_allowed139(actor,input)){
            charge_cancel139(actor);charge139.spent=false;return;
        }
        if(charge139.active){
            if(!charge_target139(charge139)||!charge_direction139(actor,movement,charge139)||!charge_task139(actor)||
               field<uintptr_t>(actor+0x250,0x1c)!=charge139.native_target||field<uintptr_t>(actor+0x250,0x20)!=charge139.target_unit){charge_cancel139(actor);}
            return;
        }
        if(charge139.spent)return;
        ChargeIntent139 current{};auto existing=charge139;
        if(charge_capture139(actor,current,existing)&&charge_direction139(actor,movement,existing)){
            existing.active=existing.spent=true;charge139=existing;return;
        }
        auto now=GetTickCount64();if(charge139.searched&&now-charge139.searched<100)return;charge139.searched=now;
        auto n=field<unsigned>(unit,0x384),list=field<uintptr_t>(unit,0x388);if(!n||n>2000||!list)return;
        unsigned target_budget=2000;
        for(unsigned i=0;i<std::min(n,32u)&&target_budget;++i){
            auto index=charge139.cursor++%n;auto rider=field<uintptr_t>(list,index*4);ChargeIntent139 intent{};auto candidate=charge139;
            InterlockedIncrement(&charge_checks139);
            if(!charge_peer139(rider,intent,candidate,&target_budget)||!charge_direction139(actor,movement,candidate))continue;
            if(field<unsigned>(unit,0x384)!=n||field<uintptr_t>(unit,0x388)!=list||
               actor!=locomotion_actor()||candidate.generation!=possession_lease.generation||
               InterlockedCompareExchange(&charge_reset_request139,0,0)||!member_owned130(actor)||!charge_target139(candidate))return;
            reinterpret_cast<ChargeOrder139>(base+0x3d8dd0)(reinterpret_cast<void*>(actor),&intent);
            if(!charge_task139(actor))return;
            candidate.active=candidate.spent=true;charge139=candidate;InterlockedIncrement(&charge_starts139);return;
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){InterlockedIncrement(&charge_errors139);charge139.active=false;charge139.spent=true;}
}
static bool charge_rate139(unsigned input,float& speed){
    __try {
        auto actor=locomotion_actor();
        if(!charge139.active||charge139.actor!=actor||charge139.generation!=possession_lease.generation||
           InterlockedCompareExchange(&charge_reset_request139,0,0)||!(input&RUN)||!member_owned130(actor)||
           !charge_task139(actor)||field<unsigned>(actor,0x244)!=2||!charge_target139(charge139))return false;
        // The ORIGINAL constructor includes special-attack adjustments in3c.
        // Gait2 has no walk/run global coefficient (original18b310).
        float raw=field<float>(actor+0x250,0x3c),factor=field<float>(unit,0x22c);
        float value=raw*factor;
        if(!std::isfinite(raw)||raw<0||raw>100||!std::isfinite(value)||value<0||value>100)return false;
        speed=value;return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void charge_log139(std::ostream& log){
    static LONG previous_starts=-1,previous_cancels=-1,previous_errors=-1;
    LONG a=charge_starts139,b=charge_cancels139,e=charge_errors139;
    if(a==previous_starts&&b==previous_cancels&&e==previous_errors)return;
    previous_starts=a;previous_cancels=b;previous_errors=e;
    log<<"{\"event\":\"native_charge139\",\"tick_ms\":"<<GetTickCount64()<<",\"starts\":"<<a
       <<",\"cancels\":"<<b<<",\"peer_checks\":"<<charge_checks139<<",\"errors\":"<<e<<"}\n";
}
