#pragma once
#include "orders.h"
#include "order_signatures.h"
// Included after adapter state and validated possession/camera helpers.
// All native calls below run inside the camera callback on the window thread.
static uint64_t next_follow;
static float follow_x,follow_z;
static bool follow_target;
static volatile LONG order_busy;
using HaltConstructor=void*(__thiscall*)(void*,const void*);
using MoveConstructor=void*(__thiscall*)(void*,const void*,const void*,unsigned,unsigned);
using MoveParameters=void*(__thiscall*)(void*,void*,const float*,unsigned,unsigned,unsigned);
using ContextConstructor=void*(__thiscall*)(void*,void*,uint32_t,unsigned);
using SerializeOrder=unsigned(__thiscall*)(void*,void*);
using NativeCleanup=void(__thiscall*)(void*);
static bool descriptor_flags(uint32_t key,unsigned& flags) {
    uintptr_t table=field<uintptr_t>(base,0x18d9000);
    unsigned count=field<unsigned>(base,0x18d9004);
    if(!key||!table||!count||count>4096)return false;
    for(unsigned i=0;i<count;++i)if(field<uint32_t>(table,i*28)==key){flags=field<unsigned char>(table,i*28+0x10);return true;}
    return false;
}
static uintptr_t selected_possessed_group() {
    uintptr_t selected=field<uintptr_t>(environment,0x2826c);if(!selected)return 0;
    uintptr_t owner=field<uintptr_t>(selected,0x4a8);if(!owner)return 0;
    uintptr_t group=owner+0xb0;
    if(field<uintptr_t>(group,0)!=base+0x15c4da0)return 0;
    for(unsigned off: {4u,0x1cu,0x28u,0x38u})if(field<unsigned>(group,off+4)!=0)return 0;
    if(field<unsigned>(group,0x14)!=1||field<unsigned>(group,0x10)<1)return 0;
    uintptr_t array=field<uintptr_t>(group,0x18);if(!array||field<uintptr_t>(array,0)!=unit)return 0;
    return group;
}
static bool issue_native_order(unsigned command,float x,float z) {
    alignas(16) unsigned char order[0x6c]{};
    alignas(16) unsigned char parameters[20]{};
    alignas(16) unsigned char context[16]{};
    alignas(16) unsigned char packet[QUEUE_CAPACITY+4]{};
    bool constructed=false,result=false;
    __try {
        if(!possessed||state->mode!=POSSESSED||!followed(possessed,false))return false;
        uintptr_t group=selected_possessed_group();if(!group){state->order_error=41;return false;}
        uintptr_t queue=field<uintptr_t>(environment,0x28048);if(!queue){state->order_error=42;return false;}
        uint32_t key=field<uint32_t>(base,command==ORDER_HOLD?0x192da04:0x192e0d0);
        unsigned flags=0;if(!descriptor_flags(key,flags)){state->order_error=43;return false;}
        if(command==ORDER_HOLD){
            reinterpret_cast<HaltConstructor>(base+0x254f00)(order,reinterpret_cast<void*>(group));
        }else{
            if(!std::isfinite(x)||!std::isfinite(z)||std::abs(x)>100000||std::abs(z)>100000){state->order_error=44;return false;}
            uintptr_t common=field<uintptr_t>(environment,8),manager=field<uintptr_t>(common,0xb8);
            if(!manager||field<uintptr_t>(manager,0)!=environment){state->order_error=45;return false;}
            const float destination[2]={x,z};
            reinterpret_cast<MoveParameters>(base+0x37c010)(reinterpret_cast<void*>(manager),parameters,destination,0,1,0);
            reinterpret_cast<MoveConstructor>(base+0x255550)(order,reinterpret_cast<void*>(group),parameters,0,0);
        }
        constructed=true;
        uintptr_t expected=base+(command==ORDER_HOLD?0x15d0020:0x15cffd8);
        if(field<uintptr_t>(reinterpret_cast<uintptr_t>(order),0)!=expected){state->order_error=47;}
        else{
            reinterpret_cast<ContextConstructor>(base+0x1cc200)(context,packet,key,flags);
            auto serializer=reinterpret_cast<SerializeOrder>(field<uintptr_t>(expected,0));
            if(serializer(order,context)){
                reinterpret_cast<NativeCleanup>(base+0x1d0d40)(context);
                uint32_t size=field<uint32_t>(reinterpret_cast<uintptr_t>(packet),QUEUE_CAPACITY);
                unsigned char* buffer=reinterpret_cast<unsigned char*>(queue+4);
                auto& used=field<uint32_t>(queue+4,QUEUE_CAPACITY);
                state->queue_before=used;
                if(append_packet(buffer,used,packet,size,key)){
                    state->packet_size=size;state->queue_after=used;state->last_order=command;
                    state->order_target_x=x;state->order_target_z=z;state->order_error=0;
                    ++state->orders_sent;result=true;
                }else state->order_error=48;
            }else state->order_error=49;
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){state->order_error=60;InterlockedExchange(&state->follow,0);}
    if(constructed){
        __try {reinterpret_cast<NativeCleanup>(base+0x1d10f0)(order+4);}
        __except(EXCEPTION_EXECUTE_HANDLER){state->order_error=61;result=false;InterlockedExchange(&state->follow,0);}
    }
    return result;
}
static void observe_formation(uintptr_t camera) {
    uintptr_t view=field<uintptr_t>(camera,0x184),array=field<uintptr_t>(view,0x1c);
    unsigned count=field<unsigned>(view,0x18),n=0;double x=0,z=0;
    for(unsigned i=0;i<count&&count<=1000;++i){
        uintptr_t holder=field<uintptr_t>(array,i*4),actor=holder?field<uintptr_t>(holder,8):0;
        if(actor&&actor!=possessed&&field<unsigned char>(actor,0xf0)==1){
            float ax=field<float>(actor,0x48),az=field<float>(actor,0x50);
            if(std::isfinite(ax)&&std::isfinite(az)){x+=ax;z+=az;++n;}
        }
    }
    state->formation_count=n;if(n){state->formation_x=float(x/n);state->formation_z=float(z/n);}
}
