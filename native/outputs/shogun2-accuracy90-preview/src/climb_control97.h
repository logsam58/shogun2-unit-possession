#pragma once
#include "climb_protocol97.h"
static volatile LONG native_air_updates99,native_air_active99;
static SRWLOCK climb_publish_lock97=SRWLOCK_INIT;
static climb97::Status* climb_shared97;static HANDLE climb_mapping97;
static volatile LONG climb_attaches97,climb_steps97,climb_holds97,climb_down97,climb_endpoints97,climb_rejected97;
static unsigned climb_lease97;
static uintptr_t climb_budget_pipe98;static unsigned climb_budget_id98,climb_budget_tick98,climb_budget_lease98;
static bool climb_budget_valid98;static float climb_rate98,climb_step98;static volatile LONG climb_duplicate98,climb_rate_errors98;static int climb_last_direction97=1;
static uint64_t climb_scan97;static uintptr_t climb_near97;static unsigned climb_near_id97;static bool climb_near_top97;
using PipeAdvance97=void(__thiscall*)(void*,uintptr_t,float);
using PipeMode97=unsigned(__thiscall*)(void*,uintptr_t);
using PipeBind97=void(__thiscall*)(void*,uintptr_t);
using PipePoint97=uintptr_t(__thiscall*)(void*,float*,float);
using NativeReset97=void(__thiscall*)(void*,const float*,unsigned,uintptr_t,unsigned);
static PipeAdvance97 original_pipe_advance97;static PipeMode97 original_pipe_mode97;
static void climb_publish97(unsigned mode,float progress=0,int direction=0){
    InterlockedExchange(&keyboard_wall98,mode>=climb97::Climbing?1:0);
    if(!climb_shared97)return;
    AcquireSRWLockExclusive(&climb_publish_lock97);
    InterlockedIncrement(&climb_shared97->sequence);MemoryBarrier();
    climb_shared97->lease=possession_lease.generation;climb_shared97->actor=state?state->actor_id:0;climb_shared97->unit=state?state->unit_id:0;
    climb_shared97->mode=mode;climb_shared97->progress=progress;climb_shared97->direction=direction;climb_shared97->tick=GetTickCount64();
    MemoryBarrier();InterlockedIncrement(&climb_shared97->sequence);ReleaseSRWLockExclusive(&climb_publish_lock97);
}
static void climb_reset97(){InterlockedExchange(&native_air_active99,0);climb_budget_valid98=false;climb_budget_pipe98=0;climb_rate98=climb_step98=0;climb_near97=0;climb_scan97=0;climb_lease97=0;climb_last_direction97=1;InterlockedExchange(&climb_action97,0);climb_publish97(climb97::Off);}
static bool climb_owned97(uintptr_t actor,uintptr_t pipe){
    if(!enabled||!state||state->stop||state->release||state->mode!=POSSESSED||actor!=possessed||!possession_lease.active||weapon_mounted()||
        GetTickCount64()-state->heartbeat>500||field<uintptr_t>(actor,0x18)!=pipe)return false;
    if(!followed(actor,false,false)||dead_view())return false;
    melee60::Request qte;if(melee_request(qte))return false;
    Wall97 w;return read_wall97(actor,pipe,w);
}
static bool climb_exit97(uintptr_t actor,const Wall97& w,bool top){
    auto portal=field<uintptr_t>(w.pipe,top?0x2c:0x28);if(!portal)return false;
    auto vt=field<uintptr_t>(portal,0);if(vt<base||vt>=base+0x1d88000)return false;
    auto method=field<uintptr_t>(vt,0x14);if(method<base||method>=base+0x1580000)return false;
    auto surface=reinterpret_cast<uintptr_t(__thiscall*)(void*)>(method)(reinterpret_cast<void*>(portal));if(!surface)return false;
    auto node=w.nodes+(top?w.count-1:0)*72;float position[2]={field<float>(node,top?0x30:0x24),field<float>(node,top?0x38:0x2c)};
    // Only a true path endpoint, within one native step. Native spatial remove,
    // ground/surface resolution and re-registration stay in original3dfe30.
    float distance2=0;for(unsigned k=0;k<3;++k){float d=field<float>(actor,0x48+k*4)-field<float>(node,(top?0x30:0x24)+k*4);distance2+=d*d;}
    if(!std::isfinite(distance2)||distance2>.25f)return false;
    reinterpret_cast<NativeReset97>(base+0x3dfe30)(reinterpret_cast<void*>(actor),position,field<unsigned short>(actor,0x64),surface,0);
    if(field<uintptr_t>(actor,0x18))return false;
    original_change(reinterpret_cast<void*>(actor),8,0,0);InterlockedIncrement(&climb_endpoints97);return true;
}
static bool native_climb_rate98(uintptr_t actor,const Wall97& wall,int direction,float& rate){
    auto native_unit=field<uintptr_t>(actor,0x214);bool fast=field<unsigned char>(native_unit,0x294)==1;
    // Native3dda00 scales ascending68/70 only. Descending69/71 uses
    // the authored segment rate without an ascending parameter multiplier.
    float native=direction<0?1.f:reinterpret_cast<float(__thiscall*)(void*)>(base+0xa0b60)(reinterpret_cast<void*>(base+(fast?0x1a4d2e0:0x1a4d3a0)));
    float segment=reinterpret_cast<float(__thiscall*)(void*,float)>(base+0x3a2370)(reinterpret_cast<void*>(wall.definition),field<float>(actor,0x20));
    rate=native*segment;return std::isfinite(rate)&&rate>=0&&rate<=20;
}
static void __fastcall pipe_advance_hook97(void* motion,void*,uintptr_t pipe,float distance){
    auto actor=reinterpret_cast<uintptr_t>(motion)-0x14;bool handled=false;
    __try{
        if(climb_owned97(actor,pipe)){
            if(crest_subject97(actor)){climb_publish97(climb97::Crest,field<float>(actor,0x20));}
            else {Wall97 wall;if(read_wall97(actor,pipe,wall)){
                int direction=climb97::direction(unsigned(state->input));auto common=possession_lease.identity.common;
                unsigned tick=field<unsigned>(common,0x50);float dt=field<float>(base,0x15b2448);
                if(climb_budget_valid98&&climb_budget_pipe98==pipe&&climb_budget_id98==wall.id&&climb_budget_lease98==possession_lease.generation&&climb_budget_tick98==tick){InterlockedIncrement(&climb_duplicate98);handled=true;}
                else {
                    climb_budget_valid98=true;climb_budget_pipe98=pipe;climb_budget_id98=wall.id;climb_budget_lease98=possession_lease.generation;climb_budget_tick98=tick;
                    if(climb_lease97!=possession_lease.generation){climb_lease97=possession_lease.generation;climb_last_direction97=direction<0?-1:1;}
                    if(direction)climb_last_direction97=direction;
                    climb_rate98=0;if(!native_climb_rate98(actor,wall,direction,climb_rate98)){direction=0;InterlockedIncrement(&climb_rate_errors98);}
                    if(!std::isfinite(dt)||dt<=0||dt>.2f)direction=0;
                    climb_step98=direction?climb_rate98*dt:0;
                    // Never feed a prior path correction/ground velocity back
                    // into native max(distance,pending). One native tick budget.
                    for(unsigned off:{0x70u,0x74u,0x78u,0x7cu,0xd8u,0xdcu,0xe0u})field<float>(actor,off)=0;
                    original_pipe_advance97(motion,pipe,climb_step98);handled=true;
                    if(direction<0){float previous=field<float>(actor,0x24),length=reinterpret_cast<float(__thiscall*)(void*)>(base+0x3931f0)(reinterpret_cast<void*>(wall.definition));
                        if(std::isfinite(previous)&&previous>=0&&previous<=1&&std::isfinite(length)&&length>.001f){
                            float target=std::clamp(previous-climb_step98/length,0.f,1.f),point[4]{};
                            reinterpret_cast<PipePoint97>(base+0x39a2e0)(reinterpret_cast<void*>(wall.definition),point,target);
                            bool finite=true;for(unsigned k=0;k<3;++k)if(!std::isfinite(point[k])||std::abs(point[k]-field<float>(actor,0x48+k*4))>2)finite=false;
                            if(finite){field<float>(actor,0x20)=target;for(unsigned k=0;k<3;++k)field<float>(actor,0xd8+k*4)=point[k]-field<float>(actor,0x48+k*4);InterlockedIncrement(&climb_down97);}
                        }
                    }
                    if(!direction)InterlockedIncrement(&climb_holds97);InterlockedIncrement(&climb_steps97);
                    float progress=field<float>(actor,0x20);climb_publish97(climb97::Climbing,progress,direction);
                    if(direction<0&&progress<=.000001f)climb_exit97(actor,wall,false);
                    else if(direction>0&&progress>=.999999f)climb_exit97(actor,wall,true);
                }
            }}
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){InterlockedIncrement(&climb_rejected97);}
    if(!handled)original_pipe_advance97(motion,pipe,distance);
}
static bool climb_native_air99(uintptr_t actor){
    bool active=native_airborne_subject99(actor);InterlockedExchange(&native_air_active99,active?1:0);
    if(!active)return false;
    InterlockedExchange(&climb_action97,0);InterlockedExchange(&state->aiming,0);InterlockedExchange(&state->fire_request,0);
    climb_publish97(climb97::Off);InterlockedExchange(&keyboard_wall98,1);return true;
}
static void climb_adopt98(uintptr_t actor){climb_budget_valid98=false;climb_near97=0;climb_lease97=possession_lease.generation;
    if(climb_native_air99(actor))return;
    if(wall_subject97(actor))climb_publish97(crest_subject97(actor)?climb97::Crest:climb97::Climbing,field<float>(actor,0x20));else climb_publish97(climb97::Off);
}
static void climb_observe98(uintptr_t actor){if(climb_native_air99(actor))return;if(wall_subject97(actor))climb_publish97(crest_subject97(actor)?climb97::Crest:climb97::Climbing,field<float>(actor,0x20),climb97::direction(unsigned(state->input)));}
static unsigned __fastcall pipe_mode_hook97(void* object,void*,uintptr_t pipe){
    unsigned mode=original_pipe_mode97(object,pipe);
    __try{auto actor=reinterpret_cast<uintptr_t>(object);
        if(climb_owned97(actor,pipe)&&field<unsigned>(actor,0x1f0)<72){
            int direction=climb97::direction(unsigned(state->input));if(!direction&&climb_lease97==possession_lease.generation)direction=climb_last_direction97;
            if(direction<0&&(mode>=68&&mode<=75))return mode==70||mode==71||(mode>=72&&field<unsigned char>(field<uintptr_t>(actor,0x214),0x294)==1)?71:69;
            if(direction>0&&(mode==69||mode==71))return mode==71?70:68;
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){InterlockedIncrement(&climb_rejected97);}
    return mode;
}
static bool climb_prepare97(uintptr_t actor){
    if(climb_native_air99(actor)){InterlockedIncrement(&native_air_updates99);return true;}
    if(wall_subject97(actor)){InterlockedExchange(&climb_action97,0);InterlockedExchange(&state->aiming,0);InterlockedExchange(&state->fire_request,0);climb_publish97(crest_subject97(actor)?climb97::Crest:climb97::Climbing,field<float>(actor,0x20),climb97::direction(unsigned(state->input)));return true;}
    if(climb_lease97!=possession_lease.generation){climb_reset97();climb_lease97=possession_lease.generation;}
    auto now=GetTickCount64();bool action=InterlockedExchange(&climb_action97,0)!=0;
    if(weapon_mounted()||field<unsigned>(actor,0x1f0)>=18||field<uintptr_t>(actor,0x18)||state->weapon_phase!=WEAPON_IDLE||state->aiming){climb_near97=0;climb_publish97(climb97::Off);return false;}
    melee60::Request qte;if(melee_request(qte)){climb_near97=0;climb_publish97(climb97::Off);return false;}
    if(action||now>=climb_scan97){
        climb_scan97=now+100;climb_near97=0;uintptr_t phase=0;
        if(wall_world97(actor,phase)){
            auto array=field<uintptr_t>(phase,0x304);unsigned n=field<unsigned>(phase,0x300);float best=2.3f*2.3f;
            if(array&&n<=8192)for(unsigned i=0;i<n;++i){auto pipe=field<uintptr_t>(array,i*4);Wall97 w;if(!read_wall97(actor,pipe,w,i))continue;
                for(unsigned top=0;top<2;++top){auto node=w.nodes+(top?w.count-1:0)*72;unsigned off=top?0x30:0x24;
                    float dx=field<float>(node,off)-field<float>(actor,0x48),dy=field<float>(node,off+4)-field<float>(actor,0x4c),dz=field<float>(node,off+8)-field<float>(actor,0x50),d=dx*dx+dz*dz;
                    if(std::abs(dy)<=1.25f&&d<best&&(d<.16f||dx*std::sin(state->yaw)+dz*std::cos(state->yaw)>0)){
                        best=d;climb_near97=pipe;climb_near_id97=w.id;climb_near_top97=top!=0;
                    }
                }
            }
        }
    }
    if(!climb_near97){climb_publish97(climb97::Off);return false;}
    Wall97 w;if(!read_wall97(actor,climb_near97,w)||w.id!=climb_near_id97){climb_near97=0;climb_publish97(climb97::Off);return false;}
    climb_publish97(climb_near_top97?climb97::NearbyDown:climb97::NearbyUp);
    if(!action)return false;
    climb_last_direction97=climb_near_top97?-1:1;
    // Original attachment owns occupancy, projection and animation selection.
    reinterpret_cast<PipeBind97>(base+0x3c1ba0)(reinterpret_cast<void*>(actor+0x14),w.pipe);
    if(field<uintptr_t>(actor,0x18)==w.pipe&&wall_subject97(actor)){InterlockedIncrement(&climb_attaches97);climb_publish97(climb97::Climbing,field<float>(actor,0x20));return true;}
    InterlockedIncrement(&climb_rejected97);return false;
}
static void climb_log97(std::ofstream& log){if(state&&state->mode!=POSSESSED)climb_publish97(climb97::Off);log<<",\"siege97\":1,\"climb_attaches97\":"<<climb_attaches97<<",\"climb_steps97\":"<<climb_steps97<<",\"climb_holds97\":"<<climb_holds97<<",\"climb_down97\":"<<climb_down97<<",\"climb_endpoints97\":"<<climb_endpoints97<<",\"climb_rejected97\":"<<climb_rejected97;log<<",\"siege98\":1,\"climb_rate98\":"<<climb_rate98<<",\"climb_step98\":"<<climb_step98<<",\"climb_duplicate98\":"<<climb_duplicate98<<",\"climb_rate_errors98\":"<<climb_rate_errors98<<",\"view_count_bypasses98\":"<<view_count_bypasses98<<",\"view_pose_bypasses98\":"<<view_pose_bypasses98<<",\"view_native_count98\":"<<view_native_count98<<",\"view_zero_count98\":"<<view_zero_count98;log<<",\"landing99\":1,\"native_air_updates99\":"<<native_air_updates99<<",\"native_air_active99\":"<<native_air_active99;if(climb_shared97)log<<",\"climb_mode97\":"<<climb_shared97->mode<<",\"climb_progress97\":"<<climb_shared97->progress;}
static void create_siege_hooks97(){
    struct Signature97 {unsigned rva;unsigned char bytes[8];};
    const Signature97 signatures[]={
        {0xa0b60,{0xd9,0x41,0x5c,0xc3,0xcc,0xcc,0xcc,0xcc}},
        {0x2d6210,{0x8b,0x41,0x1c,0x8b,0x00,0x8b,0x40,0x08}},
        {0x340d30,{0x81,0xec,0xac,0,0,0,0x56,0x8b}},
        {0x340cf0,{0xff,0x74,0x24,4,0xe8,0x77,0x10,0xfe}},
        {0x3945c0,{0x83,0xec,0x0c,0x53,0x56,0x8b,0xf1,0x57}},
        {0x3cf280,{0x53,0x55,0x56,0x57,0x8b,0x7c,0x24,0x14}},
        {0x3c1ba0,{0x53,0x56,0x8b,0xf1,0x57,0x8b,0x7c,0x24}},
        {0x3dfe30,{0x56,0xff,0x74,0x24,0x14,0x8b,0xf1,0xff}},
        {0x39a2e0,{0x51,0xf3,0x0f,0x10,0x44,0x24,0x0c,0x53}}};
    for(const auto& sig:signatures)require(!memcmp(reinterpret_cast<void*>(base+sig.rva),sig.bytes,8),"Native siege signature differs");
    const unsigned char count98[]={0x81,0xec,0xb8,0,0,0,0x56,0x8b},pose98[]={0x81,0xec,0xb8,0,0,0,0xf3,0x0f};
    require(!memcmp(reinterpret_cast<void*>(base+0x2fcb10),count98,8)&&!memcmp(reinterpret_cast<void*>(base+0x341560),pose98,8),"Native siege view count/pose signature differs");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x2fcb10),reinterpret_cast<void*>(&view_count_hook98),reinterpret_cast<void**>(&original_view_count98))==MH_OK,"Siege view count hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x341560),reinterpret_cast<void*>(&view_pose_hook98),reinterpret_cast<void**>(&original_view_pose98))==MH_OK,"Siege view pose hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x2d6210),reinterpret_cast<void*>(&view_index_hook97),reinterpret_cast<void**>(&original_view_index97))==MH_OK,"Siege view index hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x340d30),reinterpret_cast<void*>(&view_actor_hook97),reinterpret_cast<void**>(&original_view_actor97))==MH_OK,"Siege view actor hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x3945c0),reinterpret_cast<void*>(&pipe_advance_hook97),reinterpret_cast<void**>(&original_pipe_advance97))==MH_OK,"Siege traversal hook failed");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x3cf280),reinterpret_cast<void*>(&pipe_mode_hook97),reinterpret_cast<void**>(&original_pipe_mode97))==MH_OK,"Siege mode hook failed");
    wchar_t name[96];climb97::mapping_name(name,GetCurrentProcessId());climb_mapping97=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(climb97::Status),name);
    require(climb_mapping97!=nullptr,"Climb telemetry mapping failed");climb_shared97=static_cast<climb97::Status*>(MapViewOfFile(climb_mapping97,FILE_MAP_ALL_ACCESS,0,0,sizeof(climb97::Status)));
    require(climb_shared97!=nullptr,"Climb telemetry view failed");*climb_shared97=climb97::Status{};climb_shared97->pid=GetCurrentProcessId();
}
