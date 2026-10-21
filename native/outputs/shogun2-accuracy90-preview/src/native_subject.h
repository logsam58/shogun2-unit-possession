#pragma once
#include "../../shogun2-possession-cleanup-v46/src/actor_types152.h"
#include "native_wall97.h"
#include "native_air99.h"
// Proven by paused RTS/FPS snapshots and native motion capture: human+210 is
// this rider's mount. This is not a guessed nearest horse or a roster ordinal.
// Recovered native update code remains responsible for moving the rider.
struct NativeSubject {uintptr_t actor{},holder{},config{};uint32_t serial{};};
static uintptr_t locomotion_actor(){return possession_lease.identity.mount?possession_lease.identity.mount:possessed;}
static uintptr_t locomotion_config(){return possession_lease.identity.mount?possession_lease.identity.mount_config:possession_lease.identity.config;}
static bool root_motion_subject(uintptr_t actor,bool committed){
    return field<unsigned char>(actor,0xf0)==1&&!field<uintptr_t>(actor,0x18)&&
        (committed||(field<unsigned char>(actor,0xa1)==1&&!field<unsigned char>(actor,0xa2)&&
        !field<unsigned char>(actor,0xa3)&&!field<unsigned char>(actor,0xa4)));
}
// The stock jump factory3d9140 installs task16/15e0cec. Preserve its
// native motion and reciprocal rider ownership; no timer or pose-only grace.
static bool native_mounted_jump140(uintptr_t actor){
    return actor&&field<uintptr_t>(actor,0)==base+0x15e8524&&
        field<unsigned char>(actor,0xf0)==1&&!field<uintptr_t>(actor,0x18)&&
        field<unsigned>(actor,0x248)==16&&field<unsigned char>(actor,0x2e8)==1&&
        field<uintptr_t>(actor,0x250)==base+0x15e0cec&&field<uintptr_t>(actor,0x254)==actor&&
        field<unsigned char>(actor,0x258)==0;
}
static bool native_subject(uintptr_t rider,uintptr_t actor_unit,uintptr_t view,bool committed,NativeSubject& subject){
    subject={};
    if(!rider||!actor_unit||!view||!actor152::human(base,field<uintptr_t>(rider,0))||
       field<uintptr_t>(rider,0x214)!=actor_unit||field<unsigned char>(rider,0xf0)!=1)return false;
    auto config=field<uintptr_t>(rider,0x20c),movement=field<uintptr_t>(actor_unit,0xf9c);
    if(!config||!movement)return false;
    if(!field<uintptr_t>(rider,0x210)){
        if(config!=movement&&!actor152::individual(base,field<uintptr_t>(rider,0)))return false;
        // Native3e4850 handles postures through0x5c. Special soldiers use the
        // same root/attachment/recovery ownership as regular soldiers; their
        // independent config is not a reason to reject native posture>=18.
        return root_motion_subject(rider,committed)||wall_subject97(rider)||native_airborne_subject99(rider);
    }
    if(field<uintptr_t>(rider,0x18))return false;
    auto record=field<uintptr_t>(actor_unit,0x2c);
    // RVA180440's category==1 predicate applies to ordinary cavalry only.
    // Live Gen_Taisho uses category 0 despite an exact native horse/passenger
    // relationship. Admit mounted subjects through the reciprocal ownership
    // checks below rather than treating that narrow predicate as universal.
    if(!record)return false;
    auto mount=field<uintptr_t>(rider,0x210);
    if(!mount||mount==rider||field<uintptr_t>(mount,0)!=base+0x15e8524||
       field<uintptr_t>(mount,0x214)!=actor_unit||field<uintptr_t>(mount,0x20c)!=movement||
       field<uintptr_t>(mount,0x210)||(!root_motion_subject(mount,committed)&&!native_mounted_jump140(mount)))return false;
    // Horse virtual update RVA41fda0 iterates +6cc/+6d0 and updates these
    // exact passengers after integrating itself. Verify that reverse link too.
    auto passengers=field<unsigned>(mount,0x6cc),links=field<uintptr_t>(mount,0x6d0);
    if(!links||!passengers||passengers>8)return false;
    unsigned matches=0;for(unsigned i=0;i<passengers;++i)if(field<uintptr_t>(links,i*4)==rider)++matches;
    if(matches!=1)return false;
    auto count=field<unsigned>(view,0x18),array=field<uintptr_t>(view,0x1c);
    if(!array||!count||count>1000)return false;
    uintptr_t holder=0;unsigned memberships=0,owners=0;
    for(unsigned i=0;i<count;++i){
        auto h=field<uintptr_t>(array,i*4),a=h?field<uintptr_t>(h,8):0;
        if(a==mount){holder=h;++memberships;}
        if(a&&actor152::human(base,field<uintptr_t>(a,0))&&field<uintptr_t>(a,0x210)==mount&&
           field<unsigned char>(a,0xf0)==1)++owners;
    }
    if(memberships!=1||owners!=1)return false;
    subject={mount,holder,field<uintptr_t>(mount,0x20c),field<uint32_t>(mount,0x1c8)};return true;
}
