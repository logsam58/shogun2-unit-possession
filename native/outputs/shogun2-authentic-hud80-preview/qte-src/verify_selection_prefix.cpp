#include "native_selection_prefix.h"
#include <iostream>
#include <stdexcept>
#include <vector>
static void check(bool value,const char* message){if(!value)throw std::runtime_error(message);}
struct Fixture {
    static constexpr uint32_t start=0x10000,engine=start,witness=0x1b00000;
    static constexpr uint32_t environment=witness+0x1000,common=witness+0x2000;
    static constexpr uint32_t context=witness+0x3000,actor1=witness+0x4000,actor2=witness+0x5000;
    std::vector<unsigned char> data=std::vector<unsigned char>(0x1b00000);
    unsigned sorts{},selections{};bool alternate{},mounted{};
    template<class T>void put(uint32_t address,T value){check(address>=start&&address-start+sizeof value<=data.size(),"Fixture write bound");memcpy(data.data()+address-start,&value,sizeof value);}
    static bool read(void* p,uint32_t address,void* out,size_t bytes){auto& f=*static_cast<Fixture*>(p);
        if(address<start||bytes>f.data.size()||address-start>f.data.size()-bytes)return false;
        memcpy(out,f.data.data()+address-start,bytes);return true;}
    qte::Reader reader(){return {read,this};}
    void setup(bool alternate_input,bool mount){
        sorts=selections=0;alternate=alternate_input;mounted=mount;
        put(witness+4,environment);put(environment+8,common);put(common+0x9c,context);
        put(witness+0xc,uint8_t(0));put(witness+0xd0,uint8_t(0));put(witness+0xee,uint8_t(alternate));
        put(witness+0x24,2u);put(witness+0x38,2u);
        put(witness+0x10,actor1);put(witness+0x14,actor2);put(witness+0x30,actor1);put(witness+0x34,actor2);
        for(unsigned axis=0;axis<3;++axis){put(engine+0x1a4dc0c+axis*4,0.f);put(actor1+0x48+axis*4,float(axis));put(actor2+0x48+axis*4,float(axis+2));}
        put(actor1+0xf0,uint8_t(1));put(actor2+0xf0,uint8_t(1));
        put(actor1+0x21c,0u);put(actor2+0x21c,mount?1u:0u);
    }
    static bool sort(void* p,uint32_t begin,uint32_t end,uint32_t count,const float* center){auto& f=*static_cast<Fixture*>(p);
        check(begin==witness+(f.alternate?0x30:0x10)&&end==begin+8&&count==2,"Native sort arguments mismatch");
        for(unsigned axis=0;axis<3;++axis)check(center[axis]==float(axis+1),"Native mean mismatch");
        // Native sorting supplies the first actor used for the mount flag.
        f.put(begin,actor2);f.put(begin+4,actor1);++f.sorts;return true;}
    static bool select(void* p,uint32_t ctx,uint32_t begin,uint32_t end,uint32_t flags,uint32_t flags2,qte::NativeVerdict& output){auto& f=*static_cast<Fixture*>(p);
        check(f.sorts==1&&ctx==context&&begin==witness+(f.alternate?0x30:0x10)&&end==begin+8&&flags==unsigned(!f.alternate&&f.mounted)&&flags2==0,"Native selector arguments mismatch");
        ++f.selections;output={5,actor2,actor1};return true;}
    qte::SelectionCalls calls(){return {this,sort,select};}
};
int main(){try{using namespace qte;Fixture fixture;unsigned cases=0;
    for(unsigned i=0;i<1000;++i)for(bool alternate:{false,true})for(bool mounted:{false,true}){
        fixture.setup(alternate,mounted);SelectionInput input;NativeVerdict selected;
        check(selection_input(fixture.reader(),Fixture::witness,Fixture::engine,input),"Native prefix input rejected");
        check(select_once(fixture.reader(),fixture.calls(),input,selected)&&fixture.selections==1,"Native selector not called exactly once");
        SelectorReplay replay;check(replay.arm(input,selected),"Replay arm failed");NativeVerdict output{99,98,97};
        check(!replay.take(input.witness+4,0x3e5f7c,input.context,input.begin,input.end,input.flags,0,output),"Foreign witness stole replay");
        check(!replay.take(input.witness,0x433b80,input.context,input.begin,input.end,input.flags,0,output),"Foreign return site stole replay");
        check(!replay.take(input.witness,0x3e5f7c,input.context+4,input.begin,input.end,input.flags,0,output),"Wrong context accepted");
        check(!replay.take(input.witness,0x3e5f7c,input.context,input.begin,input.end,input.flags,1,output),"Wrong native flags accepted");
        check(replay.take(input.witness,0x3e5f7c,input.context,input.begin,input.end,input.flags,0,output),"Original prepare could not replay");
        check(memcmp(&output,&selected,sizeof output)==0&&fixture.selections==1,"Native role pointers/output bytes or RNG call count changed");
        check(!replay.take(input.witness,0x3e5f7c,input.context,input.begin,input.end,input.flags,0,output),"Native result replayed twice");
        check(replay.finish()&&!replay.finish(),"Replay completion mismatch");++cases;
    }
    for(unsigned fault=0;fault<6;++fault){fixture.setup(false,false);
        if(fault==0)fixture.put(Fixture::witness+0xc,uint8_t(1));
        if(fault==1)fixture.put(Fixture::witness+0xd0,uint8_t(1));
        if(fault==2)fixture.put(Fixture::witness+0x24,6u);
        if(fault==3)fixture.put(Fixture::actor1+0xf0,uint8_t(0));
        if(fault==4)fixture.put(Fixture::actor1+0x48,std::numeric_limits<float>::quiet_NaN());
        if(fault==5)fixture.put(Fixture::common+0x9c,0u);
        SelectionInput input;check(!selection_input(fixture.reader(),Fixture::witness,Fixture::engine,input)&&fixture.sorts==0&&fixture.selections==0,"Invalid prefix consumed native work");
    }
    std::cout<<"PASS: "<<cases<<" prepare-prefix/replay fixtures; native mean/order/context/flags contract, exact pointer-role bytes, one selector call, strict replay scope and invalid-prefix guards. Native callbacks are stubs; no game hooks or live RNG test.\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
