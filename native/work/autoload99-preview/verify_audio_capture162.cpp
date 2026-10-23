// Offline recorder regression: capture this fixture's process only, never the
// game, microphone, system mix or another application. Synthetic quiet tone.
#include "game_audio162.h"
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audioclientactivationparams.h>
#include <wrl.h>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cmath>
#include <fstream>
#include <iostream>
using Microsoft::WRL::ComPtr;
static void check(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
static void result(HRESULT hr,const char* text){if(FAILED(hr))throw std::runtime_error(std::string(text)+" HRESULT="+std::to_string(hr));}
class Activation final:public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,IActivateAudioInterfaceCompletionHandler,Microsoft::WRL::FtmBase>{
public:
    HANDLE ready=CreateEventW(nullptr,TRUE,FALSE,nullptr);HRESULT status=E_PENDING;ComPtr<IAudioClient> client;
    ~Activation(){if(ready)CloseHandle(ready);}
    HRESULT STDMETHODCALLTYPE ActivateCompleted(IActivateAudioInterfaceAsyncOperation* operation) override {
        ComPtr<IUnknown> value;HRESULT activation=E_FAIL;status=operation->GetActivateResult(&activation,&value);
        if(SUCCEEDED(status))status=activation;if(SUCCEEDED(status))status=value.As(&client);SetEvent(ready);return S_OK;
    }
};
int wmain(int argc,wchar_t** argv){try{
    check(argc==2,"Need synthetic WAV output path");result(CoInitializeEx(nullptr,COINIT_MULTITHREADED),"COM initialization");
    auto activation=Microsoft::WRL::Make<Activation>();check(activation&&activation->ready,"Activation event unavailable");
    AUDIOCLIENT_ACTIVATION_PARAMS params{};params.ActivationType=AUDIOCLIENT_ACTIVATION_TYPE_PROCESS_LOOPBACK;
    params.ProcessLoopbackParams.TargetProcessId=GetCurrentProcessId();params.ProcessLoopbackParams.ProcessLoopbackMode=PROCESS_LOOPBACK_MODE_INCLUDE_TARGET_PROCESS_TREE;
    PROPVARIANT property{};property.vt=VT_BLOB;property.blob.cbSize=sizeof params;property.blob.pBlobData=reinterpret_cast<BYTE*>(&params);
    ComPtr<IActivateAudioInterfaceAsyncOperation> operation;
    result(ActivateAudioInterfaceAsync(VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK,__uuidof(IAudioClient),&property,activation.Get(),&operation),"Process audio activation");
    check(WaitForSingleObject(activation->ready,10000)==WAIT_OBJECT_0,"Process audio activation timed out");result(activation->status,"Process loopback unavailable");
    WAVEFORMATEX format{WAVE_FORMAT_PCM,2,44100,176400,4,16,0};auto client=activation->client;
    result(client->Initialize(AUDCLNT_SHAREMODE_SHARED,AUDCLNT_STREAMFLAGS_LOOPBACK|AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,0,0,&format,nullptr),"Process capture initialization");
    ComPtr<IAudioCaptureClient> capture;result(client->GetService(IID_PPV_ARGS(&capture)),"Capture client");result(client->Start(),"Start process capture");
    std::vector<unsigned char> wave(44+136710,0);memcpy(wave.data(),"RIFF",4);uint32_t n=uint32_t(wave.size()-8);memcpy(wave.data()+4,&n,4);memcpy(wave.data()+8,"WAVEfmt ",8);n=16;memcpy(wave.data()+16,&n,4);
    WAVEFORMATEX tone_format{WAVE_FORMAT_PCM,1,44100,88200,2,16,0};memcpy(wave.data()+20,&tone_format,16);memcpy(wave.data()+36,"data",4);n=136710;memcpy(wave.data()+40,&n,4);
    for(size_t i=0;i<136710/2;++i){int16_t sample=int16_t(64*std::sin(2*3.141592653589793*997*i/44100));memcpy(wave.data()+44+i*2,&sample,2);}
    std::vector<unsigned char> recorded;
    {gameaudio162::Voice voice;check(voice.play(wave.data(),wave.size()),"Production voice output unavailable");check(!voice.play(wave.data(),wave.size()),"Overlapping clips queued");
        auto until=GetTickCount64()+1900;while(GetTickCount64()<until){UINT32 frames=0;result(capture->GetNextPacketSize(&frames),"Capture packet size");
            while(frames){BYTE* bytes{};DWORD flags{};result(capture->GetBuffer(&bytes,&frames,&flags,nullptr,nullptr),"Capture packet");
                if(flags&AUDCLNT_BUFFERFLAGS_SILENT)recorded.insert(recorded.end(),size_t(frames)*4,0);else recorded.insert(recorded.end(),bytes,bytes+size_t(frames)*4);
                result(capture->ReleaseBuffer(frames),"Release capture packet");result(capture->GetNextPacketSize(&frames),"Next capture packet");}Sleep(10);}}
    result(client->Stop(),"Stop process capture");uint64_t audible=0;int peak=0;
    for(size_t i=0;i+2<=recorded.size();i+=2){int16_t sample;memcpy(&sample,recorded.data()+i,2);peak=std::max(peak,std::abs(int(sample)));if(std::abs(int(sample))>=8)++audible;}
    check(recorded.size()>44100*4&&audible>10000&&peak>=20,"Application-only capture missed production voice output");
    std::vector<unsigned char> header(wave.begin(),wave.begin()+44);n=uint32_t(recorded.size()+36);memcpy(header.data()+4,&n,4);memcpy(header.data()+20,&format,16);n=uint32_t(recorded.size());memcpy(header.data()+40,&n,4);
    std::ofstream output(argv[1],std::ios::binary);output.write(reinterpret_cast<const char*>(header.data()),header.size());output.write(reinterpret_cast<const char*>(recorded.data()),recorded.size());check(bool(output),"Cannot save synthetic capture");
    std::cout<<"PASS: production voice captured by Windows process-only loopback; fixture PID="<<GetCurrentProcessId()<<", bytes="<<recorded.size()<<", nonzero samples="<<audible<<", peak="<<peak<<". No game or other-app audio captured.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
