#pragma once
static void ammunition_item83(uintptr_t record,ammo83::Item& item){
 if(!record)return;item.projectile=field<unsigned>(record,0);
 unsigned n=field<unsigned>(record,4);auto chars=field<uintptr_t>(record,0xc);
 if(!chars||!n||n>128)return;
 wchar_t key[129]{};bool wide=field<unsigned char>(chars,1)==0;
 for(unsigned i=0;i<n;++i)key[i]=wide?field<wchar_t>(chars,i*2):field<unsigned char>(chars,i);
 item.icon=ammo83::icon_for_key(key);wcscpy_s(item.label,ammo83::label(ammo83::Icon(item.icon)));
}
static void publish_ammunition83(const equipment81::AmmoOption* options,unsigned count,uintptr_t active,unsigned pending,unsigned error){
 if(!ammunition_status83||!state)return;
 ammo83::Status sample{};sample.lease=state->lease_generation;sample.actor=state->actor_id;sample.unit=state->unit_id;
 sample.tick=GetTickCount64();sample.pending=pending;sample.error=error;bool matched=false;
 for(unsigned i=0;i<count&&i<33;++i){auto& item=sample.items[sample.count++];
  ammunition_item83(options[i].record,item);item.ability=options[i].id;
  if(i==0)item.flags|=ammo83::Normal;
  bool available=i==0?field<float>(unit,0xf78)>=1:equipment_unit_ability81(options[i].id,true);
  if(available)item.flags|=ammo83::Available;
  if(options[i].record==active){sample.current=i;item.flags|=ammo83::Selected;matched=true;}
 }
 // An inconsistent native snapshot must never present normal ammo as selected.
 if(!matched){sample.count=0;sample.current=0;}
 AcquireSRWLockExclusive(&ammunition_publish_lock83);InterlockedIncrement(&ammunition_status83->sequence);MemoryBarrier();
 memcpy(&ammunition_status83->lease,&sample.lease,sizeof(sample)-offsetof(ammo83::Status,lease));
 MemoryBarrier();InterlockedIncrement(&ammunition_status83->sequence);ReleaseSRWLockExclusive(&ammunition_publish_lock83);
}
