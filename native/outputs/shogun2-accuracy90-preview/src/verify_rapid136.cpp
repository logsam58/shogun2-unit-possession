// Production read/cap path; original native engine in an isolated fixture.
#define main retained_commands136_main
#include "verify_native_commands100.cpp"
#undef main
int main(){SetUnhandledExceptionFilter(&fault100);try{
    NativeFixture100 f;using P=NativeFixture100;
    std::vector<unsigned char> buckets(32),node(0x58),effect(0x20),squad(0x600);
    auto manager=unit+0xed4,map=manager+0xc,n=P::ptr(node),e=P::ptr(effect),b=P::ptr(buckets);
    field<uintptr_t>(manager,8)=unit;field<unsigned>(map,8)=2;field<uintptr_t>(map,0xc)=b;
    auto bucket=b+((0x27u^0x4a545eedu)%2)*16;
    field<uintptr_t>(bucket,0)=n;field<unsigned>(n,8)=0x27;field<uintptr_t>(n,0x4c)=e;
    field<uintptr_t>(e,0)=base+0x15cfc00;field<uintptr_t>(e,4)=environment;field<uintptr_t>(e,8)=unit;
    field<unsigned>(e,0xc)=0x27;field<float>(e,0x18)=40;field<unsigned char>(unit,0xd3c)=1;
    field<float>(unit,0x22c)=2.4f;field<float>(P::ptr(f.config),0x1c)=2;field<float>(P::ptr(f.config),0x20)=3.75f;field<float>(P::ptr(f.config),0x6c)=2;
    state->surface_speed_factor=state->terrain_speed_factor=1;
    field<uintptr_t>(P::ptr(squad),0)=base+0x15bb21c;field<uintptr_t>(P::ptr(squad),4)=environment;field<uintptr_t>(P::ptr(squad),0x510)=unit;
    std::array<std::array<unsigned char,0x700>,9> peers{};std::array<uintptr_t,10> roster{};roster[0]=possessed;
    for(unsigned i=0;i<9;++i){auto actor=reinterpret_cast<uintptr_t>(peers[i].data());roster[i+1]=actor;
        memcpy(peers[i].data(),f.peer.data(),0x700);field<unsigned>(actor,0x1c8)=30+i;field<uintptr_t>(actor,0x3b4)=P::ptr(squad);
        field<unsigned>(actor,0x244)=1;field<float>(actor,0x8c)=i<5?5.f:float(6+i);field<float>(actor,0x94)=0;
    }
    field<unsigned>(unit,0x384)=unsigned(roster.size());field<uintptr_t>(unit,0x388)=reinterpret_cast<uintptr_t>(roster.data());
    auto rate=[&](unsigned input){float speed=0,turn=0;state->heartbeat=GetTickCount64();require(read_native_movement(input,speed,turn),"Production movement rejected fixture");cap_rapid_pace136(input,speed);return speed;};
    for(unsigned i=0;i<1000;++i){
        clear_rapid_pace136();require(rate(0)==4.8f,"Rapid walking changed");
        require(rate(RUN)==5.f,"Rapid Shift selected theoretical9 instead of native peer5");
        require(rapid_pace136.samples==9,"Peer sample bounds/identity differ");
        float lower=2;require(cap_rapid_pace136(RUN,lower)==2&&lower==2,"Peer reference accelerated a terrain-limited player");
        // The order-gait fields used by135 must have no authority over this cap.
        field<unsigned char>(unit,0xea6)=i&1;require(rate(RUN)==5.f,"Unit order re-enabled extra sprint");
    }
    // No ability: ordinary manual run and other native stat bonuses are exact.
    field<float>(e,0x18)=0;require(rate(RUN)==9.f&&!rapid_pace_present136,"Expired Rapid changed normal boosted run");
    field<float>(unit,0x22c)=1;require(rate(RUN)==3.75f&&rate(0)==2,"Normal walk/run changed");
    field<float>(unit,0x22c)=2.4f;field<float>(e,0x18)=40;
    // Non-Rapid shared fatigue flag (Second Wind) cannot invoke the cap.
    field<uintptr_t>(n,0x4c)=0;require(rate(RUN)==9.f,"Other ability running was capped");field<uintptr_t>(n,0x4c)=e;
    // Foreign/dead/combat/charge/mismatched rigs never contribute velocity.
    for(unsigned i=1;i<roster.size();++i)field<unsigned char>(roster[i],0xf0)=0;
    auto valid=roster[1];field<unsigned char>(valid,0xf0)=1;
    for(unsigned kind=0;kind<6;++kind){clear_rapid_pace136();
        auto saved_unit=field<uintptr_t>(valid,0x214),saved_env=field<uintptr_t>(valid,0x28),saved_config=field<uintptr_t>(valid,0x20c);
        if(kind==0)field<uintptr_t>(valid,0x214)=unit+4;
        if(kind==1)field<uintptr_t>(valid,0x28)=environment+4;
        if(kind==2)field<uintptr_t>(valid,0x20c)=saved_config+4;
        if(kind==3)field<unsigned>(valid,0x1f0)=18;
        if(kind==4)field<unsigned>(valid,0x244)=2;
        if(kind==5)field<float>(valid,0x8c)=NAN;
        require(rate(RUN)==4.8f,"Invalid peer escaped safe native-walk fallback");
        field<uintptr_t>(valid,0x214)=saved_unit;field<uintptr_t>(valid,0x28)=saved_env;field<uintptr_t>(valid,0x20c)=saved_config;
        field<unsigned>(valid,0x1f0)=0;field<unsigned>(valid,0x244)=1;field<float>(valid,0x8c)=5;
    }
    clear_rapid_pace136();require(rate(RUN)==5,"Valid peer did not recover");
    auto now=GetTickCount64();field<unsigned char>(valid,0xf0)=0;
    require(rapid_reference136(now+1100)==0,"Stale reference did not expire");
    field<uintptr_t>(unit,0x388)=1;clear_rapid_pace136();require(rate(RUN)==4.8f,"Unreadable roster selected extra sprint");
    require(TryAcquireSRWLockExclusive(&rapid_pace_lock136)!=0,"Failed native reads stranded pace lock");ReleaseSRWLockExclusive(&rapid_pace_lock136);
    field<uintptr_t>(unit,0x388)=reinterpret_cast<uintptr_t>(roster.data());field<unsigned char>(valid,0xf0)=1;clear_rapid_pace136();require(rate(RUN)==5,"Post-fault recovery failed");
    unsigned before=rapid_pace136.refreshes;for(unsigned i=0;i<1000;++i)rapid_reference136(rapid_pace136.checked);
    require(rapid_pace136.refreshes==before,"Per-frame calls repeated roster scan");
    auto prior_generation=possession_lease.generation;++possession_lease.generation;rapid_reference136(GetTickCount64());require(rapid_pace136.generation!=prior_generation,"Possession reused old pace identity");
    require(field<float>(unit,0x22c)==2.4f&&field<float>(valid,0x8c)==5,"Pace matching wrote native stat/peer velocity");
    clear_rapid_pace136();std::cout<<"PASS:1000 production Rapid Shift5/native-walk4.8 cases against theoretical sprint9; normal/other ability unaffected; median/ownership/foreign/death/combat/charge/rig/expiry/fault/lock recovery;1000 repeated reads do not rescan. Synthetic peer velocities; actual live matching needs a fresh test.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
