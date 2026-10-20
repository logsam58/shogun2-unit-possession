// Test133 restores the accepted Test130 production look path without new limits.
#define main retained_commands133_main
#include "verify_native_commands100.cpp"
#undef main
int main(){try{
    NativeFixture100 f;unsigned cases=0;
    for(unsigned hz:{30u,60u,120u,240u}){
        float dt=1.f/hz;state->yaw=state->pitch=0;mounted_heading85=0;
        animation_look_active104=0;combat_entry_locked126=0;
        mouse_look101(40,-40,0,dt);
        require(std::abs(state->yaw-.1f)<.00001f&&std::abs(state->pitch-.1f)<.00001f,"Accepted foot mouse sensitivity changed");
        for(unsigned i=0;i<hz;++i){mouse_look101(40,0,0,dt);++cases;}
        require(std::abs(std::remainder(state->yaw-float(hz+1)*.1f,6.283185307f))<.0001f,"Ordinary yaw was pinned to model bearing or changed sensitivity");
    }
    state->yaw=state->pitch=0;combat_entry_locked126=1;
    mouse_look101(400,-400,0,.01f);require(state->yaw==0&&state->pitch==0,"Input overrode native pair entry");
    combat_entry_locked126=0;animation_look_active104=1;combat_focus125.valid=true;combat_focus125.unlocked=true;combat_focus125.yaw=0;
    mouse_look101(40,-40,0,.001f);
    require(std::abs(state->yaw-.1f)<.00001f&&std::abs(state->pitch-.1f)<.00001f,"Accepted QTE free-look sensitivity changed");
    std::cout<<"PASS: "<<cases<<" accepted foot-look cases at30/60/120/240Hz; original sensitivity, native pair-entry lock and accepted QTE free-look retained. Exact source restoration checked separately.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
