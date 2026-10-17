#pragma once
#include <windows.h>
#include <cstdint>
namespace original93 {
struct Entry {uint32_t address{},state{};uint64_t generation{};};
// A lifetime follows the witness, not its worker. Tombstones preserve probing;
// a stale removal cannot erase a newer object that reuses the same address.
class OriginalRegistry93 {
    SRWLOCK lock_=SRWLOCK_INIT;Entry entries_[4096]{};uint64_t next_{}; public: uint64_t probes_{}; private:
    static unsigned bucket(uint32_t address){return ((address>>4)*2654435761u)&4095;}
public:
    uint64_t get(uint32_t address,bool born=false){
        if(!address)return 0;AcquireSRWLockExclusive(&lock_);
        unsigned vacant=4096;uint64_t result=0;
        for(unsigned i=0;i<4096;++i){++probes_;unsigned at=(bucket(address)+i)&4095;auto& e=entries_[at];
            if(e.state==1&&e.address==address){if(born)e.generation=++next_;result=e.generation;break;}
            if(e.state!=1&&vacant==4096)vacant=at;
            if(e.state==0){break;}
        }
        if(!result&&vacant!=4096){auto& e=entries_[vacant];e={address,1,++next_};result=e.generation;}
        ReleaseSRWLockExclusive(&lock_);return result;
    }
    void removed(uint32_t address,uint64_t generation){
        AcquireSRWLockExclusive(&lock_);
        for(unsigned i=0;i<4096;++i){auto& e=entries_[(bucket(address)+i)&4095];if(!e.state)break;
            if(e.state==1&&e.address==address){if(e.generation==generation)e.state=2;break;}}
        ReleaseSRWLockExclusive(&lock_);
    }
};
}
