#pragma once
#include <windows.h>
#include <cstdint>
namespace qte { namespace observation {
struct Entry {uint32_t address{},state{};uint64_t generation{};};
struct RegistryMetrics94 {
    uint64_t gets{},removes{},get_probes{},remove_probes{},shift_probes{},moves{},full{};
    uint32_t live{},peak{},max_get_probes{};
};
// Entries follow native witness lifetimes, independently of their worker.
// Backward-shift deletion restores a true empty slot; historical removals never
// accumulate tombstones. Generations still reject stale/ABA removals.
class Registry {
public:
    static constexpr unsigned capacity94=16384;
private:
    SRWLOCK lock_=SRWLOCK_INIT;
    Entry entries_[16384]{};
    uint64_t next_{};
    RegistryMetrics94 metrics_{};
    static unsigned bucket(uint32_t address){return ((address>>4)*2654435761u)&(capacity94-1);}
    static unsigned distance(unsigned from,unsigned to){return (to-from)&(capacity94-1);}
public:
    uint64_t get(uint32_t address,bool born=false){
        if(!address)return 0;
        AcquireSRWLockExclusive(&lock_);++metrics_.gets;
        uint64_t result=0;unsigned probes=0;
        for(unsigned i=0;i<capacity94;++i){
            ++probes;auto& e=entries_[(bucket(address)+i)&(capacity94-1)];
            if(e.state==1&&e.address==address){if(born)e.generation=++next_;result=e.generation;break;}
            if(!e.state){e={address,1,++next_};result=e.generation;
                ++metrics_.live;if(metrics_.live>metrics_.peak)metrics_.peak=metrics_.live;break;}
        }
        metrics_.get_probes+=probes;
        if(probes>metrics_.max_get_probes)metrics_.max_get_probes=probes;
        if(!result)++metrics_.full;
        ReleaseSRWLockExclusive(&lock_);return result;
    }
    void removed(uint32_t address,uint64_t generation){
        if(!address||!generation)return;
        AcquireSRWLockExclusive(&lock_);++metrics_.removes;
        for(unsigned i=0;i<capacity94;++i){
            const unsigned at=(bucket(address)+i)&(capacity94-1);auto& e=entries_[at];++metrics_.remove_probes;
            if(!e.state)break;
            if(e.address!=address)continue;
            if(e.generation!=generation)break;
            unsigned hole=at;
            for(unsigned n=1;n<capacity94;++n){
                const unsigned scan=(at+n)&(capacity94-1);const auto& following=entries_[scan];++metrics_.shift_probes;
                if(!following.state)break;
                if(distance(bucket(following.address),scan)>=distance(hole,scan)){
                    entries_[hole]=following;hole=scan;++metrics_.moves;
                }
            }
            entries_[hole]={};--metrics_.live;break;
        }
        ReleaseSRWLockExclusive(&lock_);
    }
    // An existing native lifetime proof must NEVER create a registry entry.
    // In particular, a concurrently removed charge cannot leave a ghost record.
    bool matches140(uint32_t address,uint64_t generation){
        if(!address||!generation)return false;
        AcquireSRWLockShared(&lock_);bool result=false;
        for(unsigned i=0;i<capacity94;++i){
            const auto& e=entries_[(bucket(address)+i)&(capacity94-1)];
            if(!e.state)break;
            if(e.address==address){result=e.generation==generation;break;}
        }
        ReleaseSRWLockShared(&lock_);return result;
    }
    RegistryMetrics94 metrics94(){AcquireSRWLockShared(&lock_);auto result=metrics_;ReleaseSRWLockShared(&lock_);return result;}
};
} }
