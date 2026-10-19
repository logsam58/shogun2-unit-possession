// Offline: execute the production hooks against a private, hash-verified engine
// image. Never load its entry point, attach to a process or launch the game.
#include "identity.h"
#include "native_render_copy117.h"
#include <atomic>
#include <thread>
#include <emmintrin.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>

using Copy=void*(__thiscall*)(void*,const void*);
static Copy native_copy;

static volatile unsigned sink;
static void require(bool b,const char* s){if(!b)throw std::runtime_error(s);}
static uint32_t seed=0x4679231u;
static unsigned char next_byte(){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return static_cast<unsigned char>(seed);}
static void fill(std::vector<unsigned char>& v){for(auto& b:v)b=next_byte();}
static double bench(Copy fn,unsigned count,unsigned repeats){
    std::vector<unsigned char> src(count*0x138+64),dst(count*0x138+64);fill(src);fill(dst);
    LARGE_INTEGER f,a,b;QueryPerformanceFrequency(&f);QueryPerformanceCounter(&a);
    for(unsigned r=0;r<repeats;++r)for(unsigned i=0;i<count;++i){
        fn(dst.data()+i*0x138,src.data()+i*0x138);
    }
    QueryPerformanceCounter(&b);sink+=dst[seed%dst.size()];
    return (b.QuadPart-a.QuadPart)*1e9/double(f.QuadPart)/(count*repeats);
}
struct PrivateImage {
    unsigned char* base{};
    PrivateImage(){
        wchar_t configured[32768]{};
        DWORD n=GetEnvironmentVariableW(L"SHOGUN2_GAME",configured,32768);
        require(n<32768,"SHOGUN2_GAME path too long");
        std::wstring folder=n?configured:L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Total War SHOGUN 2";
        std::wstring path=folder+L"\\Empire.Retail.dll";
        require(hash_file(path)==EXPECTED_HASH,"unsupported offline engine fixture");
        std::ifstream in(path,std::ios::binary);
        std::vector<unsigned char> raw((std::istreambuf_iterator<char>(in)),{});require(raw.size()>0x1000,"private PE read");
        auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(raw.data());
        auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(raw.data()+dos->e_lfanew);
        require(nt->Signature==IMAGE_NT_SIGNATURE&&nt->FileHeader.Machine==IMAGE_FILE_MACHINE_I386,"private PE header");
        base=static_cast<unsigned char*>(VirtualAlloc(nullptr,nt->OptionalHeader.SizeOfImage,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));require(base!=nullptr,"private image allocation");
        memcpy(base,raw.data(),nt->OptionalHeader.SizeOfHeaders);auto sections=IMAGE_FIRST_SECTION(nt);
        for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){const auto& s=sections[i];if(s.SizeOfRawData)memcpy(base+s.VirtualAddress,raw.data()+s.PointerToRawData,s.SizeOfRawData);}
        auto rel=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];unsigned off=0;
        while(off<rel.Size){auto block=reinterpret_cast<IMAGE_BASE_RELOCATION*>(base+rel.VirtualAddress+off);require(block->SizeOfBlock>=8,"relocation extent");
            auto entries=reinterpret_cast<unsigned short*>(block+1);
            for(unsigned i=0;i<(block->SizeOfBlock-8)/2;++i)if((entries[i]>>12)==IMAGE_REL_BASED_HIGHLOW)
                *reinterpret_cast<unsigned*>(base+block->VirtualAddress+(entries[i]&4095))+=reinterpret_cast<uintptr_t>(base)-nt->OptionalHeader.ImageBase;
            off+=block->SizeOfBlock;
        }
        require(rendercopy117::signatures(reinterpret_cast<uintptr_t>(base),nt->OptionalHeader.SizeOfImage),"loaded copy fingerprint mismatch");
        DWORD old{};require(VirtualProtect(base,0x1580000,PAGE_EXECUTE_READ,&old)!=0,"private image code protection");
        FlushInstructionCache(GetCurrentProcess(),base,0x1580000);
    }
    ~PrivateImage(){if(base)VirtualFree(base,0,MEM_RELEASE);}
};
using Sort=void(__cdecl*)(void*,void*,unsigned,unsigned);
static unsigned full_sort_test(PrivateImage& baseline,PrivateImage& img){
    unsigned passed=0;
    for(unsigned address:{0x14ab800u,0x14abc00u})for(unsigned count:{0u,1u,2u,3u,31u,32u,33u,127u,1024u,8192u})for(unsigned pattern=0;pattern<5;++pattern){
        std::vector<unsigned char> data(count*0x138+64);fill(data);
        for(unsigned i=0;i<count;++i){auto p=data.data()+i*0x138;
            auto put=[&](unsigned off,unsigned v){memcpy(p+off,&v,4);};
            put(0,pattern==0?0:i%5);put(0xe8,pattern==0?0:pattern==1?i:pattern==2?count-i:(seed=seed*1664525u+1013904223u)%17);
            put(0xec,pattern<3?0:i%3);put(0x118,pattern==0?0:i%11);put(0x12c,pattern==0?0:i%7);
            float depth=pattern==0?1.f:float(int(i%31)-15)*.25f;memcpy(p+0x130,&depth,4);
        }
        auto expected=data,actual=data;auto sort=reinterpret_cast<Sort>(img.base+address);unsigned before{},after{};
        auto native_sort=reinterpret_cast<Sort>(baseline.base+address);
        __asm mov before,esp
        native_sort(expected.data(),expected.data()+count*0x138,count,0);
        __asm mov after,esp
        require(before==after,"native sort ABI");
        sort(actual.data(),actual.data()+count*0x138,count,0);
        require(expected==actual,"full native sort byte/order mismatch");++passed;
    }
    return passed;
}

