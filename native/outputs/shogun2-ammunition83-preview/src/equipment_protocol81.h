#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
// Separate extension: the accepted Status46/QTE ABI is unchanged.
struct EquipmentStatus81 {
    uint32_t magic,version,size,pid;
    volatile LONG sequence;
    uint32_t lease,actor,unit,slot,kind,styles,ability,projectile;
    uint32_t crouch_supported,crouched,pending,error,switches,ammo_changes;
    uint64_t tick;
    wchar_t ammunition[64];
};
inline void equipment_mapping_name81(wchar_t (&name)[96],DWORD pid){swprintf_s(name,L"Local\\Shogun2Equipment81-%lu",pid);}
static constexpr uint32_t EQUIPMENT_MAGIC81=0x31514553;
