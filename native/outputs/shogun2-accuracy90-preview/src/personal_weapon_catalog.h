#pragma once
#include <cwchar>
#include <cstdint>
#include <cmath>
enum class PersonalWeaponKind {Unsupported,Bow,Firearm,Thrown};
struct PersonalWeaponEntry {const wchar_t* key; PersonalWeaponKind kind;};
static constexpr PersonalWeaponEntry PERSONAL_WEAPONS[]={
{L"blinding_grenade",PersonalWeaponKind::Thrown},
{L"arrow_flaming",PersonalWeaponKind::Bow},
{L"arrow_flaming_exteme_range",PersonalWeaponKind::Bow},
{L"arrow_flaming_long_range",PersonalWeaponKind::Bow},
{L"arrow_whistling",PersonalWeaponKind::Bow},
{L"arrow_whistling_extreme_range",PersonalWeaponKind::Bow},
{L"arrow_whistling_long_range",PersonalWeaponKind::Bow},
{L"boshin_breech_loading_rifle",PersonalWeaponKind::Firearm},
{L"boshin_carbine",PersonalWeaponKind::Firearm},
{L"boshin_matchlock",PersonalWeaponKind::Firearm},
{L"boshin_musket",PersonalWeaponKind::Firearm},
{L"boshin_officer_revolver",PersonalWeaponKind::Firearm},
{L"boshin_repeating_rifle",PersonalWeaponKind::Firearm},
{L"boshin_revolver",PersonalWeaponKind::Firearm},
{L"boshin_rifle",PersonalWeaponKind::Firearm},
{L"boshin_rifle_suppressive",PersonalWeaponKind::Firearm},
{L"boshin_sharpshooter_rifle",PersonalWeaponKind::Firearm},
{L"shogun_arrow",PersonalWeaponKind::Bow},
{L"shogun_arrow_daikyu",PersonalWeaponKind::Bow},
{L"shogun_arrow_genpei",PersonalWeaponKind::Bow},
{L"shogun_arrow_hero_genpei",PersonalWeaponKind::Bow},
{L"shogun_arrow_hero_genpei_mp",PersonalWeaponKind::Bow},
{L"shogun_arrow_long",PersonalWeaponKind::Bow},
{L"shogun_arrow_rider",PersonalWeaponKind::Bow},
{L"shogun_avatar_arrow",PersonalWeaponKind::Bow},
{L"shogun_carbine",PersonalWeaponKind::Firearm},
{L"shogun_donderbuss",PersonalWeaponKind::Firearm},
{L"shogun_fire_bomb",PersonalWeaponKind::Thrown},
{L"shogun_handmortar",PersonalWeaponKind::Thrown},
{L"shogun_heavygunner",PersonalWeaponKind::Firearm},
{L"shogun_hero_arrow",PersonalWeaponKind::Bow},
{L"shogun_hero_matchlock",PersonalWeaponKind::Firearm},
{L"shogun_matchlock",PersonalWeaponKind::Firearm},
{L"shogun_rocket",PersonalWeaponKind::Thrown}};
inline PersonalWeaponKind personal_key_kind(const wchar_t* text,unsigned length,unsigned capacity,unsigned range,float speed){
    if(!text||!length||length>128||capacity<length||capacity>1048576||range>10000||!std::isfinite(speed)||speed<=0||speed>10000)return PersonalWeaponKind::Unsupported;
    for(const auto& p:PERSONAL_WEAPONS)if(std::wcslen(p.key)==length&&!std::wmemcmp(text,p.key,length))return p.kind;
    return PersonalWeaponKind::Unsupported;
}
inline PersonalWeaponKind personal_ascii_key_kind(const char* text,unsigned length,unsigned capacity,unsigned range,float speed){
    if(!text||!length||length>128||capacity<length||capacity>1048576||range>10000||!std::isfinite(speed)||speed<=0||speed>10000)return PersonalWeaponKind::Unsupported;
    for(const auto& p:PERSONAL_WEAPONS){
        if(std::wcslen(p.key)!=length)continue;
        bool same=true;for(unsigned i=0;i<length;++i)if(static_cast<unsigned char>(text[i])!=static_cast<unsigned>(p.key[i])){same=false;break;}
        if(same)return p.kind;
    }
    return PersonalWeaponKind::Unsupported;
}
