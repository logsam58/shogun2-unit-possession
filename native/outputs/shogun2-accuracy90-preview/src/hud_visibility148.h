#pragma once
#include "../../shogun2-possession-cleanup-v46/src/hud_visibility148.h"
static HANDLE hud_visibility_mapping148;
static hudvisibility148::Shared* hud_visibility_shared148;
static void hud_visibility_create148(){
    wchar_t name[96];hudvisibility148::name(name,GetCurrentProcessId());
    hud_visibility_mapping148=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(hudvisibility148::Shared),name);
    require(hud_visibility_mapping148!=nullptr,"HUD visibility mapping");
    hud_visibility_shared148=static_cast<hudvisibility148::Shared*>(MapViewOfFile(hud_visibility_mapping148,FILE_MAP_ALL_ACCESS,0,0,sizeof(hudvisibility148::Shared)));
    require(hud_visibility_shared148!=nullptr,"HUD visibility view");
    new(hud_visibility_shared148)hudvisibility148::Shared;hud_visibility_shared148->pid=GetCurrentProcessId();
}
static void hud_visibility_key148(bool valid,bool owned,unsigned char current,unsigned char previous){
    if(!hud_visibility_shared148||!hudvisibility148::pressed(valid,owned,current,previous))return;
    InterlockedIncrement(&hud_visibility_shared148->sequence);MemoryBarrier();
    hud_visibility_shared148->hidden^=1;++hud_visibility_shared148->changes;
    MemoryBarrier();InterlockedIncrement(&hud_visibility_shared148->sequence);
}
static void hud_visibility_close148(){
    if(hud_visibility_shared148){UnmapViewOfFile(hud_visibility_shared148);hud_visibility_shared148=nullptr;}
    if(hud_visibility_mapping148){CloseHandle(hud_visibility_mapping148);hud_visibility_mapping148=nullptr;}
}
