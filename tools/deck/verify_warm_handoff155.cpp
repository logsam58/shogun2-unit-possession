
#define S2UP_CONTEXT181_EMBEDDED 1
#include "verify_context_backend181.cpp"
#include "../../native/deck/remote_input155.h"
#include <algorithm>
#include <thread>
#include <vector>

struct CountingInput : ContextSteam181 {
    unsigned starts{},stops{},layouts{};
    bool init(bool value) override {++starts;return ContextSteam181::init(value);}
    bool shutdown() override {++stops;return ContextSteam181::shutdown();}
    bool manifest(const char* path) override {
        assert(std::string(path)=="fixture.vdf");
        ++layouts;
        return ContextSteam181::manifest(path);
    }
};

int wmain(int argc,wchar_t** argv){
    assert(argc==2);
    const HMODULE steam=LoadLibraryW(argv[1]);
    assert(steam);
    auto publish_session=reinterpret_cast<void(__cdecl*)(uint32_t,uint32_t,bool,int)>(GetProcAddress(steam,"FixtureRemote130"));
    assert(publish_session);
    publish_session(17,17,false,14);
    assert(device192::select()&&device192::streaming());
    using remote155::Channel;
    using remote155::clock_us;
    using Result=Channel::Result;
    const auto window=reinterpret_cast<HWND>(1);
    const auto replacement=reinterpret_cast<HWND>(2);
    auto status=BackendState::Idle;
    Frame frame;

    {
        Channel channel;
        Channel::Request old;
        Channel::Request active;
        const uint64_t now=1000000;
        assert(channel.receive(window,Context::RTS,now,frame,status)==Result::Reset);
        assert(channel.request(old));
        assert(!channel.request(active));
        assert(channel.receive(replacement,Context::Campaign,now,frame,status)==Result::Reset);
        assert(!channel.publish(old,{},BackendState::Ready,true));
        assert(channel.request(active)&&active.window==replacement&&active.context==Context::Campaign);
        Frame sampled;
        sampled.epoch=1;sampled.sequence=1;sampled.connected=true;sampled.native_bindings=true;
        sampled.source=Source::SteamInput006;sampled.focused=true;sampled.sample_us=now;
        assert(channel.publish(active,sampled,BackendState::Ready,true));
        assert(channel.receive(replacement,Context::Campaign,now+35001,frame,status)==Result::Unavailable);
        assert(channel.request(active));
        sampled.sample_us=now+50000;
        assert(channel.publish(active,sampled,BackendState::Ready,true));
        assert(channel.receive(replacement,Context::Campaign,now+40000,frame,status)==Result::Unavailable);
        assert(channel.request(active));
        assert(channel.publish(active,sampled,BackendState::Ready,true));
        assert(channel.receive(replacement,Context::Campaign,now+50001,frame,status)==Result::Ready);
        assert(channel.receive(replacement,Context::Campaign,now+50002,frame,status)==Result::Pending);
        assert(channel.receive(replacement,Context::Campaign,now+85001,frame,status)==Result::Unavailable);
        assert(channel.request(active));
        channel.clear();
        assert(!channel.publish(active,sampled,BackendState::Ready,true));
        assert(!channel.request(old));
    }

    Channel channel;
    CountingInput mock;
    mock.remote=17;
    const HANDLE stop=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    const HANDLE initial=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    const HANDLE reply=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    assert(stop&&initial&&reply&&channel.wake());
    std::thread worker([&]{
        SteamBackend sampler;
        assert(sampler.select_context(Context::Campaign));
        assert(sampler.initialize({&mock},"fixture.vdf",true));
        Frame f;
        assert(sampler.sample(clock_us(),true,f));
        assert(SetEvent(initial));
        const HANDLE waits[]{stop,channel.wake()};
        for(;;){
            const auto result=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
            if(result==WAIT_OBJECT_0)break;
            assert(result==WAIT_OBJECT_0+1);
            Channel::Request request;
            if(!channel.request(request))continue;
            assert(sampler.select_context(request.context));
            const bool usable=sampler.sample(clock_us(),true,f);
            if(channel.publish(request,f,sampler.state(),usable))assert(SetEvent(reply));
        }
        assert(sampler.close());
    });
    assert(WaitForSingleObject(initial,5000)==WAIT_OBJECT_0);
    std::vector<uint64_t> timing;
    uint64_t previous_epoch=0,previous_sequence=0;
    for(unsigned round=0;round<100;++round){
        for(const auto context:{Context::RTS,Context::Possession,Context::Menu,Context::Campaign}){
            const auto began=clock_us();
            assert(channel.receive(window,context,began,frame,status)==Result::Reset);
            assert(WaitForSingleObject(reply,5000)==WAIT_OBJECT_0);
            const auto completed=clock_us();
            assert(channel.receive(window,context,completed,frame,status)==Result::Ready);
            assert(status==BackendState::Ready&&frame.sequence>previous_sequence&&frame.epoch>previous_epoch);
            previous_sequence=frame.sequence;
            previous_epoch=frame.epoch;
            timing.push_back(completed-began);
        }
    }
    assert(SetEvent(stop));
    worker.join();
    for(HANDLE h:{stop,initial,reply})assert(CloseHandle(h));
    assert(mock.starts==1&&mock.layouts==1&&mock.stops==1);
    std::sort(timing.begin(),timing.end());
    std::cout<<"PASS warm handoff155: "<<timing.size()<<" first-sample action-set transitions, one Init, one manifest, one final Shutdown"
             <<"; fake API round trip median "<<timing[timing.size()/2]<<" us, p95 "<<timing[timing.size()*95/100]
             <<" us, max "<<timing.back()<<" us\n";
}
