#include "native_qte_abi.h"
#include <iostream>
#include <stdexcept>
static void check(bool value,const char* message){if(!value)throw std::runtime_error(message);}
using namespace qte;
struct NativeFixture {
    unsigned prepare_calls{},update_calls{},remove_calls{},select_calls{};
    void* removed{};void*const* begin{};void*const* end{};uint32_t flag1{},flag2{};
    __declspec(noinline) uint32_t prepare(){++prepare_calls;return 0xdead0000;}
    __declspec(noinline) uint32_t update(){++update_calls;return 0xbeef0001;}
    __declspec(noinline) void remove(void* witness){++remove_calls;removed=witness;}
    __declspec(noinline) NativeVerdict* select(NativeVerdict* output,void*const* first,void*const* last,uint32_t a,uint32_t b){
        ++select_calls;begin=first;end=last;flag1=a;flag2=b;
        *output={a^b,uint32_t(reinterpret_cast<uintptr_t>(first[0])),uint32_t(reinterpret_cast<uintptr_t>(first[1]))};return output;
    }
};
template<class Call,class Method>Call address(Method method){static_assert(sizeof(Call)==sizeof(Method),"Member pointer ABI");Call out;memcpy(&out,&method,sizeof out);return out;}
static unsigned sort_calls;static void*const* sorted_begin;static void*const* sorted_end;static int sorted_count;static float sorted_center[3];
static __declspec(noinline) void __cdecl native_sort(void*const* begin,void*const* end,int count,const float* center){
    ++sort_calls;sorted_begin=begin;sorted_end=end;sorted_count=count;memcpy(sorted_center,center,sizeof sorted_center);
}
int main(){try{
    NativeFixture native;auto prepare=address<NativeWitnessCall>(&NativeFixture::prepare);
    auto update=address<NativeWitnessCall>(&NativeFixture::update);auto remove=address<NativeWitnessRemove>(&NativeFixture::remove);
    NativeSelectionPorts ports{native_sort,address<NativeSelector>(&NativeFixture::select)};
    void* participants[2]={&native,&ports};float center[3]={1.25f,-2.5f,3.75f};NativeVerdict output;
    auto calls=ports.calls();uintptr_t before{},after{};
    __asm mov before,esp
    for(unsigned i=0;i<1000;++i){
        check(prepare(&native)==0xdead0000&&update(&native)==0xbeef0001,"Full native EAX was truncated");
        remove(&native,&ports);
        check(calls.sort(calls.context,uint32_t(reinterpret_cast<uintptr_t>(participants)),uint32_t(reinterpret_cast<uintptr_t>(participants+2)),2,center),"Native cdecl sort call failed");
        check(calls.select(calls.context,uint32_t(reinterpret_cast<uintptr_t>(&native)),uint32_t(reinterpret_cast<uintptr_t>(participants)),uint32_t(reinterpret_cast<uintptr_t>(participants+2)),0x12340001,0x56780002,output),"Native selector pointer return mismatch");
    }
    __asm mov after,esp
    check(before==after,"Native RET20/RET4/cdecl stack balance changed");
    check(native.prepare_calls==1000&&native.update_calls==1000&&native.remove_calls==1000&&native.select_calls==1000&&sort_calls==1000,"Native calls duplicated or lost");
    check(native.removed==&ports&&native.begin==participants&&native.end==participants+2&&native.flag1==0x12340001&&native.flag2==0x56780002,"ECX or stack arguments changed");
    check(output.outcome==(0x12340001^0x56780002)&&output.attacker_address==uint32_t(reinterpret_cast<uintptr_t>(&native))&&output.defender_address==uint32_t(reinterpret_cast<uintptr_t>(&ports)),"Native 12-byte output changed");
    check(sorted_begin==participants&&sorted_end==participants+2&&sorted_count==2&&!memcmp(sorted_center,center,sizeof center),"Native sort arguments changed");
    std::cout<<"PASS: 1000 native ABI sequences through production selection ports; ECX/RET20 selector, RET4 removal, cdecl sort, full EAX prepare/update returns, pointers/floats, exact call counts and stack balance. Native implementations are fixture methods, not the game.\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
