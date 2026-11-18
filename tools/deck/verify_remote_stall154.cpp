
#define S2UP_CONTEXT181_EMBEDDED 1
#include "verify_context_backend181.cpp"

struct StalledInput : ContextSteam181 {
    bool asleep{},partial{},heal_on_refresh{};
    unsigned starts{},stops{},manifests{};
    bool init(bool manual) override {++starts;return ContextSteam181::init(manual);}
    bool shutdown() override {++stops;return ContextSteam181::shutdown();}
    bool manifest(const char* path) override {
        assert(std::string(path)=="fixture.vdf");
        ++manifests;
        if(manifests>1&&manifest_ok&&heal_on_refresh)asleep=false;
        return ContextSteam181::manifest(path);
    }
    steam006::Digital digital(uint64_t handle,uint64_t action) override {
        if(asleep&&(!partial||action%1000!=1))return {};
        return ContextSteam181::digital(handle,action);
    }
    steam006::Analog analog(uint64_t handle,uint64_t action) override {
        return asleep?steam006::Analog{}:ContextSteam181::analog(handle,action);
    }
};

int wmain(int argc,wchar_t** argv) {
    assert(argc==3);
    HMODULE steam=LoadLibraryW(argv[1]);
    assert(steam);
    const bool remote=argv[2][0]==L'R';
    auto publish=reinterpret_cast<void(__cdecl*)(uint32_t,uint32_t,bool,int)>(GetProcAddress(steam,"FixtureRemote130"));
    auto mode=reinterpret_cast<void(__cdecl*)(int)>(GetProcAddress(steam,"FixtureMode192"));
    assert(publish&&mode);
    mode(remote?0:1);
    publish(remote?17:0,remote?17:0,false,14);
    assert(device192::select()&&device192::enabled()&&device192::streaming()==remote);

    unsigned cases=0;
    for(auto context:{Context::RTS,Context::Campaign,Context::Possession}) {
        StalledInput api;
        api.remote=remote?17:0;
        SteamBackend backend;
        assert(backend.select_context(Context::Menu));
        assert(backend.initialize({&api},"fixture.vdf",true));
        uint64_t now=1000000;
        Frame frame;
        auto tick=[&](bool focused=true){
            now+=100000;
            return backend.sample(now,focused,frame);
        };
        assert(tick());
        const auto before=frame.epoch;
        assert(backend.select_context(context));
        api.asleep=true;
        api.heal_on_refresh=true;
        assert(!tick());
        for(unsigned i=0;i<59;++i){
            assert(!tick()&&api.manifests==1&&backend.state()==BackendState::WaitingBindings);
        }
        assert(!tick()&&api.manifests==(remote?2u:1u));
        assert(api.starts==1&&api.stops==0&&backend.initialized());
        if(!remote)api.asleep=false; // the local backend keeps its existing policy
        api.buttons=bit(Button::A);
        api.r2=.75f;
        assert(tick()&&frame.epoch>before);
        Normalizer filter;
        Edges edges;
        assert(!filter.accept(frame,now,{},edges)&&!edges.right_fire&&!edges.down);
        api.buttons=0;
        api.r2=0;
        assert(tick()&&filter.accept(frame,now,{},edges));
        api.buttons=bit(Button::A);
        assert(tick()&&filter.accept(frame,now,{},edges)&&(edges.down&api.buttons));
        assert(backend.close()&&api.stops==1);
        ++cases;
    }

    for(unsigned scenario=0;scenario<6;++scenario){
        StalledInput api;
        api.remote=remote?17:0;
        SteamBackend backend;
        assert(backend.select_context(Context::RTS));
        assert(backend.initialize({&api},"fixture.vdf",true));
        uint64_t now=1000000;
        Frame frame;
        auto tick=[&](bool focus){
            now+=100000;
            return backend.sample(now,focus,frame);
        };
        assert(tick(true));
        api.asleep=true;
        api.manifest_ok=scenario!=0;
        api.partial=scenario==2;
        if(scenario==3)api.disconnected=true;
        if(scenario==4){
            api.asleep=false;
            api.bad_origin=int(Button::B);
        }
        if(scenario==5)publish(17,17,true,14);
        for(unsigned i=0;i<150;++i)assert(!tick(scenario!=1));
        // Failure is bounded to one attempt. Background, partial layout, missing
        // controller, wrong physical origin and guest mode leave the layout alone.
        assert(api.manifests==(remote&&scenario==0?2u:1u));
        publish(remote?17:0,remote?17:0,false,14);
        assert(backend.close()&&api.stops==1);
        ++cases;
    }
    std::cout<<"PASS binding recovery154: "<<cases<<" cases, "<<(remote?"Remote Play":"local Deck")
             <<", original role/manifest only, no input sent\n";
}
