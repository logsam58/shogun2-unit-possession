#pragma once
struct Choice97 {uintptr_t environment{},phase{},manager{},view{},unit{},holder{},actor{};unsigned serial{},index{};};
static __declspec(thread) Choice97 choice97;
// Roster substitutions belong only to an explicit native FPS update/constructor.
// Other users of this view (including unit/formation work) keep native semantics.
static __declspec(thread) unsigned camera_scope104;
static volatile LONG camera_updates104,holder_resolutions104,roster_forwards104;
static bool camera_owned(uintptr_t);
using ViewIndex97=unsigned(__thiscall*)(void*);
using ViewActor97=uintptr_t(__thiscall*)(void*,unsigned);
static ViewIndex97 original_view_index97;static ViewActor97 original_view_actor97;
static bool choice_valid97(uintptr_t view,const Choice97& c){
    if(!c.actor||view!=c.view||!state||!enabled||state->stop||state->release||GetTickCount64()-state->heartbeat>500)return false;
    auto root=field<uintptr_t>(base,0x1bc8180);if(!root||field<uintptr_t>(root,0x110)!=c.environment)return false;
    auto common=field<uintptr_t>(c.environment,8);if(!common||field<uintptr_t>(common,0xb4)!=c.phase||field<unsigned>(c.phase,0x14)<5||field<unsigned>(c.phase,0x14)>8)return false;
    if(field<uintptr_t>(c.environment,0x281fc)!=c.manager||field<uintptr_t>(c.manager,0x8f8)!=view)return false;
    auto selected=field<uintptr_t>(c.environment,0x2826c),owner=selected?field<uintptr_t>(selected,0x4a8):0;
    if(!owner)return false;auto group=owner+0xb0,members=field<uintptr_t>(group,0x18);
    if(field<uintptr_t>(group,0)!=base+0x15c4da0||field<unsigned>(group,0x14)!=1||field<unsigned>(group,0x10)<1||!members||field<uintptr_t>(members,0)!=c.unit)return false;
    unsigned n=field<unsigned>(view,0x18);auto array=field<uintptr_t>(view,0x1c);if(!array||!n||n>1000)return false;
    bool found=false;for(unsigned i=0;i<n;++i)if(field<uintptr_t>(array,i*4)==c.holder){found=true;break;}
    if(!found||field<uintptr_t>(c.actor,0x28)!=c.environment||field<uintptr_t>(c.holder,8)!=c.actor||field<unsigned>(c.actor,0x1c8)!=c.serial||field<unsigned>(c.actor,0x218)!=c.index)return false;
    bool committed=false;
    if(possession_lease.active&&c.actor==possessed&&c.holder==possession_lease.identity.holder){
        if(!followed(possessed,false,false))return false;
        melee60::Request request{};committed=melee_request(request)||melee_handoff78.matches(locomotion_actor(),possession_lease.generation,
            possession_lease.identity.mount?possession_lease.identity.mount_id:possession_lease.identity.actor_id,possession_lease.identity.unit_id);
    }
    NativeSubject motion;return native_subject(c.actor,c.unit,view,committed,motion)&&!native_routing(c.unit);
}
static Choice97 owned_choice97(uintptr_t view){
    if(!camera_scope104||!possessed||!possession_lease.active||view!=possession_lease.identity.view||!followed(possessed,false,false)||dead_view())return {};
    const auto& i=possession_lease.identity;return {i.environment,i.phase,i.manager,i.view,i.unit,i.holder,i.actor,i.actor_id,field<unsigned>(i.actor,0x218)};
}

using CameraUpdate104=bool(__thiscall*)(void*,float,float);
using ViewHolder104=uintptr_t(__thiscall*)(void*,unsigned);
static CameraUpdate104 original_camera_update104;
static ViewHolder104 original_view_holder104;
static bool __fastcall camera_update104(void* camera,void*,float time,float interpolation){
    bool owned=false;
    __try{owned=state&&state->mode==POSSESSED&&!dead_view()&&camera_owned(reinterpret_cast<uintptr_t>(camera));}__except(EXCEPTION_EXECUTE_HANDLER){}
    if(!owned)return original_camera_update104(camera,time,interpolation);
    ++camera_scope104;InterlockedIncrement(&camera_updates104);
    __try{return original_camera_update104(camera,time,interpolation);}
    __finally{--camera_scope104;}
}
static uintptr_t __fastcall view_holder104(void* object,void*,unsigned index){
    __try{auto view=reinterpret_cast<uintptr_t>(object);auto c=choice97.actor?choice97:owned_choice97(view);
        // Resolve by the current exact holder, not the first equal logical index.
        // Casualty/roster transitions may expose another holder with that index.
        if(index==c.index&&choice_valid97(view,c)){InterlockedIncrement(&holder_resolutions104);return c.holder;}
    }__except(EXCEPTION_EXECUTE_HANDLER){}
    InterlockedIncrement(&roster_forwards104);return original_view_holder104(object,index);
}
static unsigned __fastcall view_index_hook97(void* object,void*){
    __try{auto v=reinterpret_cast<uintptr_t>(object);auto c=choice97.actor?choice97:owned_choice97(v);if(choice_valid97(v,c))return c.index;}__except(EXCEPTION_EXECUTE_HANDLER){}
    return original_view_index97(object);
}
static uintptr_t __fastcall view_actor_hook97(void* object,void*,unsigned index){
    __try{auto v=reinterpret_cast<uintptr_t>(object);auto c=choice97.actor?choice97:owned_choice97(v);
        if(index==c.index&&choice_valid97(v,c)){
            auto actor=reinterpret_cast<ViewActor97>(base+0x340cf0)(object,index);
            if(actor==c.actor)return actor;
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){}
    return original_view_actor97(object,index);
}

using ViewPose98=void*(__thiscall*)(void*,float*,unsigned,float);
static ViewIndex97 original_view_count98;static ViewPose98 original_view_pose98;
static volatile LONG view_count_bypasses98,view_pose_bypasses98,view_native_count98,view_zero_count98;
static unsigned __fastcall view_count_hook98(void* object,void*){
    __try{auto v=reinterpret_cast<uintptr_t>(object);auto c=choice97.actor?choice97:owned_choice97(v);
        if(choice_valid97(v,c)){unsigned native=original_view_count98(object);InterlockedExchange(&view_native_count98,LONG(native));if(!native)InterlockedIncrement(&view_zero_count98);
            if(choice_valid97(v,c)){InterlockedIncrement(&view_count_bypasses98);return field<unsigned>(v,0x18);}return native;}
    }__except(EXCEPTION_EXECUTE_HANDLER){}return original_view_count98(object);
}
static void* __fastcall view_pose_hook98(void* object,void*,float* matrix,unsigned index,float interpolation){
    __try{auto v=reinterpret_cast<uintptr_t>(object);auto c=choice97.actor?choice97:owned_choice97(v);
        if(matrix&&index==c.index&&choice_valid97(v,c)){
            auto actor=reinterpret_cast<ViewActor97>(base+0x340cf0)(object,index);
            if(actor==c.actor){InterlockedIncrement(&view_pose_bypasses98);return reinterpret_cast<ViewPose98>(base+0x340ec0)(object,matrix,index,interpolation);}
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){}return original_view_pose98(object,matrix,index,interpolation);
}
