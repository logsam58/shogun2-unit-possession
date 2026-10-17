#pragma once
#include "equipment_policy81.h"
#include "equipment_protocol81.h"
#include "ammunition_mapping83.h"
static volatile LONG equipment_slot81=equipment81::Ranged,equipment_crouch81,equipment_crouch_supported81,equipment_ammo_inflight81;
static volatile LONG equipment_ranged81,equipment_lease81;
static HANDLE equipment_mapping81;
static EquipmentStatus81* equipment_status81;
struct EquipmentRequests81 {unsigned lease{},slot{},tabs{};bool ctrl{};};
static EquipmentRequests81 equipment_requests81;
static SRWLOCK equipment_input_lock81=SRWLOCK_INIT;
static equipment81::Edges equipment_edges81;
static void equipment_render_reset81();
static bool equipment_ranged_selected81(){return equipment_slot81==equipment81::Ranged&&!equipment_ammo_inflight81;}
static bool equipment_crouched81(){return equipment_crouch81!=0;}
static void equipment_reset81(){
    AcquireSRWLockExclusive(&equipment_input_lock81);equipment_requests81={};ReleaseSRWLockExclusive(&equipment_input_lock81);
    InterlockedExchange(&equipment_lease81,0);InterlockedExchange(&equipment_ranged81,0);
    InterlockedExchange(&equipment_slot81,equipment81::Ranged);InterlockedExchange(&equipment_crouch81,0);
    InterlockedExchange(&equipment_crouch_supported81,0);InterlockedExchange(&equipment_ammo_inflight81,0);
}
static void equipment_sample_input81(bool focus){
    unsigned held=0;if(focus){if(GetAsyncKeyState('1')&0x8000)held|=1;if(GetAsyncKeyState('2')&0x8000)held|=2;if(GetAsyncKeyState(VK_TAB)&0x8000)held|=4;}
    bool active=focus&&state&&state->mode==POSSESSED&&!state->release&&!state->stop&&!dead_view();
    unsigned edges=equipment_edges81.sample(held,active);
    AcquireSRWLockExclusive(&equipment_input_lock81);
    if(active&&equipment_lease81&&unsigned(equipment_lease81)==state->lease_generation){
        if(equipment_requests81.lease!=state->lease_generation)equipment_requests81={state->lease_generation};
        if(edges&1)equipment_requests81.slot=equipment81::Melee;
        if(edges&2)equipment_requests81.slot=equipment81::Ranged;
        if(edges&4)equipment_requests81.tabs=std::min(equipment_requests81.tabs+1,8u);
        equipment_requests81.ctrl=(GetAsyncKeyState(VK_CONTROL)&0x8000)!=0;
    }else equipment_requests81={};
    ReleaseSRWLockExclusive(&equipment_input_lock81);
}
static EquipmentRequests81 equipment_requests_snapshot81(){
    AcquireSRWLockShared(&equipment_input_lock81);auto request=equipment_requests81;ReleaseSRWLockShared(&equipment_input_lock81);return request;
}
static void equipment_consume81(unsigned lease,unsigned slot,bool tab){
    AcquireSRWLockExclusive(&equipment_input_lock81);
    if(equipment_requests81.lease==lease){if(slot&&equipment_requests81.slot==slot)equipment_requests81.slot=0;if(tab&&equipment_requests81.tabs)--equipment_requests81.tabs;}
    ReleaseSRWLockExclusive(&equipment_input_lock81);
}
static void equipment_initialize_mapping81(){
    initialize_ammunition83();
    wchar_t name[96];equipment_mapping_name81(name,GetCurrentProcessId());
    equipment_mapping81=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(EquipmentStatus81),name);
    require(equipment_mapping81&&GetLastError()!=ERROR_ALREADY_EXISTS,"Equipment mapping already loaded");
    equipment_status81=static_cast<EquipmentStatus81*>(MapViewOfFile(equipment_mapping81,FILE_MAP_ALL_ACCESS,0,0,sizeof(EquipmentStatus81)));
    require(equipment_status81!=nullptr,"Equipment telemetry mapping failed");
    memset(equipment_status81,0,sizeof(*equipment_status81));equipment_status81->magic=EQUIPMENT_MAGIC81;
    equipment_status81->version=81;equipment_status81->size=sizeof(*equipment_status81);equipment_status81->pid=GetCurrentProcessId();
}
