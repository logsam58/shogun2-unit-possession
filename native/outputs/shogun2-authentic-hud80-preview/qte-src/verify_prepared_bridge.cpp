#define main legacy_bridge_fixture_main
#include "verify_native_bridge.cpp"
#undef main
struct PreparedWorld:World {
    unsigned resumes{};unsigned invalid{};
    static uint32_t prepare(void* p,uint32_t address){auto& w=*static_cast<PreparedWorld*>(p);
        auto answer=World::prepare(p,address);if(address!=witness)return answer;
        w.put(witness+0x38,2u);w.put(witness+0x30,player);w.put(witness+0x34,enemy);
        w.put(witness+0x80,w.committed.outcome);w.put(witness+0x84,w.committed.attacker_address);w.put(witness+0x88,w.committed.defender_address);
        w.put(witness+0x98,123u);w.put(witness+0xb4,0u);w.put(witness+0xd1,uint8_t(0));
        w.put(player+0x688,witness);w.put(enemy+0x688,witness);
        if(w.invalid==1)return 1;
        if(w.invalid==2)w.put(enemy+0x688,0u);
        if(w.invalid==3)w.put(witness+0xd1,uint8_t(1));
        if(w.invalid==4)w.put(witness+0x98,0u);
        return 0;
    }
    static uint32_t resume(void* p,uint32_t address,const qte::NativeVerdict& result,const qte::NativePlan* plans){
        auto& w=*static_cast<PreparedWorld*>(p);++w.resumes;
        check(w.get<uint32_t>(player+0x688)==address&&w.get<uint32_t>(enemy+0x688)==address,"Pair lost native registration while answering");
        check(plans[0].actor==player&&plans[1].actor==enemy&&plans[0].state==16&&plans[1].state==18,"Initial planning context not retained");
        w.put(address+0xc,uint8_t(0));return prepare(p,address);
    }
    qte::BridgeCalls ports(){return {this,prepare,World::update,World::routing,nullptr,resume};}
    void initialize(bool attack,uint32_t outcome){setup(attack,outcome);resumes=invalid=0;
        put(player+0x688,0u);put(enemy+0x688,0u);put(player+0x1d0,16u);put(enemy+0x1d0,18u);put(player+0x1d4,3u);put(enemy+0x1d4,4u);
    }
};
#ifndef PREPARED_FIXTURE_ENTRY
#define PREPARED_FIXTURE_ENTRY main
#endif
int PREPARED_FIXTURE_ENTRY(){try{using namespace qte;PreparedWorld w;unsigned cases=0;
    for(auto initial:{0u,5u})for(bool attack:{false,true})for(bool success:{false,true}){
        w.initialize(attack,initial);NativeBridge bridge(w.reader(),w.selection(),w.ports(),World::engine,{0,5,true},true,true);w.bridge=&bridge;
        auto owner=w.owner();check(bridge.prepare(World::witness,1,owner)==0&&bridge.snapshot().active,"Prepared pair did not acquire QTE");
        check(w.prepares==1&&w.rolls==1&&w.get<uint32_t>(World::player+0x688)==World::witness,"Prompt appeared before native registration");
        check(bridge.update(World::witness,1)==0&&w.updates==0,"Clip advanced during unanswered prompt");
        auto prompt=bridge.snapshot();bridge.frame(owner,success?prompt.ideal:0,false,true,prompt.serial,attack,!attack);
        check(bridge.prepare(World::witness,1,owner)==0&&w.resumes==1&&!bridge.occupied(),"Prepared pair did not hand off once");
        uint32_t desired=attack?(success?5u:0u):(success?0u:5u);
        check(w.get<uint32_t>(World::witness+0x80)==desired&&w.rolls==1&&bridge.replay_errors()==0,"Outcome changed without native reselection or rolled twice");
        check(bridge.update(World::witness,1)==0xbeef006f&&w.updates==1,"Resolved animation was held");++cases;
    }
    for(unsigned invalid=1;invalid<=4;++invalid){w.initialize(true,5);w.invalid=invalid;
        NativeBridge bridge(w.reader(),w.selection(),w.ports(),World::engine,{0,5,true},true,true);w.bridge=&bridge;
        auto answer=bridge.prepare(World::witness,1,w.owner());check(!bridge.occupied()&&!bridge.snapshot().active&&w.prepares==1&&w.rolls==1,"Invalid animation emitted a false QTE or prepared twice");
        check((answer&0xff)==(invalid==1?1u:0u),"Native invalid-preparation return changed");
    }
    w.initialize(false,5);NativeBridge canceled(w.reader(),w.selection(),w.ports(),World::engine,{0,5,true},true,true);w.bridge=&canceled;
    canceled.prepare(World::witness,1,w.owner());canceled.release();check(canceled.prepare(World::witness,1,w.owner())==0&&w.prepares==2&&w.resumes==1&&w.rolls==1,"RTS transition failed to refresh stock prepared variant");
    w.initialize(true,5);NativeBridge replaced(w.reader(),w.selection(),w.ports(),World::engine,{0,5,true},true,true);w.bridge=&replaced;
    replaced.prepare(World::witness,1,w.owner());w.put(World::enemy+0x688,World::witness+0x100u);
    check(replaced.update(World::witness,1)==1&&w.resumes==0,"Competing witness was overwritten");
    std::cout<<"PASS: "<<cases<<" prepared-native attack/defence/outcome cases; registration before prompt, clip hold, once-only native handoff, cached role/RNG replay, invalid preparation without prompts, RTS resume and competing ownership guards. Synthetic engine only.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
