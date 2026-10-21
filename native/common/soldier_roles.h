#pragma once
#include <cstdint>
// Final human-derived tables verified against the supported engine. Role
// admission does not waive holder, unit, serial, life or reciprocal-mount proofs.
// Naval human identity does not imply support for controlling a ship.
namespace soldier_roles {
inline constexpr uintptr_t human_tables[] = {
    0x15dfec8, // regular human
    0x15dbd68, // general
    0x15dbfb4, // officer
    0x15dc590, // bugler
    0x15dc1a8, // drummer
    0x15dc39c, // flautist
    0x15dcf54, // standard bearer
    0x15dc978, // admiral
    0x15dcb6c, // commodore
    0x15dcd60, // naval officer
};
inline constexpr uintptr_t horse_table = 0x15e8524;
inline bool human(uintptr_t engine,uintptr_t table){
    if(table<engine)return false;
    for(auto rva:human_tables)if(table-engine==rva)return true;
    return false;
}
}
