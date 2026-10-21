#include "../../../common/fixture_paths.h"

#include "identity.h"
#include "../vendor/include/MinHook.h"
#include "miles_zero123.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
static const uint32_t seed[12]={0x0u,0x0u,0xffffffffu,0xffffffffu,0x6c306ccu,0xf4dff508u,0x62d0628u,0x1000fu,0x7fffu,0x0u,0x3ed0820bu,0x3e991ee4u};
static uintptr_t module_base; static DWORD fault_code; static uintptr_t fault_rva,fault_address;static int32_t remaining;
__declspec(naked) static int __cdecl invoke(void*,void*,uint32_t,void*){
 __asm {
 mov ecx,[esp+4]
 mov eax,[esp+8]
 mov edx,[esp+12]
 push [esp+16]
 push eax
 call ecx
 add esp,8
 ret
 }
}
static LONG capture(EXCEPTION_POINTERS* e){fault_code=e->ExceptionRecord->ExceptionCode;fault_rva=e->ContextRecord->Eip-module_base;fault_address=e->ExceptionRecord->ExceptionInformation[1];remaining=*(int32_t*)(e->ContextRecord->Esp+0x58);return EXCEPTION_EXECUTE_HANDLER;}
static bool checked(void* fn,void* buf,uint32_t count,void* state,int* result){
 __try {*result=invoke(fn,buf,count,state);return true;}__except(capture(GetExceptionInformation())){return false;}
}
struct Result{bool ok;int value;uint8_t data[256];uint32_t state[12];};
static Result run(void* fn,uint32_t count,uint32_t phase,uint32_t mode,bool guard){
 Result r{};uint8_t* allocation=(uint8_t*)VirtualAlloc(nullptr,8192,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!allocation)ExitProcess(10);DWORD old; if(!VirtualProtect(allocation+4096,4096,PAGE_NOACCESS,&old))ExitProcess(11);
 uint8_t* buffer=allocation+4096-256;for(unsigned i=0;i<256;i++)buffer[i]=(uint8_t)(i*13+37);memcpy(r.state,seed,sizeof seed);r.state[7]=(mode<<16)|phase;
 r.ok=checked(fn,buffer,count,r.state,&r.value);
 memcpy(r.data,buffer,256);VirtualFree(allocation,0,MEM_RELEASE);return r;
}
int wmain(int argc,wchar_t** argv){
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);if(argc==1){static std::wstring path=fixture_paths::miles();static wchar_t* paths[]={nullptr,path.data()};argv=paths;}else if(argc!=2)return 2;
 HMODULE m=LoadLibraryExW(argv[1],nullptr,LOAD_WITH_ALTERED_SEARCH_PATH);if(!m){printf("load_error=%lu\n",GetLastError());return 3;}module_base=(uintptr_t)m;void* fn=(void*)(module_base+0x16bf0);
 const uint8_t start[]={0x83,0xec,0x44,0x53,0x55,0x8b,0x6c,0x24,0x50};if(memcmp(fn,start,sizeof start))return 4;
 Result bad=run(fn,0,15,1,false);if(bad.ok||fault_code!=EXCEPTION_ACCESS_VIOLATION||fault_rva!=0x16eb9)return 5;
 printf("stock_zero_count_fault=0x%08lx rva=0x%zx remaining=%d\n",fault_code,fault_rva,remaining);
 if(mileszero123::supported("bad",fn))return 12;
 if(MH_Initialize()!=MH_OK||mileszero123::install()!=1)return 13;
 void* original=mileszero123::original;
 unsigned positive=0,zeros=0;const uint32_t counts[]={1,2,7,15,16,17,31,64};
 for(uint32_t mode=0;mode<4;mode++)for(uint32_t phase=0;phase<16;phase++){
  Result z=run(fn,0,phase,mode,true);if(!z.ok||z.value!=(mode==3)||z.state[7]!=((mode<<16)|phase))return 6;
  for(unsigned i=0;i<12;i++)if(i!=7&&z.state[i]!=seed[i])return 7;
  for(unsigned i=0;i<256;i++)if(z.data[i]!=(uint8_t)(i*13+37))return 8;zeros++;
  for(uint32_t count:counts){Result a=run(original,count,phase,mode,false),b=run(fn,count,phase,mode,true);if(!a.ok||!b.ok||a.value!=b.value||memcmp(a.data,b.data,256)||memcmp(a.state,b.state,sizeof a.state))return 9;positive++;}
 }
 if(mileszero123::zero_blocks!=64)return 14;
 if(!mileszero123::disable()||memcmp(fn,mileszero123::prefix,sizeof mileszero123::prefix))return 15;
 Result restored=run(fn,0,15,1,false);if(restored.ok||fault_rva!=0x16eb9)return 16;
 printf("PASS real_MinHook_guard_stock_reproduction guarded_zero_states=%u positive_bit_exact_cases=%u\n",zeros,positive);FreeLibrary(m);return 0;
}
