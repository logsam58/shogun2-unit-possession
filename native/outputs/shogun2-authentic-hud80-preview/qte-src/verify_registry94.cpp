#define NOMINMAX
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <chrono>
#include "native_registry94.h"
#include "registry93_benchmark.h"
#include "qte_service94.h"
using qte::observation::Registry;
static void check(bool good){if(!good){std::puts("FAIL: Registry94/cadence invariant");std::exit(1);}}
static Registry reg;
static original93::OriginalRegistry93 prior;
static uint32_t address(uint32_t n){return 0x400100+n*0x150;}
int main(){
 check(reg.get(0)==0);auto x=reg.get(address(1));check(x&&reg.get(address(1))==x);auto y=reg.get(address(1),true);check(y>x);
 reg.removed(address(1),x);check(reg.get(address(1))==y);reg.removed(address(1),y);check(reg.get(address(1))>y);
 reg.removed(address(1),reg.get(address(1)));
 // Long churn across every original bucket: compare the original class with
 // probe counting as its only behavioral-neutral instrumentation.
 for(unsigned i=0;i<4096;++i){auto p=prior.get(address(i));prior.removed(address(i),p);}
 auto before=prior.probes_;auto t0=std::chrono::steady_clock::now();
 for(unsigned i=0;i<20000;++i){auto p=prior.get(address(i+4096));prior.removed(address(i+4096),p);}
 double old_ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t0).count();
 const auto old_probes=prior.probes_-before;check(old_probes==20000ull*4096);
 auto m0=reg.metrics94();t0=std::chrono::steady_clock::now();
 for(unsigned i=0;i<250000;++i){auto p=reg.get(address(i+4096));check(p);reg.removed(address(i+4096),p);}
 auto m1=reg.metrics94();double new_ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t0).count();
 check(m1.get_probes-m0.get_probes==250000&&m1.live==0&&m1.max_get_probes==1);
 std::printf("REGISTRY94: old_get_probes=%llu old_operations=20000 old_ms=%.3f new_get_probes=%llu new_operations=250000 new_ms=%.3f capacity=%u\n",old_probes,old_ms,m1.get_probes-m0.get_probes,new_ms,Registry::capacity94);
 // Identical-home clusters exercise middle/front deletion and wraparound.
 std::vector<uint32_t> colliding;for(uint32_t i=1;i<Registry::capacity94;++i)if((((i*2654435761u)&(Registry::capacity94-1)))==Registry::capacity94-2){for(unsigned j=0;j<100;++j)colliding.push_back((i+j*Registry::capacity94)<<4);break;}
 std::unordered_map<uint32_t,uint64_t> expected;
 for(auto p:colliding)expected[p]=reg.get(p);
 for(size_t i=0;i<colliding.size();i+=2){auto p=colliding[i];reg.removed(p,expected[p]);expected.erase(p);for(auto v:expected)check(reg.get(v.first)==v.second);}
 for(auto v:expected)reg.removed(v.first,v.second);check(reg.metrics94().live==0);
 // Randomized independent reference, including rebirth and stale removal.
 uint32_t rng=0x12345678;auto next=[&](){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;};expected.clear();
 for(unsigned i=0;i<120000;++i){uint32_t p=address(next()%6000);auto found=expected.find(p);unsigned op=next()%5;
  if(op<3){bool born=op==0;auto g=reg.get(p,born);check(g);if(found!=expected.end()&&!born)check(g==found->second);if(found!=expected.end()&&born)check(g>found->second);expected[p]=g;}
  else if(found!=expected.end()){if(op==3){reg.removed(p,found->second);expected.erase(p);}else{reg.removed(p,found->second-1);check(reg.get(p)==found->second);}}
 }
 for(auto v:expected)check(reg.get(v.first)==v.second);for(auto v:expected)reg.removed(v.first,v.second);check(reg.metrics94().live==0);
 // Full table, graceful overflow, deletion from a full probe chain, reuse.
 expected.clear();for(unsigned i=0;i<Registry::capacity94;++i){auto p=address(i);auto g=reg.get(p);check(g);expected[p]=g;}
 check(reg.get(address(Registry::capacity94))==0);auto erased=address(2);reg.removed(erased,expected[erased]);expected.erase(erased);
 for(auto v:expected)check(reg.get(v.first)==v.second);check(reg.get(address(Registry::capacity94))!=0);
 for(auto v:expected)reg.removed(v.first,v.second);reg.removed(address(Registry::capacity94),reg.get(address(Registry::capacity94)));check(reg.metrics94().live==0);
 // Six native-worker-like users: disjoint lifetimes plus shared stable identity.
 const auto shared=reg.get(0x70);std::atomic<unsigned> failures{0};std::vector<std::thread> workers;
 for(unsigned t=0;t<6;++t)workers.emplace_back([&,t](){for(unsigned i=0;i<30000;++i){uint32_t p=0x10000000+t*0x100000+(i%4000)*16;auto g=reg.get(p,true);if(!g||reg.get(p)!=g||reg.get(0x70)!=shared)++failures;reg.removed(p,g-1);if(reg.get(p)!=g)++failures;reg.removed(p,g);}});
 for(auto& t:workers)t.join();check(failures==0&&reg.metrics94().live==1);reg.removed(0x70,shared);check(reg.metrics94().live==0);
 qte::ServiceCadence94 cadence;unsigned services=0;
 for(unsigned i=0;i<100000;++i)services+=cadence.due(i*.001);check(services>=11000&&services<=12501);
 check(cadence.due(101));check(!cadence.due(101.001));check(cadence.due(101.001,true));check(!cadence.due(101.002));check(cadence.due(200));check(!cadence.due(200.001));
 std::printf("PASS: Registry94 original slowdown reproduced; 250000 no-growth churn cycles, collision/wrap/full/ABA/reference cases, 180000 concurrent lifetimes and 100000 fast-poll cadence samples; service_runs=%u\n",services);
}
