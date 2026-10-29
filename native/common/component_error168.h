#pragma once
#include "startup_error167.h"
#include <cstring>
#include <algorithm>
// First-party, same-process diagnostic port. No engine ABI, files or logging
// thread. Only the matching coordinator calls it after initialization fails.
extern "C" __declspec(dllexport) unsigned __cdecl S2UPInitializationError168(char* out,unsigned capacity){
    if(!out||capacity<2||capacity>1025)return 0;
    AcquireSRWLockShared(&startup167::error_lock);
    unsigned n=unsigned((std::min)(startup167::last_error.size(),size_t(capacity-1)));
    memcpy(out,startup167::last_error.data(),n);out[n]=0;
    ReleaseSRWLockShared(&startup167::error_lock);return n;
}
