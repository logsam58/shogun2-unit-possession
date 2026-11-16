#pragma once
// The center hit must win a majority of five native screen-space rays.
static constexpr float cursor_pick_radius150=3.f;
static constexpr float cursor_area_radius150=8.f;
struct CameraHover150 {
    uintptr_t exact{};
    uintptr_t hits[9]{};
    bool includes(uintptr_t actor,uintptr_t mount) const {
        for(auto hit:hits)if(hit&&(hit==actor||hit==mount))return true;
        return false;
    }
};
static bool camera_pick_majority150(uintptr_t center,const uintptr_t (&neighbors)[4]){
    if(!center)return false;
    unsigned count=1;
    for(auto hit:neighbors)if(hit==center)++count;
    return count>=3;
}
static uintptr_t camera_actor_ray150(uintptr_t phase,uintptr_t context,float x,float y){
    struct Hits150 {unsigned capacity{},count{};uintptr_t data{};} hits;
    struct Segment150 {unsigned kind{3};float start[3]{},end[3]{};} segment;
    uintptr_t result=0;bool returned=false;
    __try{
        float target[3]{};using Unproject150=void(__thiscall*)(void*,float*,float,float,float);
        reinterpret_cast<Unproject150>(base+0x13660f0)(reinterpret_cast<void*>(context),target,x,y,1.f);
        float direction[3]{},length=0;
        for(unsigned i=0;i<3;++i){
            segment.start[i]=field<float>(context,0x60+i*4);
            if(!std::isfinite(segment.start[i])||!std::isfinite(target[i])||std::abs(segment.start[i])>100000||std::abs(target[i])>100000)return 0;
            direction[i]=target[i]-segment.start[i];length+=direction[i]*direction[i];
        }
        if(!std::isfinite(length)||length<.000001f)return 0;
        length=std::sqrt(length);
        for(unsigned i=0;i<3;++i)segment.end[i]=segment.start[i]+direction[i]*(4096.f/length);
        using Query150=void(__thiscall*)(void*,void*,const void*,unsigned,unsigned,unsigned,unsigned,unsigned);
        reinterpret_cast<Query150>(base+0x158970)(reinterpret_cast<void*>(phase),&hits,&segment,0,1,0x30b,0,0);returned=true;
        if(hits.data&&hits.count&&hits.count<=4096&&hits.capacity>=hits.count){
            // Native158970 sorts the 28-byte hits. The first physical hit wins:
            // do not select a soldier through terrain, walls or another actor.
            auto shape=field<uintptr_t>(hits.data,0);
            if(shape&&field<unsigned>(shape,0)==8){
                auto physical=field<uintptr_t>(shape,12),table=physical?field<uintptr_t>(physical,0):0;
                auto getter=table?field<uintptr_t>(table,0x18):0;
                if(getter>=base&&getter<base+0x1580000){
                    using Actor150=uintptr_t(__thiscall*)(void*);
                    result=reinterpret_cast<Actor150>(getter)(reinterpret_cast<void*>(physical));
                }
            }
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){result=0;}
    if(returned&&hits.data){
        __try{using Free150=void(__cdecl*)(void*);reinterpret_cast<Free150>(base+0xf04d0)(reinterpret_cast<void*>(hits.data));}
        __except(EXCEPTION_EXECUTE_HANDLER){result=0;}
    }
    return result;
}
static CameraHover150 camera_hover_actors150(uintptr_t environment150,uintptr_t phase,uintptr_t camera){
    __try{
        if(!environment150||!phase||!camera||field<uintptr_t>(camera,0)!=base+0x15d1a74)return {};
        RECT viewport{};
        if(!input_window||!GetClientRect(input_window,&viewport)||viewport.right<200||viewport.bottom<200)
            return {};
        // Same normalized client coordinates and rendered context as 335260.
        using Cursor151=const float*(__cdecl*)();
        const auto cursor=reinterpret_cast<Cursor151>(base+0xbd4d70)();
        if(!cursor||!std::isfinite(cursor[0])||!std::isfinite(cursor[1])||
           cursor[0]<0||cursor[0]>1||cursor[1]<0||cursor[1]>1)return {};
        const float x=cursor[0]*2.f-1.f,y=1.f-cursor[1]*2.f;
        const float dx=2.f*cursor_pick_radius150/viewport.right,dy=2.f*cursor_pick_radius150/viewport.bottom;
        using Context150=uintptr_t(__thiscall*)(void*);
        const auto renderer=reinterpret_cast<Context150>(base+0x342a40)(reinterpret_cast<void*>(environment150));
        const auto table=renderer?field<uintptr_t>(renderer,0):0,getter=table?field<uintptr_t>(table,0x18):0;
        if(getter<base||getter>=base+0x1580000)return {};
        const auto context=reinterpret_cast<Context150>(getter)(reinterpret_cast<void*>(renderer));
        if(!context)return {};
        auto ray=[&](float x_offset,float y_offset){
            const float sx=x+x_offset,sy=y+y_offset;
            if(std::abs(sx)>1.f||std::abs(sy)>1.f)return uintptr_t{};
            return camera_actor_ray150(phase,context,sx,sy);
        };
        const float area_x=2.f*cursor_area_radius150/viewport.right;
        const float area_y=2.f*cursor_area_radius150/viewport.bottom;
        const auto center=ray(0,0);
        const uintptr_t neighbors[]={ray(-dx,0),ray(dx,0),ray(0,-dy),ray(0,dy)};
        // The wider samples are only a fallback pool. They cannot change a
        // confirmed exact pick or select through a native world occluder.
        CameraHover150 hover{camera_pick_majority150(center,neighbors)?center:0,
            {center,neighbors[0],neighbors[1],neighbors[2],neighbors[3],
             ray(-area_x,-area_y),ray(area_x,-area_y),ray(-area_x,area_y),ray(area_x,area_y)}};
        return hover;
    }__except(EXCEPTION_EXECUTE_HANDLER){return {};}
}
