#pragma once
#include "cone_geometry.h"
// Native RVA 0x4272e0: thiscall, RNG pointer plus four floats, ret 0x14.
using Dispersion=void(__thiscall*)(void*,uint32_t*,float,float,float,float);
static Dispersion original_dispersion;
static __declspec(thread) void* manual_dispersion_launch;
static uint32_t manual_spread_removed;
static __declspec(thread) bool manual_cone_override90;
static __declspec(thread) float manual_cone_angle90;
struct ConeMetrics {uint32_t shots{},errors{};float half_angle{},horizontal{},vertical{};};
static __declspec(thread) ConeMetrics cone_metrics;
static volatile LONG cone_shot_total,cone_error_total;
static void __fastcall dispersion_hook(void* launch,void*,uint32_t* rng,float horizontal,float vertical,float multiplier,float deviation){
    bool manual=launch==manual_dispersion_launch;
    float initial[3]{};uint32_t seed=0;bool captured=false;
    if(manual){__try {memcpy(initial,static_cast<unsigned char*>(launch)+0x50,sizeof initial);seed=*rng;captured=true;}
        __except(EXCEPTION_EXECUTE_HANDLER){++cone_metrics.errors;InterlockedIncrement(&cone_error_total);}}
    // Native accuracy calculation, RNG advancement and all other actors stay intact.
    original_dispersion(launch,rng,horizontal,vertical,multiplier,deviation);
    if(manual&&captured){
        float angle=manual_cone_override90?manual_cone_angle90:.5f*std::fmax(std::abs(horizontal),std::abs(vertical))*std::abs(multiplier),result[3];
        float u=native_random_sample(seed),v=native_random_sample(seed);
        if(!std::isfinite(horizontal)||!std::isfinite(vertical)||!std::isfinite(multiplier)||
           !cone_velocity(initial,angle,u,v,result)){++cone_metrics.errors;InterlockedIncrement(&cone_error_total);return;}
        __try {memcpy(static_cast<unsigned char*>(launch)+0x50,result,sizeof result);
            ++cone_metrics.shots;InterlockedIncrement(&cone_shot_total);cone_metrics.half_angle=angle;cone_metrics.horizontal=horizontal;cone_metrics.vertical=vertical;}
        __except(EXCEPTION_EXECUTE_HANDLER){++cone_metrics.errors;InterlockedIncrement(&cone_error_total);}
    }
}
