#pragma once
#include "../../shogun2-possession-cleanup-v46/src/friendly_fire_protocol109.h"
#include <mmsystem.h>
#ifdef S2FP_LOCAL_ASSETS
#include "../../../common/local_assets159.h"
#endif
class AudioFeedback109 {
    HANDLE mapping{};const friendly109::Status* shared{};uint64_t retry{};
    friendly109::Consumer consumer;
public:
    ~AudioFeedback109(){if(shared)UnmapViewOfFile(shared);if(mapping)CloseHandle(mapping);}
    void poll(DWORD pid,uint32_t lease,uint32_t actor,uint32_t unit,bool active){
        auto now=GetTickCount64();
        if(!shared&&now>=retry){retry=now+1000;wchar_t name[96];friendly109::mapping_name(name,pid);
            mapping=OpenFileMappingW(FILE_MAP_READ,FALSE,name);
            if(mapping){shared=static_cast<const friendly109::Status*>(MapViewOfFile(mapping,FILE_MAP_READ,0,0,sizeof(friendly109::Status)));
                if(!shared){CloseHandle(mapping);mapping=nullptr;}}
        }
        if(!shared)return;friendly109::Status s{};bool stable=false;
        for(unsigned i=0;i<4;++i){auto seq=shared->sequence;if(seq&1)continue;MemoryBarrier();memcpy(&s,shared,sizeof s);MemoryBarrier();
            if(seq==shared->sequence){stable=true;break;}}
        if(stable&&consumer.take(s,pid,lease,actor,unit,now,active)){
            // A short, asynchronous notification in this separate HUD process.
            // NOSTOP drops overlapping pellet/hit notifications; no playback queue,
            // native sound hooks, Miles state, or game-thread audio work is used.
#ifdef S2FP_LOCAL_ASSETS
            auto voice=assets159::resource(GetModuleHandleW(nullptr),301);
            if(voice.bytes)PlaySoundW(reinterpret_cast<LPCWSTR>(voice.bytes),nullptr,SND_MEMORY|SND_ASYNC|SND_NODEFAULT|SND_NOSTOP);
#else
            PlaySoundW(MAKEINTRESOURCEW(301),GetModuleHandleW(nullptr),SND_RESOURCE|SND_ASYNC|SND_NODEFAULT|SND_NOSTOP);
#endif
        }
    }
};
