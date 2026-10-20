#define main retained_movement131_main
#include "verify_native_commands100.cpp"
#undef main
int main(){SetUnhandledExceptionFilter(&fault100);try{
    NativeFixture100 f;using P=NativeFixture100;
    std::vector<unsigned char> squad(0x2800),aggregate(0x100);uintptr_t members[1]={possessed};auto s=P::ptr(squad);
    field<unsigned>(s,0x18)=1;field<uintptr_t>(s,0x1c)=reinterpret_cast<uintptr_t>(members);field<uintptr_t>(s,0x510)=unit;
    using NativeSpeed=float(__thiscall*)(void*,unsigned,unsigned);auto native=reinterpret_cast<NativeSpeed>(base+0x18b310);
    // A config mismatch is not permission to control another native subject.
    field<uintptr_t>(unit,0xf9c)=P::ptr(aggregate);field<float>(P::ptr(aggregate),0x1c)=19;field<float>(P::ptr(aggregate),0x20)=29;
    float rejected=0,rejected_turn=0;state->heartbeat=GetTickCount64();require(!read_native_movement(RUN,rejected,rejected_turn),"Mismatched movement config gained control");
    field<uintptr_t>(unit,0xf9c)=P::ptr(f.config);
    field<float>(P::ptr(f.config),0x6c)=2;
    unsigned cases=0;
    for(float walk:{.8f,1.4f,3.f})for(float run:{2.f,4.f,8.f})for(float factor:{0.f,.5f,1.f,1.5f,2.f,3.f})
    for(float global_walk:{.5f,1.f,1.25f})for(float global_run:{.5f,1.f,1.25f})for(bool allowed:{false,true})for(unsigned input:{0u,unsigned(RUN)}){
        run_allowed102=allowed;field<float>(P::ptr(f.config),0x1c)=walk;field<float>(P::ptr(f.config),0x20)=run;
        field<float>(unit,0x22c)=factor;field<float>(base,0x18d1c80+0x5c)=global_walk;field<float>(base,0x18d1ce0+0x5c)=global_run;
        unsigned gait=(input&RUN)&&allowed?1:0;float expected=native(squad.data(),gait,1),observed=0,turn=0;
        state->heartbeat=GetTickCount64();bool valid131=read_native_movement(input,observed,turn);
        if(!valid131||observed!=expected||turn!=2)std::cerr<<"case "<<cases<<" valid "<<valid131<<" input "<<input<<" expected "<<expected<<" observed "<<observed<<" turn "<<turn<<" fail "<<follow_failure93<<"\n";
        require(valid131&&observed==expected&&turn==2,"Possessed rate differs from original native entity/unit/global speed calculation");++cases;
    }
    // Rapid-Advance-like unit multiplier transitions must be linear, including
    // expiration; no ability-name heuristic, division or second multiplication.
    field<float>(P::ptr(f.config),0x20)=4;field<float>(base,0x18d1ce0+0x5c)=.75f;run_allowed102=true;
    for(float factor:{1.f,2.f,1.5f,1.f,0.f}){field<float>(unit,0x22c)=factor;float observed=0,turn=0;
        require(read_native_movement(RUN,observed,turn)&&observed==3*factor,"Live modifier was duplicated or retained after expiration");}
    field<float>(unit,0x22c)=NAN;float observed=0,turn=0;require(!read_native_movement(RUN,observed,turn),"Invalid speed modifier accepted");
    std::cout<<"PASS: "<<cases<<" exact original native18b310 comparisons: current soldier config, global walk/run scaling, native run restriction, zero/buff/debuff and expiry; mismatched config rejects control. Synthetic squad and run-predicate provider; real Rapid Advance/collision/root-motion behavior needs live comparison.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
