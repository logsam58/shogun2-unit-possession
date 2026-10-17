#include "native_battle_clock.h"
#include "qte_timeline.h"
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
static void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
static bool close(double a,double b){return std::abs(a-b)<1e-6;}
struct Memory {
    static constexpr uint32_t engine=0x10000000,env=0x10000,common=0x20000,phase=0x30000;
    static constexpr uint32_t alliances=0x40000,alliance=0x50000,armies=0x60000,first=0x70000,second=0x80000;
    std::map<uint32_t,unsigned char> bytes;unsigned tick_reads{};bool tear{},null_second_pass{};unsigned alliance_reads{};
    template<class T>void put(uint32_t address,T value){const auto* p=reinterpret_cast<const unsigned char*>(&value);for(unsigned i=0;i<sizeof value;++i)bytes[address+i]=p[i];}
    static bool read(void* p,uint32_t address,void* out,size_t size){auto& m=*static_cast<Memory*>(p);
        auto* dst=static_cast<unsigned char*>(out);for(size_t i=0;i<size;++i){auto it=m.bytes.find(address+uint32_t(i));if(it==m.bytes.end())return false;dst[i]=it->second;}
        if(address==common+0x50&&++m.tick_reads==2&&m.tear){uint32_t tick=18;memcpy(out,&tick,4);}
        if(address==alliances&&++m.alliance_reads==2&&m.null_second_pass){uint32_t zero=0;memcpy(out,&zero,4);}
        return true;}
    qte::Reader reader(){return {read,this};}
    void setup(){put(env+8,common);put(common+0xb4,phase);put(common+0x50,17u);put(phase+0x14,5u);
        put(engine+0x15b2448,.1f);put(engine+0x15ac9c8,1.f);put(engine+0x15c2764,100.f);
        put(phase+0x88,1u);put(phase+0x8c,alliances);put(alliances,alliance);put(alliance+0x20,2u);put(alliance+0x24,armies);
        put(armies,first);put(armies+4,second);for(auto army:{first,second}){put(army+0x270,uint8_t(1));put(army+0x114,160u);put(army+0x12c,0u);put(army+0x290,1.f);}}
};
int main(){try{using namespace qte;unsigned cases=0;
    struct Rates {float a,b,expected;};
    for(auto rates:{Rates{0,0,0},Rates{.5f,.2f,.5f},Rates{2,4,2},Rates{2,.5f,1},Rates{0,1,1},Rates{1,4,1}}){
        Memory m;m.setup();m.put(Memory::first+0x290,rates.a);m.put(Memory::second+0x290,rates.b);NativeClock clock;
        check(battle_clock(m.reader(),Memory::env,Memory::engine,clock)&&clock.speed==rates.expected&&clock.tick==17,"Native aggregate speed differs");++cases;
    }
    {Memory m;m.setup();m.put(Memory::first+0x114,0u);m.put(Memory::first+0x290,0.f);m.put(Memory::second+0x290,2.f);NativeClock c;
        check(battle_clock(m.reader(),Memory::env,Memory::engine,c)&&c.speed==2,"Empty human army forced pause");
        m.put(Memory::second+0x114,0u);check(battle_clock(m.reader(),Memory::env,Memory::engine,c)&&c.speed==1,"All-empty mixed requests differ");
        m.put(Memory::first+0x270,uint8_t(0));check(battle_clock(m.reader(),Memory::env,Memory::engine,c)&&c.speed==2,"AI rate contaminated human clock");}
    for(unsigned fault=0;fault<10;++fault){Memory m;m.setup();
        if(fault==0)m.put(Memory::phase+0x88,257u);
        if(fault==1)m.put(Memory::phase+0x8c,UINT32_MAX-2);
        if(fault==2)m.put(Memory::alliance+0x20,257u);
        if(fault==3)m.put(Memory::first+0x114,4097u);
        if(fault==4)m.put(Memory::second+0x290,NAN);
        if(fault==5)m.put(Memory::phase+0x14,4u);
        if(fault==6)m.bytes.erase(Memory::second+0x270);
        if(fault==7)m.tear=true;
        if(fault==8)m.null_second_pass=true;
        if(fault==9)m.put(Memory::engine+0x15b2448,0.f);
        NativeClock c{9,8,7,6,5,4};check(!battle_clock(m.reader(),Memory::env,Memory::engine,c),"Invalid clock accepted");
        check(c.environment==9&&c.speed==4,"Failed clock published partial values");}
    for(double speed:{.1,.5,1.,2.,4.})for(unsigned run=0;run<200;++run){
        Timeline timeline;double previous=0;
        for(unsigned frame=0;frame<600;++frame){double wall=frame/120.;uint64_t tick=100+uint64_t(std::floor(wall*speed/.1+1e-8));
            auto p=timeline.observe({1,tick,wall,.1,speed});double floor=double(tick-100)*.1;
            check(p.valid&&p.simulation>=previous&&p.simulation>=floor-1e-6&&p.simulation<=floor+.100001,"Timeline exceeds native horizon or regresses");
            check(interpolate(p,wall+.01)>=p.simulation&&interpolate(p,wall+.01)<=p.horizon,"Render forecast unbounded");previous=p.simulation;}
        ++cases;
    }
    {Timeline t;t.observe({1,0,0,.1,1});auto p=t.observe({1,1,.1,.1,1});auto pause=t.observe({1,1,.1,.1,0});
        check(close(pause.simulation,p.simulation)&&pause.paused,"Pause jumped");
        check(close(t.observe({1,1,60,.1,0}).simulation,pause.simulation),"Paused wall time advanced QTE");
        auto resume=t.observe({1,1,60,.1,1});check(close(resume.simulation,pause.simulation),"Resume charged paused time");
        auto next=t.observe({1,1,60.02,.1,1});check(close(next.simulation,resume.simulation+.02),"Resume clock froze");
        auto fast=t.observe({1,1,60.02,.1,4});check(close(fast.simulation,next.simulation),"Rate transition jumped");
        check(!t.observe({1,1,61,.1,4}).valid,"Stalled native clock remained active");
        check(!t.observe({1,0,62,.1,1}).valid,"Backward native tick accepted");
        auto newbattle=t.observe({2,0,62,.1,1});check(newbattle.valid&&newbattle.simulation==0,"Battle reset inherited time");
        check(!t.observe({2,0,61,.1,1}).valid&&!t.observe({2,0,62,.05,1}).valid,"Invalid timeline admitted");}
    {ClickQueue<2> queue;Click c;queue.push({1,1,.1,true,false,true,false});queue.push({1,1,.2,false,false,true,false});
        check(!queue.push({1,1,.3,true,false,true,false})&&queue.overflowed(),"Incomplete queue history not flagged");
        check(queue.pop(c)&&c.left&&c.simulation==.1&&queue.pop(c)&&!c.left&&c.simulation==.2&&!queue.pop(c),"FIFO input changed");queue.clear();check(!queue.overflowed()&&queue.push({}),"Queue reset failed");}
    std::cout<<"PASS: "<<cases<<" aggregate-clock/timeline cases, 600,000 interpolated frames; pause/resume, speed changes, stale clock, torn snapshot and bounded input queue. Synthetic memory only.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
