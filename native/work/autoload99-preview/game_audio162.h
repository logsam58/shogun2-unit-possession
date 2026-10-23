#pragma once
#include <cwchar>
#include "../../outputs/shogun2-possession-cleanup-v46/src/protocol.h"
#include "../../outputs/shogun2-possession-cleanup-v46/src/friendly_fire_protocol109.h"
#include <mmsystem.h>
#include <cstring>
// Playback belongs to the host (game) process, never to the HUD. No Miles calls,
// native hooks, audio queues or work in a battle/render/input callback.
namespace gameaudio162 {
inline bool pcm(const unsigned char* bytes,size_t size,WAVEFORMATEX& format,const unsigned char*& samples,DWORD& length){
    if(!bytes||size<12||memcmp(bytes,"RIFF",4)||memcmp(bytes+8,"WAVE",4))return false;
    uint32_t riff=0;memcpy(&riff,bytes+4,4);if(uint64_t(riff)+8!=size)return false;
    bool found=false;samples=nullptr;length=0;
    for(size_t i=12;i+8<=size;){uint32_t n=0;memcpy(&n,bytes+i+4,4);if(n>size-i-8)return false;
        if(!memcmp(bytes+i,"fmt ",4)){if(found||n<16)return false;memcpy(&format,bytes+i+8,16);format.cbSize=0;found=true;}
        if(!memcmp(bytes+i,"data",4)){if(samples||!n)return false;samples=bytes+i+8;length=n;}
        size_t next=i+8+n+(n&1);if(next>size)return false;i=next;
    }
    return found&&samples&&format.wFormatTag==WAVE_FORMAT_PCM&&format.nChannels==1&&format.nSamplesPerSec==44100&&
        format.wBitsPerSample==16&&format.nBlockAlign==2&&format.nAvgBytesPerSec==88200&&length%2==0;
}
class Voice {
    HWAVEOUT output{};WAVEHDR header{};bool prepared{};
public:
    Voice()=default;Voice(const Voice&)=delete;Voice& operator=(const Voice&)=delete;
    ~Voice(){if(output){waveOutReset(output);if(prepared)waveOutUnprepareHeader(output,&header,sizeof header);waveOutClose(output);}}
    bool play(const unsigned char* bytes,size_t size){
        if(prepared){if(!(header.dwFlags&WHDR_DONE))return false;
            if(waveOutUnprepareHeader(output,&header,sizeof header)!=MMSYSERR_NOERROR)return false;prepared=false;}
        WAVEFORMATEX format{};const unsigned char* samples{};DWORD length{};
        if(!pcm(bytes,size,format,samples,length)||length!=136710)return false;
        if(!output&&waveOutOpen(&output,WAVE_MAPPER,&format,0,0,CALLBACK_NULL)!=MMSYSERR_NOERROR){output=nullptr;return false;}
        header={};header.lpData=reinterpret_cast<LPSTR>(const_cast<unsigned char*>(samples));header.dwBufferLength=length;
        if(waveOutPrepareHeader(output,&header,sizeof header)!=MMSYSERR_NOERROR)return false;prepared=true;
        return waveOutWrite(output,&header,sizeof header)==MMSYSERR_NOERROR;
    }
};
inline bool active(const Status& s,DWORD pid,uint64_t now,bool focused){
    return focused&&s.magic==POC_MAGIC&&s.version==POC_VERSION&&s.size==sizeof s&&s.pid==pid&&s.ready&&!s.stop&&!s.release&&
        s.mode==POSSESSED&&now>=s.heartbeat&&now-s.heartbeat<=1000;
}
template<class T> inline bool snapshot(const T* source,T& value){
    if(!source)return false;
    for(unsigned i=0;i<4;++i){LONG seq=source->sequence;if(seq&1)continue;MemoryBarrier();memcpy(&value,source,sizeof value);MemoryBarrier();if(seq==source->sequence)return true;}return false;
}
class Feedback {
    HANDLE mapping{};const friendly109::Status* shared{};uint64_t retry{};friendly109::Consumer consumer;
    Voice voice;
public:
    ~Feedback(){if(shared)UnmapViewOfFile(shared);if(mapping)CloseHandle(mapping);}
    void poll(const Status& owner,bool owner_stable,const unsigned char* bytes,size_t size){
        const DWORD pid=GetCurrentProcessId();const auto now=GetTickCount64();
        if(!shared&&now>=retry){retry=now+1000;wchar_t name[96];friendly109::mapping_name(name,pid);mapping=OpenFileMappingW(FILE_MAP_READ,FALSE,name);
            if(mapping){shared=static_cast<const friendly109::Status*>(MapViewOfFile(mapping,FILE_MAP_READ,0,0,sizeof(friendly109::Status)));
                if(!shared){CloseHandle(mapping);mapping=nullptr;}}}
        friendly109::Status event{};if(!snapshot(shared,event))return;
        DWORD foreground=0;GetWindowThreadProcessId(GetForegroundWindow(),&foreground);
        if(consumer.take(event,pid,owner.lease_generation,owner.actor_id,owner.unit_id,now,owner_stable&&active(owner,pid,now,foreground==pid)))
            voice.play(bytes,size); // drop overlap; never replay or accumulate hits
    }
};
}
