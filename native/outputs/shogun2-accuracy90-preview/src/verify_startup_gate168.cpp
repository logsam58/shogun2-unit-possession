#include "../../../common/startup_gate168.h"
#include "../../../common/component_error168.h"
#include <stdexcept>
#include <iostream>
static void check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
int main(){try{
    SetEnvironmentVariableW(startup168::key,nullptr);
    check(startup168::can_possess(),"Standalone adapter was blocked");
    check(startup168::pending()&&!startup168::can_possess(),"Partial startup accepted possession");
    startup168::failed();check(!startup168::can_possess(),"Failed startup accepted possession");
    check(startup168::ready()&&startup168::can_possess(),"Complete startup was blocked");
    SetEnvironmentVariableW(startup168::key,L"foreign");check(!startup168::can_possess(),"Unknown barrier state accepted");
    SetEnvironmentVariableW(startup168::key,L"overlong-unknown-startup-state");check(!startup168::can_possess(),"Oversized barrier state accepted");
    SetEnvironmentVariableW(startup168::key,nullptr);
    startup167::remember("Native combat hook differs at RVA 0x3e5d40");
    char message[1025]{};auto n=S2UPInitializationError168(message,sizeof message);
    check(n&&std::string(message)==startup167::last_error,"Component failure reason was lost");
    check(!S2UPInitializationError168(nullptr,10)&&!S2UPInitializationError168(message,1026),"Invalid error-port buffer accepted");
    char tiny[5]{};check(S2UPInitializationError168(tiny,5)==4&&tiny[4]==0,"Error port exceeded caller buffer");
    startup167::remember(std::string(3000,'x').c_str());check(S2UPInitializationError168(message,sizeof message)==1024,"Error reason exceeded bound");
    std::cout<<"PASS standalone/pending/failed/complete/unknown startup gates; bounded first-party error port.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
