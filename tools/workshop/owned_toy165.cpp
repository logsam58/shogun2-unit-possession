#include <windows.h>
#include <cstdio>
int wmain(){std::printf("owned-pid=%lu\n",GetCurrentProcessId());std::fflush(stdout);Sleep(10000);return 0;}
