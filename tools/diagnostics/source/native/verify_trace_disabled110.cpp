#include "identity.h"
#include "targeted_trace110.h"
int wmain(int argc,wchar_t**argv){
    if(argc!=2)return 2;
    TargetedTrace110 trace(GetCurrentProcess(),GetCurrentProcessId(),argv[1],false);
    // Real-mode entry points must stay inert even when offered supported names,
    // new threads and a native single-step; this does not attach to any game.
    trace.module(0x10000000,L"Empire.Retail.dll");
    trace.module(0x20000000,L"dsound.dll");
    trace.new_thread(GetCurrentThreadId());
    trace.fixture_init();
    if(trace.exception(GetCurrentThreadId(),EXCEPTION_SINGLE_STEP))return 3;
    trace.heartbeat();
    trace.restore();
    return 0;
}
