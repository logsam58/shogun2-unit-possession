// Original native group constructor and boolean serializer in a private mapped
// engine, with explicit allocator/primitive writer/window providers from100.
#define main retained_orders131_main
#include "verify_native_commands100.cpp"
#undef main
static bool toggle_fault134(){__try{equipment_toggle134();return false;}__except(EXCEPTION_EXECUTE_HANDLER){return true;}}
static bool eligible131=true,routing131=false;
static unsigned descriptor134[5]{};
static void* __cdecl descriptor_lookup134(unsigned key){require(key==0x131131u,"Stock UI used another command descriptor");return descriptor134;}
static void native_ui_fixture134(NativeFixture100& f){
    field<uintptr_t>(NativeFixture100::ptr(f.root),0x5b4)=NativeFixture100::ptr(f.owner);
    field<uintptr_t>(NativeFixture100::ptr(f.root),0x5b8)=environment;
    field<unsigned char>(NativeFixture100::ptr(f.root),0x598)=1;
    DWORD old{};require(VirtualProtect(reinterpret_cast<void*>(base+0x207490),16,PAGE_EXECUTE_READWRITE,&old)!=0,"Fixture descriptor patch");
    f.jump(0x207490,reinterpret_cast<void*>(&descriptor_lookup134));VirtualProtect(reinterpret_cast<void*>(base+0x207490),16,old,&old);
}
static bool __fastcall eligibility131(void*,void*){return eligible131;}
static bool __fastcall routing131_provider(void*,void*){return routing131;}
int main(){SetUnhandledExceptionFilter(&fault100);try{
    NativeFixture100 f;using P=NativeFixture100;native_ui_fixture134(f);
    unsigned char descriptors131[7*28]{};memcpy(descriptors131,f.descriptors,sizeof f.descriptors);
    field<unsigned>(base,0x192e770)=0x131131u;field<unsigned>(reinterpret_cast<uintptr_t>(descriptors131),6*28)=0x131131u;
    field<uintptr_t>(base,0x18d9000)=reinterpret_cast<uintptr_t>(descriptors131);field<unsigned>(base,0x18d9004)=7;
    equipment_ranged81=1;state->heartbeat=GetTickCount64();
    for(auto patch:{std::make_pair(0x15c4c0u,reinterpret_cast<void*>(&eligibility131)),std::make_pair(0x1821e0u,reinterpret_cast<void*>(&routing131_provider))}){
        DWORD old{};require(VirtualProtect(reinterpret_cast<void*>(base+patch.first),16,PAGE_EXECUTE_READWRITE,&old)!=0,"Fixture eligibility patch");
        f.jump(patch.first,patch.second);VirtualProtect(reinterpret_cast<void*>(base+patch.first),16,old,&old);}
    using NativeMode131=bool(__thiscall*)(void*,unsigned,unsigned);auto native_mode=reinterpret_cast<NativeMode131>(base+0x1861e0);
    for(unsigned slot:{unsigned(equipment81::Melee),unsigned(equipment81::Ranged)}){
        f.clear_queue();unsigned before{},after{};auto actor_before=f.actor;auto unit_before=f.actor_unit;
        __asm mov before,esp
        require(equipment_mode_action134(slot),"Native whole-unit mode packet refused");
        __asm mov after,esp
        require(before==after&&packets100.size()==1&&packets100[0].key==0x131131u&&packets100[0].byte_count==1&&packets100[0].bytes[0]==unsigned(slot==equipment81::Melee),"Melee descriptor/boolean/ABI differs");
        require(f.actor==actor_before&&f.actor_unit==unit_before,"Queuing mode mutated native unit or player directly");
        EquipmentRequests81 request{state->lease_generation,slot};equipment_requests81=request;equipment_mode_reset131();f.clear_queue();
        field<unsigned char>(unit,0xead)=slot==equipment81::Melee?0:1;
        equipment_mode_poll131(request,true);require(packets100.size()==1&&!equipment_mode_approved131(request.lease,slot),"Personal mode approved before native acceptance");
        equipment_mode_poll131(request,true);require(packets100.size()==1,"Pending native mode was spammed");
        require(native_mode(reinterpret_cast<void*>(unit),0,slot==equipment81::Melee?1:0),"Original native mode executor refused eligible unit");
        equipment_mode_poll131(request,true);require(equipment_mode_approved131(request.lease,slot),"Native accepted mode failed to acknowledge");
        equipment_mode_reset131();require(!equipment_mode_approved131(request.lease,slot),"Exit retained mode approval");
    }
    // The original action handler works with stock RTS input/widgets hidden.
    // Its internal native selection/replay restrictions remain authoritative.
    f.clear_queue();field<unsigned char>(P::ptr(f.root),0x598)=0;
    require(equipment_mode_action134(equipment81::Melee)&&packets100.size()==1,"Hidden RTS input blocked the rebound native action");
    // Native unit command must work during a personal bow draw. The old
    // implementation blocked at Boundary::shot before ever issuing the order.
    equipment_mode_reset131();f.clear_queue();field<unsigned char>(unit,0xead)=0;
    equipment_generation81=state->lease_generation;state->window_thread=GetCurrentThreadId();
    state->weapon_phase=WEAPON_DRAW;equipment_requests81={state->lease_generation,0,0,false,true};
    process_equipment_commands81(P::ptr(f.camera));
    require(packets100.size()==1&&packets100[0].bytes[0]==1,"Native melee toggle was blocked by the personal draw");
    require(!equipment_requests81.toggle&&equipment_requests81.slot==equipment81::Melee,"Toggle did not become native melee request");
    require(native_mode(reinterpret_cast<void*>(unit),0,1),"Native draw-time unit mode rejected");
    process_equipment_commands81(P::ptr(f.camera));
    require(equipment_mode_approved131(state->lease_generation,equipment81::Melee)&&equipment_unit_melee134&&!equipment_ranged_selected81(),"Accepted native melee failed to lower personal ranged readiness");
    state->weapon_phase=0;
    // Melee-only Katana-style unit and unavailable requests must be true no-ops,
    // including at a busy personal boundary.2 is no longer a weapon binding.
    equipment_ranged81=0;equipment_requests81={state->lease_generation,2,0,false,true};
    auto ignored=equipment_toggle134();require(!ignored.slot&&!ignored.toggle,"Melee-only request remained pending");
    auto packet_count=packets100.size();equipment_mode_poll131({state->lease_generation,2},false);
    require(!equipment_requests81.slot&&packets100.size()==packet_count,"Unavailable mode queued a native action");
    EquipmentRequests81 keys{};equipment_keys134(keys,3,false);require(!keys.slot&&!keys.toggle,"Katana received a weapon switch");
    equipment_keys134(keys,2,true);require(!keys.slot&&!keys.toggle,"2 retained a redundant weapon binding");
    equipment_keys134(keys,1,true);require(keys.toggle,"1 missed native toggle");equipment_keys134(keys,1,true);require(!keys.toggle,"Paired pending toggle edges accumulated");
    equipment_ranged81=1;equipment_mode_reset131();
    auto real_unit=unit;unit=1;bool faulted=toggle_fault134();unit=real_unit;
    require(faulted&&TryAcquireSRWLockExclusive(&equipment_input_lock81),"Failed native read retained the input lock");ReleaseSRWLockExclusive(&equipment_input_lock81);
    eligible131=false;field<unsigned char>(unit,0xead)=1;
    require(!native_mode(reinterpret_cast<void*>(unit),0,0)&&field<unsigned char>(unit,0xead)==1,"Native ranged eligibility restriction bypassed");eligible131=true;
    routing131=true;require(!native_mode(reinterpret_cast<void*>(unit),0,0)&&field<unsigned char>(unit,0xead)==1,"Native routing restriction bypassed");routing131=false;
    f.clear_queue();equipment_mode_reset131();field<unsigned char>(unit,0xead)=0;
    EquipmentRequests81 reversal{state->lease_generation,equipment81::Melee};equipment_requests81=reversal;equipment_mode_poll131(reversal,true);
    reversal.slot=equipment81::Ranged;equipment_requests81=reversal;equipment_mode_poll131(reversal,true);
    require(packets100.size()==1&&!equipment_mode_approved131(reversal.lease,reversal.slot),"Rapid reversal mistook pre-order unit state for acceptance");
    require(native_mode(reinterpret_cast<void*>(unit),0,1),"Native first mode rejected");equipment_mode_poll131(reversal,true);
    require(packets100.size()==2&&!equipment_mode_approved131(reversal.lease,reversal.slot),"Coalesced mode did not wait for final native order");
    require(native_mode(reinterpret_cast<void*>(unit),0,0),"Native final mode rejected");equipment_mode_poll131(reversal,true);
    require(equipment_mode_approved131(reversal.lease,reversal.slot),"Rapid reversal never reached final requested mode");equipment_mode_reset131();
    f.clear_queue();auto used=field<unsigned>(P::ptr(f.queue)+4,QUEUE_CAPACITY)=QUEUE_CAPACITY-1;
    require(!equipment_mode_action134(equipment81::Melee)&&field<unsigned>(P::ptr(f.queue)+4,QUEUE_CAPACITY)==used,"Full queue accepted a partial mode order");
    f.clear_queue();++field<unsigned>(possessed,0x1c8);require(!equipment_mode_action134(equipment81::Melee)&&packets100.empty(),"Reused player identity issued a unit order");--field<unsigned>(possessed,0x1c8);
    EquipmentRequests81 request{state->lease_generation,equipment81::Melee};equipment_requests81=request;field<unsigned char>(unit,0xead)=0;
    equipment_mode_poll131(request,false);require(packets100.empty(),"Busy animation issued weapon order");
    equipment_mode_poll131(request,true);unit_weapon131.deadline=GetTickCount64()-1;
    equipment_mode_poll131(request,true);require(equipment_error81==9&&!equipment_requests81.slot&&!equipment_mode_approved131(request.lease,request.slot),"Native rejection failed to retire request");
    std::cout<<"PASS: native constructor/boolean serializer and exact melee descriptor; deferred native acceptance, no repeated orders, full queue, stale identity, busy and rejection timeout. Original stock UI action wrapper/controller/selection predicates/order producer/boolean serializer executed; descriptor lookup and primitive writers are providers. Unit scheduler/animations still need live validation.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
