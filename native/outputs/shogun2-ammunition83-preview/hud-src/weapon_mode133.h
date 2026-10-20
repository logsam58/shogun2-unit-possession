#pragma once
namespace weaponmode133 {
struct Mode {bool known,melee;};
inline Mode from_native(unsigned mode,bool has_ranged){return {mode<=1,!has_ranged||mode==1};}
struct Display {const wchar_t* icon;const wchar_t* label;const wchar_t* key;};
inline Display resolve(bool known,bool melee){
    if(!known)return {nullptr,L"Weapon mode unavailable",nullptr};
    return melee?Display{L"ui/icons/data__status_melee.png",L"Melee mode",L"1"}:
        Display{L"ui/icons/data__status_firing.png",L"Ranged mode",L"1"};
}
}
