#pragma once
#include <cstdint>
#include <cstring>
constexpr uint32_t QUEUE_CAPACITY=0x5000;
enum { ORDER_NONE=0, ORDER_HOLD=1, ORDER_ADVANCE=2, ORDER_FOLLOW=3 };
// Native queue packets: uint16 LE total size, uint32 BE descriptor, byte flags.
inline bool valid_packet(const unsigned char* packet,uint32_t size,uint32_t descriptor) {
    if(size<7||size>QUEUE_CAPACITY)return false;
    uint32_t length=uint32_t(packet[0])|(uint32_t(packet[1])<<8);
    uint32_t type=(uint32_t(packet[2])<<24)|(uint32_t(packet[3])<<16)|(uint32_t(packet[4])<<8)|uint32_t(packet[5]);
    return length==size&&type==descriptor;
}
inline bool append_packet(unsigned char* queue,uint32_t& used,const unsigned char* packet,uint32_t size,uint32_t descriptor) {
    if(!valid_packet(packet,size,descriptor)||used>QUEUE_CAPACITY||size>QUEUE_CAPACITY-used)return false;
    std::memcpy(queue+used,packet,size);used+=size;return true;
}
