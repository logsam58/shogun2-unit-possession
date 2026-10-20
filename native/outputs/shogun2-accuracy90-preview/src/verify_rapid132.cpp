// Exact native Rapid Advance modifier and hash lookup; synthetic owned records.
#define main retained_commands132_main
#include "verify_native_commands100.cpp"
#undef main
int main(){SetUnhandledExceptionFilter(&fault100);try{
    NativeFixture100 f;using P=NativeFixture100;
    std::vector<unsigned char> buckets(32),node(0x58),effect(0x20),squad(0x2800);
    uintptr_t manager=unit+0xed4,map=manager+0xc,n=P::ptr(node),e=P::ptr(effect),b=P::ptr(buckets);
    field<uintptr_t>(manager,8)=unit;field<unsigned>(map,8)=2;field<uintptr_t>(map,0xc)=b;
    auto bucket=b+((0x27u^0x4a545eedu)%2)*16;
    field<uintptr_t>(bucket,0)=n;field<uintptr_t>(bucket,4)=0;field<uintptr_t>(n,4)=0;field<unsigned>(n,8)=0x27;field<uintptr_t>(n,0x4c)=e;
    field<uintptr_t>(e,0)=base+0x15cfc00;field<uintptr_t>(e,4)=environment;field<uintptr_t>(e,8)=unit;field<unsigned>(e,0xc)=0x27;field<float>(e,0x18)=40;
    using Lookup=uintptr_t*(__thiscall*)(void*,uintptr_t*,const unsigned*);uintptr_t found[3]{};unsigned key=0x27;
    reinterpret_cast<Lookup>(base+0x2d5560)(reinterpret_cast<void*>(map),found,&key);
    require(found[0]==map+4&&found[1]==bucket&&found[2]==n,"Production Rapid lookup layout differs from original native lookup");
    uintptr_t members[1]={possessed};field<unsigned>(P::ptr(squad),0x18)=1;field<uintptr_t>(P::ptr(squad),0x1c)=reinterpret_cast<uintptr_t>(members);field<uintptr_t>(P::ptr(squad),0x510)=unit;
    field<float>(P::ptr(f.config),0x1c)=2;field<float>(P::ptr(f.config),0x20)=3.75f;field<float>(P::ptr(f.config),0x6c)=2;
    field<float>(base,0x18d1c80+0x5c)=field<float>(base,0x18d1ce0+0x5c)=1;
    using Modify=void(__thiscall*)(void*,float);auto modify=reinterpret_cast<Modify>(base+0xc1a580);
    using NativeSpeed=float(__thiscall*)(void*,unsigned,unsigned);auto native=reinterpret_cast<NativeSpeed>(base+0x18b310);
    state->surface_speed_factor=state->terrain_speed_factor=1;
    run_allowed102=true;
    for(unsigned repeat=0;repeat<1000;++repeat){
        field<float>(unit,0x22c)=1;modify(reinterpret_cast<void*>(unit+0x130),1.4f);field<unsigned char>(unit,0xd3c)=1;
        require(std::abs(field<float>(unit,0x22c)-2.4f)<.000001f,"Original Rapid stat addition differs");
        for(unsigned input:{0u,unsigned(RUN)}){float speed=0,turn=0;state->heartbeat=GetTickCount64();
            require(native_sprint_requested133(input)==bool(input&RUN),"Manual walking/running selection changed");
            require(rapid_advance_active132()&&read_native_movement(input,speed,turn),"Rapid speed read failed");
            cap_rapid_pace136(input,speed);
            require(speed==native(squad.data(),0,1),"No-peer Rapid fallback escaped native walk pace");}
        // Second Wind shares d3c, but must not suppress ordinary Shift running.
        field<uintptr_t>(n,0x4c)=0;float speed=0,turn=0;
        require(!rapid_advance_active132()&&native_sprint_requested133(RUN)&&read_native_movement(RUN,speed,turn)&&speed==native(squad.data(),1,1),"Shared non-Rapid flag suppressed normal sprint");field<uintptr_t>(n,0x4c)=e;
        field<unsigned char>(unit,0xd3c)=0;modify(reinterpret_cast<void*>(unit+0x130),-1.4f);
        for(unsigned input:{0u,unsigned(RUN)}){require(read_native_movement(input,speed,turn)&&speed==native(squad.data(),input?1:0,1),"Ability expiry failed to restore normal gait");}
    }
    field<unsigned char>(unit,0xd3c)=1;
    for(auto item:{std::make_pair(e,base+0x15cfcd8),std::make_pair(e+4,environment+4),std::make_pair(e+8,unit+4),std::make_pair(e+0xc,uintptr_t(0x29)),std::make_pair(manager+8,unit+4)}){
        auto prior=*reinterpret_cast<uintptr_t*>(item.first);*reinterpret_cast<uintptr_t*>(item.first)=item.second;require(!rapid_advance_active132(),"Foreign/non-Rapid effect accepted");*reinterpret_cast<uintptr_t*>(item.first)=prior;}
    for(float remaining:{0.f,-1.f,NAN}){field<float>(e,0x18)=remaining;require(!rapid_advance_active132(),"Expired/invalid Rapid effect accepted");}field<float>(e,0x18)=40;
    field<uintptr_t>(n,4)=n;field<unsigned>(n,8)=0x29;require(!rapid_advance_active132(),"Cyclic unrelated bucket was accepted");field<unsigned>(map,8)=0;require(!rapid_advance_active132(),"Empty map divided by zero");
    std::cout<<"PASS: original native Rapid stat addition, original hash lookup and1000 active/expiry gait cycles; Shift final pace ceiling during exact active Rapid; normal sprint/Second Wind retained; foreign, expired, cyclic and empty records reject. Native peer ceiling covered separately by verify_rapid136; no-peer player fallback; native unit modifiers and AI unchanged. Live acceptance pending.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
