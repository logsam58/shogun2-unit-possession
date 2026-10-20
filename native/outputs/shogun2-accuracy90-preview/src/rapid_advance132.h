#pragma once
// Read-only lookup of the exact active Rapid Advance effect. Native2899e0
// stores enum27 at node+8 and the278080 effect at node+4c. Native2d5560
// uses the285ed0 hash and16-byte buckets. Do not mistake the shared d3c
// fatigue-immunity flag (also used by Second Wind) for Rapid Advance itself.
static bool rapid_advance_active132(){
    __try {
        if(!field<unsigned char>(unit,0xd3c))return false;
        uintptr_t manager=unit+0xed4;
        if(field<uintptr_t>(manager,8)!=unit)return false;
        uintptr_t map=manager+0xc;unsigned count=field<unsigned>(map,8);
        uintptr_t buckets=field<uintptr_t>(map,0xc);
        if(!count||count>1024||!buckets)return false;
        uintptr_t bucket=buckets+((0x27u^0x4a545eedu)%count)*16;
        uintptr_t node=field<uintptr_t>(bucket,0),end=field<uintptr_t>(bucket,4);
        for(unsigned n=0;node&&node!=end&&n<256;++n){
            if(field<unsigned>(node,8)==0x27){
                uintptr_t effect=field<uintptr_t>(node,0x4c);
                return effect&&field<uintptr_t>(effect,0)==base+0x15cfc00&&
                    field<uintptr_t>(effect,8)==unit&&field<unsigned>(effect,0xc)==0x27&&
                    field<uintptr_t>(effect,4)==environment&&
                    std::isfinite(field<float>(effect,0x18))&&field<float>(effect,0x18)>0;
            }
            node=field<uintptr_t>(node,4);
        }
        return false;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
