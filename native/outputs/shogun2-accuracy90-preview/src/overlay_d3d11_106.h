#pragma once
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <cstring>
#include <utility>
#include "../../shogun2-possession-cleanup-v46/src/overlay_surface106.h"
namespace nativeoverlay106 {
using Microsoft::WRL::ComPtr;
using overlay106::Snapshot;using overlay106::Frame;using overlay106::Key;
struct Stats {volatile LONG initialized{},error{},presents11{},presents9{},draws{},uploads{},resizes{},width{},height{},fullscreen{};};
static Stats stats;
inline bool ok(HRESULT h){if(FAILED(h)){InterlockedExchange(&stats.error,LONG(h));return false;}return true;}
struct Vertex11 {float x,y,u,v;};
class Renderer11 {
 IDXGISwapChain* chain_{};ComPtr<ID3D11Device> device_;ComPtr<ID3D11DeviceContext1> context_;
 ComPtr<ID3DDeviceContextState> own_;ComPtr<ID3D11RenderTargetView> target_;
 ComPtr<ID3D11VertexShader> vs_;ComPtr<ID3D11PixelShader> ps_;ComPtr<ID3D11InputLayout> layout_;
 ComPtr<ID3D11Buffer> vertices_;ComPtr<ID3D11SamplerState> sampler_;ComPtr<ID3D11BlendState> blend_;
 ComPtr<ID3D11RasterizerState> raster_;ComPtr<ID3D11DepthStencilState> depth_;
 struct Texture {ComPtr<ID3D11Texture2D> texture;ComPtr<ID3D11ShaderResourceView> view;unsigned w{},h{};uint64_t number{};Key key{};} textures_[overlay106::Wheel];
 unsigned width_{},height_{};
 bool bind_target(IDXGISwapChain* chain){
  ComPtr<ID3D11Texture2D> back;
  if(!ok(chain->GetBuffer(0,__uuidof(ID3D11Texture2D),reinterpret_cast<void**>(back.GetAddressOf()))))return false;
  D3D11_TEXTURE2D_DESC desc{};back->GetDesc(&desc);width_=desc.Width;height_=desc.Height;
  return ok(device_->CreateRenderTargetView(back.Get(),nullptr,&target_));
 }
public:
 using Compiler=decltype(&D3DCompile);
 bool owns(IDXGISwapChain* chain)const{return chain_==chain;}
 unsigned width()const{return width_;}unsigned height()const{return height_;}
 void clear(){for(auto& t:textures_)t={};own_.Reset();target_.Reset();vs_.Reset();ps_.Reset();layout_.Reset();vertices_.Reset();sampler_.Reset();blend_.Reset();raster_.Reset();depth_.Reset();context_.Reset();device_.Reset();chain_=nullptr;width_=height_=0;}
 bool begin(IDXGISwapChain* chain,Compiler compile=D3DCompile){
  if(!chain||!compile)return false;
  if(chain_==chain){
   if(target_)return true;
   ComPtr<ID3D11Device> current;
   if(SUCCEEDED(chain->GetDevice(__uuidof(ID3D11Device),reinterpret_cast<void**>(current.GetAddressOf())))&&
      current.Get()==device_.Get())return bind_target(chain);
  }
  clear();
  if(!ok(chain->GetDevice(__uuidof(ID3D11Device),reinterpret_cast<void**>(device_.GetAddressOf()))))return false;
  ComPtr<ID3D11Device1> one;ComPtr<ID3D11DeviceContext> c;device_->GetImmediateContext(&c);
  if(!ok(device_.As(&one))||!ok(c.As(&context_)))return false;
  auto level=device_->GetFeatureLevel();unsigned flags=(device_->GetCreationFlags()&D3D11_CREATE_DEVICE_SINGLETHREADED)?D3D11_1_CREATE_DEVICE_CONTEXT_STATE_SINGLETHREADED:0;
  if(!ok(one->CreateDeviceContextState(flags,&level,1,D3D11_SDK_VERSION,__uuidof(ID3D11Device),nullptr,&own_)))return false;
  if(!bind_target(chain))return false;
  static const char code[]= "Texture2D tex:register(t0);SamplerState samp:register(s0);struct V{float4 p:SV_POSITION;float2 uv:TEXCOORD0;};V vs(float2 p:POSITION,float2 uv:TEXCOORD0){V o;o.p=float4(p,0,1);o.uv=uv;return o;}float4 ps(V v):SV_TARGET{return tex.Sample(samp,v.uv);}";
  ComPtr<ID3DBlob> v,p,e;
  if(!ok(compile(code,sizeof(code)-1,nullptr,nullptr,nullptr,"vs","vs_4_0",D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&v,&e))||
     !ok(compile(code,sizeof(code)-1,nullptr,nullptr,nullptr,"ps","ps_4_0",D3DCOMPILE_OPTIMIZATION_LEVEL3,0,&p,&e)))return false;
  if(!ok(device_->CreateVertexShader(v->GetBufferPointer(),v->GetBufferSize(),nullptr,&vs_))||!ok(device_->CreatePixelShader(p->GetBufferPointer(),p->GetBufferSize(),nullptr,&ps_)))return false;
  D3D11_INPUT_ELEMENT_DESC elements[]={{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,8,D3D11_INPUT_PER_VERTEX_DATA,0}};
  if(!ok(device_->CreateInputLayout(elements,2,v->GetBufferPointer(),v->GetBufferSize(),&layout_)))return false;
  D3D11_BUFFER_DESC vb{};vb.ByteWidth=sizeof(Vertex11)*6;vb.Usage=D3D11_USAGE_DYNAMIC;vb.BindFlags=D3D11_BIND_VERTEX_BUFFER;vb.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  D3D11_SAMPLER_DESC sd{};sd.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;sd.AddressU=sd.AddressV=sd.AddressW=D3D11_TEXTURE_ADDRESS_CLAMP;sd.MaxLOD=D3D11_FLOAT32_MAX;
  D3D11_BLEND_DESC bd{};auto& rt=bd.RenderTarget[0];rt.BlendEnable=TRUE;rt.SrcBlend=rt.SrcBlendAlpha=D3D11_BLEND_ONE;rt.DestBlend=rt.DestBlendAlpha=D3D11_BLEND_INV_SRC_ALPHA;rt.BlendOp=rt.BlendOpAlpha=D3D11_BLEND_OP_ADD;rt.RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
  D3D11_RASTERIZER_DESC rd{};rd.FillMode=D3D11_FILL_SOLID;rd.CullMode=D3D11_CULL_NONE;rd.DepthClipEnable=TRUE;rd.MultisampleEnable=TRUE;
  D3D11_DEPTH_STENCIL_DESC dd{};dd.DepthFunc=D3D11_COMPARISON_ALWAYS;
  if(!ok(device_->CreateBuffer(&vb,nullptr,&vertices_))||!ok(device_->CreateSamplerState(&sd,&sampler_))||!ok(device_->CreateBlendState(&bd,&blend_))||!ok(device_->CreateRasterizerState(&rd,&raster_))||!ok(device_->CreateDepthStencilState(&dd,&depth_)))return false;
  chain_=chain;return true;
 }
 bool draw(const Snapshot& s,unsigned layer,bool transient_target=false){
  if(!target_||layer<1||layer>overlay106::Wheel)return false;auto& t=textures_[layer-1];auto& f=s.frame;
  ComPtr<ID3DDeviceContextState> previous;context_->SwapDeviceContextState(own_.Get(),&previous);
  struct Restore {
   ID3D11DeviceContext1* context;ID3DDeviceContextState* state;bool transient;
   ~Restore(){
    if(transient)context->OMSetRenderTargets(0,nullptr,nullptr);
    context->SwapDeviceContextState(state,nullptr);
   }
  } restore{context_.Get(),previous.Get(),transient_target};
  if(t.w!=f.width||t.h!=f.height){t={};D3D11_TEXTURE2D_DESC d{};d.Width=f.width;d.Height=f.height;d.MipLevels=d.ArraySize=1;d.Format=DXGI_FORMAT_B8G8R8A8_UNORM;d.SampleDesc.Count=1;d.Usage=D3D11_USAGE_DEFAULT;d.BindFlags=D3D11_BIND_SHADER_RESOURCE;
   if(!ok(device_->CreateTexture2D(&d,nullptr,&t.texture))||!ok(device_->CreateShaderResourceView(t.texture.Get(),nullptr,&t.view)))return false;t.w=f.width;t.h=f.height;}
  if(t.number!=f.number||!overlay106::same(t.key,f.key)){context_->UpdateSubresource(t.texture.Get(),0,nullptr,s.pixels.data(),f.width*4,0);t.number=f.number;t.key=f.key;InterlockedIncrement(&stats.uploads);}
  D3D11_VIEWPORT viewport{0,0,float(width_),float(height_),0,1};context_->RSSetViewports(1,&viewport);context_->RSSetState(raster_.Get());
  auto target=target_.Get();context_->OMSetRenderTargets(1,&target,nullptr);context_->OMSetDepthStencilState(depth_.Get(),0);context_->OMSetBlendState(blend_.Get(),nullptr,0xffffffff);
  context_->SetPredication(nullptr,FALSE);context_->IASetInputLayout(layout_.Get());context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  UINT stride=sizeof(Vertex11),offset=0;auto buffer=vertices_.Get();context_->IASetVertexBuffers(0,1,&buffer,&stride,&offset);
  context_->VSSetShader(vs_.Get(),nullptr,0);context_->PSSetShader(ps_.Get(),nullptr,0);auto sampler=sampler_.Get();auto view=t.view.Get();context_->PSSetSamplers(0,1,&sampler);context_->PSSetShaderResources(0,1,&view);
  for(unsigned i=0;i<f.rect_count;++i){auto q=f.rect[i];float l=2.f*q.x/f.view_width-1,r=2.f*(q.x+q.width)/f.view_width-1,top=1-2.f*q.y/f.view_height,b=1-2.f*(q.y+q.height)/f.view_height;
   Vertex11 v[6]={{l,top,0,0},{r,top,1,0},{l,b,0,1},{l,b,0,1},{r,top,1,0},{r,b,1,1}};D3D11_MAPPED_SUBRESOURCE mapped{};
   if(!ok(context_->Map(vertices_.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mapped)))return false;memcpy(mapped.pData,v,sizeof v);context_->Unmap(vertices_.Get(),0);context_->Draw(6,0);}
  InterlockedIncrement(&stats.draws);return true;
 }
 // Frontend rendering borrows the current back buffer and releases it before
 // returning to the game. This lets native ResizeBuffers run without a hook.
 bool draw_frame(IDXGISwapChain* chain,Compiler compile,const Snapshot& sprite){
  const bool success=begin(chain,compile)&&draw(sprite,overlay106::HUD,true);
  target_.Reset();
  return success;
 }
};
}
