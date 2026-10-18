// Real private Direct3D devices/back buffers. No game launch, input, foreground
// window or desktop display-mode changes. Fullscreen scanout remains live work.
#include "native_overlay106.h"
#include <iostream>
#include <stdexcept>
#include <cmath>
using namespace nativeoverlay106;
static void check(bool b,const char* why){if(!b)throw std::runtime_error(why);}
static void hr(HRESULT h,const char* why){if(FAILED(h)){std::cerr<<"HRESULT "<<std::hex<<unsigned(h)<<std::dec<<' ';throw std::runtime_error(why);}}
static Snapshot image(unsigned w,unsigned h){Snapshot s;s.frame.key={3,5,7};s.frame.active=1;s.frame.number=1;s.frame.tick=GetTickCount64();s.frame.width=s.frame.height=2;s.frame.view_width=w;s.frame.view_height=h;s.frame.rect_count=1;s.frame.rect[0]={int(w/4),int(h/4),int(w/2),int(h/2)};s.pixels.resize(16);for(unsigned i=0;i<4;++i)reinterpret_cast<unsigned*>(s.pixels.data())[i]=0x80800000;return s;}
static void pixel(unsigned value){int r=(value>>16)&255,b=value&255;check(std::abs(r-128)<=2&&std::abs(b-127)<=2,"Premultiplied alpha/back-buffer pixel differs");}
static void eleven(HWND window){
 ComPtr<IDXGISwapChain> swap;ComPtr<ID3D11Device> device;ComPtr<ID3D11DeviceContext> context;
 DXGI_SWAP_CHAIN_DESC d{};d.BufferDesc.Width=1280;d.BufferDesc.Height=720;d.BufferDesc.Format=DXGI_FORMAT_B8G8R8A8_UNORM;d.SampleDesc.Count=1;d.BufferCount=1;d.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;d.OutputWindow=window;d.Windowed=TRUE;
 hr(D3D11CreateDeviceAndSwapChain(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&d,&swap,&device,nullptr,&context),"DX11 private device");
 unsigned cases=0;Renderer11 renderer;
 for(auto size:{std::pair<UINT,UINT>{1280,720},{1920,1080},{3840,2160}}){
  renderer.clear();context->ClearState();hr(swap->ResizeBuffers(1,size.first,size.second,DXGI_FORMAT_B8G8R8A8_UNORM,0),"DX11 resize releases all compositor backbuffer references");
  for(UINT sample:{1u,4u,8u}){
   UINT quality=0;hr(device->CheckMultisampleQualityLevels(d.BufferDesc.Format,sample,&quality),"DX11 quality query");if(!quality)continue;
   // Recreate only the test chain to exercise multisampled native backbuffers.
   renderer.clear();context->ClearState();swap.Reset();d.BufferDesc.Width=size.first;d.BufferDesc.Height=size.second;d.SampleDesc.Count=sample;d.SampleDesc.Quality=quality-1;
   ComPtr<IDXGIDevice> dx;ComPtr<IDXGIAdapter> adapter;ComPtr<IDXGIFactory> factory;hr(device.As(&dx),"DXGI device");hr(dx->GetAdapter(&adapter),"DXGI adapter");hr(adapter->GetParent(__uuidof(IDXGIFactory),reinterpret_cast<void**>(factory.GetAddressOf())),"DXGI factory");hr(factory->CreateSwapChain(device.Get(),&d,&swap),"DX11 multisample chain");
   ComPtr<ID3D11Texture2D> back;ComPtr<ID3D11RenderTargetView> target;hr(swap->GetBuffer(0,__uuidof(ID3D11Texture2D),reinterpret_cast<void**>(back.GetAddressOf())),"DX11 backbuffer");hr(device->CreateRenderTargetView(back.Get(),nullptr,&target),"DX11 fixture RTV");float blue[]={0,0,1,1};context->ClearRenderTargetView(target.Get(),blue);
   D3D11_VIEWPORT viewport{11,13,100,110,.2f,.8f};context->RSSetViewports(1,&viewport);context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);auto rt=target.Get();context->OMSetRenderTargets(1,&rt,nullptr);
   D3D11_RASTERIZER_DESC rd{};rd.FillMode=D3D11_FILL_WIREFRAME;rd.CullMode=D3D11_CULL_FRONT;rd.ScissorEnable=TRUE;ComPtr<ID3D11RasterizerState> raster;hr(device->CreateRasterizerState(&rd,&raster),"DX11 fixture raster");context->RSSetState(raster.Get());
   check(renderer.begin(swap.Get()),"DX11 compositor initialization");auto s=image(size.first,size.second);LONG uploads=stats.uploads;
   check(renderer.draw(s,1),"DX11 compositor draw");check(stats.uploads==uploads+1,"DX11 upload count");
   D3D11_VIEWPORT restored{};UINT count=1;context->RSGetViewports(&count,&restored);D3D11_PRIMITIVE_TOPOLOGY topology{};context->IAGetPrimitiveTopology(&topology);ComPtr<ID3D11RenderTargetView> restored_rt;context->OMGetRenderTargets(1,&restored_rt,nullptr);ComPtr<ID3D11RasterizerState> restored_raster;context->RSGetState(&restored_raster);
   check(count==1&&!memcmp(&viewport,&restored,sizeof viewport)&&topology==D3D11_PRIMITIVE_TOPOLOGY_LINELIST&&restored_rt.Get()==target.Get()&&restored_raster.Get()==raster.Get(),"DX11 native pipeline restoration");
   ComPtr<ID3D11Texture2D> resolved,staging;D3D11_TEXTURE2D_DESC td{};back->GetDesc(&td);td.SampleDesc={1,0};td.BindFlags=0;td.Usage=D3D11_USAGE_DEFAULT;
   hr(device->CreateTexture2D(&td,nullptr,&resolved),"DX11 resolve texture");if(sample>1)context->ResolveSubresource(resolved.Get(),0,back.Get(),0,td.Format);else context->CopyResource(resolved.Get(),back.Get());td.Usage=D3D11_USAGE_STAGING;td.CPUAccessFlags=D3D11_CPU_ACCESS_READ;hr(device->CreateTexture2D(&td,nullptr,&staging),"DX11 readback texture");context->CopyResource(staging.Get(),resolved.Get());
   D3D11_MAPPED_SUBRESOURCE mapped{};hr(context->Map(staging.Get(),0,D3D11_MAP_READ,0,&mapped),"DX11 readback map");auto bytes=static_cast<unsigned char*>(mapped.pData);pixel(*reinterpret_cast<unsigned*>(bytes+(size.second/2)*mapped.RowPitch+(size.first/2)*4));check(*reinterpret_cast<unsigned*>(bytes+20*mapped.RowPitch+20*4)==0xff0000ff,"DX11 untouched outside HUD");context->Unmap(staging.Get(),0);
   for(unsigned i=0;i<300;++i)check(renderer.draw(s,1),"DX11 sustained draw");check(stats.uploads==uploads+1,"DX11 repeated image reuploaded");++s.frame.number;check(renderer.draw(s,1)&&stats.uploads==uploads+2,"DX11 new image upload");
   s.frame.rect_count=2;s.frame.rect[0]={0,0,int(size.first),30};s.frame.rect[1]={0,int(size.second)-30,int(size.first),30};check(renderer.draw(s,3),"DX11 death lids");
   renderer.clear();context->ClearState();++cases;
  }
 }
 std::cout<<"DX11: "<<cases<<" real 720p/1080p/2160p sample1/4/8 cases, pixels, alpha, pipeline restoration, upload reuse, sustained draws, death rectangles and resize lifetime.\n";
}
static void nine(HWND window){
 ComPtr<IDirect3D9> api;api.Attach(Direct3DCreate9(D3D_SDK_VERSION));check(bool(api),"DX9 API");D3DPRESENT_PARAMETERS p{};p.Windowed=TRUE;p.SwapEffect=D3DSWAPEFFECT_DISCARD;p.hDeviceWindow=window;p.BackBufferWidth=1280;p.BackBufferHeight=720;p.BackBufferFormat=D3DFMT_A8R8G8B8;
 ComPtr<IDirect3DDevice9> device;hr(api->CreateDevice(0,D3DDEVTYPE_HAL,window,D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_FPU_PRESERVE|D3DCREATE_MULTITHREADED,&p,&device),"DX9 private device");Renderer9 renderer;unsigned cases=0;
 for(auto size:{std::pair<UINT,UINT>{1280,720},{1920,1080},{3840,2160}})for(auto sample:{D3DMULTISAMPLE_NONE,D3DMULTISAMPLE_4_SAMPLES,D3DMULTISAMPLE_8_SAMPLES}){
  DWORD quality{};if(FAILED(api->CheckDeviceMultiSampleType(0,D3DDEVTYPE_HAL,p.BackBufferFormat,TRUE,sample,&quality)))continue;
  renderer.clear();p.BackBufferWidth=size.first;p.BackBufferHeight=size.second;p.MultiSampleType=sample;p.MultiSampleQuality=quality?quality-1:0;hr(device->Reset(&p),"DX9 default-pool release/reset");
  hr(device->Clear(0,nullptr,D3DCLEAR_TARGET,0xff0000ff,1,0),"DX9 background");D3DVIEWPORT9 viewport{11,13,100,110,.2f,.8f};hr(device->SetViewport(&viewport),"DX9 fixture viewport");device->SetRenderState(D3DRS_ZENABLE,TRUE);device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);device->SetRenderState(D3DRS_SCISSORTESTENABLE,TRUE);device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
  ComPtr<IDirect3DSurface9> before;hr(device->GetRenderTarget(0,&before),"DX9 fixture target");auto s=image(size.first,size.second);LONG uploads=stats.uploads;check(renderer.draw(device.Get(),s,1),"DX9 compositor draw");
  D3DVIEWPORT9 restored{};DWORD z{},cull{},scissor{},fvf{};device->GetViewport(&restored);device->GetRenderState(D3DRS_ZENABLE,&z);device->GetRenderState(D3DRS_CULLMODE,&cull);device->GetRenderState(D3DRS_SCISSORTESTENABLE,&scissor);device->GetFVF(&fvf);ComPtr<IDirect3DSurface9> after;device->GetRenderTarget(0,&after);
  check(!memcmp(&viewport,&restored,sizeof viewport)&&z==TRUE&&cull==D3DCULL_CW&&scissor==TRUE&&fvf==(D3DFVF_XYZ|D3DFVF_DIFFUSE)&&before.Get()==after.Get(),"DX9 native state/target restoration");
  ComPtr<IDirect3DSurface9> resolve,system;hr(device->CreateRenderTarget(size.first,size.second,p.BackBufferFormat,D3DMULTISAMPLE_NONE,0,FALSE,&resolve,nullptr),"DX9 resolve target");hr(device->StretchRect(before.Get(),nullptr,resolve.Get(),nullptr,D3DTEXF_NONE),"DX9 resolve");hr(device->CreateOffscreenPlainSurface(size.first,size.second,p.BackBufferFormat,D3DPOOL_SYSTEMMEM,&system,nullptr),"DX9 readback target");hr(device->GetRenderTargetData(resolve.Get(),system.Get()),"DX9 readback");D3DLOCKED_RECT mapped{};hr(system->LockRect(&mapped,nullptr,D3DLOCK_READONLY),"DX9 readback map");auto bytes=static_cast<unsigned char*>(mapped.pBits);pixel(*reinterpret_cast<unsigned*>(bytes+(size.second/2)*mapped.Pitch+(size.first/2)*4));check(*reinterpret_cast<unsigned*>(bytes+20*mapped.Pitch+20*4)==0xff0000ff,"DX9 untouched outside HUD");system->UnlockRect();
  for(unsigned i=0;i<300;++i)check(renderer.draw(device.Get(),s,1),"DX9 sustained draw");check(stats.uploads==uploads+1,"DX9 repeated image reuploaded");++s.frame.number;check(renderer.draw(device.Get(),s,1)&&stats.uploads==uploads+2,"DX9 new image upload");++cases;
 }
 renderer.clear();std::cout<<"DX9: "<<cases<<" real resolution/MSAA cases, pixels/alpha, state/target restoration, sustained draws and reset lifetime.\n";
}
static unsigned forwarded;static UINT last_sync,last_flags;
static HRESULT STDMETHODCALLTYPE fake_present(IDXGISwapChain*,UINT sync,UINT flags){++forwarded;last_sync=sync;last_flags=flags;return E_FAIL;}
static bool accept(const Frame&,unsigned){return true;}
static void interception(HWND window){
 check(MH_Initialize()==MH_OK,"Graphics hooks initialize");install(&accept);check(stats.initialized==3,"Both DirectX hook pairs available");unsigned char before[4][16];for(unsigned i=0;i<4;++i)memcpy(before[i],hooks[i],16);check(MH_EnableHook(MH_ALL_HOOKS)==MH_OK,"Graphics hooks enable");
 ComPtr<IDXGISwapChain> swap;ComPtr<ID3D11Device> device;DXGI_SWAP_CHAIN_DESC d{};d.BufferDesc.Width=1280;d.BufferDesc.Height=720;d.BufferDesc.Format=DXGI_FORMAT_B8G8R8A8_UNORM;d.SampleDesc.Count=1;d.BufferCount=1;d.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;d.OutputWindow=window;d.Windowed=TRUE;hr(D3D11CreateDeviceAndSwapChain(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,0,nullptr,0,D3D11_SDK_VERSION,&d,&swap,&device,nullptr,nullptr),"Hook fixture swapchain");
 overlay106::Publisher publisher;check(publisher.open(GetCurrentProcessId(),1),"Hook fixture channel");auto s=image(1280,720);publisher.publish(s.frame,s.pixels.data());LONG presents=stats.presents11;HRESULT result=swap->Present(0,0);check(stats.presents11==presents+1,"Actual Present hook not reached");overlay106::Ack ack;if(result==S_OK)check(publisher.ack(ack)&&ack.backend==11&&ack.number,"Successful Present did not acknowledge drawn image");else check(!publisher.ack(ack),"Failed/occluded Present hid fallback");
 auto native=original11;original11=&fake_present;auto count=stats.presents11;check(present11(swap.Get(),2,DXGI_PRESENT_TEST)==E_FAIL&&forwarded==1&&last_sync==2&&last_flags==DXGI_PRESENT_TEST&&stats.presents11==count,"TEST Present arguments/one forwarding");check(present11(swap.Get(),0,0)==E_FAIL&&forwarded==2&&!publisher.ack(ack),"Failed Present acknowledged pixels");original11=native;
 auto resized=stats.resizes;hr(swap->ResizeBuffers(1,1920,1080,d.BufferDesc.Format,0),"Hook resize");check(stats.resizes==resized+1,"Actual DXGI resize hook did not release retained target");
 ComPtr<IDirect3D9> api;api.Attach(Direct3DCreate9(D3D_SDK_VERSION));ComPtr<IDirect3DDevice9> nine;D3DPRESENT_PARAMETERS p{};p.Windowed=TRUE;p.SwapEffect=D3DSWAPEFFECT_DISCARD;p.hDeviceWindow=window;p.BackBufferWidth=1280;p.BackBufferHeight=720;p.BackBufferFormat=D3DFMT_A8R8G8B8;
 hr(api->CreateDevice(0,D3DDEVTYPE_HAL,window,D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED|D3DCREATE_FPU_PRESERVE,&p,&nine),"DX9 hook fixture");auto presents9=stats.presents9;nine->Present(nullptr,nullptr,nullptr,nullptr);check(stats.presents9==presents9+1,"Actual DX9 Present hook not reached");resized=stats.resizes;p.BackBufferWidth=1920;p.BackBufferHeight=1080;hr(nine->Reset(&p),"Actual DX9 hooked Reset");check(stats.resizes==resized+1,"DX9 Reset did not release default-pool resources");
 stop();for(unsigned i=0;i<4;++i)check(!memcmp(before[i],hooks[i],16),"Graphics hook bytes not restored");check(MH_Uninitialize()==MH_OK,"Graphics hooks teardown");
 std::cout<<"Hooks: actual DX11 Present/resize and DX9 Present/Reset, both pair creation/exact restoration, TEST/failure forwarding and acknowledgement. No game or display-mode change.\n";
}
int main(){HWND window=nullptr;try{WNDCLASSW wc{};wc.hInstance=GetModuleHandleW(nullptr);wc.lpszClassName=L"SHOGUN 2";wc.lpfnWndProc=DefWindowProcW;check(RegisterClassW(&wc)!=0,"Private fixture window class");window=CreateWindowExW(0,wc.lpszClassName,L"Private DirectX regression",WS_POPUP,-30000,-30000,1280,720,nullptr,nullptr,wc.hInstance,nullptr);check(window!=nullptr,"Private fixture window");eleven(window);nine(window);interception(window);DestroyWindow(window);std::cout<<"PASS: DirectX106 native compositor offline checks.\n";return 0;}catch(const std::exception& e){if(window)DestroyWindow(window);std::cerr<<e.what()<<'\n';return 1;}}