static void modify_byte(unsigned char* p,unsigned char value){
    DWORD old{};require(VirtualProtect(p,1,PAGE_EXECUTE_READWRITE,&old)!=0,"fixture protection");
    *p=value;DWORD unused{};require(VirtualProtect(p,1,old,&unused)!=0,"fixture protection restore");
    require(FlushInstructionCache(GetCurrentProcess(),p,1)!=0,"fixture code flush");
}
static void lifecycle_prepare(PrivateImage& img){
    uintptr_t base=reinterpret_cast<uintptr_t>(img.base);
    require(!rendercopy117::create(0,0x1d88000)&&!rendercopy117::create(base,1),"invalid image accepted");
    for(unsigned i=0;i<2;++i){
        auto p=img.base+rendercopy117::rvas[i]+rendercopy117::lengths[i]-1;auto saved=*p;
        modify_byte(p,saved^1);require(!rendercopy117::create(base,0x1d88000),"tail mutation accepted");
        require(!rendercopy117::targets[0]&&!rendercopy117::targets[1],"signature failure created hooks");modify_byte(p,saved);
    }
    // Force the second creation to fail. The first must be removed, with both
    // entry bytes untouched and without removing somebody else's hook.
    void* second=img.base+rendercopy117::rvas[1];void* foreign_original{};
    require(MH_CreateHook(second,reinterpret_cast<void*>(&rendercopy117::construction),&foreign_original)==MH_OK,"fixture collision");
    require(!rendercopy117::create(base,0x1d88000),"hook collision accepted");
    require(MH_RemoveHook(img.base+rendercopy117::rvas[0])==MH_ERROR_NOT_CREATED,"partial hook leaked");
    require(MH_RemoveHook(second)==MH_OK,"foreign disabled hook removed");
    require(rendercopy117::signatures(base,0x1d88000),"preparation patched code");
    require(rendercopy117::create(base,0x1d88000),"production hook creation");
    require(!rendercopy117::create(base,0x1d88000),"duplicate preparation accepted");
    require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"production hook enable");
}
// Observe x86 stack cleanup, return value and all ABI nonvolatile registers.
static __declspec(naked) bool __cdecl abi_probe(Copy,void*,const void*){
    __asm {
        push ebp
        push ebx
        push esi
        push edi
        sub esp,4
        mov ebp,012345678h
        mov ebx,023456789h
        mov esi,03456789ah
        mov edi,0456789abh
        mov ecx,[esp+28]
        push [esp+32]
        call dword ptr [esp+28]
        cmp eax,[esp+28]
        jne bad
        cmp ebp,012345678h
        jne bad
        cmp ebx,023456789h
        jne bad
        cmp esi,03456789ah
        jne bad
        cmp edi,0456789abh
        jne bad
        mov eax,1
        jmp done
    bad:
        xor eax,eax
    done:
        add esp,4
        pop edi
        pop esi
        pop ebx
        pop ebp
        ret
    }
}
static unsigned abi_test(PrivateImage& baseline,PrivateImage& img){
    unsigned count=0;unsigned char src[0x138]{},dst[0x138]{};
    for(auto base:{baseline.base,img.base})for(auto rva:rendercopy117::rvas)for(bool overlap:{false,true}){
        require(abi_probe(reinterpret_cast<Copy>(base+rva),overlap?src:dst,src),"copy ABI changed");++count;
    }
    return count;
}
static void concurrent_shutdown(PrivateImage& baseline,PrivateImage& img){
    std::atomic<unsigned> ready{0},completed{0},progress{0};std::atomic<bool> start{false},disabled{false},okay{true};
    std::vector<std::thread> threads;
    for(unsigned t=0;t<8;++t)threads.emplace_back([&,t]{
        std::array<unsigned char,0x138> src{},dst{},expected{};
        for(unsigned i=0;i<src.size();++i)src[i]=static_cast<unsigned char>(i*31+t);
        dst.fill(0xa5);expected=dst;
        auto native=reinterpret_cast<Copy>(baseline.base+rendercopy117::rvas[t%2]);
        auto fn=reinterpret_cast<Copy>(img.base+rendercopy117::rvas[t%2]);native(expected.data(),src.data());
        ++ready;while(!start.load())SwitchToThread();
        for(unsigned n=0;n<50000;++n){
            if(fn(dst.data(),src.data())!=dst.data()||dst!=expected)okay=false;
            if(n==24999)while(!disabled.load())SwitchToThread();
            if((n&255)==0)progress.fetch_add(256);
            if((n&255)==0)SwitchToThread();
        }
        ++completed;
    });
    while(ready.load()!=8)SwitchToThread();start=true;
    while(progress.load()<4096)SwitchToThread();
    // Exercise native MinHook thread freezing while callers are active. No
    // trampoline is freed; workers may finish in the replacement or original.
    require(rendercopy117::disable(),"concurrent production disable");
    disabled=true;
    for(auto& t:threads)t.join();
    require(okay&&completed==8,"concurrent output/return mismatch");
}

