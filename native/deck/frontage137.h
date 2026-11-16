#pragma once
#include "terrain224.h"
#include "orders175.h"

namespace deck175 {

class GroupFrontage {
    static constexpr unsigned queue_capacity = 0x5000;
    uintptr_t group_{};
    uintptr_t group_table_{};
    unsigned group_id_{};
    float initial_width_{}, minimum_{}, maximum_{};

    static uintptr_t word(uintptr_t address) {
        uintptr_t value{};
        native214::value(address,value);
        return value;
    }

    static uintptr_t selected_group(uintptr_t root) {
        const auto owner=word(root+0x5b4);
        if(!owner||word(owner+0xb8)!=1||word(owner+0xc4))return 0;
        const auto entries=word(owner+0xbc);
        return entries?word(entries):0;
    }

    static bool group_limits(uintptr_t image,uintptr_t group,float& low,float& high) {
        const auto policy=word(group+0x28);
        const auto table=policy?word(policy):0;
        const auto min_slot=table?word(table+0x10):0;
        const auto max_slot=table?word(table+0x14):0;
        if(min_slot<image+0x1000||min_slot>=image+0x1580000||
           max_slot<image+0x1000||max_slot>=image+0x1580000)return false;
        __try {
            using Limit=float(__thiscall*)(uintptr_t);
            low=reinterpret_cast<Limit>(image+0x465620)(group);
            high=reinterpret_cast<Limit>(image+0x464a40)(group);
            return std::isfinite(low)&&std::isfinite(high)&&low>0&&high>low&&high<=8192;
        } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    }

    static bool serialize(uintptr_t image,uintptr_t selection,const terrain224::Point& point,
                          uint16_t facing,float width,unsigned key,unsigned char flags,
                          unsigned char* packet,unsigned& length) {
        alignas(16) unsigned char order[0x74]{},context[16]{};
        bool context_live=false,serialized=false;
        __try {
            using Construct=void*(__thiscall*)(void*,uintptr_t,const terrain224::Point*,unsigned,float,unsigned,unsigned);
            using Serialize=unsigned char(__thiscall*)(void*,void*);
            using Context=void(__thiscall*)(void*,void*,unsigned,unsigned);
            reinterpret_cast<Construct>(image+0x2557e0)(order,selection,&point,facing,width,0,1);
            const auto table=word(reinterpret_cast<uintptr_t>(order));
            if(table==image+0x15cffdc&&word(table)==image+0x2e6f00) {
                reinterpret_cast<Context>(image+0x1cc200)(context,packet,key,flags);
                context_live=true;
                serialized=reinterpret_cast<Serialize>(image+0x2e6f00)(order,context)!=0;
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) { serialized=false; }
        __try {
            using Cleanup=void(__thiscall*)(void*);
            if(context_live)reinterpret_cast<Cleanup>(image+0x1d0d40)(context);
            if(word(reinterpret_cast<uintptr_t>(order)+4)==image+0x15c4da0)
                reinterpret_cast<Cleanup>(image+0x1d10f0)(order+4);
        } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
        memcpy(&length,packet+queue_capacity,4);
        if(!serialized||length<7||length>queue_capacity)return false;
        const unsigned packet_size=unsigned(packet[0])|(unsigned(packet[1])<<8);
        const unsigned descriptor=(unsigned(packet[2])<<24)|(unsigned(packet[3])<<16)|
            (unsigned(packet[4])<<8)|packet[5];
        return packet_size==length&&descriptor==key;
    }

    // The stock terrain-drag release emits one MOVE_ORIENTATION_AND_WIDTH
    // order. Use its original constructor and serializer, then append the
    // validated packet the same way as the existing Deck formation orders.
    static bool send(uintptr_t image,uintptr_t queue,uintptr_t selection,
                     const terrain224::Point& point,uint16_t facing,float width) {
        const auto key=word(image+0x192eaac);
        const auto registry=word(image+0x18d9000);
        const auto count=word(image+0x18d9004);
        if(!key||!registry||!count||count>4096)return false;

        unsigned entry=0;
        while(entry<count&&word(registry+entry*28)!=key)++entry;
        unsigned char flags{};
        if(entry==count||!native214::value(registry+entry*28+0x10,flags))return false;

        alignas(16) unsigned char packet[queue_capacity+4]{};
        unsigned length{},used{};
        if(!serialize(image,selection,point,facing,width,unsigned(key),flags,packet,length)||
           !native214::value(queue+4+queue_capacity,used)||used>queue_capacity||length>queue_capacity-used)return false;
        SIZE_T written{};
        if(!WriteProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(queue+4+used),packet,length,&written)||
           written!=length)return false;
        MemoryBarrier();
        InterlockedExchange(reinterpret_cast<volatile LONG*>(queue+4+queue_capacity),LONG(used+length));
        return true;
    }

public:
    static bool has_groups(uintptr_t root) {
        const auto owner=word(root+0x5b4);
        return owner&&word(owner+0xb8)!=0;
    }

    void clear() { *this={}; }

    bool selection_live(uintptr_t image,uintptr_t root) const {
        return group_&&word(image+0x1bc8180)==root&&selected_group(root)==group_&&
            word(group_)==group_table_&&word(group_+0x7c)==group_id_;
    }

    template<class Current>
    bool begin(uintptr_t image,uintptr_t root,uintptr_t environment,Current current) {
        clear();
        if(!native_order_ready(image,root,environment,word,current,true))return false;
        const auto group=selected_group(root);
        if(!group||!group_limits(image,group,minimum_,maximum_))return false;
        if(!native214::value(group+0x44,initial_width_)||!std::isfinite(initial_width_)||initial_width_<=0)return false;
        initial_width_=std::clamp(initial_width_,minimum_,maximum_);
        group_table_=word(group);
        group_id_=unsigned(word(group+0x7c));
        group_=group_table_?group:0;
        return group_!=0;
    }

    template<class Current>
    bool release(uintptr_t image,uintptr_t root,uintptr_t environment,float spread,Current current) {
        const auto group=group_;
        const bool still_selected=selection_live(image,root);
        const float limit=spread<0?minimum_:maximum_;
        const float width=initial_width_+std::fabs(spread)*(limit-initial_width_);
        const bool changed=std::isfinite(spread)&&std::fabs(spread)<=1&&
            std::isfinite(width)&&std::fabs(width-initial_width_)>.001f;
        clear();  // The release is consumed even if the native writer refuses it.
        if(!still_selected||!changed||!native_order_ready(image,root,environment,word,current,true))return false;

        terrain224::Point point{};
        uint16_t facing{};
        if(!native214::value(group+0x2c,point)||!native214::value(group+0x40,facing))return false;
        for(float axis:point.p)if(!std::isfinite(axis))return false;
        const auto owner=word(root+0x5b4);
        const auto queue=word(environment+0x28048);
        return owner&&queue&&send(image,queue,owner+0xb0,point,facing,width);
    }
};

}
