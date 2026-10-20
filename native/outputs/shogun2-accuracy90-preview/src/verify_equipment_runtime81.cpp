#include "adapter.cpp"
static uintptr_t fixture_projectile81;static bool fixture_kneel81=true;
static unsigned fixture_changes81,fixture_mode81;
static void* __fastcall fixture_projectile_get81(void*,void*){return reinterpret_cast<void*>(fixture_projectile81);}
static bool __fastcall fixture_ability81(void*,void*,unsigned id){return id==57&&fixture_kneel81;}
static void __fastcall fixture_stop81(void*,void*){}
static void __fastcall fixture_change81(void* actor,void*,unsigned mode,unsigned,unsigned){++fixture_changes81;fixture_mode81=mode;field<unsigned>(reinterpret_cast<uintptr_t>(actor),0x1f0)=mode;}
static void jump81(unsigned char* destination,const void* target){destination[0]=0xe9;auto relative=uint32_t(reinterpret_cast<uintptr_t>(target)-reinterpret_cast<uintptr_t>(destination)-5);memcpy(destination+1,&relative,4);}
int main(){void* memory=nullptr;try{
    memory=VirtualAlloc(nullptr,0x1d88000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);require(memory!=nullptr,"Fixture allocation failed");base=reinterpret_cast<uintptr_t>(memory);
    jump81(reinterpret_cast<unsigned char*>(base+0x14ad40),&fixture_projectile_get81);
    jump81(reinterpret_cast<unsigned char*>(base+0x15c530),&fixture_ability81);
    jump81(reinterpret_cast<unsigned char*>(base+0x3ca920),&fixture_stop81);
    std::vector<unsigned char> actor_bytes(0x800),unit_bytes(0x1100),projectile_bytes(0x200),database_bytes(0x30),clip_bytes(8),asset_bytes(16);
    std::vector<uintptr_t> clips(0x4a3);possessed=reinterpret_cast<uintptr_t>(actor_bytes.data());unit=reinterpret_cast<uintptr_t>(unit_bytes.data());fixture_projectile81=reinterpret_cast<uintptr_t>(projectile_bytes.data());
    Status fixture{};state=&fixture;state->mode=POSSESSED;state->lease_generation=42;state->actor_id=923;state->unit_id=9;
    possession_lease.active=true;possession_lease.generation=42;possession_lease.identity.actor=possessed;possession_lease.identity.actor_id=923;possession_lease.identity.unit=unit;possession_lease.identity.unit_id=9;
    field<unsigned>(possessed,0x1c8)=923;field<unsigned char>(possessed,0xf0)=1;field<unsigned>(possessed,0x1f0)=8;field<unsigned>(unit,4)=9;
    field<float>(base,0x15b2448)=.04f;field<unsigned>(unit,0xf7c)=10;
    field<unsigned>(fixture_projectile81,0)=60;field<unsigned>(fixture_projectile81,0x60)=100;field<float>(fixture_projectile81,0x6c)=200;
    auto db=reinterpret_cast<uintptr_t>(database_bytes.data());field<uintptr_t>(possessed,0x23c)=db;field<uintptr_t>(db,0xc)=reinterpret_cast<uintptr_t>(clips.data());
    // Native valid_weapon_clip checks both index and a non-null asset.
    std::vector<unsigned char> records(4*8);unsigned ids[]={106,127,132,138};
    for(unsigned i=0;i<4;++i){auto r=reinterpret_cast<uintptr_t>(records.data()+i*8);field<unsigned>(r,0)=ids[i];field<uintptr_t>(r,4)=reinterpret_cast<uintptr_t>(asset_bytes.data());clips[ids[i]]=r;}
    original_change=reinterpret_cast<ChangeState>(&fixture_change81);
    equipment_requests81={42,0,0,true};require(equipment_prepare_motion81()&&equipment_crouch_supported81&&fixture_mode81==9,"Ctrl failed to select native kneeling posture");
    unsigned changes=fixture_changes81;state->weapon_phase=WEAPON_DRAW;equipment_requests81.ctrl=false;
    require(equipment_prepare_motion81()&&fixture_changes81==changes,"Ctrl release interrupted committed shot");
    state->weapon_phase=0;require(!equipment_prepare_motion81()&&fixture_mode81==8,"Ctrl release failed to stand");
    field<unsigned>(possessed,0x1f0)=9;require(!equipment_prepare_motion81()&&fixture_mode81==8,"Native unit kneeling left personal walking in a kneeling pose");
    fixture_kneel81=false;equipment_requests81.ctrl=true;changes=fixture_changes81;
    require(!equipment_prepare_motion81()&&!equipment_crouch_supported81&&fixture_changes81==changes,"Locked kneeling ability granted");fixture_kneel81=true;
    auto reload=clips[138];clips[138]=0;require(!equipment_prepare_motion81()&&!equipment_crouch_supported81,"Missing kneel reload accepted");clips[138]=reload;
    equipment_requests81.lease=41;require(!equipment_prepare_motion81(),"Old lease Ctrl request accepted");equipment_requests81.lease=42;
    melee_handoff78.observe(possessed,42,923,9,7);changes=fixture_changes81;
    require(!equipment_prepare_motion81()&&fixture_changes81==changes,"Crouching interrupted melee handoff");melee_handoff78.reset();
    require(equipment_prepare_motion81(),"Crouching did not resume at safe boundary");
    equipment_requests81.slot=equipment81::Melee;field<unsigned char>(unit,0xead)=1;equipment_mode_poll131(equipment_requests81,true);require(!equipment_prepare_motion81()&&equipment_slot81==equipment81::Melee&&fixture_mode81==10,"Melee switching failed to stand and equip native melee pose");
    equipment_requests81.ctrl=false;equipment_requests81.slot=equipment81::Ranged;field<unsigned char>(unit,0xead)=0;equipment_mode_poll131(equipment_requests81,true);require(!equipment_prepare_motion81()&&equipment_slot81==equipment81::Ranged&&fixture_mode81==8,"Ranged switching failed");
    equipment_requests81.slot=0;field<unsigned char>(unit,0xead)=1;
    require(!equipment_prepare_motion81()&&equipment_slot81==equipment81::Melee&&fixture_mode81==10,"Actual native melee mode without private request was ignored");
    field<unsigned char>(unit,0xead)=0;
    require(!equipment_prepare_motion81()&&equipment_slot81==equipment81::Ranged&&fixture_mode81==8,"Actual native ranged mode without private request was ignored");
    possession_lease.identity.mount=1;equipment_requests81.ctrl=true;changes=fixture_changes81;require(!equipment_prepare_motion81()&&!equipment_crouch_supported81&&fixture_changes81==changes,"Mounted soldier crouched");possession_lease.identity.mount=0;
    equipment_reset81();require(!equipment_crouch81&&!equipment_crouch_supported81&&!equipment_lease81&&!equipment_requests81.ctrl&&!equipment_requests81.slot&&!equipment_requests81.tabs,"Release retained equipment requests");
    VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: production equipment boundary selects actual native kneel/stand/melee modes; shot/handoff/old lease/unlocked ability/missing clip/mounted guards; release clears state. Synthetic objects only, game untouched.\n";return 0;
}catch(const std::exception& e){if(memory)VirtualFree(memory,0,MEM_RELEASE);std::cerr<<e.what()<<'\n';return 1;}}
