
#pragma once
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace exactreplay147 {
enum class Kind : uint8_t {
    motion=1,
    create_squad, retire_squad, transfer_member, insert_member, remove_member,
    cancel_formation_task, reselect_main, reform_unit, officer_slot,
    detach_begin, detach_end,
    gait_cancel_idle, gait_mode, gait_pose,
};
enum class Boundary : uint8_t { none, before_unit, after_unit, before_motion };
enum class Stream { motion, formation, gait };
inline constexpr Stream stream(Kind kind){
    if(kind==Kind::motion)return Stream::motion;
    if(kind>=Kind::gait_cancel_idle)return Stream::gait;
    return Stream::formation;
}
struct Event {
    uint32_t tick{}, actor{}, unit{};
    float target_x{}, target_z{};
    uint16_t facing{};
    Kind kind{};
    Boundary boundary{};
    uint32_t target{},source{},proof{};
    uint16_t occurrence{};
};

constexpr uint32_t key=0xe14750a3u;
constexpr uint32_t wire_version=2;
constexpr uint32_t queue_limit=20480;
constexpr uint32_t tick_limit=180000;
constexpr uint32_t block_limit=8;
constexpr size_t frame_size=46;

inline bool valid(Event e){
    if(!e.tick||e.tick>tick_limit||!e.actor||!e.unit||e.kind<Kind::motion||e.kind>Kind::gait_pose)return false;
    if(!std::isfinite(e.target_x)||!std::isfinite(e.target_z))return false;
    const auto lane=stream(e.kind);
    const auto unit_boundary=e.boundary==Boundary::before_unit||e.boundary==Boundary::after_unit;
    if(lane==Stream::formation&&(!unit_boundary||!e.occurrence))return false;
    if(lane!=Stream::formation&&e.boundary!=Boundary::none&&
       !(e.kind==Kind::gait_cancel_idle&&e.boundary==Boundary::before_motion))return false;
    if(lane==Stream::gait&&!e.occurrence)return false;
    if(e.kind==Kind::motion){
        return !e.source&&std::abs(e.target_x)<=100000.f&&std::abs(e.target_z)<=100000.f;
    }
    if(e.target_x||e.target_z)return false;
    switch(e.kind){
    case Kind::create_squad: return e.target&&e.proof&&(e.facing==2||e.facing==3)&&(e.facing==3||!e.source);
    case Kind::retire_squad: case Kind::remove_member:
        return e.target&&!e.source&&e.proof&&!e.facing;
    case Kind::transfer_member:
        return e.target&&e.proof&&e.facing<=1;
    case Kind::insert_member:
        return e.target&&!e.source&&e.proof&&e.facing==1;
    case Kind::reselect_main:
        return e.target&&e.source&&e.proof&&!e.facing;
    case Kind::reform_unit:
        return e.target&&!e.source&&e.proof&&e.facing<=1;
    case Kind::officer_slot:
        return e.target&&!e.source&&e.proof&&!e.facing;
    case Kind::cancel_formation_task: case Kind::detach_begin: case Kind::detach_end:
        return !e.target&&!e.source&&!e.proof&&!e.facing;
    case Kind::gait_cancel_idle:
        return e.target&&!e.source&&!e.proof&&!e.facing;
    case Kind::gait_mode:
        return e.target!=e.source&&e.target<=255&&e.source<=255&&!e.proof&&!e.facing;
    case Kind::gait_pose:
        return e.target<=1&&e.source<=1&&e.proof&&!e.facing;
    default:return false;
    }
}
template<class T> inline void store(unsigned char* at,T value){std::memcpy(at,&value,sizeof value);}
template<class T> inline T read(const unsigned char* at){T value;std::memcpy(&value,at,sizeof value);return value;}
inline bool encode(unsigned char (&frame)[frame_size],Event event){
    if(!valid(event))return false;
    std::memset(frame,0,frame_size);
    store<uint16_t>(frame,frame_size);
    for(unsigned i=0;i<4;++i)frame[2+i]=uint8_t(key>>(24-8*i));
    frame[7]=wire_version;
    frame[8]=static_cast<uint8_t>(event.kind);
    store<uint32_t>(frame+9,event.tick);
    store<uint32_t>(frame+13,event.actor);
    store<uint32_t>(frame+17,event.unit);
    store<float>(frame+21,event.target_x);
    store<float>(frame+25,event.target_z);
    store<uint16_t>(frame+29,event.facing);
    frame[31]=static_cast<uint8_t>(event.boundary);
    store<uint32_t>(frame+32,event.target);
    store<uint32_t>(frame+36,event.source);
    store<uint32_t>(frame+40,event.proof);
    store<uint16_t>(frame+44,event.occurrence);
    return true;
}
inline uint32_t token(const unsigned char* frame){
    uint32_t result=0;
    for(unsigned i=2;i<6;++i)result=(result<<8)|frame[i];
    return result;
}
inline bool decode(const unsigned char* frame,size_t length,uint32_t tick,Event& event){
    if(length!=frame_size||read<uint16_t>(frame)!=frame_size||token(frame)!=key||
       frame[6]||frame[7]!=wire_version)return false;
    Event found{read<uint32_t>(frame+9),read<uint32_t>(frame+13),read<uint32_t>(frame+17),
                read<float>(frame+21),read<float>(frame+25),read<uint16_t>(frame+29),static_cast<Kind>(frame[8]),static_cast<Boundary>(frame[31]),
                read<uint32_t>(frame+32),read<uint32_t>(frame+36),read<uint32_t>(frame+40),read<uint16_t>(frame+44)};
    if(found.tick!=tick||!valid(found))return false;
    event=found;
    return true;
}

template<class Accept>
inline bool visit(const unsigned char* data,size_t length,uint32_t tick,Accept accept){
    if(length>queue_limit||(!data&&length))return false;
    while(length){
        if(length<7)return false;
        const auto size=read<uint16_t>(data);
        if(size<7||size>length)return false;
        if(token(data)==key){
            Event event;
            if(!decode(data,size,tick,event)||!accept(event))return false;
        }
        length-=size;
        data+=size;
    }
    return true;
}

// Stock b90ba0 and 2359a0 read these records as a borrowed view.
struct Block {uint32_t capacity{},length{};const unsigned char* bytes{};};
struct Tick {uint32_t delta{},capacity{},count{};const Block* blocks{};};

class SaveView {
    struct Owned { Tick tick{};std::vector<Block> blocks;std::vector<unsigned char> bytes; };
    std::vector<const Tick*> ticks_;
    std::vector<Owned> owned_;
public:
    SaveView()=default;
    SaveView(const SaveView&)=delete;
    SaveView& operator=(const SaveView&)=delete;
    bool build(const Tick* const* originals,uint32_t count,const std::vector<Event>& events){
        if(!originals||!count||count>tick_limit||events.empty()||events.size()>tick_limit)return false;
        ticks_.assign(originals,originals+count);
        owned_.clear();
        owned_.reserve(events.size());
        size_t start=0;
        uint32_t previous=0;
        while(start<events.size()){
            const auto tick=events[start].tick;
            if(tick<=previous||tick>count)return false;
            const auto* source=ticks_[tick-1];
            if(!source||source->count>block_limit||(source->count&&!source->blocks))return false;
            size_t end=start+1;
            while(end<events.size()&&events[end].tick==tick)++end;
            const size_t additional=(end-start)*frame_size;
            if(additional>queue_limit)return false;

            owned_.emplace_back();
            auto& owner=owned_.back();
            owner.tick=*source;
            if(source->count)owner.blocks.assign(source->blocks,source->blocks+source->count);
            for(const auto& block:owner.blocks)
                if(block.length>queue_limit||(!block.bytes&&block.length))return false;
            const bool extend=!owner.blocks.empty()&&owner.blocks.back().length<=queue_limit-additional;
            if(!extend&&owner.blocks.size()==block_limit)return false;
            if(extend){
                const auto& block=owner.blocks.back();
                if(block.length)owner.bytes.assign(block.bytes,block.bytes+block.length);
            }
            owner.bytes.reserve(owner.bytes.size()+additional);
            for(size_t i=start;i<end;++i){
                unsigned char frame[frame_size];
                if(!encode(frame,events[i]))return false;
                owner.bytes.insert(owner.bytes.end(),frame,frame+frame_size);
            }
            const auto total=static_cast<uint32_t>(owner.bytes.size());
            const Block block{total,total,owner.bytes.data()};
            if(extend)owner.blocks.back()=block;
            else owner.blocks.push_back(block);
            owner.tick.capacity=owner.tick.count=static_cast<uint32_t>(owner.blocks.size());
            owner.tick.blocks=owner.blocks.data();
            ticks_[tick-1]=&owner.tick;
            start=end;
            previous=tick;
        }
        return true;
    }
    const Tick* const* ticks() const {return ticks_.data();}
};
}

