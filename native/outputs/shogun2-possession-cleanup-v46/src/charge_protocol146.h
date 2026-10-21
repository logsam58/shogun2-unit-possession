#pragma once
#include <windows.h>
#include <cstdint>
// Read-only numeric observation for the QTE runtime. It never grants actor
// identity: the consumer separately proves the current camera/lease/actors.
namespace charge146 {
constexpr uint32_t Magic=0x36474843;
struct Value {uint32_t pid{},lease{},human{},unit{},physical{},kind{},active{};uint64_t tick{};};
struct Shared {volatile LONG sequence{};uint32_t magic{Magic},version{1},bytes{sizeof(Value)};Value value{};};
inline void name(wchar_t* out,DWORD pid){swprintf_s(out,96,L"Local\\Shogun2NativeCharge146-%lu",pid);}
inline void publish(Shared* shared,const Value& value){if(!shared)return;InterlockedIncrement(&shared->sequence);MemoryBarrier();shared->value=value;MemoryBarrier();InterlockedIncrement(&shared->sequence);}
inline bool read(const Shared* shared,Value& value){if(!shared||shared->magic!=Magic||shared->version!=1||shared->bytes!=sizeof(Value))return false;for(unsigned i=0;i<4;++i){LONG seq=shared->sequence;if(seq&1)continue;MemoryBarrier();auto copy=shared->value;MemoryBarrier();if(seq==shared->sequence){value=copy;return true;}}return false;}
inline bool matches(const Value& v,uint32_t pid,uint32_t lease,uint32_t human,uint32_t unit,uint32_t physical,uint32_t kind,uint64_t now){return v.active==1&&v.pid==pid&&v.lease==lease&&lease&&v.human==human&&v.unit==unit&&v.physical==physical&&v.kind==kind&&now>=v.tick&&now-v.tick<=500;}
}
