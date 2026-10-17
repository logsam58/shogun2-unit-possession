#include "identity.h"
#include "steering.h"
#include "orders.h"
#include "../vendor/include/MinHook.h"
#include <cassert>
static bool close_float(float a,float b){return std::abs(a-b)<0.00001f;}
struct Probe {float intent=2,position=0;unsigned calls=0;__declspec(noinline) void step(){position+=intent;++calls;}__declspec(noinline) unsigned argument(unsigned value){calls+=value;return calls+13;}};
using Step=void(__thiscall*)(Probe*);static Step trampoline;static unsigned hooks=0;
using Argument=unsigned(__thiscall*)(Probe*,unsigned);static Argument argument_trampoline;
static unsigned __fastcall argument_intercept(Probe* p,void*,unsigned value){return argument_trampoline(p,value+1);}
static void __fastcall intercepted(Probe* p,void*){++hooks;p->intent=3;trampoline(p);}
int main(){
    try {
        require(sizeof(void*)==4,"Wrong ABI");
        unsigned char packet[8]={8,0,0x12,0x34,0x56,0x78,0,99};
        unsigned char queue[QUEUE_CAPACITY+16]{};uint32_t used=QUEUE_CAPACITY-8;
        require(append_packet(queue,used,packet,8,0x12345678)&&used==QUEUE_CAPACITY,"Queue exact fit");
        require(!append_packet(queue,used,packet,8,0x12345678)&&used==QUEUE_CAPACITY,"Queue overflow rejected");
        require(!valid_packet(packet,8,0x12345679)&&!valid_packet(packet,6,0x12345678),"Descriptor and truncated header rejected");
        packet[0]=9;used=0;
        require(!append_packet(queue,used,packet,8,0x12345678)&&used==0,"Malformed size cannot change queue");
        packet[0]=8;used=QUEUE_CAPACITY+1;
        require(!append_packet(queue,used,packet,8,0x12345678),"Corrupt queue cursor rejected");
        require(queue[QUEUE_CAPACITY]==0,"Queue end sentinel preserved");
        Intent zero=steering(0,0,0.1f);require(close_float(zero.x,0)&&close_float(zero.z,0)&&zero.turn==0,"Neutral intent");
        Intent forward=steering(FORWARD,0,0.1f);require(close_float(forward.x,0)&&close_float(forward.z,0.18f),"Bearing zero");
        Intent quarter=steering(FORWARD,16384,0.1f);require(close_float(quarter.x,0.18f)&&close_float(quarter.z,0),"Quarter bearing");
        Intent diagonal=steering(FORWARD|RIGHT,0,0.1f);require(close_float(std::hypot(diagonal.x,diagonal.z),0.18f),"Diagonal speed normalized");
        Intent opposing=steering(FORWARD|BACKWARD|LEFT|RIGHT,42000,0.1f);require(close_float(opposing.x,0)&&close_float(opposing.z,0),"Opposite inputs cancel");
        Intent native_slow=steering_view(FORWARD|RIGHT,0,.1f,.9f),native_fast=steering_view(FORWARD|RIGHT,0,.1f,4.2f);
        require(close_float(std::hypot(native_slow.x,native_slow.z),.09f)&&close_float(std::hypot(native_fast.x,native_fast.z),.42f),"Distinct native unit speeds preserved");
        Intent no_run=steering_view(FORWARD|RUN,0,.1f,.9f);
        require(close_float(no_run.z,.09f),"Denied native running cannot use hardcoded sprint speed");
        require(steering(TURN_LEFT,0,0.1f).turn<0&&steering(TURN_RIGHT,0,0.1f).turn>0,"Turn signs");
        require(face_view(65500,0,0.1f)==36,"Facing wraps across zero");
        require(face_view(0,3.0f,0.1f)>0&&face_view(0,3.0f,0.1f)<=1311,"Facing turn bounded");
        // Actual current battle records: samurai=120 deg/s; ashigaru=80 deg/s.
        require(face_view(0,3.0f,.1f,2.094395160675049f)==2184,"Samurai native turn limit");
        require(face_view(0,3.0f,.1f,1.3962633609771729f)==1456,"Ashigaru native turn limit");
        require(face_view(65500,0,.1f,1.3962633609771729f)==36,"Native turn crosses zero");
        require(face_view(0,-3.0f,.1f,1.3962633609771729f)==-1456,"Native turn symmetric");
        require(face_view(0,3.0f,.1f,0)==0&&face_view(0,3.0f,0,2)==0,"Zero native turn and pause respected");
        require(face_view(0,3.0f,.1f,-1)==0&&face_view(0,NAN,.1f,2)==0,"Invalid turn cannot write intent");
        require(face_view(0,3.0f,10,100)>0,"Large turn saturates without overflow");
        require(face_view(0,1e20f,.1f,2)>=-2086&&face_view(0,1e20f,.1f,2)<=2086,"Large yaw reduced before conversion");
        auto member=&Probe::step;Step address=nullptr;static_assert(sizeof(member)==sizeof(address),"Unexpected member ABI");memcpy(&address,&member,sizeof(address));
        unsigned char bytes[16];memcpy(bytes,reinterpret_cast<void*>(address),16);
        require(MH_Initialize()==MH_OK,"Hook init");require(MH_CreateHook(reinterpret_cast<void*>(address),reinterpret_cast<void*>(&intercepted),reinterpret_cast<void**>(&trampoline))==MH_OK,"Hook create");
        require(MH_EnableHook(reinterpret_cast<void*>(address))==MH_OK,"Hook enable");Probe p;volatile Step call=address;call(&p);
        require(hooks==1&&p.calls==1&&close_float(p.position,3),"Native thiscall detour/trampoline");
        require(MH_DisableHook(reinterpret_cast<void*>(address))==MH_OK,"Hook disable");require(memcmp(bytes,reinterpret_cast<void*>(address),16)==0,"Original bytes restored");
        call(&p);require(hooks==1&&p.calls==2&&close_float(p.position,6),"Unhooked native call");require(MH_Uninitialize()==MH_OK,"Hook cleanup");
        auto member_arg=&Probe::argument;Argument arg_address=nullptr;memcpy(&arg_address,&member_arg,sizeof(arg_address));
        memcpy(bytes,reinterpret_cast<void*>(arg_address),16);require(MH_Initialize()==MH_OK,"Argument init");
        require(MH_CreateHook(reinterpret_cast<void*>(arg_address),reinterpret_cast<void*>(&argument_intercept),reinterpret_cast<void**>(&argument_trampoline))==MH_OK,"Argument create");
        require(MH_EnableHook(reinterpret_cast<void*>(arg_address))==MH_OK,"Argument enable");volatile Argument arg_call=arg_address;
        require(arg_call(&p,4)==20&&p.calls==7,"Stack argument and return value ABI");
        require(MH_DisableHook(MH_ALL_HOOKS)==MH_OK&&memcmp(bytes,reinterpret_cast<void*>(arg_address),16)==0,"Argument restoration");
        require(arg_call(&p,1)==21&&p.calls==8,"Argument after disable");require(MH_Uninitialize()==MH_OK,"Argument cleanup");
        std::cout<<"PASS: x86 thiscall interception, original trampoline, byte restoration, native call after disable, neutral/diagonal/bearing/turn steering, packet identity and queue bounds.\n";return 0;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}


