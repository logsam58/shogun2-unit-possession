#pragma once
#include "../../native/deck/steam_input174.h"
#include <cassert>
#include <cstring>
namespace deck174 {
// Real C++ virtual member ABI, including packed structure returns. Unused slots
// remain explicit so the tested indices cannot silently slide by one.
struct FakeSteam {
    bool closed{},manifest_ok{true},manifest_called{};int type{14},count{1},bad_origin{-1},gamepad{-1};bool disconnected{};
    int trigger_origins{};uint64_t buttons{};float l2{},r2{};
    uint32_t remote{};uint64_t current_set{};
    virtual bool init(bool){return true;} // 0
    virtual bool shutdown(){closed=true;return true;}
    virtual bool manifest(const char*){manifest_called=true;return manifest_ok;}
    virtual void run(bool){}
    virtual void s4(){} virtual void s5(){}
    virtual int connected(uint64_t* p){p[0]=44;return disconnected?0:count;}
    virtual void s7(){} virtual void s8(){}
    virtual uint64_t set(const char*){return 3;}
    virtual void activate(uint64_t,uint64_t a){current_set=a;}
    virtual uint64_t current(uint64_t){return current_set;}
    virtual void s12(){}virtual void s13(){}virtual void s14(){}virtual void s15(){}
    virtual uint64_t digital_handle(const char* n){for(unsigned i=0;i<button_count;++i)if(!strcmp(n,steam006::digital[i].name))return i+1;return 0;}
    virtual steam006::Digital digital(uint64_t,uint64_t action){return {(buttons&(uint64_t(1)<<(action-1)))!=0,true};}
    virtual int origins(uint64_t,uint64_t,uint64_t a,int* o){o[0]=int(a)-1==bad_origin?333:steam006::digital[a-1].origin;return 1;}
    virtual void s19(){}
    virtual uint64_t analog_handle(const char* n){for(unsigned i=0;i<6;++i)if(!strcmp(n,steam006::analog[i]))return i+100;return 0;}
    virtual steam006::Analog analog(uint64_t,uint64_t a){return {a>=104?10:6,a==104?l2:a==105?r2:0,0,true};}
    virtual int aorigins(uint64_t,uint64_t,uint64_t a,int* o){
        o[0]=steam006::analog_origin[a-100];
        if(a>=104){
            if(trigger_origins==1)++o[0];
            if(trigger_origins==2){o[1]=o[0]+1;return 2;}
            if(trigger_origins==3)o[0]=a==104?357:355;
        }return 1;}
    virtual const char* s23(int,int,uint32_t){return "owned-glyph-fixture.png";}virtual void s24(){}virtual void s25(){}virtual void s26(){}virtual void s27(){}virtual void s28(){}
    virtual steam006::MotionData motion(uint64_t){return {0,0,0,1,0,0,16383.5f,100,200,300};}
    virtual void s30(){}virtual void s31(){}virtual void s32(){}virtual void s33(){}virtual void s34(){}virtual void s35(){}virtual bool s36(uint64_t h){assert(h==44);return true;}
    virtual int input_type(uint64_t){return type;}
    virtual void s38(){}
    virtual int index(uint64_t){return gamepad;}
    virtual void s40(){}virtual void s41(){}virtual void s42(){}virtual void s43(){}virtual void s44(){}
    virtual uint32_t remote_session(uint64_t){return remote;}
    virtual void s46(){}virtual void s47(){}
};

}
