
#define S2UP_CONTEXT181_EMBEDDED 1
#include "verify_context_backend181.cpp"
#include "../../native/deck/ownership177.h"

struct StreamInput : ContextSteam181 {
    static inline unsigned owned{};
    bool permit_init{true};
    unsigned manifests{}, activations{};
    bool init(bool) override {
        if(!permit_init)return false;
        assert(owned==0);
        ++owned;
        return true;
    }
    bool shutdown() override {
        assert(owned==1);
        --owned;
        return true;
    }
    bool manifest(const char* path) override {
        ++manifests;
        return FakeSteam::manifest(path);
    }
    int connected(uint64_t* handles) override {
        const int n=FakeSteam::connected(handles);
        if(n==2)handles[1]=45;
        return n;
    }
    void activate(uint64_t h,uint64_t action) override {
        ++activations;
        FakeSteam::activate(h,action);
    }
};

int wmain(int argc,wchar_t** argv) {
    assert(argc==2);
    HMODULE steam=LoadLibraryW(argv[1]);
    assert(steam);
    auto publish=reinterpret_cast<void(__cdecl*)(uint32_t,uint32_t,bool,int)>(GetProcAddress(steam,"FixtureRemote130"));
    assert(publish);
    publish(17,17,false,14);
    assert(device192::select()&&device192::streaming());

    using deck177::battle_request;
    StreamInput front_api,battle_api;
    SteamBackend front,battle;
    deck177::Lease front_lease,battle_lease;
    Frame frame;
    uint64_t now=1000000;
    assert(battle_request().set(false));
    for(int round=0;round<20;++round) {
        const uint32_t session=17+4*round;
        publish(session,session,false,14);
        front_api.remote=session;
        front_api.current_set=0;
        assert(deck177::claim_frontend(front_lease));
        assert(front.select_context(Context::Campaign));
        assert(front.initialize({&front_api},"fixture.vdf",true));
        assert(front.sample(now,true,frame)&&front_api.current_set==12);

        assert(battle_request().set(true));
        assert(!deck177::claim_frontend(front_lease)&&front_lease.handle);
        assert(!deck177::claim_battle(battle_lease));
        assert(front.close());
        front_lease.release();
        assert(deck177::claim_battle(battle_lease));
        assert(!deck177::claim_frontend(front_lease));

        assert(battle.select_context(Context::RTS));
        battle_api.permit_init=false;
        assert(!battle.initialize({&battle_api},"fixture.vdf",true));
        assert(retry_open(battle.state(),true)&&!retry_open(battle.state(),false));
        assert(!retry_open(BackendState::NoInterface,true));
        battle_api.permit_init=true;
        publish(0,0,false,14);
        battle_api.remote=session;
        battle_api.manifest_ok=false;
        battle_api.manifests=0;
        assert(battle.initialize({&battle_api},"fixture.vdf",true)&&battle.initialized());
        now+=8000;
        assert(!battle.sample(now,true,frame)&&!frame.connected&&battle_api.manifests==0);
        publish(session+1,session+1,false,14);
        battle_api.remote=session+1;
        battle_api.count=2;
        now+=250000;
        assert(!battle.sample(now,true,frame)&&battle.state()==BackendState::AmbiguousDevice);
        assert(battle_api.manifests==0);
        battle_api.count=1;
        now+=8000;
        assert(!battle.sample(now,true,frame)&&battle.state()==BackendState::ManifestRejected);
        assert(battle_api.manifests==1);
        battle_api.manifest_ok=true;
        now+=8000;
        assert(!battle.sample(now,true,frame)&&battle_api.manifests==1);
        now+=500000;
        assert(battle.sample(now,true,frame)&&battle_api.current_set==13);
        assert(battle_api.manifests==2);

        assert(battle.select_context(Context::Possession));
        now+=8000;
        assert(battle.sample(now,true,frame)&&battle_api.current_set==14);
        const auto epoch=frame.epoch;
        const auto activations=battle_api.activations;
        publish(session+2,session+2,false,14);
        battle_api.remote=session+2;
        // Drop the stale controller frame while its new session is unknown.
        now+=240000;
        assert(battle.sample(now,true,frame)==false&&!frame.connected);
        now+=8000;
        assert(battle.sample(now,true,frame)&&frame.epoch>epoch);
        assert(battle_api.activations>activations);

        now+=240000;
        assert(battle.sample(now,true,frame));
        const auto same_handle_epoch=frame.epoch;
        const auto same_handle_activations=battle_api.activations;
        publish(session+3,session+3,false,14);
        battle_api.remote=session+3;
        now+=16000;
        assert(battle.sample(now,true,frame)&&frame.epoch>same_handle_epoch);
        assert(battle_api.activations==same_handle_activations+1);

        assert(battle_request().set(false));
        assert(!deck177::claim_frontend(front_lease));
        assert(battle.close());
        battle_lease.release();

        publish(session+3,session+3,true,14);
        front_api.remote=session+3;
        assert(deck177::claim_frontend(front_lease));
        assert(front.select_context(Context::Campaign));
        assert(front.initialize({&front_api},"fixture.vdf",true));
        now+=8000;
        assert(!front.sample(now,true,frame)&&!frame.connected);
        publish(session+3,session+3,false,14);
        now+=250000;
        assert(front.sample(now,true,frame)&&front_api.current_set==12);
        assert(front.close());
        front_lease.release();
        assert(StreamInput::owned==0);
        now+=8000;
    }
    std::cout<<"PASS remote153: 20 bidirectional owner handoffs with late sessions, duplicate controllers, transient Init/manifest failure, session change, guest rejection and RTS/possession/campaign recovery\n";
}
