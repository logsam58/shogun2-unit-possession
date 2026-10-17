#pragma once
#include "native_selection_prefix.h"
#if !defined(_MSC_VER) || !defined(_M_IX86)
#error Native Shogun combat ABI requires an MSVC x86 build
#endif
namespace qte {
using NativeWitnessCall=uint32_t(__thiscall*)(void*);
using NativeWitnessRemove=void(__thiscall*)(void*,void*);
using NativeSelector=NativeVerdict*(__thiscall*)(void*,NativeVerdict*,void*const*,void*const*,uint32_t,uint32_t);
using NativeSort=void(__cdecl*)(void*const*,void*const*,int,const float*);
static_assert(sizeof(void*)==4,"Native pointer size");
struct NativeSelectionPorts {
    NativeSort sort{};NativeSelector select{};
    static bool sort_call(void* opaque,uint32_t begin,uint32_t end,uint32_t count,const float* center){
        auto& ports=*static_cast<NativeSelectionPorts*>(opaque);
        if(!ports.sort)return false;
        ports.sort(reinterpret_cast<void*const*>(begin),reinterpret_cast<void*const*>(end),int(count),center);
        return true;
    }
    static bool select_call(void* opaque,uint32_t context,uint32_t begin,uint32_t end,
                            uint32_t flags,uint32_t flags2,NativeVerdict& output){
        auto& ports=*static_cast<NativeSelectionPorts*>(opaque);
        if(!ports.select)return false;
        // Native exception propagation is deliberate; never mask a fault by
        // making a second selector call. Captures prove EAX returns this buffer.
        return ports.select(reinterpret_cast<void*>(context),&output,
            reinterpret_cast<void*const*>(begin),reinterpret_cast<void*const*>(end),flags,flags2)==&output;
    }
    SelectionCalls calls(){return {this,sort_call,select_call};}
};
}
