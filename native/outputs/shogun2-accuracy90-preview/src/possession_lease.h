#pragma once
#include <cstdint>
// Internal only. These native addresses never enter the shared status protocol.
struct PossessionIdentity {
    uintptr_t environment{},common{},phase{},manager{},camera{},view{},holder{};
    uintptr_t actor{},unit{},config{};
    uint32_t actor_id{},unit_id{};
    // Camera and weapons remain on the human; QTE uses the native principal.
    // A mounted lease also owns
    // the exact native horse and its holder/configuration/serial independently.
    uintptr_t mount{},mount_holder{},mount_config{};
    uint32_t mount_id{};
};
inline bool same_soldier(const PossessionIdentity& a,const PossessionIdentity& b){
    return a.actor && a.environment==b.environment && a.common==b.common && a.phase==b.phase &&
        a.manager==b.manager && a.camera==b.camera && a.view==b.view && a.holder==b.holder &&
        a.actor==b.actor && a.unit==b.unit && a.config==b.config &&
        a.actor_id==b.actor_id && a.unit_id==b.unit_id &&
        a.mount==b.mount && a.mount_holder==b.mount_holder &&
        a.mount_config==b.mount_config && a.mount_id==b.mount_id;
}
// The caller proves native liveness, camera ownership and selected-unit membership
// before presenting a candidate. Zero is a valid native actor serial.
struct PossessionLease {
    PossessionIdentity identity{};
    bool active{};
    uint32_t generation{};
    bool acquire(const PossessionIdentity& candidate){
        if(!candidate.environment||!candidate.common||!candidate.phase||!candidate.manager||
           !candidate.camera||!candidate.view||!candidate.holder||!candidate.actor||
           !candidate.unit||!candidate.config||!candidate.unit_id)return false;
        if(candidate.mount&&(!candidate.mount_holder||!candidate.mount_config||candidate.mount==candidate.actor))return false;
        if(!candidate.mount&&(candidate.mount_holder||candidate.mount_config||candidate.mount_id))return false;
        identity=candidate;active=true;++generation;return true;
    }
    bool owns(const PossessionIdentity& candidate)const{return active&&same_soldier(identity,candidate);}
    void release(){active=false;identity={};}
};
