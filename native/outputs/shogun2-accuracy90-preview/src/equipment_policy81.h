#pragma once
#include <cstdint>
namespace equipment81 {
enum Slot : unsigned { Melee=1, Ranged=2 };
struct Boundary {
    bool alive{}, running{}, commitment{}, handoff{}, shot{}, reload{};
    bool safe() const { return alive&&running&&!commitment&&!handoff&&!shot&&!reload; }
};
inline bool valid_slot(unsigned slot,bool ranged){return slot==Melee||(slot==Ranged&&ranged);}
inline unsigned movement_input(unsigned held,bool crouched){return crouched?0u:held;}
inline bool crouch_supported(bool mounted,bool firearm,bool unlocked,bool idle_clip,bool aim_clip,bool fire_clip,bool reload_clip){
    return !mounted&&firearm&&unlocked&&idle_clip&&aim_clip&&fire_clip&&reload_clip;
}
inline unsigned ready_mode(bool mounted,bool crouched){return mounted?32:crouched?15:14;}
inline unsigned lower_mode(bool mounted,bool crouched){return mounted?29:crouched?9:8;}
inline unsigned reload_mode(bool mounted,bool crouched){return mounted?33:crouched?17:16;}
struct Edges {
    unsigned previous{};
    unsigned sample(unsigned held,bool active){unsigned rising=held&~previous;previous=held;return active?rising:0;}
};
struct AmmoOption {unsigned id{};std::uintptr_t record{};bool active{},eligible{};};
// Include normal ammunition at index0. Cycling skips unusable styles without
// consuming their charges or changing the active style on a failed request.
inline unsigned next_ammo(const AmmoOption* options,unsigned count,unsigned current){
    if(!options||!count||count>33||current>=count)return current;
    for(unsigned n=1;n<count;++n){unsigned index=(current+n)%count;if(index==0||options[index].eligible)return index;}
    return current;
}
}
