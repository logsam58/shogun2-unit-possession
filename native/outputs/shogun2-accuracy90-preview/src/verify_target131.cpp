#define main retained_target131_main
#include "verify_native_commands100.cpp"
#undef main
static void __fastcall filter131(void*,void*,void*,unsigned){}
static uintptr_t __fastcall actor131(void* p,void*){return static_cast<uintptr_t*>(p)[1];}
int main(){SetUnhandledExceptionFilter(&fault100);try{
    NativeFixture100 f;using P=NativeFixture100;
    HWND window=CreateWindowExW(0,L"STATIC",L"Target131",WS_POPUP,0,0,1280,720,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
    require(window!=nullptr,"Target fixture window");input_window=window;native_window_proc=&stock_window100;
    original_cursor_filter103=reinterpret_cast<CursorFilter103>(&filter131);
    state->heartbeat=GetTickCount64();
    command_native_root100=P::ptr(f.root);command_native_environment100=environment;command_native_lease100=possession_lease.generation;
    require(command_native_ready100(window),"Synthetic Q context is not ready");
    uintptr_t side[8]{},ally_side[8]{},enemy_side[8]{},physical_vt[7]{};
    side[4]=ally_side[4]=0;enemy_side[4]=1;physical_vt[6]=reinterpret_cast<uintptr_t>(&actor131);
    std::vector<unsigned char> ally(0x700),enemy(0x700);
    field<uintptr_t>(possessed,0x44)=reinterpret_cast<uintptr_t>(side);
    for(auto actor:{P::ptr(ally),P::ptr(enemy)}){field<uintptr_t>(actor,0x214)=unit+16;field<uintptr_t>(actor,0x28)=environment;}
    field<uintptr_t>(P::ptr(ally),0x44)=reinterpret_cast<uintptr_t>(ally_side);field<uintptr_t>(P::ptr(enemy),0x44)=reinterpret_cast<uintptr_t>(enemy_side);
    uintptr_t physical[4][2]{},shape[4][4]{};unsigned char hits[4*28]{},saved[sizeof hits]{};
    for(unsigned i=0;i<4;++i){physical[i][0]=reinterpret_cast<uintptr_t>(physical_vt);physical[i][1]=i==0?possessed:i==1?P::ptr(ally):P::ptr(enemy);
        shape[i][0]=8;shape[i][3]=reinterpret_cast<uintptr_t>(physical[i]);field<uintptr_t>(reinterpret_cast<uintptr_t>(hits),i*28)=reinterpret_cast<uintptr_t>(shape[i]);}
    field<unsigned>(reinterpret_cast<uintptr_t>(hits),3*28+24)=1; // Native banner priority metadata.
    memcpy(saved,hits,sizeof hits);unsigned vector[3]={4,4,reinterpret_cast<unsigned>(hits)};auto cursor=P::ptr(f.root)+0x10c;
    for(unsigned repeat=0;repeat<100;++repeat){memcpy(hits,saved,sizeof hits);vector[1]=4;
        cursor_filter103(reinterpret_cast<void*>(cursor),nullptr,vector,0);
        require(vector[1]==2&&field<uintptr_t>(reinterpret_cast<uintptr_t>(hits),0)==reinterpret_cast<uintptr_t>(shape[2])&&field<unsigned>(reinterpret_cast<uintptr_t>(hits),28+24)==1,"Friendly foreground hid hostile actor/banner or lost native banner metadata");
    }
    memcpy(hits,saved,sizeof hits);vector[1]=2;cursor_filter103(reinterpret_cast<void*>(cursor),nullptr,vector,0);
    require(vector[1]==1&&field<uintptr_t>(reinterpret_cast<uintptr_t>(hits),0)==reinterpret_cast<uintptr_t>(shape[1]),"Friendly-only targeting was removed");
    // World hits must survive, including a world hit before enemy geometry.
    shape[0][0]=1;memcpy(hits,saved,sizeof hits);vector[1]=4;cursor_filter103(reinterpret_cast<void*>(cursor),nullptr,vector,0);
    require(vector[1]==3&&field<uintptr_t>(reinterpret_cast<uintptr_t>(hits),0)==reinterpret_cast<uintptr_t>(shape[0]),"World occluder was reordered or removed");
    shape[0][0]=8;memcpy(hits,saved,sizeof hits);vector[1]=4;command_native_root100=0;cursor_filter103(reinterpret_cast<void*>(cursor),nullptr,vector,0);
    require(vector[1]==4&&!memcmp(hits,saved,sizeof hits),"Normal RTS selection changed");command_native_root100=P::ptr(f.root);
    ++field<unsigned>(possessed,0x1c8);cursor_filter103(reinterpret_cast<void*>(cursor),nullptr,vector,0);require(vector[1]==4&&!memcmp(hits,saved,sizeof hits),"Stale ownership filtered native hits");--field<unsigned>(possessed,0x1c8);
    input_window=nullptr;native_window_proc=nullptr;DestroyWindow(window);
    std::cout<<"PASS: production Q filter across own/allied-army/enemy/banner overlaps; banner metadata and world order retained, friendly-only/RTS/stale identity unchanged. Native scene query/filter providers substituted; stock constructor remains covered by context103.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
