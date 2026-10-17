#pragma once
static volatile LONG raw_mouse_active,raw_mouse_x,raw_mouse_y;
static RAWINPUTDEVICE previous_mouse;
static bool had_previous_mouse;
static bool capture_raw_mouse(){
    if(raw_mouse_active)return true;
    if(!input_window)return false;
    RAWINPUTDEVICE devices[64]{};UINT count=64;
    UINT n=GetRegisteredRawInputDevices(devices,&count,sizeof(RAWINPUTDEVICE));
    if(n==UINT(-1)||n>64){state->raw_mouse_error=GetLastError();return false;}
    had_previous_mouse=false;
    for(UINT i=0;i<n;++i)if(devices[i].usUsagePage==1&&devices[i].usUsage==2){previous_mouse=devices[i];had_previous_mouse=true;break;}
    RAWINPUTDEVICE mouse{1,2,RIDEV_INPUTSINK,input_window};
    if(!RegisterRawInputDevices(&mouse,1,sizeof(mouse))){state->raw_mouse_error=GetLastError();return false;}
    InterlockedExchange(&raw_mouse_x,0);InterlockedExchange(&raw_mouse_y,0);
    InterlockedExchange(&raw_mouse_active,1);state->raw_mouse_active=1;return true;
}
static void restore_raw_mouse(){
    if(!InterlockedExchange(&raw_mouse_active,0))return;
    RAWINPUTDEVICE mouse=had_previous_mouse?previous_mouse:RAWINPUTDEVICE{1,2,RIDEV_REMOVE,nullptr};
    if(!RegisterRawInputDevices(&mouse,1,sizeof(mouse)))state->raw_mouse_error=GetLastError();
    InterlockedExchange(&raw_mouse_x,0);InterlockedExchange(&raw_mouse_y,0);state->raw_mouse_active=0;
}
static bool read_raw_mouse(LPARAM packet){
    RAWINPUTHEADER header{};UINT header_bytes=sizeof(header);
    UINT got=GetRawInputData(reinterpret_cast<HRAWINPUT>(packet),RID_HEADER,&header,&header_bytes,sizeof(RAWINPUTHEADER));
    if(got==UINT(-1)||header.dwType!=RIM_TYPEMOUSE)return false;
    RAWINPUT raw{};UINT bytes=sizeof(raw);
    UINT n=GetRawInputData(reinterpret_cast<HRAWINPUT>(packet),RID_INPUT,&raw,&bytes,sizeof(RAWINPUTHEADER));
    if(n==UINT(-1)){state->raw_mouse_error=GetLastError();return false;}
    if(n<sizeof(RAWINPUTHEADER)+sizeof(RAWMOUSE)||(raw.data.mouse.usFlags&MOUSE_MOVE_ABSOLUTE))return false;
    LONG x=raw.data.mouse.lLastX,y=raw.data.mouse.lLastY;
    InterlockedExchangeAdd(&raw_mouse_x,x);InterlockedExchangeAdd(&raw_mouse_y,y);
    if(x||y){++state->raw_mouse_events;state->raw_y_total+=y;if(state->aiming){++state->raw_aim_events;state->raw_aim_y_total+=y;}}
    return true;
}
