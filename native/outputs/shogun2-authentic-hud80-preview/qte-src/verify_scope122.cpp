// Exercise the production callback, not just the reservation prefilter.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cstdint>
static uint32_t mutate_read122,mutate_write122,mutate_value122;static bool mutated122;
static uint64_t reads122,bytes122;static bool reject_bulk122;
static BOOL WINAPI counted_read122(HANDLE process,LPCVOID address,LPVOID out,SIZE_T size,SIZE_T* got){
    ++reads122;bytes122+=size;
    if(reject_bulk122&&(size==0x50||size==0x218||size==8||size==0x68c)){if(got)*got=0;return FALSE;}
    BOOL result=ReadProcessMemory(process,address,out,size,got);
    if(result&&uint32_t(address)==mutate_read122&&size==8){
        *reinterpret_cast<uint32_t*>(mutate_write122)=mutate_value122;mutate_read122=0;mutated122=true;
    }return result;
}
#define ReadProcessMemory counted_read122
#include "playable_runtime.cpp"
#undef ReadProcessMemory
static unsigned forwards122,selects122;
static uint32_t __fastcall foreign_update122(void*,void*){++forwards122;return 0xcafe006f;}
static void*const* expected_begin122;static void*const* expected_end122;
static NativeVerdict* __fastcall foreign_select122(void*,void*,NativeVerdict* out,void*const* begin,void*const* end,uint32_t a,uint32_t b){
    ++selects122;require(a==7&&b==9,"Selector scalar arguments changed");
    if(expected_begin122)require(begin==expected_begin122&&end==expected_end122,"Selector pointers changed");
    *out={5,0xabc000,0xabc100};return out;
}
static void* prepare_participants122[2];
static uint32_t __fastcall foreign_prepare122(void*,void*){
    ++forwards122;NativeVerdict out{};
    require(select_hook(nullptr,nullptr,&out,prepare_participants122,prepare_participants122+2,7,9)==&out&&out.outcome==5&&out.attacker_address==0xabc000&&out.defender_address==0xabc100,"Foreign native preparation verdict changed");
    return 0xcafe006f;
}
static bool contains_before122(void*const* begin,void*const* end){
    bool contains=false;uint32_t player=fps_actor(),camera=fps_camera_actor();
    uintptr_t start=uintptr_t(begin),finish=uintptr_t(end);
    if(player&&finish>=start&&finish-start<=20&&((finish-start)&3)==0){
        for(uintptr_t slot=start;slot<finish;slot+=4){uint32_t actor{};if(!reader().get(uint32_t(slot),actor))break;contains|=actor==player||actor==camera;}
    }return contains;
}
// Frozen pre-Test122 predicate, including individual field reads and fallback.
static bool includes_before122(const Reader& r,uint32_t actor,uint32_t witness){
    if(!actor||!witness||witness>UINT32_MAX-0x100)return false;
    uint32_t backlink{};if(r.get(actor+0x688,backlink)&&backlink==witness)return true;
    for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
        uint32_t count{};if(!r.get(witness+offsets.second,count)||count>5)continue;
        for(uint32_t i=0;i<count;++i){uint32_t member{};if(!r.get(witness+offsets.first+i*4,member))break;if(member==actor)return true;}
    }return false;
}
static bool scoped_before122(uint32_t witness){
    // The old OR also protected the current camera actor, including death,
    // even when combat_identity rejected it. Prove that same camera identity
    // first. For infantry the physical subject can only be this actor, so a
    // negative membership result needs no duplicate combat/camera traversal.
    auto camera=fps_camera_actor();if(!camera)return false;
    auto r=reader();if(strictmelee::includes(r,camera,witness))return true;
    uint32_t mount{};if(!r.get(camera+0x210,mount)||!mount)return false;
    // Exclude witnesses containing neither current camera actor nor mount
    // before the reciprocal rider/horse proof. This filter never grants scope.
    if(!strictmelee::includes(r,mount,witness))return false;
    return owned_combat_actor(witness)!=0;
}
static uint32_t camera_before122(uint64_t* lease=nullptr){
    if(!possession||!state)return 0;
    observation::PossessionPrefix p{};if(!possession_snapshot93(p))return 0;
    if(p.magic!=0x31504353||p.version!=46||p.pid!=GetCurrentProcessId()||
       !p.ready||p.stop||p.release||p.mode!=2||!p.lease_generation||GetTickCount64()-p.heartbeat>1000)return 0;
    uint32_t root{},env{},manager{},camera{},actor{},table{},serial{},unit{},id{};auto r=reader();uint8_t alive{};
    if(!r.get(engine+0x1bc8180,root)||!r.get(root+0x110,env)||!r.get(env+0x281fc,manager)||
       !r.get(manager+0x8e0,camera)||!r.get(camera+0x188,actor)||!r.get(actor,table)||table!=engine+0x15dfec8||
       !r.get(actor+0x1c8,serial)||serial!=p.actor_id||!r.get(actor+0x214,unit)||!r.get(unit+4,id)||id!=p.unit_id||
       !r.get(actor+0xf0,alive)||alive>1)return 0;
    if(lease)*lease=p.lease_generation;return actor;
}



