#include "launch161.h"
int WINAPI wWinMain(HINSTANCE module,HINSTANCE,LPWSTR command,int){try{
    Handle mutex(CreateMutexW(nullptr,FALSE,L"Local\\Shogun2UnitPossessionLaunch161"));
    require(mutex.h!=nullptr,"Cannot initialize the camera chooser");
    if(GetLastError()==ERROR_ALREADY_EXISTS)return 0;
    auto plan=launch161::parse(command);launch161::validate(plan);
    return static_cast<int>(launch161::choose_then_start(module,plan));
}catch(const std::exception& error){
    std::string text=std::string(error.what())+"\n\nIn Steam, open Shogun 2 > Properties > Launch Options.\nPaste the line shown by setup, ending in %command%.\nThe game was not started.";
    MessageBoxA(nullptr,text.c_str(),"Shogun 2 Unit Possession",MB_OK|MB_ICONERROR);return 1;
}}
