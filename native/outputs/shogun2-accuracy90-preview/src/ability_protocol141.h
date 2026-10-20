#pragma once
#include <windows.h>
#include <cstdint>
#include <cmath>
namespace ability141 {
constexpr unsigned Magic=0x31344153,Capacity=64;
enum Flags:unsigned {Available=1,Active=2,Ammunition=4,Targeted=8};
struct Item {unsigned id{},flags{};float cooldown{};};
struct Status {
 unsigned magic{},version{},size{},pid{};volatile LONG sequence{};
 unsigned lease{},actor{},unit{},count{},selected{},pending{},error{};
 uint64_t tick{};Item items[Capacity]{};
};
inline void mapping(wchar_t (&name)[96],DWORD pid){swprintf_s(name,L"Local\\Shogun2Abilities141-%lu",pid);}
inline bool valid(const Status& s,unsigned pid,unsigned lease,unsigned actor,unsigned unit,uint64_t now){
 if(s.magic!=Magic||s.version!=141||s.size!=sizeof s||s.pid!=pid||s.lease!=lease||s.actor!=actor||s.unit!=unit||now<s.tick||now-s.tick>1000||s.count>Capacity||s.selected>=s.count)return false;
 for(unsigned i=0;i<s.count;++i)if(s.items[i].id>255||!std::isfinite(s.items[i].cooldown)||s.items[i].cooldown<0)return false;
 return true;
}
}
