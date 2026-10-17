#pragma once
// Native3ca920 -> human virtual+74/3e9c50 publishes a1=a2=1 at
// crest cleanup.3eade0/367600/3afb50 own airborne physics and contact.
// This is current native state, independent of pose/mode or a time grace.
static bool native_airborne_subject99(uintptr_t actor){
    if(!actor||field<uintptr_t>(actor,0)!=base+0x15dfec8||field<unsigned char>(actor,0xf0)!=1||
       field<uintptr_t>(actor,0x18)||field<uintptr_t>(actor,0x210)||field<unsigned char>(actor,0xa1)!=1||
       field<unsigned char>(actor,0xa2)!=1||field<unsigned char>(actor,0xa3)||field<unsigned char>(actor,0xa4))return false;
    auto native_unit=field<uintptr_t>(actor,0x214),config=field<uintptr_t>(actor,0x20c);
    return native_unit&&config&&config==field<uintptr_t>(native_unit,0xf9c);
}
