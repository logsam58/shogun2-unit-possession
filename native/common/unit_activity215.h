#pragma once
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include "../outputs/shogun2-possession-cleanup-v46/src/actor_types152.h"
namespace unitactivity215 {
enum class Activity {Holding,Moving,Firing,Melee};
struct Snapshot {Activity activity{Activity::Holding};unsigned peers{},moving{},firing{},melee{};};
// Read-only observation of CURRENT unit roster members. The possessed soldier
// and its mount never contribute to these counts. This does not issue an order,
// run an engine task or change native unit/soldier state.
template<class Reader>bool sample(Reader read,uintptr_t image,uintptr_t env,uintptr_t unit,unsigned id,uintptr_t player,Snapshot& out){
 auto get=[&](uintptr_t at,auto& v){return read(at,&v,sizeof v);};
 unsigned count{},current_id{};uintptr_t list{};
 if(!unit||!get(unit+4,current_id)||current_id!=id||!get(unit+0x2bc,count)||count>2000||!get(unit+0x2c0,list)||(count&&!list))return false;
 std::array<uintptr_t,2000> members{};if(count&&!read(list,members.data(),count*sizeof(uintptr_t)))return false;
 Snapshot s;
 for(unsigned n=0;n<count;++n){auto actor=members[n];if(!actor||actor==player)continue;
  std::array<unsigned char,0x68c> bytes{};if(!read(actor,bytes.data(),bytes.size()))return false;
  auto field=[&](unsigned offset,auto& v){memcpy(&v,bytes.data()+offset,sizeof v);};
  uintptr_t table{},owner{},environment{},paired{},animation{},mount{};unsigned serial{},mode{},clip{};float vx{},vz{};
  field(0,table);if(!actor152::human(image,table)||bytes[0xf0]!=1)continue;
  field(0x214,owner);field(0x28,environment);if(owner!=unit||environment!=env)return false;
  field(0x1c8,serial);field(0x1f0,mode);field(0x688,paired);field(0x474,animation);field(0x1d4,clip);field(0x210,mount);field(0x8c,vx);field(0x94,vz);
  unsigned final_serial{};uintptr_t final_owner{};
  if(!get(actor+0x1c8,final_serial)||final_serial!=serial||!get(actor+0x214,final_owner)||final_owner!=unit)return false;
  if(mount){uintptr_t horse_table{},horse_unit{};unsigned char alive{};float horse_vx{},horse_vz{};
   if(!get(mount,horse_table)||horse_table!=image+soldier_roles::horse_table||!get(mount+0x214,horse_unit)||horse_unit!=unit||!get(mount+0xf0,alive)||alive!=1||!get(mount+0x8c,horse_vx)||!get(mount+0x94,horse_vz))return false;
   vx=horse_vx;vz=horse_vz;
  }
  if(!std::isfinite(vx)||!std::isfinite(vz))return false;
  ++s.peers;if(std::hypot(vx,vz)>.1f)++s.moving;if(paired)++s.melee;
  // Native ready/fire/reload modes already used by personal_weapon_profile
  // and equipment81. An active matching native animation is required; a stale
  // clip ID, the player's aiming flag and unit fire-at-will permission are not
  // evidence that the formation is executing its ranged firing cycle.
  if(animation&&clip<0x4a3&&(mode==14||mode==15||mode==16||mode==17||mode==32||mode==33)){
   unsigned playing{};if(!get(animation,playing))return false;if(playing==clip)++s.firing;
  }
 }
 uintptr_t final_list{};unsigned final_count{};
 if(!get(unit+0x2c0,final_list)||final_list!=list||!get(unit+0x2bc,final_count)||final_count!=count||!get(unit+4,current_id)||current_id!=id)return false;
 if(s.melee)s.activity=Activity::Melee;else if(s.firing)s.activity=Activity::Firing;
 else if(s.moving&&s.moving>=std::max(1u,(s.peers+3)/4))s.activity=Activity::Moving;
 out=s;return true;
}
inline const wchar_t* label(Activity state){switch(state){case Activity::Moving:return L"Moving";case Activity::Firing:return L"Firing";case Activity::Melee:return L"In melee";default:return L"Holding";}}
inline const wchar_t* icon(Activity state){switch(state){case Activity::Moving:return L"ui/icons/data__status_walk.png";case Activity::Firing:return L"ui/icons/data__status_firing.png";case Activity::Melee:return L"ui/icons/data__status_melee.png";default:return L"ui/buttons/data__button_halt_active.png";}}
inline float ammunition(float remaining,unsigned capacity){return std::isfinite(remaining)&&remaining>=0&&capacity&&capacity<=10000000?std::clamp(remaining/float(capacity),0.f,1.f):0.f;}
}
