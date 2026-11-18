// Synthetic WARP swap chain. No Steam, game process, hooks or pointer input.
#include "../../native/deck/remote_cursor152.h"
#include <cassert>
#include <iostream>

namespace {
using Microsoft::WRL::ComPtr;

struct Graphics {
    HWND window{};
    ComPtr<IDXGISwapChain> chain;
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    Graphics() {
        window=CreateWindowExW(WS_EX_TOOLWINDOW,L"STATIC",L"",WS_POPUP,0,0,1280,800,nullptr,nullptr,
                               GetModuleHandleW(nullptr),nullptr);
        assert(window&&!IsWindowVisible(window));
        DXGI_SWAP_CHAIN_DESC desc{};
        desc.BufferDesc.Width=1280;desc.BufferDesc.Height=800;
        desc.BufferDesc.Format=DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count=1;desc.BufferCount=1;
        desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.OutputWindow=window;desc.Windowed=TRUE;
        assert(SUCCEEDED(D3D11CreateDeviceAndSwapChain(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,nullptr,0,D3D11_SDK_VERSION,&desc,&chain,&device,nullptr,&context)));
    }
    ~Graphics(){context.Reset();device.Reset();chain.Reset();DestroyWindow(window);}

    ComPtr<ID3D11Texture2D> backbuffer() {
        ComPtr<ID3D11Texture2D> back;
        assert(SUCCEEDED(chain->GetBuffer(0,IID_PPV_ARGS(&back))));
        return back;
    }
    void clear() {
        ComPtr<ID3D11RenderTargetView> view;
        assert(SUCCEEDED(device->CreateRenderTargetView(backbuffer().Get(),nullptr,&view)));
        const float black[]{0,0,0,1};
        context->ClearRenderTargetView(view.Get(),black);
    }
    bool painted(const overlay106::Rect& area) {
        auto back=backbuffer();
        D3D11_TEXTURE2D_DESC desc{};
        back->GetDesc(&desc);
        desc.Usage=D3D11_USAGE_STAGING;desc.BindFlags=0;
        desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;desc.MiscFlags=0;
        ComPtr<ID3D11Texture2D> read;
        assert(SUCCEEDED(device->CreateTexture2D(&desc,nullptr,&read)));
        context->CopyResource(read.Get(),back.Get());
        D3D11_MAPPED_SUBRESOURCE mapped{};
        assert(SUCCEEDED(context->Map(read.Get(),0,D3D11_MAP_READ,0,&mapped)));
        bool found=false;
        for(unsigned y=0;y<desc.Height;++y){
            auto row=reinterpret_cast<const uint32_t*>(static_cast<const unsigned char*>(mapped.pData)+mapped.RowPitch*y);
            for(unsigned x=0;x<desc.Width;++x){
                if(!(row[x]&0x00ffffffu))continue;
                assert(int(x)>=area.x&&int(x)<area.x+area.width&&int(y)>=area.y&&int(y)<area.y+area.height);
                found=true;
            }
        }
        context->Unmap(read.Get(),0);
        return found;
    }
};
}

int main() {
    for(unsigned mask=0;mask<32;++mask){
        const bool expected=mask==27;
        assert(streamcursor152::scope(mask&1,mask&2,mask&4,mask&8,mask&16)==expected);
    }
    assert(streamcursor152::present_signature[16]==0xc2&&streamcursor152::present_signature[17]==8);
    assert(streamcursor152::present_signature[51]==0xc2&&streamcursor152::present_signature[52]==8);
    const RECT old_client{0,0,1280,800},new_client{0,0,900,600};
    assert(streamcursor152::cursor_position({1279,799},old_client));
    assert(!streamcursor152::cursor_position({1280,799},old_client));
    assert(!streamcursor152::cursor_position({-1,30},old_client));
    assert(!streamcursor152::cursor_position({0,0},{0,0,200,200}));

    Graphics graphics;
    nativeoverlay106::Renderer11 renderer;
    streamcursor152::Icon icon;
    auto arrow=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512)),hand=LoadCursorW(nullptr,MAKEINTRESOURCEW(32649));
    assert(arrow&&hand&&icon.load(arrow));
    const auto image=icon.sprite.frame.number;
    assert(image&&icon.load(arrow)&&icon.sprite.frame.number==image);
    graphics.clear();
    assert(!renderer.draw_frame(graphics.chain.Get(),nullptr,icon.place({224,310},old_client)));
    assert(!graphics.painted({0,0,1280,800}));

    for(POINT at: {POINT{224,310},POINT{721,446},POINT{1080,515}}){
        graphics.clear();
        const auto& frame=icon.place(at,old_client);
        assert(renderer.draw_frame(graphics.chain.Get(),D3DCompile,frame));
        assert(graphics.painted(frame.frame.rect[0]));
        ComPtr<ID3D11RenderTargetView> live_target;
        graphics.context->OMGetRenderTargets(1,&live_target,nullptr);
        assert(!live_target); // Original game state after our draw.
    }
    assert(icon.sprite.frame.number==image); // Movement reuses the same texture.
    assert(icon.load(hand)&&icon.sprite.frame.number==image+1);
    assert(SUCCEEDED(graphics.chain->ResizeBuffers(1,900,600,DXGI_FORMAT_B8G8R8A8_UNORM,0)));
    graphics.clear();
    const auto& after_resize=icon.place({601,451},new_client);
    assert(renderer.draw_frame(graphics.chain.Get(),D3DCompile,after_resize));
    assert(graphics.painted(after_resize.frame.rect[0]));
    std::cout<<"PASS scoped stream state, native cursor and hotspot, D3D11 cursor pixels, cached texture and ResizeBuffers\n";
}

