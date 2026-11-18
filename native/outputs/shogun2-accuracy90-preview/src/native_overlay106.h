#pragma once
#include <d3d9.h>
#include <ostream>
#include <utility>
#include "overlay_d3d11_106.h"
#include "../../shogun2-possession-cleanup-v46/src/game_window105.h"
#include "../vendor/include/MinHook.h"
namespace nativeoverlay106 {
class Renderer9 {
 IDirect3DDevice9* device_{};struct Texture {ComPtr<IDirect3DTexture9> texture;unsigned w{},h{};uint64_t number{};Key key{};} textures_[overlay106::Wheel];
public:
 bool owns(IDirect3DDevice9* device)const{return device_==device;}
 void clear(){for(auto& t:textures_)t={};device_=nullptr;}
 bool draw(IDirect3DDevice9* device,const Snapshot& s,unsigned layer){
  if(layer<1||layer>overlay106::Wheel)return false;if(device_!=device){clear();device_=device;}auto& t=textures_[layer-1];const auto& f=s.frame;
  ComPtr<IDirect3DStateBlock9> state;ComPtr<IDirect3DSurface9> targets[4],depth,back;D3DVIEWPORT9 viewport{};
  if(!ok(device->CreateStateBlock(D3DSBT_ALL,&state))||!ok(state->Capture())||!ok(device->GetViewport(&viewport)))return false;
  for(unsigned i=0;i<4;++i)device->GetRenderTarget(i,&targets[i]);device->GetDepthStencilSurface(&depth);
  struct Restore {IDirect3DDevice9* d;IDirect3DStateBlock9* s;ComPtr<IDirect3DSurface9>* t;IDirect3DSurface9* z;D3DVIEWPORT9 v;~Restore(){s->Apply();for(unsigned i=0;i<4;++i)d->SetRenderTarget(i,t[i].Get());d->SetDepthStencilSurface(z);d->SetViewport(&v);}} restore{device,state.Get(),targets,depth.Get(),viewport};
  if(t.w!=f.width||t.h!=f.height){t={};if(!ok(device->CreateTexture(f.width,f.height,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&t.texture,nullptr)))return false;t.w=f.width;t.h=f.height;}
  if(t.number!=f.number||!overlay106::same(t.key,f.key)){D3DLOCKED_RECT mapped{};if(!ok(t.texture->LockRect(0,&mapped,nullptr,D3DLOCK_DISCARD)))return false;
   for(unsigned row=0;row<f.height;++row)memcpy(static_cast<unsigned char*>(mapped.pBits)+row*mapped.Pitch,s.pixels.data()+size_t(row)*f.width*4,f.width*4);t.texture->UnlockRect(0);t.number=f.number;t.key=f.key;InterlockedIncrement(&stats.uploads);}
  if(!ok(device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&back)))return false;D3DSURFACE_DESC bd{};back->GetDesc(&bd);
  if(!ok(device->SetRenderTarget(0,back.Get())))return false;for(unsigned i=1;i<4;++i)device->SetRenderTarget(i,nullptr);device->SetDepthStencilSurface(nullptr);
  D3DVIEWPORT9 v{0,0,bd.Width,bd.Height,0,1};device->SetViewport(&v);device->SetVertexShader(nullptr);device->SetPixelShader(nullptr);device->SetFVF(D3DFVF_XYZRHW|D3DFVF_TEX1);
  for(auto pair:{std::pair<D3DRENDERSTATETYPE,DWORD>{D3DRS_ZENABLE,FALSE},{D3DRS_FILLMODE,D3DFILL_SOLID},{D3DRS_ZWRITEENABLE,FALSE},{D3DRS_ALPHABLENDENABLE,TRUE},{D3DRS_SRCBLEND,D3DBLEND_ONE},{D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA},{D3DRS_BLENDOP,D3DBLENDOP_ADD},{D3DRS_SEPARATEALPHABLENDENABLE,FALSE},{D3DRS_CULLMODE,D3DCULL_NONE},{D3DRS_LIGHTING,FALSE},{D3DRS_FOGENABLE,FALSE},{D3DRS_ALPHATESTENABLE,FALSE},{D3DRS_STENCILENABLE,FALSE},{D3DRS_SCISSORTESTENABLE,FALSE},{D3DRS_COLORWRITEENABLE,15},{D3DRS_SRGBWRITEENABLE,FALSE}})device->SetRenderState(pair.first,pair.second);
  device->SetTexture(0,t.texture.Get());device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);device->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
  device->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);device->SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE);device->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
  device->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);device->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);device->SetSamplerState(0,D3DSAMP_SRGBTEXTURE,0);
  if(!ok(device->BeginScene()))return false;bool result=true;
  struct V {float x,y,z,w,u,v;};
  for(unsigned i=0;i<f.rect_count;++i){auto q=f.rect[i];float l=float(q.x)*bd.Width/f.view_width-.5f,r=float(q.x+q.width)*bd.Width/f.view_width-.5f,top=float(q.y)*bd.Height/f.view_height-.5f,b=float(q.y+q.height)*bd.Height/f.view_height-.5f;
   V vertices[]={{l,top,0,1,0,0},{r,top,0,1,1,0},{l,b,0,1,0,1},{l,b,0,1,0,1},{r,top,0,1,1,0},{r,b,0,1,1,1}};result=ok(device->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,vertices,sizeof(V)))&&result;}
  device->EndScene();if(result)InterlockedIncrement(&stats.draws);return result;
 }
};
using Gate=bool(*)(const Frame&,unsigned);static Gate gate;
// Explicit stop releases GPU resources. The adapter/trampolines stay resident;
// process termination must not call a graphics driver from CRT loader-lock
// destructors after its render threads have already been terminated.
static Renderer11& renderer11=*new Renderer11;static Renderer9& renderer9=*new Renderer9;
static overlay106::Reader readers[overlay106::Wheel];static uint64_t retry[overlay106::Wheel];
static SRWLOCK lock=SRWLOCK_INIT;static volatile LONG stopping;
using Present11=HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*,UINT,UINT);using Resize11=HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*,UINT,UINT,UINT,DXGI_FORMAT,UINT);
using Present9=HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,const RECT*,const RECT*,HWND,const RGNDATA*);using Reset9=HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*,D3DPRESENT_PARAMETERS*);
static Present11 original11;static Resize11 resize11;static Present9 original9;static Reset9 reset9;static void* hooks[4]{};
// Acknowledgement follows successful native Present. Record the call time,
// never claim physical scanout; TEST/occluded/lost presents cannot hide fallback.
struct Submission {unsigned backend[overlay106::Wheel]{};Frame frame[overlay106::Wheel]{};bool has[overlay106::Wheel]{};double wall{};};
template<class Draw>inline Submission surfaces(unsigned backend,Draw draw){
 Submission submitted;auto now=GetTickCount64();for(unsigned layer:{overlay106::HUD,overlay106::Wheel,overlay106::QTE,overlay106::Death,overlay106::Help}){const unsigned i=layer-1;if(now<retry[i])continue;if(!readers[i].open(GetCurrentProcessId(),i+1)){retry[i]=now+250;continue;}
  submitted.has[i]=true;auto image=readers[i].read();
  if(image&&gate&&gate(image->frame,i+1)){if(draw(*image,i+1)){submitted.backend[i]=backend;submitted.frame[i]=image->frame;}}
  else submitted.backend[i]=backend;
 }submitted.wall=overlay106::wall();return submitted;
}
inline void acknowledge(const Submission& s,HRESULT result){
 AcquireSRWLockExclusive(&lock);if(!stopping)for(unsigned i=0;i<overlay106::Wheel;++i)if(s.has[i])readers[i].acknowledge(result==S_OK?s.backend[i]:0,s.frame[i].number?&s.frame[i]:nullptr,s.wall);ReleaseSRWLockExclusive(&lock);
}
static __declspec(thread) bool presenting;
struct PresentScope {PresentScope(){presenting=true;}~PresentScope(){presenting=false;}};
static HRESULT STDMETHODCALLTYPE present11(IDXGISwapChain* chain,UINT sync,UINT flags){
 if(presenting||stopping||(flags&DXGI_PRESENT_TEST))return original11(chain,sync,flags);
 PresentScope scope;Submission submitted;DXGI_SWAP_CHAIN_DESC d{};
 if(SUCCEEDED(chain->GetDesc(&d))&&gamewindow105::belongs(d.OutputWindow,GetCurrentProcessId())){
  AcquireSRWLockExclusive(&lock);try{if(!stopping&&renderer11.begin(chain)){InterlockedIncrement(&stats.presents11);stats.width=LONG(renderer11.width());stats.height=LONG(renderer11.height());stats.fullscreen=!d.Windowed;submitted=surfaces(11,[](const Snapshot& s,unsigned layer){return renderer11.draw(s,layer);});}}catch(...){stats.error=E_OUTOFMEMORY;}ReleaseSRWLockExclusive(&lock);
 }HRESULT result=original11(chain,sync,flags);acknowledge(submitted,result);return result;
}
static HRESULT STDMETHODCALLTYPE resized11(IDXGISwapChain* chain,UINT count,UINT width,UINT height,DXGI_FORMAT format,UINT flags){
 AcquireSRWLockExclusive(&lock);if(renderer11.owns(chain)){renderer11.clear();InterlockedIncrement(&stats.resizes);}ReleaseSRWLockExclusive(&lock);return resize11(chain,count,width,height,format,flags);
}
static HRESULT STDMETHODCALLTYPE present9(IDirect3DDevice9* device,const RECT* source,const RECT* dest,HWND window,const RGNDATA* dirty){
 if(presenting||stopping)return original9(device,source,dest,window,dirty);
 PresentScope scope;Submission submitted;D3DDEVICE_CREATION_PARAMETERS p{};
 if(SUCCEEDED(device->GetCreationParameters(&p))&&gamewindow105::belongs(window?window:p.hFocusWindow,GetCurrentProcessId())){
  AcquireSRWLockExclusive(&lock);try{if(!stopping){InterlockedIncrement(&stats.presents9);ComPtr<IDirect3DSwapChain9> swap;D3DPRESENT_PARAMETERS d{};if(SUCCEEDED(device->GetSwapChain(0,&swap))&&SUCCEEDED(swap->GetPresentParameters(&d))){stats.width=LONG(d.BackBufferWidth);stats.height=LONG(d.BackBufferHeight);stats.fullscreen=!d.Windowed;}submitted=surfaces(9,[&](const Snapshot& s,unsigned layer){return renderer9.draw(device,s,layer);});}}catch(...){stats.error=E_OUTOFMEMORY;}ReleaseSRWLockExclusive(&lock);
 }HRESULT result=original9(device,source,dest,window,dirty);acknowledge(submitted,result);return result;
}
static HRESULT STDMETHODCALLTYPE reset_device9(IDirect3DDevice9* device,D3DPRESENT_PARAMETERS* params){AcquireSRWLockExclusive(&lock);if(renderer9.owns(device)){renderer9.clear();InterlockedIncrement(&stats.resizes);}ReleaseSRWLockExclusive(&lock);return reset9(device,params);}
inline bool hook(void* target,void* function,void** original,unsigned slot){if(!target||MH_CreateHook(target,function,original)!=MH_OK)return false;hooks[slot]=target;return true;}
inline void remove_pair(unsigned first){for(unsigned i=first;i<first+2;++i)if(hooks[i]){MH_RemoveHook(hooks[i]);hooks[i]=nullptr;}}
inline void install(Gate accept){
 gate=accept;stopping=0;HWND w=CreateWindowExW(0,L"STATIC",L"",WS_POPUP,0,0,2,2,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
 if(!w){stats.error=HRESULT_FROM_WIN32(GetLastError());return;}
 DXGI_SWAP_CHAIN_DESC sd{};sd.BufferCount=1;sd.BufferDesc.Width=sd.BufferDesc.Height=2;sd.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;sd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;sd.OutputWindow=w;sd.SampleDesc.Count=1;sd.Windowed=TRUE;
 ComPtr<IDXGISwapChain> sc;ComPtr<ID3D11Device> d;ComPtr<ID3D11DeviceContext> c;HRESULT h=D3D11CreateDeviceAndSwapChain(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&sd,&sc,&d,nullptr,&c);
 if(FAILED(h))h=D3D11CreateDeviceAndSwapChain(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&sd,&sc,&d,nullptr,&c);
 bool a=false,b=false;if(SUCCEEDED(h)){auto table=*reinterpret_cast<void***>(sc.Get());a=hook(table[8],reinterpret_cast<void*>(&present11),reinterpret_cast<void**>(&original11),0)&&hook(table[13],reinterpret_cast<void*>(&resized11),reinterpret_cast<void**>(&resize11),1);}
 ComPtr<IDirect3D9> api;api.Attach(Direct3DCreate9(D3D_SDK_VERSION));if(api){D3DPRESENT_PARAMETERS p{};p.Windowed=TRUE;p.SwapEffect=D3DSWAPEFFECT_DISCARD;p.hDeviceWindow=w;p.BackBufferWidth=p.BackBufferHeight=2;p.BackBufferFormat=D3DFMT_UNKNOWN;ComPtr<IDirect3DDevice9> nine;
  if(SUCCEEDED(api->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,w,D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED|D3DCREATE_FPU_PRESERVE,&p,&nine))){auto table=*reinterpret_cast<void***>(nine.Get());b=hook(table[17],reinterpret_cast<void*>(&present9),reinterpret_cast<void**>(&original9),2)&&hook(table[16],reinterpret_cast<void*>(&reset_device9),reinterpret_cast<void**>(&reset9),3);}}
 if(!a)remove_pair(0);if(!b)remove_pair(2);c.Reset();sc.Reset();d.Reset();api.Reset();
 DestroyWindow(w);stats.initialized=(a?1:0)|(b?2:0);if(!a&&!b)stats.error=E_FAIL;
}
inline void stop(){InterlockedExchange(&stopping,1);for(auto p:hooks)if(p)MH_DisableHook(p);AcquireSRWLockExclusive(&lock);renderer11.clear();renderer9.clear();for(auto& r:readers)r.close();ReleaseSRWLockExclusive(&lock);}
inline void log(std::ostream& out){out<<"{\"event\":\"presentation106\",\"tick_ms\":"<<GetTickCount64()<<",\"initialized\":"<<stats.initialized<<",\"error\":"<<stats.error<<",\"presents11\":"<<stats.presents11<<",\"presents9\":"<<stats.presents9<<",\"draws\":"<<stats.draws<<",\"uploads\":"<<stats.uploads<<",\"resizes\":"<<stats.resizes<<",\"width\":"<<stats.width<<",\"height\":"<<stats.height<<",\"fullscreen\":"<<stats.fullscreen<<"}\n";}
}
