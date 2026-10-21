#pragma once
#include "../../shogun2-possession-cleanup-v46/src/actor_types152.h"
// Read-only proof of a CURRENT native formation charge.
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
    uint64_t searched{};bool active{};
};
static volatile LONG charge_reset_request139;
static volatile LONG charge_starts139,charge_checks139,charge_errors139;
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
    if(!actor152::human(base,field<uintptr_t>(owner,0))||field<unsigned>(owner,0x1c8)!=c.owner_serial||
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
      if(!target||owner==target||(actor152::human(base,field<uintptr_t>(owner,0))&&field<uintptr_t>(owner,0x210)==target)){
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
    if(!rider||rider==possessed||!actor152::human(base,field<uintptr_t>(rider,0))||
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

#include "native_charge146.h"
