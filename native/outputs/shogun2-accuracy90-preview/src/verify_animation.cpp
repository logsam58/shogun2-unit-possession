#include "identity.h"
#include "../vendor/include/MinHook.h"
struct Mock {
    unsigned clip=0,force=0,attached=0,mode=0,skip=0,calls=0;float speed=0;
    __declspec(noinline) void play(unsigned c,unsigned f,unsigned a,float s){clip=c;force=f;attached=a;speed=s;++calls;}
    __declspec(noinline) void change(unsigned m,unsigned s,unsigned a){mode=m;skip=s;attached=a;++calls;}
    __declspec(noinline) float release(unsigned c,unsigned n){clip=c;force=n;++calls;return 0.35f;}
    __declspec(noinline) float reload(void* record){++calls;return record?17.5f:7.5f;}
    __declspec(noinline) void* origin(float* out,unsigned c,unsigned n){clip=c;force=n;++calls;out[0]=1;out[1]=2;out[2]=3;return out;}
};
template<class T>static void* address(T m){static_assert(sizeof(m)==4,"x86 member ABI");void* p;memcpy(&p,&m,4);return p;}
using Play=void(__thiscall*)(void*,unsigned,unsigned,unsigned,float);
using Change=void(__thiscall*)(void*,unsigned,unsigned,unsigned);
using Release=float(__thiscall*)(void*,unsigned,unsigned);
using Reload=float(__thiscall*)(void*,void*);
using Origin=void*(__thiscall*)(void*,float*,unsigned,unsigned);
static Play play_original;static Change change_original;static unsigned play_count,change_count;
static void __fastcall play_hook(void* self,void*,unsigned c,unsigned f,unsigned a,float s){++play_count;play_original(self,c,f,a,s);}
static void __fastcall change_hook(void* self,void*,unsigned m,unsigned s,unsigned a){++change_count;change_original(self,m,s,a);}
int main(){try{
    void* p=address(&Mock::play),*c=address(&Mock::change);unsigned char saved[16];memcpy(saved,p,16);
    require(MH_Initialize()==MH_OK,"init");
    require(MH_CreateHook(p,reinterpret_cast<void*>(&play_hook),reinterpret_cast<void**>(&play_original))==MH_OK,"play hook");
    require(MH_CreateHook(c,reinterpret_cast<void*>(&change_hook),reinterpret_cast<void**>(&change_original))==MH_OK,"state hook");
    require(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"enable");
    Mock m;float out[3];
    auto release=reinterpret_cast<Release>(address(&Mock::release));auto reload=reinterpret_cast<Reload>(address(&Mock::reload));auto origin=reinterpret_cast<Origin>(address(&Mock::origin));
    for(unsigned i=0;i<10000;++i){
        reinterpret_cast<Play>(p)(&m,130,1,0,1.25f);require(m.clip==130&&m.force==1&&m.attached==0&&m.speed==1.25f,"play args");
        reinterpret_cast<Change>(c)(&m,16,0,1);require(m.mode==16&&m.skip==0&&m.attached==1,"state args");
        require(release(&m,130,0)==0.35f&&m.clip==130&&m.force==0,"x87 release return");
        require(reload(&m,p)==17.5f,"x87 reload return");
        require(origin(&m,out,130,0)==out&&out[0]==1&&out[1]==2&&out[2]==3,"muzzle output ABI");
    }
    require(m.calls==50000&&play_count==10000&&change_count==10000,"exact original calls");
    require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&memcmp(saved,p,16)==0,"restoration");
    std::cout<<"PASS: 50000 native-style animation calls; thiscall/fastcall interception; 4/3/2/1 arguments; x87 float returns; muzzle vector; exact forwarding; restored bytes.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
