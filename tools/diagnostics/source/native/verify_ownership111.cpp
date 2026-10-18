#include "trace_ownership111.h"
#include <iostream>
#include <stdexcept>
#include <string>
static unsigned checks{};
static void check(bool ok){++checks;if(!ok)throw std::runtime_error("ownership assertion "+std::to_string(checks));}
int main(){try{
    TraceOwnership111 h;
    // Both real failures: bit3, old actual DR3, current desired DR3=0.
    for(auto address:{0x09df29acu,0x08b5da4cu}){
        check(h.remember(3,{address,5,address}));
        auto p=h.match(3,address,0xdd000051,0x6aa61268);
        check(p&&p->owner==address&&p->kind==5);
    }
    check(!h.match(3,0x12340000,0xdd000051,0x6aa61268));
    check(!h.match(3,0x09df29ac,0,0x6aa61268));
    check(!h.match(3,0x09df29ac,0xdd000091,0x6aa61268)); // foreign global enable
    check(!h.match(3,0x09df29ac,0x99000051,0x6aa61268)); // foreign width/type
    check(h.remember(0,{0x1448700,1,0}));
    check(h.match(0,0x1448700,1,0x1448700)!=nullptr);
    check(!h.match(0,0x1448700,1,0x1448701));
    check(!h.remember(4,{0x10000,5,0x10000}));
    check(!h.remember(1,{0x10001,5,0x10001}));
    check(!h.remember(1,{0,5,0}));
    check(!h.remember(1,{0x10000,99,0}));
    // No eviction when capacity fills: reject transition before programming.
    for(unsigned i=0;i<TraceOwnership111::capacity;++i)check(h.remember(2,{0x10000+i*4,4,0x20000+i*4}));
    check(!h.can_remember(2,{0x30000,4,0x30000}));
    check(!h.remember(2,{0x30000,4,0x30000}));
    check(h.match(2,0x10000,0x0d000010,0x40000)!=nullptr);
    check(h.remember(2,{0x10000,4,0x20000})); // duplicate consumes no history
    check(h.counts[2]==TraceOwnership111::capacity);
    std::cout<<"PASS "<<checks<<" ownership assertions\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