namespace qte { namespace reservation_before122 {
inline bool owns(const Reader& r,uint32_t engine,uint32_t held,const NativeActor& identity){
    NativeActor current;
    NativeActor rider;
    return held&&identity.address&&combat_identity(r,identity.address,engine,current)&&same_actor(current,identity)&&
        (current.kind!=Horse||horse_rider(r,engine,current,rider));
}
inline bool member(const Reader& r,uint32_t engine,uint32_t held,uint32_t actor,const NativeActor& identity){
    // A human has no rider alias: a different address cannot be a member.
    // This only rejects; every possible hit still proves current identity.
    if(identity.kind==Human&&actor!=identity.address)return false;
    if(!owns(r,engine,held,identity))return false;
    if(actor==identity.address)return true;
    NativeActor rider;
    return identity.kind==Horse&&horse_rider(r,engine,identity,rider)&&actor==rider.address;
}
inline bool preserve_release(const Reader& r,uint32_t engine,uint32_t held,uint32_t releasing,uint32_t actor,const NativeActor& player,const NativeActor& enemy){
    if(!held||releasing==held)return false;
    return member(r,engine,held,actor,player)||member(r,engine,held,actor,enemy);
}
inline bool overlaps(const Reader& r,uint32_t engine,uint32_t held,uint32_t candidate,const NativeActor& player,const NativeActor& enemy){
    if(!held||held==candidate||!candidate||candidate>UINT32_MAX-0x100)return false;
    // Snapshot only the bounded fields used by these native layouts.
    // A short/failed read retains the original individual-field path.
    // This is participant data, never an actor identity proof.
    std::array<uint32_t,0x50/4> snapshot{};const bool sampled=r.get(candidate,snapshot);
    auto field=[&](unsigned offset,uint32_t& value){
        if(sampled){value=snapshot[offset/4];return true;}
        return r.get(candidate+offset,value);
    };
    uint32_t table{},target{};const bool contact=field(0,table)&&table==engine+0x15e1510;
    // Contact objects end at +0x54: their later fields are not participant
    // arrays. Only matched witnesses carry principals and extras there.
    for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
        if(contact&&offsets.first!=0x10)continue;
        uint32_t count{};if(!field(offsets.second,count)||count>5)continue;
        for(uint32_t i=0;i<count;++i){uint32_t actor{};
            if(!field(offsets.first+i*4,actor))break;
            if((member(r,engine,held,actor,player))||(member(r,engine,held,actor,enemy)))return true;
        }
    }
    if(contact&&field(0x4c,target)&&
       ((member(r,engine,held,target,player))||(member(r,engine,held,target,enemy))))return true;
    return false;
}
} }

static bool pose_before122(const NativeActor& actor,float* position){
    auto r=reader();uint32_t table{},serial{},unit{},id{};
    if(!r.get(actor.address,table)||table!=actor_table(engine,actor.kind)||!r.get(actor.address+0x1c8,serial)||serial!=actor.serial||
       !r.get(actor.address+0x214,unit)||unit!=actor.unit||!r.get(unit+4,id)||id!=actor.unit_serial)return false;
    for(unsigned i=0;i<3;++i)if(!r.get(actor.address+0x48+i*4,position[i])||!std::isfinite(position[i]))return false;
    return true;
}

