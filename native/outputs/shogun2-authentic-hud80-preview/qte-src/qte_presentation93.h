#pragma once
#include "qte_transport.h"
namespace qte {
struct Frame93 {Snapshot shown{};uint64_t battle{},lease{};double submitted{};};
inline bool frame_click93(const transport::Payload& live,const Frame93& frame,double now,bool left,bool right,bool focus,Click& click){
    if(!transport::click(live,now,left,right,focus,click))return false;
    if(click.paused)return true; // Observe held/released levels through pause without judging or cancelling.
    if(frame.battle!=live.anchor.battle||frame.lease!=live.lease||
       frame.shown.serial!=live.prompt.serial||frame.shown.result!=Result::Pending||frame.shown.paused||
       !std::isfinite(frame.submitted)||now<frame.submitted||now-frame.submitted>.050)return false;
    click.visible93=true;click.shown93=frame.shown;return true;
}
class PreciseWait93 {
    HANDLE timer_{};
public:
    PreciseWait93(){timer_=CreateWaitableTimerExW(nullptr,nullptr,0x2,TIMER_MODIFY_STATE|SYNCHRONIZE);}
    ~PreciseWait93(){if(timer_)CloseHandle(timer_);}
    bool precise()const{return timer_!=nullptr;}
    void seconds(double seconds){
        if(seconds<=0)return;
        if(timer_){LARGE_INTEGER due;due.QuadPart=-LONGLONG(std::max(1.,seconds*1e7));
            if(SetWaitableTimer(timer_,&due,0,nullptr,nullptr,FALSE)){WaitForSingleObject(timer_,100);return;}}
        Sleep(DWORD(std::max(1.,std::ceil(seconds*1000.))));
    }
};
}
