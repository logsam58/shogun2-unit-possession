#pragma once
// No wall clock. Advance only at unique native animation-update ticks.
namespace mountedblend88 {
struct Key {
    uintptr_t actor,animation;unsigned lease,draw;
    bool operator==(const Key& b)const{return actor==b.actor&&animation==b.animation&&lease==b.lease&&draw==b.draw;}
};
struct Clock {
    bool valid=false;Key key{};uint32_t tick=0;float elapsed=0;
    void reset(){valid=false;elapsed=0;}
    void begin(const Key& k,uint32_t native_tick){
        reset();key=k;tick=native_tick;valid=k.actor&&k.animation;
    }
    void advance(const Key& k,uint32_t native_tick,float delta){
        if(!valid||!(key==k))return;
        if(!std::isfinite(delta)||delta<0||delta>.25f){reset();return;}
        if(native_tick==tick)return;
        uint32_t distance=native_tick-tick;tick=native_tick;
        if(distance>0x7fffffffU){reset();return;}
        // A missed update is not permission to infer time at an unknown speed.
        // Zero delta and repeated render callbacks cannot age the blend.
        if(distance==1&&delta>0)elapsed=std::min(1.f,elapsed+delta);
    }
    bool ready(const Key& k)const{return valid&&key==k&&elapsed>=.5f-.000001f;}
};
}
