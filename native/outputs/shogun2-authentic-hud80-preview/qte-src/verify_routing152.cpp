#define PREPARED_FIXTURE_ENTRY retained_prepared152
#include "verify_prepared_bridge.cpp"
#undef PREPARED_FIXTURE_ENTRY
#include "qte_session.h"
static unsigned routed_calls152;
static bool route_fact152(void* p,uint32_t unit,bool& routed){auto& w=*static_cast<PreparedWorld*>(p);auto state=w.get<uint32_t>(unit+0xcec);routed=state==2||state==3;return true;}
static uint32_t route_prepare_test152(void* p,uint32_t address,const qte::NativeActor& player,const qte::NativeActor& enemy,const qte::NativeVerdict& desired,const qte::NativePlan* plans){
    auto& w=*static_cast<PreparedWorld*>(p);++routed_calls152;
    check(!w.bridge->occupied(),"Routing created/retained a QTE hold");bool broken=false;route_fact152(p,player.unit,broken);
    check(desired.outcome==5&&desired.attacker_address==(broken?enemy.address:player.address)&&desired.defender_address==(broken?player.address:enemy.address),"Routing native roles/outcome wrong");
    if(plans){check(plans[0].actor==player.address&&plans[1].actor==enemy.address,"Routing lost original pair plan");w.put(address+0xc,uint8_t(0));}
    return PreparedWorld::prepare(p,address);
}
struct RouteSession152 {
 World world;qte::Session* session{};
 static bool routing(void* p,uint32_t unit,bool& value){auto& f=*static_cast<RouteSession152*>(p);auto state=f.world.get<uint32_t>(unit+0xcec);value=state==2||state==3;return true;}
 static uint32_t prepare(void* p,uint32_t address){auto& f=*static_cast<RouteSession152*>(p);++f.world.prepares;qte::NativeVerdict selected;check(f.session->try_replay(address,0x3e5f7c,World::context,address+0x10,address+0x18,0,0,selected),"Session routing replay missing");f.world.committed=selected;f.world.put(address+0xc,uint8_t(1));return 0;}
 static uint32_t update(void* p,uint32_t address){return World::update(&static_cast<RouteSession152*>(p)->world,address);}
 static uint32_t routed(void* p,uint32_t address,const qte::NativeActor&,const qte::NativeActor&,const qte::NativeVerdict&,const qte::NativePlan*){auto& f=*static_cast<RouteSession152*>(p);check(!f.session->occupied(),"Public Session retained routed prompt");return prepare(p,address);}
 qte::BridgeCalls ports(){return {this,prepare,update,routing,nullptr,nullptr,nullptr,nullptr,routed};}
};
int main(){try{using namespace qte;PreparedWorld w;unsigned cases=0;
    for(bool attack:{false,true})for(bool during:{false,true})for(unsigned own=0;own<4;++own)for(unsigned enemy=0;enemy<4;++enemy){
        w.initialize(attack,0);w.put(World::unit1+0xcec,during?0u:own);w.put(World::unit2+0xcec,during?0u:enemy);
        auto ports=w.ports();ports.routing=route_fact152;ports.routed152=route_prepare_test152;
        NativeBridge b(w.reader(),w.selection(),ports,World::engine,{0,5,true},true,true,true);w.bridge=&b;auto owner=w.owner();auto before=routed_calls152;
        check(b.prepare(World::witness,1,owner)==0,"Routing prepare failed");
        if(during){check(b.occupied(),"Normal encounter missing prompt");w.put(World::unit1+0xcec,own);w.put(World::unit2+0xcec,enemy);b.frame(owner,.01,false,true,b.snapshot().serial,false,false);}
        bool routed=own>=2||enemy>=2;
        if(routed){check(routed_calls152==before+1&&!b.occupied()&&!b.snapshot().active&&w.committed.outcome==5&&w.rolls==1&&b.replay_errors()==0,"Routing did not bypass prompt with one native selection");
            check(w.committed.defender_address==(own>=2?World::player:World::enemy),"Routing did not choose correct loser");}
        else check(routed_calls152==before&&b.occupied()&&b.snapshot().active&&w.committed.outcome==0,"Unbroken/wavering combat changed");
        ++cases;
    }
    // Public Session returns the native transition answer once on the same
    // prepare/update callback, without re-entering the newly active witness.
    for(bool update:{false,true}){
        RouteSession152 f;f.world.setup(true,0);f.world.put(World::unit1+0xcec,0u);f.world.put(World::unit2+0xcec,0u);
        Session session(f.world.reader(),f.world.selection(),f.ports(),World::engine,{0,5,true},true,true,false,true);f.session=&session;auto owner=f.world.owner();
        check(session.prepare(World::witness,1,owner,{1,0,200,.1,1},true)==0&&session.occupied(),"Session transition baseline");
        f.world.put(World::unit1+0xcec,2u);auto answer=update?session.update(World::witness,1,owner,{1,1,200.1,.1,1},true):session.prepare(World::witness,1,owner,{1,1,200.1,.1,1},true);
        check(answer==0&&!session.occupied()&&f.world.prepares==1&&f.world.rolls==1&&f.world.committed.defender_address==World::player,"Same-callback route answer lost or native preparation repeated");
    }
    for(bool own_route:{false,true}){w.initialize(true,0);w.put(World::unit1+0xcec,own_route?2u:0u);w.put(World::unit2+0xcec,own_route?0u:3u);auto ports=w.ports();ports.routing=route_fact152;ports.routed152=route_prepare_test152;NativeBridge b(w.reader(),w.selection(),ports,World::engine,{0,5,true},true,true,true);w.bridge=&b;check(b.prepare(World::witness,1,w.owner(),false)==0&&!b.occupied()&&w.rolls==1,"Routing blocked by QTE cooldown");}
    std::cout<<"PASS: "<<cases<<" original selected-pair routing cases: broken/shattered player loses, enemy loses, both prioritise player defeat; initial/mid-prompt, one selection, no prompt/hold, ordinary non-routing unchanged. Native preparation/playback are explicit fixture ports.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
