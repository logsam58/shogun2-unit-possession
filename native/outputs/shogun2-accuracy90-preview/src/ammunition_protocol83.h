#pragma once
#include <windows.h>
#include <cstdint>
#include <cwchar>
namespace ammo83 {
enum Icon : uint32_t {Unknown,Arrow,Bullet,Bomb,Rocket,Mortar,HeavyShot,FireArrow,WhistleArrow,BlindBomb,Suppression};
enum Flags : uint32_t {Selected=1,Available=2,Normal=4};
struct Item {uint32_t ability,projectile,icon,flags;wchar_t label[64];};
struct Status {
 uint32_t magic,version,size,pid;volatile LONG sequence;
 uint32_t lease,actor,unit,count,current,pending,error;uint64_t tick;
 Item items[33];
};
static_assert(sizeof(Item)==144&&sizeof(Status)==4808,"Ammunition83 wire layout differs");
inline void mapping_name(wchar_t (&name)[96],DWORD pid){swprintf_s(name,L"Local\\Shogun2Ammunition83-%lu",pid);}
inline bool valid(const Status& s,unsigned pid,unsigned lease,unsigned actor,unsigned unit,uint64_t now){
 if(s.magic!=0x334d4153||s.version!=83||s.size!=sizeof(Status)||s.pid!=pid||s.lease!=lease||
    s.actor!=actor||s.unit!=unit||now<s.tick||now-s.tick>=1000||s.count>33||(s.count&&s.current>=s.count))return false;
 for(unsigned i=0;i<s.count;++i)if(s.items[i].icon>Suppression||(s.items[i].flags&~7u)||s.items[i].label[63])return false;
 return true;
}
inline Icon icon_for_key(const wchar_t* key){
 if(wcsstr(key,L"flaming"))return FireArrow;if(wcsstr(key,L"whistling"))return WhistleArrow;
 if(wcsstr(key,L"blinding"))return BlindBomb;if(wcsstr(key,L"suppressive"))return Suppression;
 if(wcsstr(key,L"handmortar"))return Mortar;if(wcsstr(key,L"heavygunner"))return HeavyShot;
 if(wcsstr(key,L"rocket"))return Rocket;if(wcsstr(key,L"fire_bomb"))return Bomb;
 if(wcsstr(key,L"arrow"))return Arrow;
 if(wcsstr(key,L"rifle")||wcsstr(key,L"carbine")||wcsstr(key,L"matchlock")||
    wcsstr(key,L"musket")||wcsstr(key,L"revolver")||wcsstr(key,L"donderbuss"))return Bullet;
 return Unknown;
}
inline const wchar_t* label(Icon i){
 static const wchar_t* names[]={L"Ammunition",L"Arrows",L"Bullets",L"Fire bombs",L"Rockets",L"Mortar bombs",L"Heavy shot",L"Fire arrows",L"Whistling arrows",L"Blinding grenades",L"Suppressive fire"};
 return i<=Suppression?names[i]:names[0];
}
inline const wchar_t* image(Icon i){
 switch(i){
 case FireArrow:return L"ui/ability_icons/data__flaming_arrows_ability_icon.png";
 case WhistleArrow:return L"ui/ability_icons/data__whistling_arrows_ability_icon.png";
 case BlindBomb:return L"ui/ability_icons/data__blinding_grenade_icon.png";
 case Suppression:return L"ui/ability_icons/data__suppression_fire_icon.png";
 case Arrow:return L"ui/icons/data__unit_cat_card_bow_inf.png";
 case Bullet:case HeavyShot:return L"ui/icons/data__unit_cat_card_gun_inf.png";
 case Bomb:return L"ui/icons/data__unit_cat_card_bomb_inf.png";
 case Rocket:case Mortar:return L"ui/icons/data__unit_cat_card_siege.png";
 default:return L"ui/ability_icons/data__fire_at_will_icon.png";
 }
}
}
