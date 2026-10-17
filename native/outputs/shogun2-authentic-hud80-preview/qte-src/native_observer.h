#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#include "native_qte_abi.h"
#include "native_animation_snapshot.h"
#include "native_registry94.h"
namespace qte { namespace observation {

enum class Kind:uint32_t {Prepare,Update,Remove,Select};
struct Event {
    uint64_t qpc{},generation{};
    uint32_t kind{},exit{},thread{},caller{},witness{},environment{},tick{},native_valid{};
    uint32_t arguments[6]{},returned{},completed{},verdict_valid{};
    uint32_t active{},phase{},count{},principals{},participants[5]{};
    NativeVerdict verdict{};
    uint32_t owner_valid{},owner_actor{},owner_serial{},owner_unit_serial{},owner_lease{};
    ActorSnapshot actors[5]{};AnimationSnapshot animation{};
};
struct Ports {
    NativeWitnessCall prepare{},update{};NativeWitnessRemove remove{};NativeSelector select{};
    void* context{};void(*emit)(void*,const Event&){};
    bool(*snapshot)(void*,uint32_t,Event&){};
    bool(*read)(void*,uint32_t,void*,size_t){};
    uint32_t engine{};
};
static Registry registry;
static Ports ports;
static volatile LONG active_callbacks;
static __declspec(thread) uint32_t parent_witness;
static __declspec(thread) uint64_t parent_generation;
inline Event event(Kind kind,uint32_t witness,uint64_t generation,uintptr_t caller){
    Event e;e.kind=uint32_t(kind);e.witness=witness;e.generation=generation;e.thread=GetCurrentThreadId();
    e.caller=caller>=ports.engine&&caller-ports.engine<0x1d88000?uint32_t(caller-ports.engine):0;
    return e;
}
inline void emit(Event& e,bool read_witness=true){
    LARGE_INTEGER clock;QueryPerformanceCounter(&clock);e.qpc=uint64_t(clock.QuadPart);
    if(read_witness&&ports.snapshot&&e.witness)e.native_valid=ports.snapshot(ports.context,e.witness,e)?1:0;
    if(ports.emit)ports.emit(ports.context,e);
}
static uint32_t __fastcall prepare_hook(void* self,void*){
    InterlockedIncrement(&active_callbacks);
    auto address=uint32_t(reinterpret_cast<uintptr_t>(self));auto caller=reinterpret_cast<uintptr_t>(_ReturnAddress());
    uint64_t generation=registry.get(address,caller==uintptr_t(ports.engine)+0x3e210e);
    Event e=event(Kind::Prepare,address,generation,caller);emit(e);
    uint32_t previous_witness=parent_witness;uint64_t previous_generation=parent_generation;
    parent_witness=address;parent_generation=generation;uint32_t returned=0;
    __try {returned=ports.prepare(self);e.returned=returned;e.completed=1;}
    __finally {parent_witness=previous_witness;parent_generation=previous_generation;e.exit=1;emit(e);InterlockedDecrement(&active_callbacks);}
    return returned;
}
static uint32_t __fastcall update_hook(void* self,void*){
    InterlockedIncrement(&active_callbacks);
    auto address=uint32_t(reinterpret_cast<uintptr_t>(self));auto caller=reinterpret_cast<uintptr_t>(_ReturnAddress());
    Event e=event(Kind::Update,address,registry.get(address),caller);emit(e);uint32_t returned=0;
    __try {returned=ports.update(self);e.returned=returned;e.completed=1;}
    __finally {e.exit=1;emit(e);InterlockedDecrement(&active_callbacks);}
    return returned;
}
static void __fastcall remove_hook(void* manager,void*,void* witness){
    InterlockedIncrement(&active_callbacks);
    auto address=uint32_t(reinterpret_cast<uintptr_t>(witness));auto caller=reinterpret_cast<uintptr_t>(_ReturnAddress());
    uint64_t generation=registry.get(address);Event e=event(Kind::Remove,address,generation,caller);
    e.arguments[0]=uint32_t(reinterpret_cast<uintptr_t>(manager));emit(e);
    __try {ports.remove(manager,witness);e.completed=1;}
    __finally {
        // The native removal may free the witness. No post-remove dereference.
        registry.removed(address,generation);e.exit=1;emit(e,false);InterlockedDecrement(&active_callbacks);
    }
}
static NativeVerdict* __fastcall select_hook(void* self,void*,NativeVerdict* output,
                       void*const* begin,void*const* end,uint32_t flags,uint32_t flags2){
    InterlockedIncrement(&active_callbacks);
    auto caller=reinterpret_cast<uintptr_t>(_ReturnAddress());
    Event e=event(Kind::Select,parent_witness,parent_generation,caller);
    e.arguments[0]=uint32_t(reinterpret_cast<uintptr_t>(self));e.arguments[1]=uint32_t(reinterpret_cast<uintptr_t>(output));
    e.arguments[2]=uint32_t(reinterpret_cast<uintptr_t>(begin));e.arguments[3]=uint32_t(reinterpret_cast<uintptr_t>(end));
    e.arguments[4]=flags;e.arguments[5]=flags2;emit(e);NativeVerdict* returned=nullptr;
    __try {
        returned=ports.select(self,output,begin,end,flags,flags2);e.returned=uint32_t(reinterpret_cast<uintptr_t>(returned));e.completed=1;
        if(returned&&ports.read)e.verdict_valid=ports.read(ports.context,e.returned,&e.verdict,sizeof e.verdict)?1:0;
    }__finally {e.exit=1;emit(e);InterlockedDecrement(&active_callbacks);}
    return returned;
}
} }