int main(int argc,char** argv){void* memory=nullptr;try{
    bool baseline=argc>1&&!strcmp(argv[1],"--baseline");
    memory=VirtualAlloc(nullptr,0x1bc9000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory,"Fixture engine allocation");engine=uint32_t(memory);
    std::vector<uint8_t> root(0x122),env(0x28300),manager(0x900),camera(0x200),p(0x700),other(0x700),pu(0x1000),ou(0x1000),held(0x110),w(0x110),horse(0x700),riders(4);
    auto ptr=[](auto& x){return uint32_t(x.data());};auto put=[](uint32_t address,auto value){memcpy(reinterpret_cast<void*>(address),&value,sizeof value);};
    put(engine+0x1bc8180,ptr(root));put(ptr(root)+0x110,ptr(env));put(ptr(env)+0x281fc,ptr(manager));put(ptr(manager)+0x8e0,ptr(camera));put(ptr(camera)+0x188,ptr(p));
    for(auto a:{ptr(p),ptr(other),ptr(horse)}){put(a,engine+(a==ptr(horse)?0x15e8524:0x15dfec8));put(a+0xf0,uint8_t(1));put(a+0x28,ptr(env));put(a+0x688,ptr(held));put(a+0x4c,1.f);}
    put(ptr(p)+0x214,ptr(pu));put(ptr(other)+0x214,ptr(ou));put(ptr(horse)+0x214,ptr(pu));put(ptr(pu)+4,7u);put(ptr(ou)+4,8u);
    put(ptr(p)+0x1c8,11u);put(ptr(other)+0x1c8,12u);put(ptr(horse)+0x1c8,13u);put(ptr(horse)+0x6cc,1u);put(ptr(horse)+0x6d0,ptr(riders));put(ptr(riders),ptr(p));
    observation::PossessionPrefix lease{};lease.magic=0x31504353;lease.version=46;lease.pid=GetCurrentProcessId();lease.ready=1;lease.mode=2;lease.actor_id=11;lease.unit_id=7;lease.lease_generation=1;lease.heartbeat=GetTickCount64();possession=&lease;
    Status telemetry{};state=&telemetry;enabled=1;native_update=reinterpret_cast<NativeWitnessCall>(foreign_update122);native_prepare=reinterpret_cast<NativeWitnessCall>(foreign_update122);
    LARGE_INTEGER freq;QueryPerformanceFrequency(&freq);frequency=double(freq.QuadPart);
    player_address=LONG(ptr(p));selection_ports.select=reinterpret_cast<NativeSelector>(foreign_select122);
    session=new Session(reader(),selection_ports.calls(),{nullptr,prepare_port,update_port,routing,facing_ready,retarget_port,effective_pair,retain_selected_pair},engine,{0,5,true},true,true,true,true,true);
    NativeActor held_horse{ptr(horse),13u,ptr(pu),7u,0u,1.f,Horse},held_enemy{ptr(other),12u,ptr(ou),8u,0u,1.f,Human};
    // Test the production selector wrapper against its frozen original
    // decision, including output pointers, native forwarding and hook scope.
    unsigned selector_cases122=0;uint32_t rand122=0x27ef191b;
    auto random122=[&](){rand122^=rand122<<13;rand122^=rand122>>17;return rand122^=rand122<<5;};
    void* members122[6]{};owns_coordinator=true;replay_parent=ptr(w);
    for(unsigned n=0;n<10000;++n){
        for(auto& member:members122){auto k=random122()%5;member=reinterpret_cast<void*>(k==0?ptr(p):k==1?ptr(horse):k==2?ptr(other):k==3?0u:7u);}
        unsigned count=n%7;auto begin=members122,end=members122+count;
        put(ptr(p)+0x210,n%3==0?ptr(horse):0u);put(ptr(p)+0x1c8,n%17==0?99u:11u);put(ptr(pu)+4,n%19==0?99u:7u);
        put(ptr(p)+0xf0,uint8_t(n%23==0?0:1));put(ptr(horse)+0x6cc,n%29==0?2u:1u);
        lease.release=n%31==0;lease.heartbeat=GetTickCount64();reject_bulk122=n%7==0;
        bool expected=contains_before122(begin,end);selector_missed=false;expected_begin122=begin;expected_end122=end;
        auto calls=selects122;NativeVerdict out{};
        require(select_hook(nullptr,nullptr,&out,begin,end,7,9)==&out&&selects122==calls+1,"Selector did not forward once");
        require(selector_missed==expected&&out.outcome==(expected?0u:5u)&&out.attacker_address==0xabc000&&out.defender_address==0xabc100,"Selector outcome/roles differ from prior implementation");++selector_cases122;
    }
    expected_begin122=expected_end122=nullptr;owns_coordinator=false;replay_parent=0;selector_missed=false;reject_bulk122=false;selects122=0;
    put(ptr(p)+0x210,0u);put(ptr(p)+0x1c8,11u);put(ptr(pu)+4,7u);put(ptr(p)+0xf0,uint8_t(1));put(ptr(horse)+0x6cc,1u);lease.release=0;
    std::cout<<"SELECTOR122 differential_cases="<<selector_cases122<<'\n';
    // Thousands of possession transitions: no field snapshot may survive
    // lease release, camera transfer, actor-address reuse or another callback.
    for(unsigned n=0;n<10000;++n){
        bool alternate=n&1;auto actor=alternate?ptr(other):ptr(p);
        put(ptr(camera)+0x188,actor);lease.actor_id=alternate?12u:11u;lease.unit_id=alternate?8u:7u;
        lease.lease_generation=n+1;lease.heartbeat=GetTickCount64();lease.mode=2;lease.release=0;
        put(ptr(p)+0x688,0u);put(ptr(other)+0x688,0u);
        put(ptr(w)+0x24,1u);put(ptr(w)+0x10,actor);
        require(scoped(ptr(w)),"New lease/camera not recognized");
        lease.mode=0;require(!scoped(ptr(w)),"Released possession retained scope");lease.mode=2;
        put(ptr(w)+0x10,alternate?ptr(p):ptr(other));
        require(!scoped(ptr(w)),"Previous soldier retained scope");
        ++lease.actor_id;require(!scoped(ptr(w)),"Reused actor address bypassed serial proof");
    }
    put(ptr(camera)+0x188,ptr(p));lease.actor_id=11;lease.unit_id=7;lease.lease_generation=1;
    put(ptr(p)+0x688,ptr(held));put(ptr(other)+0x688,ptr(held));std::fill(w.begin(),w.end(),uint8_t(0));
    NativeActor pose_identity{ptr(p),11,ptr(pu),7,0,1.f,Human};unsigned pose_checks=0;
    for(unsigned n=0;n<6000;++n){
        put(ptr(p),n%13==0?5u:actor_table(engine,Human));
        put(ptr(p)+0x1c8,n%17==0?99u:11u);put(ptr(p)+0x214,n%19==0?0u:ptr(pu));put(ptr(pu)+4,n%23==0?99u:7u);
        for(unsigned k=0;k<3;++k)put(ptr(p)+0x48+4*k,n%29==k?std::numeric_limits<float>::quiet_NaN():float(n+k));
        reject_bulk122=n%7==0;float old_pose[3]={-9,-8,-7},new_pose[3]={-9,-8,-7};
        bool old_ok=pose_before122(pose_identity,old_pose),new_ok=refresh_committed_pose(pose_identity,new_pose);
        require(old_ok==new_ok&&!memcmp(old_pose,new_pose,sizeof old_pose),"Committed pose identity/failure output changed");++pose_checks;
    }
    reject_bulk122=false;put(ptr(p),actor_table(engine,Human));put(ptr(p)+0x1c8,11u);put(ptr(p)+0x214,ptr(pu));put(ptr(pu)+4,7u);
    for(unsigned k=0;k<3;++k)put(ptr(p)+0x48+4*k,float(k+1));
    float pose_out[3];reads122=0;require(refresh_committed_pose(pose_identity,pose_out),"Pose benchmark identity");auto pose_reads122=reads122;
    if(!baseline)require(pose_reads122<=2,"Committed pose still repeats field reads");
    std::cout<<"TRANSITIONS122 switches=10000 pose_cases="<<pose_checks<<" pose_reads="<<pose_reads122<<'\n';
    unsigned checks=0;uint32_t random=0x31415926;auto next=[&](){random^=random<<13;random^=random>>17;return random^=random<<5;};
    for(unsigned n=0;n<6000;++n){
        std::fill(w.begin(),w.end(),uint8_t(0));
        for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
            for(unsigned i=0;i<5;++i){auto k=next()%5;put(ptr(w)+offsets.first+i*4,k==0?ptr(p):k==1?ptr(horse):k==2?ptr(other):k==3?0u:7u);}
            put(ptr(w)+offsets.second,next()%8);
        }
        bool mounted=n%3==0;put(ptr(p)+0x210,mounted?ptr(horse):0u);
        put(ptr(p)+0x688,n%11==0?ptr(w):ptr(held));put(ptr(horse)+0x688,n%13==0?ptr(w):ptr(held));
        put(ptr(p)+0x1c8,n%17==0?99u:11u);put(ptr(pu)+4,n%19==0?99u:7u);put(ptr(p)+0xf0,uint8_t(n%23==0?0:1));
        put(ptr(horse)+0x6cc,n%29==0?2u:1u);lease.release=n%31==0;lease.heartbeat=GetTickCount64();reject_bulk122=n%7==0;
        uint64_t old_lease=0x9988,new_lease=0x9988;
        require(fps_camera_actor(&new_lease)==camera_before122(&old_lease)&&new_lease==old_lease,"Camera identity or output lease differs from Test118");
        require(reservation::overlaps(reader(),engine,ptr(held),ptr(w),held_horse,held_enemy)==reservation_before122::overlaps(reader(),engine,ptr(held),ptr(w),held_horse,held_enemy),"Mounted overlap differs from Test119");
        for(auto actor:{ptr(p),ptr(horse),ptr(other),7u})require(reservation::preserve_release(reader(),engine,ptr(held),ptr(w),actor,held_horse,held_enemy)==reservation_before122::preserve_release(reader(),engine,ptr(held),ptr(w),actor,held_horse,held_enemy),"Mounted release differs from Test119");
        require(scoped(ptr(w))==scoped_before122(ptr(w)),"Scope differs from original on identity, death, mounted alias, backlink, counts or fallback");++checks;
    }
    reject_bulk122=false;lease.release=0;lease.heartbeat=GetTickCount64();put(ptr(p)+0x1c8,11u);put(ptr(pu)+4,7u);put(ptr(p)+0xf0,uint8_t(1));put(ptr(p)+0x210,0u);put(ptr(p)+0x688,ptr(held));
    // Native reads can straddle pages. Failed bulk reads must retain the exact
    // individual-read behavior and leave PAGE_GUARD/NOACCESS protections alone.
    SYSTEM_INFO si;GetSystemInfo(&si);auto pages=static_cast<uint8_t*>(VirtualAlloc(nullptr,si.dwPageSize*2,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(pages,"Page fixture");
    DWORD old;require(VirtualProtect(pages+si.dwPageSize,si.dwPageSize,PAGE_NOACCESS,&old),"Page protection");auto edge=uint32_t(pages+si.dwPageSize-0x28);
    put(edge+0x24,1u);put(edge+0x10,ptr(p));require(scoped(edge)&&scoped_before122(edge),"Readable first group lost at a page boundary");
    put(edge+0x10,ptr(other));require(!scoped(edge)&&!scoped_before122(edge),"Unreadable groups invented ownership");VirtualFree(pages,0,MEM_RELEASE);checks+=2;
    std::fill(w.begin(),w.end(),uint8_t(0));put(ptr(w)+0x24,5u);for(unsigned i=0;i<5;++i)put(ptr(w)+0x10+i*4,ptr(other));
    reads122=bytes122=0;forwards122=0;coordinator=0;
    require(dispatch(ptr(w),1,true)==0xcafe006f&&forwards122==1,"Foreign full callback did not forward exactly once");
    auto callback_reads=reads122,callback_bytes=bytes122;
    // A selector outside an owned replay cannot change its stock verdict. It
    // must not perform any possession lookup solely to discard the result.
    void* participants[2]={other.data(),horse.data()};NativeVerdict verdict{};owns_coordinator=false;replay_parent=0;reads122=bytes122=0;
    require(select_hook(nullptr,nullptr,&verdict,participants,participants+2,7,9)==&verdict&&selects122==1&&verdict.outcome==5,"Unrelated selector changed native output");auto selector_reads=reads122;
    const unsigned iterations=20000;forwards122=0;reads122=bytes122=0;LARGE_INTEGER begin,end;QueryPerformanceCounter(&begin);
    for(unsigned i=0;i<iterations;++i){if(!(i%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),1,true)==0xcafe006f,"Crowd callback changed native result");}
    QueryPerformanceCounter(&end);require(forwards122==iterations&&coordinator==0&&!owns_coordinator&&!pending_address&&!delivery_address,"Crowd changed player/session ownership");
    std::cout<<"SCOPE122 baseline="<<baseline<<" checks="<<checks<<" callback_reads="<<callback_reads<<" callback_bytes="<<callback_bytes<<" selector_reads="<<selector_reads<<" callbacks="<<iterations<<" reads="<<reads122<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
    if(!baseline){require(callback_reads<=17,"Unrelated full callback still repeats expensive ownership proof");require(selector_reads==0,"Foreign selector still queries possession");}
    // With a held pair the older reservation path proves both identities for
    // every unrelated participant. Exercise the full production dispatch,
    // contact path and release filter, not just an isolated overlap function.
    NativeActor own,enemy;require(combat_identity(reader(),ptr(p),engine,own)&&combat_identity(reader(),ptr(other),engine,enemy),"Held identities");
    committed_player=own;committed_enemy=enemy;delivery_address=LONG(ptr(held));
    std::vector<uint8_t> neutral(0x700);auto unrelated=ptr(neutral);
    for(unsigned i=0;i<5;++i)put(ptr(w)+0x10+i*4,unrelated);
    reads122=bytes122=0;forwards122=0;
    require(dispatch(ptr(w),2,true)==0xcafe006f&&forwards122==1,"Held foreign callback forwarding");auto held_reads=reads122;
    reads122=bytes122=0;require(!reservation::preserve_release(reader(),engine,ptr(held),ptr(w),unrelated,own,enemy),"Unrelated release suppressed");auto release_reads=reads122;
    native_contact_update=reinterpret_cast<NativeWitnessCall>(foreign_update122);reads122=bytes122=0;forwards122=0;
    require(contact_dispatch(ptr(w),true)==0xcafe006f&&forwards122==1,"Held contact forwarding");auto contact_reads=reads122;
    reads122=bytes122=0;forwards122=0;QueryPerformanceCounter(&begin);
    for(unsigned i=0;i<iterations;++i){if(!(i%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),2,true)==0xcafe006f,"Held crowd result changed");}
    QueryPerformanceCounter(&end);require(forwards122==iterations&&uint32_t(delivery_address)==ptr(held),"Held crowd changed reservation");
    std::cout<<"HELD122 baseline="<<baseline<<" callback_reads="<<held_reads<<" contact_reads="<<contact_reads<<" release_reads="<<release_reads<<" callbacks="<<iterations<<" reads="<<reads122<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
    if(!baseline){require(held_reads<=18,"Held crowd still repeats identity work");require(contact_reads==1&&release_reads==0,"Foreign contact/release still reads identities");}
    delivery_address=0;committed_player={};committed_enemy={};
    // The cavalry path must reject a foreign encounter before traversing
    // its full rider proof. Keep a real reciprocal synthetic horse/rider pair.
    put(ptr(p)+0x210,ptr(horse));put(ptr(horse)+0x6cc,1u);put(ptr(horse)+0x688,ptr(held));
    put(ptr(horse)+0x1c8,13u);put(ptr(horse)+0xf0,uint8_t(1));lease.heartbeat=GetTickCount64();
    require(fps_actor()==ptr(horse),"Mounted fixture not valid");
    reads122=bytes122=0;forwards122=0;require(dispatch(ptr(w),3,true)==0xcafe006f&&forwards122==1,"Mounted foreign forwarding");auto mount_reads=reads122;
    const unsigned mounted_iterations=10000;reads122=bytes122=0;forwards122=0;QueryPerformanceCounter(&begin);
    for(unsigned i=0;i<mounted_iterations;++i){if(!(i%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),3,true)==0xcafe006f,"Mounted foreign result");}
    QueryPerformanceCounter(&end);require(forwards122==mounted_iterations,"Mounted native forwarding count");
    std::cout<<"MOUNT122 baseline="<<baseline<<" callback_reads="<<mount_reads<<" callbacks="<<mounted_iterations<<" reads="<<reads122<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
    committed_player=held_horse;committed_enemy=held_enemy;delivery_address=LONG(ptr(held));
    reads122=bytes122=0;forwards122=0;require(dispatch(ptr(w),3,true)==0xcafe006f&&forwards122==1,"Mounted held forwarding");auto mount_held_reads=reads122;
    reads122=bytes122=0;forwards122=0;require(contact_dispatch(ptr(w),true)==0xcafe006f&&forwards122==1,"Mounted contact forwarding");auto mount_contact_reads=reads122;
    reads122=bytes122=0;require(!reservation::preserve_release(reader(),engine,ptr(held),ptr(w),unrelated,held_horse,held_enemy),"Mounted unrelated release");auto mount_release_reads=reads122;
    reads122=bytes122=0;forwards122=0;QueryPerformanceCounter(&begin);
    for(unsigned i=0;i<mounted_iterations;++i){if(!(i%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),3,true)==0xcafe006f,"Mounted held result");}
    QueryPerformanceCounter(&end);require(forwards122==mounted_iterations&&uint32_t(delivery_address)==ptr(held),"Mounted reservation changed");
    std::cout<<"MOUNT_HELD122 baseline="<<baseline<<" callback_reads="<<mount_held_reads<<" contact_reads="<<mount_contact_reads<<" release_reads="<<mount_release_reads<<" callbacks="<<mounted_iterations<<" reads="<<reads122<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
    if(!baseline){require(mount_reads<=19&&mount_held_reads<=22,"Mounted callback still repeats ownership proof");require(mount_contact_reads<=3&&mount_release_reads<=2,"Mounted reservation filter too costly");}
    delivery_address=0;committed_player={};committed_enemy={};
    // Full foreign preparation calls the real selector hook from inside
    // the native-port stub. This exercises replay_parent/owns_coordinator.
    delivery_address=0;committed_player={};committed_enemy={};put(ptr(p)+0x210,0u);
    native_prepare=reinterpret_cast<NativeWitnessCall>(foreign_prepare122);
    prepare_participants122[0]=neutral.data();prepare_participants122[1]=neutral.data();
    for(bool mounted:{false,true}){
        put(ptr(p)+0x210,mounted?ptr(horse):0u);lease.heartbeat=GetTickCount64();
        reads122=bytes122=0;forwards122=selects122=0;
        require(dispatch(ptr(w),9,false)==0xcafe006f&&forwards122==1&&selects122==1,"Native prepare/selector count differs");auto prepare_reads122=reads122;
        const unsigned loops=10000;forwards122=selects122=0;reads122=0;QueryPerformanceCounter(&begin);
        for(unsigned n=0;n<loops;++n){if(!(n%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),9,false)==0xcafe006f,"Crowd prepare forwarding");}
        QueryPerformanceCounter(&end);require(forwards122==loops&&selects122==loops&&!coordinator&&!owns_coordinator&&!replay_parent&&!selector_missed,"Prepare leaked replay/coordination state");
        std::cout<<(mounted?"MOUNT_PREP122":"PREP122")<<" baseline="<<baseline<<" callback_reads="<<prepare_reads122<<" callbacks="<<loops<<" reads="<<reads122<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
        if(!baseline)require(prepare_reads122<=(mounted?27u:25u),"Foreign preparation still repeats full player proof");
    }
    // A readable first role remains meaningful if the next page is inaccessible.
    auto role_pages=static_cast<uint8_t*>(VirtualAlloc(nullptr,si.dwPageSize*2,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(role_pages,"Role pages");
    require(VirtualProtect(role_pages+si.dwPageSize,si.dwPageSize,PAGE_NOACCESS,&old),"Role page protection");
    auto roles=reinterpret_cast<void**>(role_pages+si.dwPageSize-4);roles[0]=p.data();put(ptr(p)+0x210,0u);
    owns_coordinator=true;replay_parent=ptr(w);selector_missed=false;NativeVerdict partial{};
    require(contains_before122(roles,roles+2),"Old partial role fixture invalid");
    require(select_hook(nullptr,nullptr,&partial,roles,roles+2,7,9)==&partial&&selector_missed&&partial.outcome==0,"Partial role read lost player");
    roles[0]=neutral.data();selector_missed=false;
    require(!contains_before122(roles,roles+2),"Old unrelated partial fixture invalid");
    require(select_hook(nullptr,nullptr,&partial,roles,roles+2,7,9)==&partial&&!selector_missed&&partial.outcome==5,"Partial role read invented player");
    owns_coordinator=false;replay_parent=0;VirtualFree(role_pages,0,MEM_RELEASE);
    if(!baseline){
        // The filter may see a candidate which loses identity or membership
        // immediately afterward. Only the fresh full proof can grant a match.
        for(bool change_identity:{false,true}){
            void* changing[2]={p.data(),neutral.data()};owns_coordinator=true;replay_parent=ptr(w);selector_missed=false;
            mutate_read122=uint32_t(changing);mutated122=false;
            mutate_write122=change_identity?ptr(p)+0x1c8:uint32_t(changing);mutate_value122=change_identity?99u:ptr(neutral);
            NativeVerdict out{};require(select_hook(nullptr,nullptr,&out,changing,changing+2,7,9)==&out&&mutated122&&!selector_missed&&out.outcome==5,"Prefilter granted stale ownership/membership");
            put(ptr(p)+0x1c8,11u);mutate_read122=0;
        }
        owns_coordinator=false;replay_parent=0;
    }
    auto actor_pages=static_cast<uint8_t*>(VirtualAlloc(nullptr,si.dwPageSize*2,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(actor_pages,"Actor page fixture");
    require(VirtualProtect(actor_pages+si.dwPageSize,si.dwPageSize,PAGE_NOACCESS,&old),"Actor tail protection");
    auto edge_actor=uint32_t(actor_pages+si.dwPageSize-0x218);put(edge_actor,actor_table(engine,Human));put(edge_actor+0x1c8,11u);
    put(edge_actor+0x214,ptr(pu));put(edge_actor+0xf0,uint8_t(1));put(ptr(camera)+0x188,edge_actor);
    std::fill(w.begin(),w.end(),uint8_t(0));put(ptr(w)+0x24,1u);put(ptr(w)+0x10,edge_actor);lease.heartbeat=GetTickCount64();
    require(scoped(ptr(w))&&scoped_before122(ptr(w)),"Unreadable camera tail lost valid membership");
    put(ptr(w)+0x10,ptr(other));require(!scoped(ptr(w))&&!scoped_before122(ptr(w)),"Unreadable tail invented membership");
    MEMORY_BASIC_INFORMATION mbi{};require(VirtualQuery(actor_pages+si.dwPageSize,&mbi,sizeof mbi)&&mbi.Protect==PAGE_NOACCESS,"Read changed page protection");
    VirtualFree(actor_pages,0,MEM_RELEASE);
    delete session;session=nullptr;VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: full production dispatch and selection wrappers, fresh-identity differential cases and unreadable-page fallback. Offline CPU benchmark, not game FPS.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';if(memory)VirtualFree(memory,0,MEM_RELEASE);return 1;}}
