#pragma once
#include <cstdint>
using RangedTaskGate=bool(__thiscall*)(void*);
inline bool ranged_task_nonblocking(bool controlled,void* task,RangedTaskGate native_gate){
    return controlled?true:native_gate(task);
}
// Native ranged release treats an absent/mismatched animation as a skipped,
// completed task (+6e=1). A player-owned animation is unrelated to this AI task.
// Acknowledge it without emitting an AI projectile or scheduling a second reload.
inline void acknowledge_skipped_ranged_release(void* task){
    auto bytes=static_cast<unsigned char*>(task);
    bytes[0x6c]=bytes[0x6d]=bytes[0x6e]=1;
}
inline void acknowledge_skipped_ranged_draw(void* task){
    auto bytes=static_cast<unsigned char*>(task);
    bytes[0x84]=1;
    acknowledge_skipped_ranged_release(task);
}
