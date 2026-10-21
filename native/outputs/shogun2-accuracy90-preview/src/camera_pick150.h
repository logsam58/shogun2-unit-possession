#pragma once
// One native cursor-ray query at F9 entry, only at the stock RTS zoom floor.
// No cursor/input mutation, orders, recurring scan or replacement geometry.
static uintptr_t camera_center_actor150(uintptr_t environment150,uintptr_t phase,uintptr_t camera){
    struct Hits150 {unsigned capacity{},count{};uintptr_t data{};} hits;
    struct Segment150 {unsigned kind{3};float start[3]{},end[3]{};} segment;
    uintptr_t result=0;bool returned=false;
    __try{
        if(!environment150||!phase||!camera||field<uintptr_t>(camera,0)!=base+0x15d1a74)return 0;
        // Original293cd0 clamps camera+180 against this virtual144 getter.
        // Original29a450 selects the live native property; a0b60 returns x87.
        using Minimum150=float(__thiscall*)(void*);
        float minimum=reinterpret_cast<Minimum150>(base+0x29a450)(reinterpret_cast<void*>(camera));
        float height=field<float>(camera,0x180);
        if(!std::isfinite(minimum)||!std::isfinite(height)||minimum<0||height<0||height>minimum)return 0;
        // Original335260 reads bd4d70's normalized CLIENT cursor coordinates.
        // Use the same coordinates and transform, not the center of the screen.
        using Cursor151=const float*(__cdecl*)();
        auto cursor=reinterpret_cast<Cursor151>(base+0xbd4d70)();
        if(!cursor||!std::isfinite(cursor[0])||!std::isfinite(cursor[1])||
           cursor[0]<0||cursor[0]>1||cursor[1]<0||cursor[1]>1)return 0;
        float x151=cursor[0]*2.f-1.f,y151=1.f-cursor[1]*2.f;
        // Same rendered context/unprojection, including cached matrices.
        using Context150=uintptr_t(__thiscall*)(void*);
        auto renderer=reinterpret_cast<Context150>(base+0x342a40)(reinterpret_cast<void*>(environment150));
        auto table=renderer?field<uintptr_t>(renderer,0):0,getter=table?field<uintptr_t>(table,0x18):0;
        if(getter<base||getter>=base+0x1580000)return 0;
        auto context=reinterpret_cast<Context150>(getter)(reinterpret_cast<void*>(renderer));if(!context)return 0;
        float target[3]{};using Unproject150=void(__thiscall*)(void*,float*,float,float,float);
        reinterpret_cast<Unproject150>(base+0x13660f0)(reinterpret_cast<void*>(context),target,x151,y151,1.f);
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
    // Entry's existing selected-view/NativeSubject checks prove this pointer
    // against the current holder, ownership, mount backlinks and lifetime.
    return result;
}
