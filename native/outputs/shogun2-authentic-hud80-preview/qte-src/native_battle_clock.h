#pragma once
#include "native_unit_context.h"
namespace qte {
struct NativeClock {
    uint32_t environment{},common{},phase{},tick{};
    float step{},speed{};
};
inline bool battle_clock(const Reader& r,uint32_t environment,uint32_t engine,NativeClock& out){
    if(!environment||environment>UINT32_MAX-12||engine>UINT32_MAX-0x1bc8184)return false;
    uint32_t common{},phase{},id{},tick{},count{},alliances{};
    float step{},one{},closest{};
    if(!r.get(environment+8,common)||!common||common>UINT32_MAX-0xb8||
       !r.get(common+0xb4,phase)||!phase||phase>UINT32_MAX-0x100||
       !r.get(phase+0x14,id)||id<5||id>8||!r.get(common+0x50,tick)||
       !r.get(engine+0x15b2448,step)||!std::isfinite(step)||step<.001f||step>.2f||
       !r.get(engine+0x15ac9c8,one)||one!=1.f||
       !r.get(engine+0x15c2764,closest)||!std::isfinite(closest)||closest<=0||
       !r.get(phase+0x88,count)||count>256||!r.get(phase+0x8c,alliances)||
       (count&&(!alliances||alliances>UINT32_MAX-count*4)))return false;
    // Exact read-only logic of native getter321880 ->17a420. It considers
    // human armies with units/ships, or all human armies if all are empty,
    // picks the requested rate closest to1 and resolves mixed faster/slower
    // requests to1. No native functions are called from the render/input path.
    bool all_empty=true;
    for(uint32_t i=0;i<count;++i){uint32_t alliance{},armies{},n{};
        if(!r.get(alliances+i*4,alliance)||!alliance||alliance>UINT32_MAX-0x28||
           !r.get(alliance+0x20,n)||n>256||!r.get(alliance+0x24,armies)||
           (n&&(!armies||armies>UINT32_MAX-n*4)))return false;
        for(uint32_t j=0;j<n;++j){uint32_t army{},units{},ships{};uint8_t human{};
            if(!r.get(armies+j*4,army)||!army||army>UINT32_MAX-0x294||
               !r.get(army+0x270,human))return false;
            if(human){if(!r.get(army+0x114,units)||!r.get(army+0x12c,ships)||units>4096||ships>4096)return false;
                if(units+ships)all_empty=false;}
        }
    }
    float result=1.f;unsigned faster=0,slower=0;
    for(uint32_t i=0;i<count;++i){uint32_t alliance{},armies{},n{};
        if(!r.get(alliances+i*4,alliance)||!alliance||alliance>UINT32_MAX-0x28||!r.get(alliance+0x20,n)||n>256||
           !r.get(alliance+0x24,armies)||(n&&(!armies||armies>UINT32_MAX-n*4)))return false;
        for(uint32_t j=0;j<n;++j){uint32_t army{},units{},ships{};uint8_t human{};float rate{};
            if(!r.get(armies+j*4,army)||!army||army>UINT32_MAX-0x294||!r.get(army+0x270,human))return false;
            if(!human)continue;
            if(!all_empty){if(!r.get(army+0x114,units)||!r.get(army+0x12c,ships)||units>4096||ships>4096)return false;if(!units&&!ships)continue;}
            if(!r.get(army+0x290,rate)||!std::isfinite(rate)||rate<0||rate>16)return false;
            float distance=std::abs(rate-one);
            if(closest>distance)result=rate;
            closest=std::min(distance,closest);
            if(rate>one)++faster;else ++slower;
        }
    }
    if(faster&&slower)result=1.f;
    uint32_t current_common{},current_phase{},current_tick{};
    if(!r.get(environment+8,current_common)||current_common!=common||
       !r.get(common+0xb4,current_phase)||current_phase!=phase||
       !r.get(common+0x50,current_tick)||current_tick!=tick)return false;
    out={environment,common,phase,tick,step,result};return true;
}
}
