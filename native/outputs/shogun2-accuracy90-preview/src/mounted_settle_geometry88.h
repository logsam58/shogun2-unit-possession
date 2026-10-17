#pragma once
#include "mounted_hold_geometry.h"
// Only the exact verified rider-bow asset uses this acquisition gate.
// Existing held-palette math and firearm paths are unchanged.
namespace heldpose {
enum class SettleResult {Waiting,Applied,Rejected};
struct Acquisition88 {
    Cache sample{};uint32_t tick=0;unsigned matches=0;
    void reset(){sample.reset();matches=0;}
};
inline SettleResult apply_settled88(Cache& cache,Acquisition88& acquire,const Key& key,
    float* m,unsigned n,const int32_t* parents,unsigned spine,const unsigned* weapons,
    bool blend_complete,uint32_t native_tick,bool paused){
    auto reject=[&](){cache.reset();acquire.reset();return SettleResult::Rejected;};
    if(!m||!parents||!weapons||!n||n>256||spine>=n)return reject();
    if(cache.valid&&cache.key==key&&cache.count==n&&cache.spine==spine&&!memcmp(cache.weapons,weapons,3*sizeof(unsigned))&&
       !memcmp(cache.hierarchy,parents,n*sizeof(int32_t))){
        return apply(cache,key,m,n,parents,spine,weapons)?SettleResult::Applied:reject();
    }
    cache.reset();
    if(!blend_complete){acquire.reset();return SettleResult::Waiting;}
    if(paused)return SettleResult::Waiting;
    bool prior=acquire.sample.valid&&acquire.sample.key==key&&acquire.sample.count==n&&
        acquire.sample.spine==spine&&!memcmp(acquire.sample.weapons,weapons,3*sizeof(unsigned))&&!memcmp(acquire.sample.hierarchy,parents,n*sizeof(int32_t));
    // Multiple skin/camera evaluations within a native tick are not independent
    // stability observations. Keep the untouched current native palette.
    if(prior&&native_tick==acquire.tick)return SettleResult::Waiting;
    float scratch[256*12];memcpy(scratch,m,n*48);Cache sample;
    if(!apply(sample,key,scratch,n,parents,spine,weapons))return reject();
    bool stable=prior&&uint32_t(native_tick-acquire.tick)==1;
    for(unsigned i=0;stable&&i<n;++i)if(sample.mask[i])for(unsigned c=0;c<12;++c)
        if(std::abs(sample.relative[i*12+c]-acquire.sample.relative[i*12+c])>.0001f){stable=false;break;}
    acquire.matches=stable?acquire.matches+1:0;acquire.tick=native_tick;acquire.sample=sample;
    if(acquire.matches<2)return SettleResult::Waiting;
    cache=sample;acquire.reset();
    return apply(cache,key,m,n,parents,spine,weapons)?SettleResult::Applied:reject();
}
}
