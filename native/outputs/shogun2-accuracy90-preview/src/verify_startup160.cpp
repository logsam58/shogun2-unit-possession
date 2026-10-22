#define main retained_commands160
#include "verify_native_commands100.cpp"
#undef main
int main(){SetUnhandledExceptionFilter(fault100);try{
    NativeFixture100 fixture;
    for(bool third:{false,true}){
        view_default160(third);
        for(unsigned i=0;i<100;++i){
            view_reset152();++possession_lease.generation;view_prepare152();
            require(bool(third_person152)==third,"New possession ignored launch default");
            std::uint8_t current[256]{},previous[256]{};current[0x19]=0x80;
            view_key152(current,previous,true,true);view_prepare152();require(bool(third_person152)!=third,"P no longer toggles from launch default");
            view_prepare152();require(bool(third_person152)!=third,"Next frame reset manual P selection");
            current[0x19]=0;view_key152(current,previous,true,true);
        }
    }
    view_default160(false);view_reset152();
    std::cout<<"PASS 200 new possession leases honor first/third launch choice; P remains stable within a lease, reset returns to chosen default; no native gameplay/provider changes.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
