#pragma once
#include "ammunition_protocol83.h"
static HANDLE ammunition_mapping83;
static ammo83::Status* ammunition_status83;
static SRWLOCK ammunition_publish_lock83=SRWLOCK_INIT;
static void initialize_ammunition83(){
 wchar_t name[96];ammo83::mapping_name(name,GetCurrentProcessId());
 ammunition_mapping83=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(ammo83::Status),name);
 require(ammunition_mapping83&&GetLastError()!=ERROR_ALREADY_EXISTS,"Ammunition mapping already loaded");
 ammunition_status83=static_cast<ammo83::Status*>(MapViewOfFile(ammunition_mapping83,FILE_MAP_ALL_ACCESS,0,0,sizeof(ammo83::Status)));
 require(ammunition_status83!=nullptr,"Ammunition telemetry mapping failed");
 memset(ammunition_status83,0,sizeof(*ammunition_status83));ammunition_status83->magic=0x334d4153;
 ammunition_status83->version=83;ammunition_status83->size=sizeof(ammo83::Status);ammunition_status83->pid=GetCurrentProcessId();
}
