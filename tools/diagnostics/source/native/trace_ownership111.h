#pragma once
#include <array>
#include <cstdint>

// A resumed SEH/APC context can restore a previously programmed DR binding.
// Retain proven bindings until thread exit; elapsed time is not ownership proof.
struct TraceOwnership111 {
    struct Binding {uint32_t address{},kind{},owner{};};
    static constexpr unsigned capacity=64;
    std::array<std::array<Binding,capacity>,4> history{};
    std::array<unsigned,4> counts{};
    static unsigned control(unsigned kind){return kind==1||kind==2?0:13;}
    bool can_remember(unsigned slot,Binding b)const{
        if(slot>=4||!b.address||!b.kind||b.kind>5)return false;
        if(control(b.kind)&&b.address%4)return false;
        for(unsigned i=0;i<counts[slot];++i){const auto& x=history[slot][i];if(x.address==b.address&&x.kind==b.kind&&x.owner==b.owner)return true;}
        return counts[slot]<capacity;
    }
    bool remember(unsigned slot,Binding b){
        if(!can_remember(slot,b))return false;
        for(unsigned i=0;i<counts[slot];++i){const auto& x=history[slot][i];if(x.address==b.address&&x.kind==b.kind&&x.owner==b.owner)return true;}
        history[slot][counts[slot]++]=b;return true;
    }
    const Binding* match(unsigned slot,uint32_t address,uint32_t dr7,uint32_t eip)const{
        if(slot>=4||!(dr7&(1u<<(2*slot)))||(dr7&(2u<<(2*slot))))return nullptr;
        for(unsigned n=counts[slot];n;--n){const auto& b=history[slot][n-1];
            if(b.address==address&&((dr7>>(16+4*slot))&15)==control(b.kind)&&
               (control(b.kind)||eip==address))return &b;
        }
        return nullptr;
    }
};
