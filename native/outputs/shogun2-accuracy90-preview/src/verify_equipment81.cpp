#include "equipment_policy81.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
using namespace equipment81;
int main(){
    Boundary safe{true,true,false,false,false,false};assert(safe.safe());
    for(unsigned i=0;i<6;++i){auto blocked=safe;switch(i){case 0:blocked.alive=false;break;case 1:blocked.running=false;break;case 2:blocked.commitment=true;break;case 3:blocked.handoff=true;break;case 4:blocked.shot=true;break;case 5:blocked.reload=true;break;}assert(!blocked.safe());}
    // Holding or entering FPS with a held key cannot repeat or leak an RTS edge.
    Edges keys;assert(keys.sample(1,false)==0);assert(keys.sample(1,true)==0);assert(keys.sample(0,true)==0);
    assert(keys.sample(4,true)==4);for(unsigned i=0;i<100;++i)assert(keys.sample(4,true)==0);
    assert(keys.sample(0,true)==0);assert(keys.sample(2,true)==2);
    assert(valid_slot(Melee,false)&&!valid_slot(Ranged,false)&&valid_slot(Ranged,true));
    for(unsigned bad:{0u,3u,9u,100u})assert(!valid_slot(bad,true));
    assert(crouch_supported(false,true,true,true,true,true,true));
    assert(!crouch_supported(true,true,true,true,true,true,true));
    assert(!crouch_supported(false,false,true,true,true,true,true));
    assert(!crouch_supported(false,true,false,true,true,true,true));
    assert(!crouch_supported(false,true,true,false,true,true,true));
    assert(!crouch_supported(false,true,true,true,false,true,true));
    assert(!crouch_supported(false,true,true,true,true,false,true));
    assert(!crouch_supported(false,true,true,true,true,true,false));
    assert(ready_mode(false,true)==15&&lower_mode(false,true)==9&&reload_mode(false,true)==17);
    assert(ready_mode(false,false)==14&&lower_mode(false,false)==8&&reload_mode(false,false)==16);
    assert(ready_mode(true,true)==32&&lower_mode(true,true)==29&&reload_mode(true,true)==33);
    for(unsigned input=0;input<4096;++input){assert(movement_input(input,true)==0);assert(movement_input(input,false)==input);}
    AmmoOption styles[]={{0,100,false,true},{23,200,false,false},{24,300,false,true}};
    assert(next_ammo(styles,3,0)==2);assert(next_ammo(styles,3,2)==0); // cooldown style skipped; normal always reachable
    styles[2].eligible=false;assert(next_ammo(styles,3,0)==0);
    styles[1].eligible=true;assert(next_ammo(styles,3,0)==1);
    assert(next_ammo(styles,1,0)==0&&next_ammo(nullptr,0,0)==0);
    puts("Equipment81: commitment, input-edge, gear, ability eligibility and kneeling posture regressions passed");
}
