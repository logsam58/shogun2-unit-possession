#pragma once

// Production recorder: no thread-context writes, register programming or tracer
// code is compiled into this class. Experimental code lives in another build.
class CaptureOnly111 {
    DWORD pid{};std::wstring folder;bool finished{};uint64_t last{};
public:
    CaptureOnly111(HANDLE,DWORD p,const std::wstring&dir,bool):pid(p),folder(dir){
        std::ofstream f(folder+L"\\targeted110.jsonl",std::ios::app);
        f<<"{\"event\":\"hardware_trace_withdrawn\",\"pid\":"<<pid<<",\"scope\":\"capture-only binary; hardware tracer not compiled\"}\n";
    }
    void fixture_init(){}
    void module(uintptr_t,const std::wstring&){}
    void new_thread(DWORD){}
    void exit_thread(DWORD){}
    void unload(uintptr_t){}
    bool exception(DWORD,DWORD){return false;}
    void restore(){}
    void heartbeat(){
        auto now=GetTickCount64();if(now-last<1000)return;last=now;
        auto path=folder+L"\\targeted110-health.json",temp=path+L".tmp";
        {std::ofstream f(temp);f<<"{\"version\":110,\"pid\":"<<pid<<",\"tick_ms\":"<<now
            <<",\"state\":\""<<(finished?"finished":"withdrawn")
            <<"\",\"hardware_compiled\":false,\"effects_destructor\":\"0x0\",\"audio_scout\":\"0x0\",\"audio_wrapper\":\"0x0\",\"audio_interface\":\"0x0\",\"effects_word_watch\":\"0x0\",\"armed_threads\":0,\"traps\":0}\n";}
        MoveFileExW(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING);
    }
    void finish(){finished=true;last=0;heartbeat();}
};
