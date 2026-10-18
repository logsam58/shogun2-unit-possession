#pragma once
#include "qte_presentation93.h"
#include "../../shogun2-possession-cleanup-v46/src/overlay_surface106.h"
namespace qte {
// Numeric image history shared by the render and input workers. Input reads the
// native acknowledgement directly; it need not wait for another GDI render poll.
class PresentedImages113 {
    struct Image {uint64_t number{},battle{};overlay106::Key key{};Snapshot prompt{};};
    Image images_[32]{};
    mutable SRWLOCK lock_=SRWLOCK_INIT;
public:
    void remember(uint64_t number,overlay106::Key key,uint64_t battle,const Snapshot& prompt){
        if(!number)return;
        AcquireSRWLockExclusive(&lock_);images_[number%32]={number,battle,key,prompt};ReleaseSRWLockExclusive(&lock_);
    }
    bool read(const overlay106::Ack& ack,overlay106::Key key,uint64_t battle,uint64_t serial,double now,Frame93& output)const{
        if(!ack.number||(ack.backend!=9&&ack.backend!=11)||!overlay106::same(ack.key,key)||
           !key.lease||!battle||!std::isfinite(ack.submitted)||ack.submitted<=0||!std::isfinite(now)||now<ack.submitted)return false;
        AcquireSRWLockShared(&lock_);const auto image=images_[ack.number%32];ReleaseSRWLockShared(&lock_);
        if(image.number!=ack.number||image.battle!=battle||!overlay106::same(image.key,key)||image.prompt.serial!=serial)return false;
        output={image.prompt,battle,key.lease,ack.submitted};return true;
    }
    bool newest(const overlay106::Ack& ack,overlay106::Key key,const transport::Payload& live,double now,Frame93& selected)const{
        Frame93 current;
        if(key.lease!=live.lease||!read(ack,key,live.anchor.battle,live.prompt.serial,now,current)||current.submitted<selected.submitted)return false;
        selected=current;return true;
    }
};
}
