#pragma once
#include "head_look131.h"
namespace bodylook132 {
// Numeric desired bearing only. It never owns native state or stores an actor.
// The visible foot view stays on the body; prepare uses this bounded goal in
// the existing native face_view step. Discard excess input at the stop.
struct Goal {
    bool valid=false;float yaw=0;
    void reset(){valid=false;yaw=0;}
    float target(float body){if(!valid){yaw=body;valid=true;}return yaw=head131::constrain(yaw,body);}
    void mouse(float body,float delta){yaw=head131::mouse(target(body),delta,body);}
};
}