int main(){try{
    SetPriorityClass(GetCurrentProcess(),BELOW_NORMAL_PRIORITY_CLASS);
    PrivateImage baseline,img;
    require(MH_Initialize()==MH_OK,"MinHook init");
    lifecycle_prepare(img);
    Copy originals[]={reinterpret_cast<Copy>(baseline.base+rendercopy117::rvas[0]),reinterpret_cast<Copy>(baseline.base+rendercopy117::rvas[1])};
    unsigned cases=0;
    printf("{\"offline_only\":true,\"production_hooks_exercised\":true,\"tests\":[");
    for(unsigned which=0;which<2;++which){native_copy=originals[which];
        Copy fast=reinterpret_cast<Copy>(img.base+rendercopy117::rvas[which]);
        for(unsigned repeat=0;repeat<4;++repeat)for(unsigned sa=0;sa<32;++sa)for(unsigned da=0;da<32;++da){
            std::vector<unsigned char> source(0x138+64),one(0x138+64),two;fill(source);fill(one);two=one;
            auto snapshot=source;
            require(native_copy(one.data()+da,source.data()+sa)==one.data()+da,"native return");
            require(fast(two.data()+da,source.data()+sa)==two.data()+da,"optimized return");
            require(one==two,"copy/unaligned/padding mismatch");require(source==snapshot,"source modified");++cases;
        }
        for(int offset=-311;offset<=311;++offset){
            std::vector<unsigned char> one(1600),two;fill(one);two=one;
            native_copy(one.data()+650+offset,one.data()+650);fast(two.data()+650+offset,two.data()+650);
            require(one==two,"native overlap semantics changed");++cases;
        }
        // Last touched byte sits immediately before an inaccessible guard page.
        SYSTEM_INFO info{};GetSystemInfo(&info);const auto page=info.dwPageSize;
        auto src=static_cast<unsigned char*>(VirtualAlloc(nullptr,page*2,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
        auto dst=static_cast<unsigned char*>(VirtualAlloc(nullptr,page*2,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
        require(src&&dst,"guard allocation");DWORD old{};
        require(VirtualProtect(src+page,page,PAGE_NOACCESS,&old)&&VirtualProtect(dst+page,page,PAGE_NOACCESS,&old),"guard protection");
        for(unsigned i=0;i<page;++i){src[i]=next_byte();dst[i]=next_byte();}
        std::vector<unsigned char> before(dst,dst+page);native_copy(dst+page-0x135,src+page-0x135);
        std::vector<unsigned char> expected(dst,dst+page);memcpy(dst,before.data(),page);fast(dst+page-0x135,src+page-0x135);
        require(memcmp(dst,expected.data(),page)==0,"guard-boundary result differs");++cases;
        VirtualFree(src,0,MEM_RELEASE);VirtualFree(dst,0,MEM_RELEASE);
        if(which)printf(",");printf("{\"procedure\":\"%s\",\"benchmarks\":[",which?"construction14b1880":"assignment14b4530");
        for(unsigned scale=0;scale<3;++scale){unsigned count=scale==0?1:scale==1?1024:8192,repeats=scale==0?1048576:128;
            std::vector<double> old_times,new_times;
            for(unsigned r=0;r<7;++r){if(r&1){new_times.push_back(bench(fast,count,repeats));old_times.push_back(bench(native_copy,count,repeats));}
                else{old_times.push_back(bench(native_copy,count,repeats));new_times.push_back(bench(fast,count,repeats));}}
            std::sort(old_times.begin(),old_times.end());std::sort(new_times.begin(),new_times.end());
            if(scale)printf(",");printf("{\"records\":%u,\"native_ns_per_copy\":%.4f,\"vector_ns_per_copy\":%.4f,\"speedup\":%.4f}",count,old_times[3],new_times[3],old_times[3]/new_times[3]);
        }
        printf("]}");
    }
    unsigned sort_cases=full_sort_test(baseline,img);
    unsigned abi_cases=abi_test(baseline,img);
    concurrent_shutdown(baseline,img);
    require(rendercopy117::signatures(reinterpret_cast<uintptr_t>(img.base),0x1d88000),"F12 copy bytes not restored");
    require(rendercopy117::disable(),"repeat disable");
    printf("],\"differential_cases\":%u,\"full_native_sort_cases\":%u,\"abi_cases\":%u,\"concurrent_copies\":400000,\"passed\":true,\"limits\":\"Production-hook microbenchmark, not whole-frame gain. Finite private-image copy/sort/concurrency/lifecycle coverage; no live-game acceptance or proof of render object lifetime.\"}\n",cases,sort_cases,abi_cases);
    require(MH_Uninitialize()==MH_OK,"fixture hook cleanup");return 0;
}catch(const std::exception& e){fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
