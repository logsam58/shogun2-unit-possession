#pragma once
#include "../../shogun2-possession-cleanup-v46/src/actor_types152.h"
// Indexed native pipe constructor359770/registration385b20 and live siege trace.
// Discovery caching stores only an index; every use proves CURRENT membership.
struct Wall97 {uintptr_t pipe{},definition{},nodes{},phase{};unsigned id{},count{},kind{};};
static bool wall_world97(uintptr_t actor,uintptr_t& phase){
    auto root=field<uintptr_t>(base,0x1bc8180),env=root?field<uintptr_t>(root,0x110):0;
    if(!env||field<uintptr_t>(actor,0x28)!=env)return false;
    auto common=field<uintptr_t>(env,8);phase=common?field<uintptr_t>(common,0xb4):0;
    return phase&&field<unsigned>(phase,0x14)>=5&&field<unsigned>(phase,0x14)<=8;
}
static bool read_wall97(uintptr_t actor,uintptr_t pipe,Wall97& w,unsigned hint=~0u){
    w={};uintptr_t phase=0;if(!pipe||!wall_world97(actor,phase))return false;
    unsigned count=field<unsigned>(phase,0x300),capacity=field<unsigned>(phase,0x2fc);auto array=field<uintptr_t>(phase,0x304);
    if(!array||!count||count>8192||capacity<count||capacity>65536)return false;
    static __declspec(thread) unsigned previous_index97;
    unsigned index=hint<count?hint:previous_index97;
    if(index>=count||field<uintptr_t>(array,index*4)!=pipe){for(index=0;index<count&&field<uintptr_t>(array,index*4)!=pipe;++index){}if(index==count)return false;previous_index97=index;}
    if(field<uintptr_t>(pipe,0)!=base+0x15e7fd0||field<unsigned char>(pipe,4)!=1)return false;
    auto source=field<uintptr_t>(pipe,0x30),definition=field<uintptr_t>(pipe,0x34);unsigned kind=field<unsigned>(pipe,0x40);
    // Live registered wall resources clear their construction source (+30=0).
    // Current phase-vector membership, native type/definition and registry ID
    // establish the live resource; a present source must still be reciprocal.
    if(!field<uintptr_t>(pipe,0x14)||(source&&(field<uintptr_t>(source,0x30)!=pipe||field<unsigned>(source,0x40)!=kind))||kind<11||kind>14||!definition||field<uintptr_t>(definition,0)!=base+0x15dd88c)return false;
    unsigned n=field<unsigned>(definition,0x14);auto nodes=field<uintptr_t>(definition,0x18);
    if(!nodes||n<1||n>64||field<unsigned>(definition,0x10)<n)return false;
    float length=0;for(unsigned i=0;i<n;++i){auto node=nodes+i*72;unsigned mode=field<unsigned>(node,0x3c);
        float speed=field<float>(node,0x40);if(mode<68||mode>71||!std::isfinite(speed)||speed<=0||speed>100)return false;
        float distance2=0;for(unsigned k=0;k<3;++k){float a=field<float>(node,0x24+k*4),b=field<float>(node,0x30+k*4);
            if(!std::isfinite(a)||!std::isfinite(b)||std::abs(a)>100000||std::abs(b)>100000)return false;
            float d=b-a;distance2+=d*d;if(i&&std::abs(a-field<float>(nodes+(i-1)*72,0x30+k*4))>.05f)return false;
        }if(distance2<.000001f||distance2>10000)return false;length+=std::sqrt(distance2);
    }
    if(length>1000)return false;
    w={pipe,definition,nodes,phase,field<unsigned>(pipe,0x10),n,kind};return true;
}
// Native3e8b50 publishes the active clip BEFORE3988e0 clears pipe/a1;
// a7 is set only afterward. Prove the current native clip, not that torn flag.
static bool crest_subject97(uintptr_t actor){
    unsigned mode=field<unsigned>(actor,0x1f0),clip=field<unsigned>(actor,0x1d4);
    if(!(mode<=17||(mode>=68&&mode<=75)))return false;
    auto animation=field<uintptr_t>(actor,0x474);
    if(animation){unsigned active=field<unsigned>(animation,0);return active>=1067&&active<=1074&&field<uintptr_t>(animation,4)!=0;}
    return !field<uintptr_t>(actor,0x18)&&mode>=72&&mode<=75&&clip>=1067&&clip<=1074;
}
static bool wall_subject97(uintptr_t actor){
    auto unit97=field<uintptr_t>(actor,0x214),config97=field<uintptr_t>(actor,0x20c);
    if(!actor152::human(base,field<uintptr_t>(actor,0))||!unit97||!config97||(config97!=field<uintptr_t>(unit97,0xf9c)&&!actor152::individual(base,field<uintptr_t>(actor,0)))||field<uintptr_t>(actor,0x210)||field<unsigned char>(actor,0xf0)!=1)return false;
    if(crest_subject97(actor))return true;
    auto pipe=field<uintptr_t>(actor,0x18);if(!pipe)return false;Wall97 w;
    float p=field<float>(actor,0x20);
    return std::isfinite(p)&&p>=0&&p<=1&&read_wall97(actor,pipe,w);
}
