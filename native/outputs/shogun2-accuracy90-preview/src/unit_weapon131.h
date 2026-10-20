#pragma once
// BCQ_MULTIPLE_SELECTION_ORDER_CHANGE_MELEE_MODE: registration1a3c0,
// native UI18/19 and dispatcher2a8400, executor45d250 -> unit1861e0.
// The native executor owns restrictions and writes unit+ead only on acceptance.
struct UnitWeapon131 {unsigned lease{},slot{},approved{};uint64_t deadline{};};
static UnitWeapon131 unit_weapon131;
static SRWLOCK unit_weapon_lock131=SRWLOCK_INIT;
static void equipment_mode_reset131(){AcquireSRWLockExclusive(&unit_weapon_lock131);unit_weapon131={};ReleaseSRWLockExclusive(&unit_weapon_lock131);}
static UnitWeapon131 equipment_mode_snapshot131(){AcquireSRWLockShared(&unit_weapon_lock131);auto out=unit_weapon131;ReleaseSRWLockShared(&unit_weapon_lock131);return out;}
static bool equipment_mode_approved131(unsigned lease,unsigned slot){auto s=equipment_mode_snapshot131();return s.lease==lease&&s.slot==slot&&s.approved==slot;}
// Native battle UI action:10dfb80 ->10f97d0 cases18/19 ->10d0e50.
// The original handler constructs, serializes and queues the mode order. Do not
// synthesize a replacement packet or write the unit's mode flag directly.
static bool equipment_mode_action134(unsigned slot){
    __try{
        if(!followed(possessed,false)||state->release||state->stop)return false;
        auto root=field<uintptr_t>(base,0x1bc8180),group=selected_possessed_group();
        auto queue=field<uintptr_t>(environment,0x28048);
        if(!root||!group||!queue||field<uintptr_t>(root,0x110)!=environment||
           field<uintptr_t>(root,0x5b8)!=environment||field<uintptr_t>(root,0x5b4)+0xb0!=group)return false;
        auto before=field<unsigned>(queue+4,QUEUE_CAPACITY);
        // One owned unit, no other selection categories: leave room for the
        // native selection packet before entering its ordinary queue writer.
        if(before>QUEUE_CAPACITY-256)return false;
        using NativeAction=void(__thiscall*)(void*,unsigned,float);
        reinterpret_cast<NativeAction>(base+0x10dfb80)(reinterpret_cast<void*>(root),slot==equipment81::Melee?0x18:0x19,0.f);
        auto after=field<unsigned>(queue+4,QUEUE_CAPACITY);
        return after>before&&after<=QUEUE_CAPACITY;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static EquipmentRequests81 equipment_toggle134(){
    // Native reads may fault during retirement; never hold the input lock over one.
    unsigned native_slot=equipment81::Melee;
    if(equipment_ranged81)native_slot=field<unsigned char>(unit,0xead)?equipment81::Melee:equipment81::Ranged;
    AcquireSRWLockExclusive(&equipment_input_lock81);
    if(equipment_requests81.lease==state->lease_generation){
        if(!equipment_ranged81){equipment_requests81.slot=0;equipment_requests81.toggle=false;}
        else if(equipment_requests81.toggle){
            // Reverse an outstanding requested mode, otherwise toggle the real
            // native unit flag. No fixed melee/ranged1/2 UI of our own.
            unsigned current=equipment_requests81.slot?equipment_requests81.slot:
                native_slot;
            equipment_requests81.slot=current==equipment81::Melee?equipment81::Ranged:equipment81::Melee;
            equipment_requests81.toggle=false;
        }
    }
    auto request=equipment_requests81;ReleaseSRWLockExclusive(&equipment_input_lock81);return request;
}
static void equipment_mode_poll131(const EquipmentRequests81& request,bool safe){
    if(request.lease!=state->lease_generation||!request.slot){equipment_mode_reset131();return;}
    if(!equipment_ranged81||!equipment81::valid_slot(request.slot,true)){
        equipment_consume81(request.lease,request.slot,false);equipment_mode_reset131();return;
    }
    if(!safe)return;
    bool melee=field<unsigned char>(unit,0xead)!=0;InterlockedExchange(&equipment_unit_melee134,melee?1:0);
    auto pending=equipment_mode_snapshot131();unsigned generation=state->lease_generation;
    if(pending.lease==generation&&pending.slot!=request.slot&&pending.deadline&&
       melee!=(pending.slot==equipment81::Melee)){
        // Coalesce rapid1/2 changes behind the already-queued native order.
        // Seeing the OLD unit state equal the new request is not an ack: the
        // previous packet could still execute afterward and undo that choice.
        if(GetTickCount64()>pending.deadline){equipment_error81=9;equipment_consume81(request.lease,request.slot,false);equipment_mode_reset131();}
        return;
    }
    if(pending.lease!=generation||pending.slot!=request.slot)pending={generation,request.slot,0,0};
    if(melee==(request.slot==equipment81::Melee))pending.approved=request.slot;
    else if(!pending.deadline){
        if(!equipment_mode_action134(request.slot)){equipment_error81=8;equipment_consume81(request.lease,request.slot,false);equipment_mode_reset131();return;}
        pending.deadline=GetTickCount64()+2000;pending.approved=0;
    }else if(GetTickCount64()>pending.deadline){
        equipment_error81=9;equipment_consume81(request.lease,request.slot,false);equipment_mode_reset131();return;
    }
    if(melee!=(request.slot==equipment81::Melee))pending.approved=0;
    AcquireSRWLockExclusive(&unit_weapon_lock131);unit_weapon131=pending;ReleaseSRWLockExclusive(&unit_weapon_lock131);
}
