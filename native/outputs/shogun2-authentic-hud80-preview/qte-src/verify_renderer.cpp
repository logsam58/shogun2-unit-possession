// Actual production renderer with hidden local windows. Never attach to a game.
#include "playable_runtime.cpp"
static HANDLE blocked,unblock;
static WNDPROC saved_proc;
static LRESULT CALLBACK stalled_proc(HWND w,UINT msg,WPARAM wp,LPARAM lp){
    if(msg==WM_APP+77){SetEvent(blocked);WaitForSingleObject(unblock,5000);return 0;}
    return CallWindowProcW(saved_proc,w,msg,wp,lp);
}
int main(){try{
    Status s{};state=&s;module=GetModuleHandleW(nullptr);LARGE_INTEGER f;QueryPerformanceFrequency(&f);frequency=double(f.QuadPart);
    render_initialized=CreateEventW(nullptr,TRUE,FALSE,nullptr);blocked=CreateEventW(nullptr,TRUE,FALSE,nullptr);unblock=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    HANDLE renderer=CreateThread(nullptr,0,render_worker,nullptr,0,nullptr);
    require(renderer&&WaitForSingleObject(render_initialized,5000)==WAIT_OBJECT_0&&render_ready,"Production renderer did not initialize");
    require(GetWindow(overlay,GW_OWNER)==nullptr,"Overlay still owned by game window thread");
    saved_proc=reinterpret_cast<WNDPROC>(SetWindowLongPtrW(overlay,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(stalled_proc)));
    require(saved_proc!=nullptr&&PostMessageW(overlay,WM_APP+77,0,0),"Fixture message failed");
    require(WaitForSingleObject(blocked,5000)==WAIT_OBJECT_0,"Renderer stall fixture not reached");
    auto heartbeat=InterlockedCompareExchange64(&render_heartbeat,0,0);
    for(unsigned i=0;i<2000;++i){
        transport::Payload data;data.pid=GetCurrentProcessId();data.enabled=1;data.published_wall=wall();
        transport::publish(publication,data);transport::Payload readback;
        require(transport::read(publication,readback)&&readback.pid==GetCurrentProcessId(),"Blocked renderer prevented control publication");
    }
    require(heartbeat==InterlockedCompareExchange64(&render_heartbeat,0,0),"Renderer was not actually blocked during fixture");
    InterlockedExchange(&render_stop,1);SetEvent(unblock);
    require(WaitForSingleObject(renderer,5000)==WAIT_OBJECT_0&&!render_ready,"Production renderer did not drain");
    CloseHandle(renderer);CloseHandle(render_initialized);CloseHandle(blocked);CloseHandle(unblock);
    std::cout<<"PASS: actual renderer uses an unowned popup; 2000 control publications remain accessible while its window thread is blocked; renderer resumes and drains. Hidden local fixture only; actual game message stall still requires live verification.\n";return 0;
}catch(const std::exception& e){if(unblock)SetEvent(unblock);std::cerr<<e.what()<<'\n';return 1;}}
