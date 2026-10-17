#pragma once
#include <cmath>
inline bool mounted_bow_profile(unsigned range,float speed){return range==150&&speed==50.0f;}
inline bool personal_projectile_supported(unsigned id,unsigned range,float speed,bool mounted){
    return mounted?mounted_bow_profile(range,speed):
        ((id==3&&range==150&&speed==50.0f)||(id==60&&range==100&&speed==200.0f));
}
inline bool personal_bow_projectile(unsigned id,unsigned range,float speed,bool mounted){
    return mounted?mounted_bow_profile(range,speed):(id==3&&range==150&&speed==50.0f);
}
inline bool personal_weapon_mode(unsigned mode,bool mounted){return mounted?mode>=29&&mode<=33:mode<18;}
inline unsigned personal_ready_mode(bool mounted){return mounted?32:14;}
inline unsigned personal_lower_mode(bool mounted){return mounted?29:8;}
inline unsigned personal_reload_mode(bool mounted){return mounted?33:16;}
inline unsigned bow_raise_clip(bool mounted){return mounted?751:126;}
inline unsigned bow_hold_clip(bool mounted){return mounted?753:124;}
