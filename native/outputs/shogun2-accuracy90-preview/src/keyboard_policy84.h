#pragma once
#include <cstdint>
namespace keyboard84 {
// Shogun's verified keyboard format is the standard 256-byte DirectInput
// format (keyboard_create RVA bba540). Top-row 1..0 are scans 02..0b.
// Windows GetAsyncKeyState, used by our equipment sampler, is untouched.
constexpr unsigned scan(unsigned index){return index<10?index+2:0x0f;}
constexpr unsigned all=(1u<<11)-1;
inline unsigned window_bit(unsigned key){
    return key>='1'&&key<='9'?1u<<(key-'1'):key=='0'?1u<<9:key==9?1u<<10:0;
}
struct Filter {
    unsigned held{};
    // On failed acquisition the buffers may be stale: preserve the latch and
    // native error result. A valid release must be observed before unmasking.
    unsigned apply(std::uint8_t* current,std::uint8_t* previous,bool fps,bool valid){
        if(!valid||!current||!previous)return 0;
        unsigned physical=0;
        for(unsigned i=0;i<11;++i)if(current[scan(i)]&0x80)physical|=1u<<i;
        unsigned masked=(fps?all:0)|held;
        held=fps?physical:held&physical;
        for(unsigned i=0;i<11;++i)if(masked&(1u<<i))current[scan(i)]=previous[scan(i)]=0;
        return masked&physical;
    }
};
}
