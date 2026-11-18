#pragma once
#include <atomic>
#include "steam_input174.h"

namespace remote155 {
// Both modules use the same address in the game process. Only the bridge's
// existing input worker calls Steam. The battle worker requests one sample at
// a time, so moving the reader does not add a second sampling cadence.
class Channel {
    struct Lock {
        SRWLOCK& value;
        explicit Lock(SRWLOCK& lock):value(lock){AcquireSRWLockExclusive(&value);}
        ~Lock(){ReleaseSRWLockExclusive(&value);}
    };
public:
    static constexpr unsigned version=155;
    enum class Result {Pending,Reset,Unavailable,Ready};
    struct Request {
        uint64_t generation{};
        HWND window{};
        deck174::Context context{deck174::Context::Inactive};
    };
private:
    SRWLOCK lock_=SRWLOCK_INIT;
    const HANDLE wake_=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    Request request_{};
    bool requested_{},completed_{},usable_{};
    deck174::Frame reply_{};
    deck174::BackendState state_{deck174::BackendState::WaitingSteam};
    uint64_t last_good_us_{};
public:
    Channel()=default;
    Channel(const Channel&)=delete;
    Channel& operator=(const Channel&)=delete;
    ~Channel(){if(wake_)CloseHandle(wake_);}
    HANDLE wake()const{return wake_;}

    Result receive(HWND window,deck174::Context context,uint64_t now,
                   deck174::Frame& frame,deck174::BackendState& status){
        Lock lock(lock_);
        if(request_.window!=window||request_.context!=context){
            request_={request_.generation+1,window,context};
            requested_=true;completed_=false;last_good_us_=0;
            if(wake_)SetEvent(wake_);
            status=deck174::BackendState::WaitingBindings;
            return Result::Reset;
        }
        if(!requested_){
            requested_=true;
            if(wake_)SetEvent(wake_);
        }
        status=state_;
        if(!completed_)
            return last_good_us_&&now>=last_good_us_&&now-last_good_us_<=35000?
                   Result::Pending:Result::Unavailable;
        completed_=false;
        if(!usable_||!deck174::valid(reply_,now)||now-reply_.sample_us>35000){
            last_good_us_=0;
            return Result::Unavailable;
        }
        frame=reply_;
        last_good_us_=reply_.sample_us;
        return Result::Ready;
    }
    bool request(Request& result){
        Lock lock(lock_);
        if(!requested_||!request_.window||completed_)return false;
        requested_=false;
        result=request_;
        return true;
    }
    bool publish(const Request& request,const deck174::Frame& frame,
                 deck174::BackendState status,bool usable){
        Lock lock(lock_);
        if(request.generation!=request_.generation||!request_.window||completed_)return false;
        reply_=frame;state_=status;usable_=usable;completed_=true;
        return true;
    }
    void clear(){
        Lock lock(lock_);
        if(request_.window)request_={request_.generation+1,nullptr,deck174::Context::Inactive};
        requested_=completed_=usable_=false;
        last_good_us_=0;
    }
};
using Bind=bool(__cdecl*)(Channel*,unsigned,unsigned);
inline Channel& channel(){static Channel transport;return transport;}
inline std::atomic<Channel*> battle_channel{};
inline uint64_t clock_us(){
    LARGE_INTEGER time{},frequency{};
    QueryPerformanceCounter(&time);QueryPerformanceFrequency(&frequency);
    return uint64_t(time.QuadPart/frequency.QuadPart)*1000000+
        uint64_t(time.QuadPart%frequency.QuadPart)*1000000/uint64_t(frequency.QuadPart);
}
} // namespace remote155
