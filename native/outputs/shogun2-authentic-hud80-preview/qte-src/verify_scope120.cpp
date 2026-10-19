// Exercise the production callback, not just the reservation prefilter.
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cstdint>
static uint64_t reads120,bytes120;static bool reject_bulk120;
static BOOL WINAPI counted_read120(HANDLE process,LPCVOID address,LPVOID out,SIZE_T size,SIZE_T* got){
    ++reads120;bytes120+=size;
    if(reject_bulk120&&(size==0x50||size==0x218||size==8)){if(got)*got=0;return FALSE;}
    return ReadProcessMemory(process,address,out,size,got);
}
#define ReadProcessMemory counted_read120
#include "playable_runtime.cpp"
#undef ReadProcessMemory
static unsigned forwards120,selects120;
static uint32_t __fastcall foreign_update120(void*,void*){++forwards120;return 0xcafe006f;}
static NativeVerdict* __fastcall foreign_select120(void*,void*,NativeVerdict* out,void*const* begin,void*const* end,uint32_t a,uint32_t b){
    ++selects120;require(end-begin==2&&a==7&&b==9,"Selector arguments changed");
    *out={5,uint32_t(begin[0]),uint32_t(begin[1])};return out;
}
// Frozen pre-Test120 predicate, including individual field reads and fallback.
static bool includes_before120(const Reader& r,uint32_t actor,uint32_t witness){
    if(!actor||!witness||witness>UINT32_MAX-0x100)return false;
    uint32_t backlink{};if(r.get(actor+0x688,backlink)&&backlink==witness)return true;
    for(auto offsets:{std::pair<unsigned,unsigned>{0x10,0x24},{0x30,0x38},{0x3c,0x48}}){
        uint32_t count{};if(!r.get(witness+offsets.second,count)||count>5)continue;
        for(uint32_t i=0;i<count;++i){uint32_t member{};if(!r.get(witness+offsets.first+i*4,member))break;if(member==actor)return true;}
    }return false;
}
static bool scoped_before120(uint32_t witness){
    return includes_before120(reader(),fps_actor(),witness)||includes_before120(reader(),fps_camera_actor(),witness);
}
static uint32_t camera_before120(uint64_t* lease=nullptr){
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



namespace qte { namespace reservation_before120 {
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

int main(int argc,char** argv){void* memory=nullptr;try{
    bool baseline=argc>1&&!strcmp(argv[1],"--baseline");
    memory=VirtualAlloc(nullptr,0x1bc9000,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(memory,"Fixture engine allocation");engine=uint32_t(memory);
    std::vector<uint8_t> root(0x120),env(0x28300),manager(0x900),camera(0x200),p(0x700),other(0x700),pu(0x1000),ou(0x1000),held(0x110),w(0x110),horse(0x700),riders(4);
    auto ptr=[](auto& x){return uint32_t(x.data());};auto put=[](uint32_t address,auto value){memcpy(reinterpret_cast<void*>(address),&value,sizeof value);};
    put(engine+0x1bc8180,ptr(root));put(ptr(root)+0x110,ptr(env));put(ptr(env)+0x281fc,ptr(manager));put(ptr(manager)+0x8e0,ptr(camera));put(ptr(camera)+0x188,ptr(p));
    for(auto a:{ptr(p),ptr(other),ptr(horse)}){put(a,engine+(a==ptr(horse)?0x15e8524:0x15dfec8));put(a+0xf0,uint8_t(1));put(a+0x28,ptr(env));put(a+0x688,ptr(held));put(a+0x4c,1.f);}
    put(ptr(p)+0x214,ptr(pu));put(ptr(other)+0x214,ptr(ou));put(ptr(horse)+0x214,ptr(pu));put(ptr(pu)+4,7u);put(ptr(ou)+4,8u);
    put(ptr(p)+0x1c8,11u);put(ptr(other)+0x1c8,12u);put(ptr(horse)+0x1c8,13u);put(ptr(horse)+0x6cc,1u);put(ptr(horse)+0x6d0,ptr(riders));put(ptr(riders),ptr(p));
    observation::PossessionPrefix lease{};lease.magic=0x31504353;lease.version=46;lease.pid=GetCurrentProcessId();lease.ready=1;lease.mode=2;lease.actor_id=11;lease.unit_id=7;lease.lease_generation=1;lease.heartbeat=GetTickCount64();possession=&lease;
    Status telemetry{};state=&telemetry;enabled=1;native_update=reinterpret_cast<NativeWitnessCall>(foreign_update120);native_prepare=reinterpret_cast<NativeWitnessCall>(foreign_update120);
    LARGE_INTEGER freq;QueryPerformanceFrequency(&freq);frequency=double(freq.QuadPart);
    player_address=LONG(ptr(p));selection_ports.select=reinterpret_cast<NativeSelector>(foreign_select120);
    session=new Session(reader(),selection_ports.calls(),{nullptr,prepare_port,update_port,routing,facing_ready,retarget_port,effective_pair,retain_selected_pair},engine,{0,5,true},true,true,true,true,true);
    NativeActor held_horse{ptr(horse),13u,ptr(pu),7u,0u,1.f,Horse},held_enemy{ptr(other),12u,ptr(ou),8u,0u,1.f,Human};
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
        put(ptr(horse)+0x6cc,n%29==0?2u:1u);lease.release=n%31==0;lease.heartbeat=GetTickCount64();reject_bulk120=n%7==0;
        uint64_t old_lease=0x9988,new_lease=0x9988;
        require(fps_camera_actor(&new_lease)==camera_before120(&old_lease)&&new_lease==old_lease,"Camera identity or output lease differs from Test118");
        require(reservation::overlaps(reader(),engine,ptr(held),ptr(w),held_horse,held_enemy)==reservation_before120::overlaps(reader(),engine,ptr(held),ptr(w),held_horse,held_enemy),"Mounted overlap differs from Test119");
        for(auto actor:{ptr(p),ptr(horse),ptr(other),7u})require(reservation::preserve_release(reader(),engine,ptr(held),ptr(w),actor,held_horse,held_enemy)==reservation_before120::preserve_release(reader(),engine,ptr(held),ptr(w),actor,held_horse,held_enemy),"Mounted release differs from Test119");
        require(scoped(ptr(w))==scoped_before120(ptr(w)),"Scope differs from original on identity, death, mounted alias, backlink, counts or fallback");++checks;
    }
    reject_bulk120=false;lease.release=0;lease.heartbeat=GetTickCount64();put(ptr(p)+0x1c8,11u);put(ptr(pu)+4,7u);put(ptr(p)+0xf0,uint8_t(1));put(ptr(p)+0x210,0u);put(ptr(p)+0x688,ptr(held));
    // Native reads can straddle pages. Failed bulk reads must retain the exact
    // individual-read behavior and leave PAGE_GUARD/NOACCESS protections alone.
    SYSTEM_INFO si;GetSystemInfo(&si);auto pages=static_cast<uint8_t*>(VirtualAlloc(nullptr,si.dwPageSize*2,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));require(pages,"Page fixture");
    DWORD old;require(VirtualProtect(pages+si.dwPageSize,si.dwPageSize,PAGE_NOACCESS,&old),"Page protection");auto edge=uint32_t(pages+si.dwPageSize-0x28);
    put(edge+0x24,1u);put(edge+0x10,ptr(p));require(scoped(edge)&&scoped_before120(edge),"Readable first group lost at a page boundary");
    put(edge+0x10,ptr(other));require(!scoped(edge)&&!scoped_before120(edge),"Unreadable groups invented ownership");VirtualFree(pages,0,MEM_RELEASE);checks+=2;
    std::fill(w.begin(),w.end(),uint8_t(0));put(ptr(w)+0x24,5u);for(unsigned i=0;i<5;++i)put(ptr(w)+0x10+i*4,ptr(other));
    reads120=bytes120=0;forwards120=0;coordinator=0;
    require(dispatch(ptr(w),1,true)==0xcafe006f&&forwards120==1,"Foreign full callback did not forward exactly once");
    auto callback_reads=reads120,callback_bytes=bytes120;
    // A selector outside an owned replay cannot change its stock verdict. It
    // must not perform any possession lookup solely to discard the result.
    void* participants[2]={other.data(),horse.data()};NativeVerdict verdict{};owns_coordinator=false;replay_parent=0;reads120=bytes120=0;
    require(select_hook(nullptr,nullptr,&verdict,participants,participants+2,7,9)==&verdict&&selects120==1&&verdict.outcome==5,"Unrelated selector changed native output");auto selector_reads=reads120;
    const unsigned iterations=20000;forwards120=0;reads120=bytes120=0;LARGE_INTEGER begin,end;QueryPerformanceCounter(&begin);
    for(unsigned i=0;i<iterations;++i){if(!(i%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),1,true)==0xcafe006f,"Crowd callback changed native result");}
    QueryPerformanceCounter(&end);require(forwards120==iterations&&coordinator==0&&!owns_coordinator&&!pending_address&&!delivery_address,"Crowd changed player/session ownership");
    std::cout<<"SCOPE120 baseline="<<baseline<<" checks="<<checks<<" callback_reads="<<callback_reads<<" callback_bytes="<<callback_bytes<<" selector_reads="<<selector_reads<<" callbacks="<<iterations<<" reads="<<reads120<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
    if(!baseline){require(callback_reads<=21,"Unrelated full callback still repeats expensive ownership proof");require(selector_reads==0,"Foreign selector still queries possession");}
    // With a held pair the older reservation path proves both identities for
    // every unrelated participant. Exercise the full production dispatch,
    // contact path and release filter, not just an isolated overlap function.
    NativeActor own,enemy;require(combat_identity(reader(),ptr(p),engine,own)&&combat_identity(reader(),ptr(other),engine,enemy),"Held identities");
    committed_player=own;committed_enemy=enemy;delivery_address=LONG(ptr(held));
    std::vector<uint8_t> neutral(0x700);auto unrelated=ptr(neutral);
    for(unsigned i=0;i<5;++i)put(ptr(w)+0x10+i*4,unrelated);
    reads120=bytes120=0;forwards120=0;
    require(dispatch(ptr(w),2,true)==0xcafe006f&&forwards120==1,"Held foreign callback forwarding");auto held_reads=reads120;
    reads120=bytes120=0;require(!reservation::preserve_release(reader(),engine,ptr(held),ptr(w),unrelated,own,enemy),"Unrelated release suppressed");auto release_reads=reads120;
    native_contact_update=reinterpret_cast<NativeWitnessCall>(foreign_update120);reads120=bytes120=0;forwards120=0;
    require(contact_dispatch(ptr(w),true)==0xcafe006f&&forwards120==1,"Held contact forwarding");auto contact_reads=reads120;
    reads120=bytes120=0;forwards120=0;QueryPerformanceCounter(&begin);
    for(unsigned i=0;i<iterations;++i){if(!(i%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),2,true)==0xcafe006f,"Held crowd result changed");}
    QueryPerformanceCounter(&end);require(forwards120==iterations&&uint32_t(delivery_address)==ptr(held),"Held crowd changed reservation");
    std::cout<<"HELD120 baseline="<<baseline<<" callback_reads="<<held_reads<<" contact_reads="<<contact_reads<<" release_reads="<<release_reads<<" callbacks="<<iterations<<" reads="<<reads120<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
    if(!baseline){require(held_reads<=22,"Held crowd still repeats identity work");require(contact_reads==1&&release_reads==0,"Foreign contact/release still reads identities");}
    delivery_address=0;committed_player={};committed_enemy={};
    // The cavalry path must reject a foreign encounter before traversing
    // its full rider proof. Keep a real reciprocal synthetic horse/rider pair.
    put(ptr(p)+0x210,ptr(horse));put(ptr(horse)+0x6cc,1u);put(ptr(horse)+0x688,ptr(held));
    put(ptr(horse)+0x1c8,13u);put(ptr(horse)+0xf0,uint8_t(1));lease.heartbeat=GetTickCount64();
    require(fps_actor()==ptr(horse),"Mounted fixture not valid");
    reads120=bytes120=0;forwards120=0;require(dispatch(ptr(w),3,true)==0xcafe006f&&forwards120==1,"Mounted foreign forwarding");auto mount_reads=reads120;
    const unsigned mounted_iterations=10000;reads120=bytes120=0;forwards120=0;QueryPerformanceCounter(&begin);
    for(unsigned i=0;i<mounted_iterations;++i){if(!(i%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),3,true)==0xcafe006f,"Mounted foreign result");}
    QueryPerformanceCounter(&end);require(forwards120==mounted_iterations,"Mounted native forwarding count");
    std::cout<<"MOUNT120 baseline="<<baseline<<" callback_reads="<<mount_reads<<" callbacks="<<mounted_iterations<<" reads="<<reads120<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
    committed_player=held_horse;committed_enemy=held_enemy;delivery_address=LONG(ptr(held));
    reads120=bytes120=0;forwards120=0;require(dispatch(ptr(w),3,true)==0xcafe006f&&forwards120==1,"Mounted held forwarding");auto mount_held_reads=reads120;
    reads120=bytes120=0;forwards120=0;require(contact_dispatch(ptr(w),true)==0xcafe006f&&forwards120==1,"Mounted contact forwarding");auto mount_contact_reads=reads120;
    reads120=bytes120=0;require(!reservation::preserve_release(reader(),engine,ptr(held),ptr(w),unrelated,held_horse,held_enemy),"Mounted unrelated release");auto mount_release_reads=reads120;
    reads120=bytes120=0;forwards120=0;QueryPerformanceCounter(&begin);
    for(unsigned i=0;i<mounted_iterations;++i){if(!(i%128))lease.heartbeat=GetTickCount64();require(dispatch(ptr(w),3,true)==0xcafe006f,"Mounted held result");}
    QueryPerformanceCounter(&end);require(forwards120==mounted_iterations&&uint32_t(delivery_address)==ptr(held),"Mounted reservation changed");
    std::cout<<"MOUNT_HELD120 baseline="<<baseline<<" callback_reads="<<mount_held_reads<<" contact_reads="<<mount_contact_reads<<" release_reads="<<mount_release_reads<<" callbacks="<<mounted_iterations<<" reads="<<reads120<<" elapsed_ms="<<double(end.QuadPart-begin.QuadPart)*1000/frequency<<'\n';
    if(!baseline){require(mount_reads<=25&&mount_held_reads<=28,"Mounted callback still repeats ownership proof");require(mount_contact_reads<=3&&mount_release_reads<=2,"Mounted reservation filter too costly");}
    delivery_address=0;committed_player={};committed_enemy={};
    delete session;session=nullptr;VirtualFree(memory,0,MEM_RELEASE);memory=nullptr;
    std::cout<<"PASS: full production dispatch and selection wrappers, fresh-identity differential cases and unreadable-page fallback. Offline CPU benchmark, not game FPS.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';if(memory)VirtualFree(memory,0,MEM_RELEASE);return 1;}}
