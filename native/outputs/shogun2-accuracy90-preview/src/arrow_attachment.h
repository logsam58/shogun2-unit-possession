#pragma once
// Native attachment release predicate472b70 forwards actor3d2f20 for type9.
// The ordinary release condition can miss manual clips without a marker.
// Notify the native attachment state machine only after our real arrow exists.
using AttachmentRelease=bool(__thiscall*)(void*);
static AttachmentRelease original_attachment_release;
static bool __fastcall arrow_attachment_release(void* object,void*){
    __try {
        if(enabled&&state&&!state->stop&&!state->release&&state->mode==POSSESSED&&manual_arrow_released&&
           (state->weapon_phase==WEAPON_FIRED||state->weapon_phase==WEAPON_RELOAD)&&is_bow_projectile(bow_record(unit))){
            uintptr_t attachment=reinterpret_cast<uintptr_t>(object);
            uintptr_t description=field<uintptr_t>(attachment,0x14);
            uintptr_t kind=description?field<uintptr_t>(description,0x1c):0;
            uintptr_t holder=field<uintptr_t>(attachment,0x10);
            uintptr_t actor=holder?field<uintptr_t>(holder,8):0;
            if(kind&&field<unsigned>(kind,0)==9&&actor==possessed&&actor==weapon_actor&&followed(actor,false)){
                InterlockedIncrement(reinterpret_cast<volatile LONG*>(&state->arrow_release_overrides));return true;
            }
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){InterlockedIncrement(reinterpret_cast<volatile LONG*>(&state->arrow_release_errors));}
    return original_attachment_release(object);
}
static void create_arrow_attachment_hook(){
    const unsigned char prefix[]={0x8b,0x41,0x14,0x8b,0x40,0x1c,0x83,0x38,9};
    require(!memcmp(reinterpret_cast<void*>(base+0x472b70),prefix,sizeof prefix),"Arrow attachment release signature differs");
    require(MH_CreateHook(reinterpret_cast<void*>(base+0x472b70),reinterpret_cast<void*>(&arrow_attachment_release),reinterpret_cast<void**>(&original_attachment_release))==MH_OK,"Arrow attachment release hook failed");
}
