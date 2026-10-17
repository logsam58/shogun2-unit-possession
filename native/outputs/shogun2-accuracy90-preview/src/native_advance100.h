#pragma once
// Q remaps the stock RMB gesture. No frontage math, custom target picking,
// custom preview or independently constructed Q move packet lives here.
using NativeMouseDecode100=unsigned(__cdecl*)(HWND,UINT,WPARAM,LPARAM,int,int,int);
using NativeCursorEvent100=void(__thiscall*)(void*,const unsigned*,unsigned,unsigned,unsigned);
static uintptr_t command_native_root100;
static unsigned command_native_lease100,command_native_sequence100;
static uintptr_t command_native_environment100;
static volatile LONG command_native_presses100,command_native_moves100,command_native_releases100,command_native_cancels100;
static bool command_native_context100(uintptr_t root,uintptr_t expected_environment=0){
    if(!expected_environment)expected_environment=environment;
    return root&&field<uintptr_t>(base,0x1bc8180)==root&&field<uintptr_t>(root,0x110)==expected_environment&&
        field<uintptr_t>(root,0)==base+0x170d284&&field<uintptr_t>(root,0x18)==base+0x170d290&&
        field<uintptr_t>(root,0x10c)==base+0x170d304;
}
static WPARAM command_native_modifiers100(){return ((GetKeyState(VK_SHIFT)&0x8000)?MK_SHIFT:0)|((GetKeyState(VK_CONTROL)&0x8000)?MK_CONTROL:0);}
static LPARAM command_native_point100(HWND window,RECT& client){
    POINT p{};if(!GetClientRect(window,&client)||client.right<200||client.bottom<200||!GetCursorPos(&p)||!ScreenToClient(window,&p))return -1;
    p.x=std::clamp(p.x,0L,client.right-1);p.y=std::clamp(p.y,0L,client.bottom-1);return MAKELPARAM(p.x,p.y);
}
static void command_native_cancel100(){
    if(!command_native_root100)return;
    if(state&&GetCurrentThreadId()!=state->window_thread){
        InterlockedExchange(&command_placing100,0);InterlockedExchange(&command_recenter100,1);
        if(input_window)PostMessageW(input_window,COMMAND_CANCEL_MESSAGE100,command_native_sequence100,0);return;
    }
    auto root=command_native_root100;auto prior_environment=command_native_environment100;command_native_root100=0;command_native_lease100=0;command_native_environment100=0;
    InterlockedExchange(&command_placing100,0);InterlockedExchange(&command_recenter100,1);
    __try{
        if(!command_native_context100(root,prior_environment))return;
        // The stock cursor's release cleanup frees its snapshot and clears its
        // held state. Calling the base cleanup bypasses the order-commit layer.
        RECT client{};auto point=command_native_point100(input_window,client);
        if(point!=-1)reinterpret_cast<NativeMouseDecode100>(base+0xbd4a10)(input_window,WM_RBUTTONUP,command_native_modifiers100(),point,client.right,client.bottom,0);
        unsigned released=4;reinterpret_cast<NativeCursorEvent100>(base+0x2f7350)(reinterpret_cast<void*>(root+0x10c),&released,0,0,1);
        if(native_window_proc&&input_window)CallWindowProcW(native_window_proc,input_window,WM_CANCELMODE,0,0);
        InterlockedIncrement(&command_native_cancels100);
    }__except(EXCEPTION_EXECUTE_HANDLER){}
}
static bool command_native_ready100(HWND window){
    if(!state||!native_window_proc||window!=input_window||GetCurrentThreadId()!=state->window_thread||!possession_lease.active||
       !enabled||state->stop||state->release||state->mode!=POSSESSED||!followed(possessed,false)||!camera_owned(possession_lease.identity.camera)||dead_view())return false;
    melee60::Request commitment{};return !melee_request(commitment)&&!wall_subject97(possessed)&&!native_airborne_subject99(possessed);
}
static bool command_native_begin100(HWND window){
    __try{
        if(command_native_root100)command_native_cancel100();
        if(!command_native_ready100(window))return false;
        auto root=field<uintptr_t>(base,0x1bc8180);if(!command_native_context100(root))return false;
        RECT client{};auto point=command_native_point100(window,client);if(point==-1)return false;
        command_native_root100=root;command_native_environment100=environment;command_native_lease100=possession_lease.generation;if(!++command_native_sequence100)++command_native_sequence100;InterlockedExchange(&command_placing100,1);
        InterlockedExchange(&state->aiming,0);InterlockedExchange(&state->fire_request,0);
        CallWindowProcW(native_window_proc,window,WM_RBUTTONDOWN,MK_RBUTTON|command_native_modifiers100(),point);InterlockedIncrement(&command_native_presses100);return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){command_native_cancel100();return false;}
}
static bool command_native_end100(HWND window){
    __try{
        if(!command_native_ready100(window)||command_native_lease100!=possession_lease.generation||!command_native_context100(command_native_root100)){
            command_native_cancel100();return false;
        }
        RECT client{};auto point=command_native_point100(window,client);if(point==-1){command_native_cancel100();return false;}
        CallWindowProcW(native_window_proc,window,WM_RBUTTONUP,command_native_modifiers100(),point);command_native_root100=0;command_native_environment100=0;command_native_lease100=0;
        InterlockedExchange(&command_placing100,0);InterlockedExchange(&command_recenter100,1);InterlockedIncrement(&command_native_releases100);return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){command_native_cancel100();return false;}
}
static bool command_native_mouse100(HWND window,UINT message,WPARAM flags,LPARAM point){
    if(!command_native_root100)return false;
    if(!command_native_ready100(window)||command_native_lease100!=possession_lease.generation){command_native_cancel100();return false;}
    __try{
        if(!command_native_context100(command_native_root100)){command_native_cancel100();return false;}
        if(message==WM_MOUSEMOVE){CallWindowProcW(native_window_proc,window,message,(flags|MK_RBUTTON)&~MK_LBUTTON,point);InterlockedIncrement(&command_native_moves100);return true;}
    }__except(EXCEPTION_EXECUTE_HANDLER){command_native_cancel100();}
    return false;
}
