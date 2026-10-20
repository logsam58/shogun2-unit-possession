#pragma once
using CameraCycle103=void(__thiscall*)(void*,uintptr_t,float);
using CursorFilter103=void(__thiscall*)(void*,void*,unsigned);
static CameraCycle103 original_camera_cycle103;
static CursorFilter103 original_cursor_filter103;
static volatile LONG camera_cycle_blocks103,cursor_self_hits103,cursor_queries103,cursor_errors103;
using HoverIdentity104=unsigned(__thiscall*)(void*);
static HoverIdentity104 original_hover_identity104;
static volatile LONG selection_follow_blocks104;
static unsigned __fastcall hover_identity104(void* object,void*){
    auto caller=reinterpret_cast<uintptr_t>(_ReturnAddress());unsigned identity=original_hover_identity104(object);
    __try{
        // Only the identity comparison immediately before UI1221ae0 writes
        // manager+8fc. Preserve the accessor for every other native caller.
        if(caller==base+0x1221b9d&&state&&state->mode==POSSESSED&&!dead_view()&&
           GetCurrentThreadId()==state->window_thread&&identity==possession_lease.identity.unit_id&&identity!=0xffffffffu&&
           camera_owned(possession_lease.identity.camera)){
            InterlockedIncrement(&selection_follow_blocks104);return 0xffffffffu;
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){}
    return identity;
}
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
        unsigned char retain[4096]{};unsigned kept=0;bool hostile=false;InterlockedIncrement(&cursor_queries103);
        auto own_side=field<uintptr_t>(possessed,0x44);
        unsigned own_team=own_side?field<unsigned>(own_side,0x10):0xffffffffu;
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
            // Other allied armies are not the possessed unit. Only deprioritize
            // them when this native ray also contains a proven hostile soldier
            // or banner. Unknown shapes and world occluders retain native order.
            if(!self&&own_team<=31&&shape&&field<unsigned>(shape,0)==8){
                auto physical=field<uintptr_t>(shape,12);
                if(physical){using Actor=uintptr_t(__thiscall*)(void*);
                    auto vt=field<uintptr_t>(physical,0);
                    auto actor=reinterpret_cast<Actor>(field<uintptr_t>(vt,0x18))(reinterpret_cast<void*>(physical));
                    if(actor&&field<uintptr_t>(actor,0x28)==environment){
                        auto side=field<uintptr_t>(actor,0x44);
                        if(side){unsigned team=field<unsigned>(side,0x10);
                            if(team<=31){if(team==own_team)retain[i]=2;else hostile=true;}
                        }
                    }
                }
            }
        }
        for(unsigned i=0;i<count;++i){if(!retain[i]||(hostile&&retain[i]==2)){InterlockedIncrement(&cursor_self_hits103);continue;}
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
    const unsigned char identity104[]={0x8b,0x81,0x1c,0x02,0,0,0xc3},comparison104[]={0x39,0x46,0x08,0x75,0x15};
    require(!memcmp(reinterpret_cast<void*>(base+0x3e5170),identity104,7)&&!memcmp(reinterpret_cast<void*>(base+0x1221b9d),comparison104,5),"Native selection identity104 signature differs");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x3e5170),reinterpret_cast<void*>(&hover_identity104),reinterpret_cast<void**>(&original_hover_identity104))==MH_OK,"Native selection follow104 hook failed");
}
