#pragma once
#include "native_unit_context.h"

namespace qte {
// Native selector's actual 12-byte output contains POINTERS, never actor IDs.
// The adapter converts these to a Pair only after validating both identities.
struct NativeVerdict {uint32_t outcome{},attacker_address{},defender_address{};};
static_assert(sizeof(NativeVerdict)==12,"Native output ABI");
struct SelectionCalls {
    void* context{};
    bool(*sort)(void*,uint32_t,uint32_t,uint32_t,const float*){};
    bool(*select)(void*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,NativeVerdict&){};
};
struct SelectionInput {
    uint32_t witness{},environment{},context{},begin{},end{},count{},flags{};
    float center[3]{};
};
enum class SelectionProgress {NotStarted,Attempted,Complete};
inline bool selection_input(const Reader& reader,uint32_t witness,uint32_t engine,
                            SelectionInput& out){
    // Exact prepare3e5d40 prefix before call3e5f77. Reject prepared/transitional
    // witnesses; AL0 means defer ONLY with a matching update hold installed.
    if(!witness||witness>UINT32_MAX-0x100||!engine||engine>UINT32_MAX-0x1bc8184)return false;
    uint8_t active{},phase{},alternate{};uint32_t environment{},common{},context{},count{};
    if(!reader.get(witness+0xc,active)||active!=0||!reader.get(witness+0xd0,phase)||phase!=0||
       !reader.get(witness+0xee,alternate)||alternate>1||
       !reader.get(witness+4,environment)||!environment||environment>UINT32_MAX-12||
       !reader.get(environment+8,common)||!common||common>UINT32_MAX-0xa0||
       !reader.get(common+0x9c,context)||!context)return false;
    const uint32_t count_offset=alternate?0x38:0x24,begin_offset=alternate?0x30:0x10;
    if(!reader.get(witness+count_offset,count)||count<2||count>(alternate?2u:5u))return false;
    SelectionInput input{witness,environment,context,witness+begin_offset,witness+begin_offset+count*4,count,0,{}};
    for(unsigned axis=0;axis<3;++axis){
        if(!reader.get(engine+0x1a4dc0c+axis*4,input.center[axis])||!std::isfinite(input.center[axis]))return false;
    }
    // Native starts from its shared zero/vector constant and sums in native
    // participant order, then divides before the native distance comparator.
    for(uint32_t i=0;i<count;++i){
        uint32_t actor{};if(!reader.get(input.begin+i*4,actor)||!actor||actor>UINT32_MAX-0x700)return false;
        uint8_t alive{};if(!reader.get(actor+0xf0,alive)||alive!=1)return false;
        for(unsigned axis=0;axis<3;++axis){float position{};
            if(!reader.get(actor+0x48+axis*4,position)||!std::isfinite(position)||std::abs(position)>100000)return false;
            input.center[axis]+=position;
        }
    }
    float inverse=1.f/float(count);for(float& axis:input.center)axis*=inverse;
    out=input;return true;
}
inline bool preparation_participant(const Reader& reader,uint32_t witness,uint32_t engine,uint32_t actor){
    SelectionInput input;
    if(!actor||!selection_input(reader,witness,engine,input))return false;
    for(uint32_t i=0;i<input.count;++i){uint32_t member{};
        if(!reader.get(input.begin+i*4,member))return false;
        if(member==actor)return true;
    }
    return false;
}
inline bool player_encounter(const Reader& reader,uint32_t actor,uint32_t witness,uint32_t engine,bool updating){
    uint32_t backlink{};
    if(!actor||!reader.get(actor+0x688,backlink))return false;
    if(backlink==witness)return true;
    // Captured initial preparation has active0/phase0 and backlinks0. The
    // native registration is made inside original prepare, after selection.
    // Only a verified unprepared participant may use that initial zero link.
    return !backlink&&!updating&&preparation_participant(reader,witness,engine,actor);
}
inline bool select_once(const Reader& reader,const SelectionCalls& calls,
                        SelectionInput& input,NativeVerdict& output,SelectionProgress* progress=nullptr){
    if(progress)*progress=SelectionProgress::NotStarted;
    if(!calls.sort||!calls.select)return false;
    if(!calls.sort(calls.context,input.begin,input.end,input.count,input.center))return false;
    uint8_t alternate{};
    if(!reader.get(input.witness+0xee,alternate)||alternate>1)return false;
    if(!alternate){
        uint32_t first{},mount{};
        if(!reader.get(input.begin,first)||!first||first>UINT32_MAX-0x220||!reader.get(first+0x21c,mount))return false;
        input.flags=mount?1:0;
    }
    // Invoking this consumes native RNG/bookkeeping. The caller MUST retain
    // every successful output, including a non-player contest, and replay it
    // at selector return-site3e5f7c during the original prepare invocation.
    if(progress)*progress=SelectionProgress::Attempted;
    bool ok=calls.select(calls.context,input.context,input.begin,input.end,input.flags,0,output);
    if(ok&&progress)*progress=SelectionProgress::Complete;
    return ok;
}
class SelectorReplay {
    SelectionInput expected_{};NativeVerdict selected_{};
    bool armed_{},taken_{};
public:
    bool arm(const SelectionInput& input,const NativeVerdict& selected){
        if(armed_||!input.witness||!input.context||input.begin>=input.end)return false;
        expected_=input;selected_=selected;armed_=true;taken_=false;return true;
    }
    bool take(uint32_t witness,uint32_t return_rva,uint32_t context,
              uint32_t begin,uint32_t end,uint32_t flags,uint32_t flags2,NativeVerdict& out){
        if(!armed_||taken_||witness!=expected_.witness||return_rva!=0x3e5f7c||
           context!=expected_.context||begin!=expected_.begin||end!=expected_.end||
           flags!=expected_.flags||flags2!=0)return false;
        out=selected_;taken_=true;return true;
    }
    bool finish(){const bool consumed=armed_&&taken_;armed_=false;taken_=false;return consumed;}
};
}
