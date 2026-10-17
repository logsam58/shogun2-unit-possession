#include "identity.h"
// Exercise the exact x86 stack layouts used by the manually invoked native methods.
struct Mock {
    const float* origin=nullptr;float angle=0,damage=0,aux=0,modifier=0;
    void *actor=nullptr,*context=nullptr,*record=nullptr,*p=nullptr;unsigned calls=0;
    __declspec(noinline) void* parameters(const float* o,float a,void* who,float d,float x,void* p1,void* p2,void* p3){origin=o;angle=a;actor=who;damage=d;aux=x;context=p1;record=p2;p=p3;++calls;return this;}
    __declspec(noinline) void* launch(void* c,void* r,void* v,float m){context=c;record=r;p=v;modifier=m;++calls;return this;}
    __declspec(noinline) void spawn(void* packet,float m){p=packet;modifier=m;++calls;}
    __declspec(noinline) bool boolean(const float* v){++calls;return v&&v[0]>0;}
};
template<class T>static void* code(T member){static_assert(sizeof(member)==sizeof(void*),"simple x86 member pointer");void* p=nullptr;memcpy(&p,&member,sizeof(p));return p;}
using Parameters=void*(__thiscall*)(void*,const float*,float,void*,float,float,void*,void*,void*);
using Launch=void*(__thiscall*)(void*,void*,void*,void*,float);
using Spawn=void(__thiscall*)(void*,void*,float);
using Boolean=bool(__thiscall*)(void*,const float*);
int main(){try{
    Mock out;float origin[3]={1,2,3};void* a=reinterpret_cast<void*>(0x11110000);void* b=reinterpret_cast<void*>(0x22220000);void* c=reinterpret_cast<void*>(0x33330000);
    auto parameters=reinterpret_cast<Parameters>(code(&Mock::parameters));auto launch=reinterpret_cast<Launch>(code(&Mock::launch));auto spawn=reinterpret_cast<Spawn>(code(&Mock::spawn));auto boolean=reinterpret_cast<Boolean>(code(&Mock::boolean));
    for(unsigned i=0;i<10000;++i){
        require(parameters(&out,origin,3.14f,a,1.25f,0.2f,a,b,c)==&out&&out.origin==origin&&out.angle==3.14f&&out.actor==a&&out.damage==1.25f&&out.aux==0.2f&&out.context==a&&out.record==b&&out.p==c,"8-argument parameters ABI");
        require(launch(&out,a,b,c,50)==&out&&out.context==a&&out.record==b&&out.p==c&&out.modifier==50,"4-argument launch ABI");
        spawn(&out,b,1);require(out.p==b&&out.modifier==1,"2-argument spawn ABI");
        require(boolean(&out,origin)&&!boolean(&out,nullptr),"AL boolean return ABI");
    }
    require(out.calls==50000,"Native-style calls exactly once");
    std::cout<<"PASS: 50000 calls; x86 thiscall 8/4/2 stack args, float arguments, pointer and AL returns, stack preserved.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
