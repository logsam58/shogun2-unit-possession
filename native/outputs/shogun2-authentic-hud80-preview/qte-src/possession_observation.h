#pragma once
#include <cstdint>
namespace qte { namespace observation {
// Read-only prefix of the version46 status mapping. All fields are numeric;
// this is not a new possession protocol and it has no write operations.
struct PossessionPrefix {
    uint32_t magic,version,pid,size;
    int32_t stop,ready,mode,error,input,acquire,release,sequence;
    uint64_t heartbeat,frame;
    uint32_t unit_id,actor_index,actor_id,lease_generation,identity_releases;
};
} }
