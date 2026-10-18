#pragma once
using CameraCycle103=void(__thiscall*)(void*,uintptr_t,float);
using CursorFilter103=void(__thiscall*)(void*,void*,unsigned);
static CameraCycle103 original_camera_cycle103;
static CursorFilter103 original_cursor_filter103;
static volatile LONG camera_cycle_blocks103,cursor_self_hits103,cursor_queries103,cursor_errors103;
static void __fastcall camera_cycle103(void* object,void*,uintptr_t first,float delta){
    auto camera=reinterpret_cast<uintptr_t>(object);
    if(state&&state->mode==POSSESSED&&!dead_view()&&camera_owned(camera)){InterlockedIncrement(&camera_cycle_blocks103);return;}
    original_camera_cycle103(object,first,delta);
}
static void __fastcall cursor_filter103(void* object,void*,void* vector,unsigned flags){
    original_cursor_filter103(object,vector,flags);
    __try{
        auto cursor=reinterpret_cast<uintptr_t>(object),root=field<uintptr_t>(base,0x1bc8180);
        if(!command_native_root100||root!=command_native_root100||cursor!=root+0x10c||
           command_native_lease100!=possession_lease.generation||!command_native_ready100(input_window)||!command_native_context100(root))return;
        auto v=reinterpret_cast<uintptr_t>(vector);unsigned count=field<unsigned>(v,4),capacity=field<unsigned>(v,0);auto data=field<uintptr_t>(v,8);
        if(!data||count>capacity||count>4096)return;
        unsigned char retain[4096]{};unsigned kept=0;InterlockedIncrement(&cursor_queries103);
        for(unsigned i=0;i<count;++i){auto hit=data+i*28,shape=field<uintptr_t>(hit,0);bool self=false;
            if(shape&&field<unsigned>(shape,0)==8){auto physical=field<uintptr_t>(shape,12);
                if(physical){using Actor=uintptr_t(__thiscall*)(void*);auto vt=field<uintptr_t>(physical,0);
                    auto actor=reinterpret_cast<Actor>(field<uintptr_t>(vt,0x18))(reinterpret_cast<void*>(physical));
                    // Use the same native unit accessor as hit constructor252f50.
                    // Type8 also contains special/banner models with other vtables.
                    if(actor){using Unit=uintptr_t(__thiscall*)(void*);self=reinterpret_cast<Unit>(base+0x3ea960)(reinterpret_cast<void*>(actor))==possession_lease.identity.unit;}
                }
            }
            retain[i]=self?0:1;
        }
        for(unsigned i=0;i<count;++i){if(!retain[i]){InterlockedIncrement(&cursor_self_hits103);continue;}
            if(kept!=i)memmove(reinterpret_cast<void*>(data+kept*28),reinterpret_cast<void*>(data+i*28),28);++kept;
        }
        field<unsigned>(v,4)=kept; // Native owner retains the allocation/capacity.
    }__except(EXCEPTION_EXECUTE_HANDLER){InterlockedIncrement(&cursor_errors103);}
}
static void create_context_hooks103(){
    const unsigned char cycle[]={0x83,0xec,0x0c,0x56,0x8b,0xf1,0x8b,0x8e},filter[]={0x57,0x8b,0xf9,0x8b,0x8f,0xf0,0x0b,0};
    require(!memcmp(reinterpret_cast<void*>(base+0x2f79a0),cycle,8)&&!memcmp(reinterpret_cast<void*>(base+0x10ee5b0),filter,8),"Native context103 signatures differ");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x2f79a0),reinterpret_cast<void*>(&camera_cycle103),reinterpret_cast<void**>(&original_camera_cycle103))==MH_OK,"Native camera cycle103 hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x10ee5b0),reinterpret_cast<void*>(&cursor_filter103),reinterpret_cast<void**>(&original_cursor_filter103))==MH_OK,"Native cursor filter103 hook failed");
}
