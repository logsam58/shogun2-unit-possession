#pragma once
#include <cstdint>

namespace deck175 {
// The native UI dispatcher retains command eligibility, construction and
// serialization. This preflight only establishes the borrowed land selection
// and reserves space before entering its ordinary queue writer.
template<class Read,class Current>
bool native_order_ready(uintptr_t base,uintptr_t root,uintptr_t environment,
                        Read read,Current current,bool allow_groups=false){
    if(!current()||!root||!environment||read(root+0x110)!=environment||
       read(root+0x5b8)!=environment)return false;
    const auto selected=read(environment+0x2826c);
    const auto owner=selected?read(selected+0x4a8):0;
    const auto group=owner?owner+0xb0:0;
    const auto queue=read(environment+0x28048);
    if(!group||!queue||read(root+0x5b4)!=owner||read(group)!=base+0x15c4da0)return false;
    const auto units=read(group+0x14),groups=read(group+8);
    if(units>512||groups>512||units+groups==0||units+groups>512)return false;
    if(units&&(read(group+0x10)<units||!read(group+0x18)))return false;
    if(groups&&(!allow_groups||read(group+4)<groups||!read(group+0xc)))return false;
    if(read(group+0x20)||read(group+0x2c))return false;
    const auto ui_entries=read(group+0x3c);
    if(!allow_groups&&(ui_entries||read(group+0x34)))return false;
    if(ui_entries&&(ui_entries>512||read(group+0x38)<ui_entries||!read(group+0x40)))return false;
    // Both selected land vectors use one tagged 32-bit ID per entry.
    // Keep the conservative headroom before entering the native queue writer.
    constexpr uintptr_t capacity_bytes=0x5000;
    const auto before=read(queue+4+capacity_bytes),budget=128+8*(units+groups);
    if(before>capacity_bytes||budget>capacity_bytes-before||!current())return false;
    return true;
}
template<class Read,class Current,class Invoke>
bool native_halt(uintptr_t base,uintptr_t root,uintptr_t environment,
                 Read read,Current current,Invoke invoke){
    if(!native_order_ready(base,root,environment,read,current))return false;
    const auto queue=read(environment+0x28048),before=read(queue+4+0x5000);
    invoke(root,4,0.f); //10dfb80 ->10f97d0 case4, native Halt.
    const auto after=read(queue+4+0x5000);
    return after>before&&after<=0x5000;
}
}
