#pragma once
// Native posture transition table34f7e0 maps12/13 ->0 through clips455/456.
// The posture can already be0 while the noninterruptible get-up clip is active.
// Never infer completion from stale planned clip1d4 or a wall-clock timeout.
inline bool recovery_clip103(unsigned clip){return clip>=0x441&&clip<=0x458;}
inline bool prone_mode103(unsigned mode){return mode==12||mode==13;}
static bool native_recovery103(uintptr_t actor){
    auto mode=field<unsigned>(actor,0x1f0);
    if(prone_mode103(mode))return true;
    auto animation=field<uintptr_t>(actor,0x474);if(!animation)return false;
    unsigned clip=field<unsigned>(animation,0);
    return recovery_clip103(clip)&&field<unsigned>(actor,0x1d4)==clip;
}
