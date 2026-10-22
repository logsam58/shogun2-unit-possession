#include "startup_view160.h"
#include <iostream>
#include <stdexcept>
static void check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
struct Action {int button;bool valid{};};
static void inspect(HWND window,void* raw){
    auto a=static_cast<Action*>(raw);wchar_t text[256]{};GetWindowTextW(window,text,256);
    a->valid=!wcscmp(text,L"Shogun 2 Unit Possession")&&LOWORD(SendMessageW(window,DM_GETDEFID,0,0))==startupdialog160::third_button;
    RECT client{};GetClientRect(window,&client);
    for(int id:{startupdialog160::third_button,startupdialog160::first_button,1003}){
        HWND control=GetDlgItem(window,id);RECT box{};GetWindowRect(control,&box);MapWindowPoints(nullptr,window,reinterpret_cast<POINT*>(&box),2);
        a->valid=a->valid&&control&&box.left>=0&&box.top>=0&&box.right<=client.right&&box.bottom<=client.bottom;
    }
    GetWindowTextW(GetDlgItem(window,startupdialog160::third_button),text,256);a->valid=a->valid&&!wcscmp(text,L"Third-Person");
    GetWindowTextW(GetDlgItem(window,startupdialog160::first_button),text,256);a->valid=a->valid&&!wcscmp(text,L"First-Person");
    if(a->button==IDCANCEL)PostMessageW(window,WM_CLOSE,0,0);else PostMessageW(window,WM_COMMAND,a->button,0);
}
int wmain(){try{
    wchar_t previous[32768]{};DWORD length=GetEnvironmentVariableW(startup160::environment,previous,32768);
    SetEnvironmentVariableW(startup160::environment,nullptr);check(!startup160::third_person(),"Missing choice changed historical fallback");
    for(const wchar_t* invalid:{L"",L"third",L"THIRD-PERSON",L"third-person-extra",L"abcdefghijklmnopqrstuvwxyz0123456789"}){SetEnvironmentVariableW(startup160::environment,invalid);check(!startup160::third_person(),"Invalid environment value enabled third person");}
    for(int id:{startupdialog160::third_button,startupdialog160::first_button,IDCANCEL}){
        Action action{id};startupdialog160::Observer observer{inspect,&action};bool third=startupdialog160::choose(GetModuleHandleW(nullptr),nullptr,&observer);
        check(action.valid,"Real dialog title, labels, default or bounds differ");check(third==(id!=startupdialog160::first_button),"Dialog choice differs");
        check(startup160::publish(third)&&startup160::third_person()==third,"Published process choice differs");
    }
    check(!startupdialog160::resolve(-1)&&!startupdialog160::resolve(0),"Dialog failure did not preserve first person");
    SetEnvironmentVariableW(startup160::environment,length&&length<32768?previous:nullptr);
    std::cout<<"PASS real native chooser: third/first/close choices, default/labels/control bounds; strict process-local preference and failure fallback; no game or settings accessed.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
