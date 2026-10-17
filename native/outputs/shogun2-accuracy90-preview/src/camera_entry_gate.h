#pragma once
#include <cstdint>
// Observe camera methods on their actual owner; soldier simulation workers do
// not establish camera affinity. Callback depth belongs to the current thread.
inline bool camera_entry_thread(uint32_t current,uint32_t window,uint32_t camera,long depth){
    return current!=0&&current==window&&current==camera&&depth==0;
}
inline uint32_t camera_entry_owner(uint32_t window,uint32_t matrix,uint32_t input){
    if(!window||(matrix&&matrix!=window)||(input&&input!=window))return 0;
    return matrix?matrix:input;
}
inline bool camera_entry_live(bool enabled,bool stop,bool release,uint64_t age,long mode){
    return enabled&&!stop&&!release&&age<=500&&mode==1;
}
