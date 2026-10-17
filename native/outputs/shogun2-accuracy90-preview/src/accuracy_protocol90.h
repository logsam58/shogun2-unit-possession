#pragma once
#include <windows.h>
#include <cstdint>
#include <cmath>
namespace accuracy90 {
constexpr uint32_t Magic=0x30434153,Version=90;
struct Status {
 uint32_t magic,version,size,pid;volatile LONG sequence;
 uint32_t lease,actor,unit,record,valid,error,updates;
 uint64_t tick;
 float half_angle,horizontal,vertical,chance,calibration,core,control,projection_y;
 uint32_t rts_captures,rts_restores,rts_rejects,rts_saved,simulation_tick;
};
inline void mapping_name(wchar_t* out,DWORD pid){swprintf_s(out,96,L"Local\\Shogun2Accuracy90-%lu",pid);}
inline bool valid(const Status& s,uint32_t pid,uint32_t lease,uint32_t actor,uint32_t unit,uint64_t now,uint32_t simulation_tick=0xffffffffu){
 return s.magic==Magic&&s.version==Version&&s.size==sizeof s&&s.pid==pid&&!(s.sequence&1)&&s.valid&&s.lease==lease&&s.actor==actor&&s.unit==unit&&
  now>=s.tick&&(now-s.tick<1000||(simulation_tick!=0xffffffffu&&simulation_tick==s.simulation_tick))&&std::isfinite(s.half_angle)&&s.half_angle>=0&&s.half_angle<1.570796327f&&std::isfinite(s.projection_y)&&s.projection_y>0;
}
}
