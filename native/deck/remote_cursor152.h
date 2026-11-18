#pragma once
#include <windows.h>
#include <commctrl.h>
#include <array>
#include "cursor194.h"
#include "remote_display130.h"
#include "../outputs/shogun2-accuracy90-preview/src/overlay_d3d11_106.h"
#include "../outputs/shogun2-accuracy90-preview/vendor/include/MinHook.h"

namespace streamcursor152 {
using Present = unsigned(__thiscall*)(void*,uintptr_t,uintptr_t);
inline constexpr uintptr_t present_rva=0x1487180, dx11_table=0x17666f8;
inline constexpr unsigned present_slot=0xfc, chain_member=0xbc;
inline constexpr unsigned char present_signature[]{
    0x56,0x8b,0xf1,0x8b,0x96,0xbc,0x00,0x00,0x00,0x85,0xd2,0x75,0x06,0x32,0xc0,0x5e,0xc2,0x08,0x00,
    0x8b,0x02,0x6a,0x00,0x8b,0x48,0x20,0x33,0xc0,0x38,0x86,0xf1,0x1b,0x00,0x00,0x0f,0x95,0xc0,0x50,
    0x52,0xff,0xd1,0x50,0xe8,0x41,0x9d,0xf5,0xff,0x83,0xc4,0x04,0x5e,0xc2,0x08,0x00
};
inline bool scope(bool streamed,bool root,bool battle,bool focused,bool owned_thread) {
    return streamed&&root&&!battle&&focused&&owned_thread;
}
inline bool cursor_position(POINT at,const RECT& client) {
    return at.x>=0&&at.y>=0&&at.x<client.right&&at.y<client.bottom&&
        client.right>=640&&client.bottom>=360&&client.right<=16384&&client.bottom<=16384;
}

struct Icon {
    HCURSOR shape{};
    unsigned hot_x{},hot_y{};
    overlay106::Snapshot sprite;
    std::vector<uint32_t> pixels;

    bool load(HCURSOR next) {
        if(!next)return false;
        if(next==shape)return true;
        return deck194::raster_icon(next,pixels,[&](unsigned w,unsigned h,unsigned hx,unsigned hy,HDC,HDC){
            const auto* first=reinterpret_cast<const unsigned char*>(pixels.data());
            sprite.pixels.assign(first,first+pixels.size()*sizeof(uint32_t));
            auto& f=sprite.frame;
            f.width=w;f.height=h;f.key={1,0,0};++f.number;
            hot_x=hx;hot_y=hy;shape=next;
            return true;
        });
    }
    const overlay106::Snapshot& place(POINT at,const RECT& client) {
        auto& f=sprite.frame;
        f.view_width=unsigned(client.right);f.view_height=unsigned(client.bottom);f.rect_count=1;
        f.rect[0]={at.x-int(hot_x),at.y-int(hot_y),int(f.width),int(f.height)};
        return sprite;
    }
};

class Cursor {
    using SetSubclass=decltype(&SetWindowSubclass);
    using RemoveSubclass=decltype(&RemoveWindowSubclass);
    using DefSubclass=decltype(&DefSubclassProc);
    using GetDescription=HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*,DXGI_SWAP_CHAIN_DESC*);
    static constexpr UINT_PTR subclass_id=152;
    HWND window_{};
    DWORD owner_{};
    uintptr_t engine_{},backend_{};
    HCURSOR transparent_{},native_{};
    ULONGLONG last_draw_{},retry_draw_{};
    bool (*context_)(){};
    nativeoverlay106::Renderer11 renderer_;
    nativeoverlay106::Renderer11::Compiler compiler_{};
    SetSubclass set_subclass_{};
    RemoveSubclass remove_subclass_{};
    DefSubclass def_subclass_{};
    Icon icon_;

    bool on_owner()const {return owner_&&GetCurrentThreadId()==owner_;}
    bool current()const {
        return on_owner()&&window_&&GetForegroundWindow()==window_&&context_&&context_();
    }
    void restore() {
        last_draw_=0;
        if(on_owner()&&window_&&GetForegroundWindow()==window_&&transparent_&&
           GetCursor()==transparent_)SetCursor(native_);
    }
    void clear_window() {
        restore();
        if(window_&&on_owner())remove_subclass_(window_,&window_proc,subclass_id);
        window_=nullptr;backend_=0;native_=nullptr;owner_=0;retry_draw_=0;
        renderer_.clear();
    }
    static HCURSOR blank_cursor() {
        constexpr unsigned bytes=32*32/8;
        std::array<unsigned char,bytes> and_mask{};
        std::array<unsigned char,bytes> xor_mask{};
        and_mask.fill(255);
        return CreateCursor(GetModuleHandleW(nullptr),0,0,32,32,and_mask.data(),xor_mask.data());
    }
    bool apis() {
        if(transparent_)return true;
        static HMODULE controls=LoadLibraryExW(L"comctl32.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
        static HMODULE shaders=LoadLibraryExW(L"d3dcompiler_47.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32);
        if(!controls||!shaders)return false;
        set_subclass_=reinterpret_cast<SetSubclass>(GetProcAddress(controls,"SetWindowSubclass"));
        remove_subclass_=reinterpret_cast<RemoveSubclass>(GetProcAddress(controls,"RemoveWindowSubclass"));
        def_subclass_=reinterpret_cast<DefSubclass>(GetProcAddress(controls,"DefSubclassProc"));
        compiler_=reinterpret_cast<nativeoverlay106::Renderer11::Compiler>(GetProcAddress(shaders,"D3DCompile"));
        if(!set_subclass_||!remove_subclass_||!def_subclass_||!compiler_)return false;
        transparent_=blank_cursor();
        return transparent_!=nullptr;
    }
    static LRESULT CALLBACK window_proc(HWND window,UINT message,WPARAM w,LPARAM l,UINT_PTR, DWORD_PTR ref) {
        auto& cursor=*reinterpret_cast<Cursor*>(ref);
        if(message==WM_NCDESTROY){
            const auto next=cursor.def_subclass_;
            cursor.clear_window();
            return next(window,message,w,l);
        }
        const LRESULT result=cursor.def_subclass_(window,message,w,l);
        if(message==WM_SETCURSOR&&LOWORD(l)==HTCLIENT){
            if(!cursor.last_draw_||GetTickCount64()-cursor.last_draw_>=100||!cursor.current()){
                if(cursor.last_draw_)cursor.restore();
                return result;
            }
            CURSORINFO info{sizeof info};
            if(GetCursorInfo(&info)&&deck194::visible_shape(info)){
                if(info.hCursor!=cursor.transparent_)cursor.native_=info.hCursor;
                if(cursor.native_){SetCursor(cursor.transparent_);return TRUE;}
            }
        }
        return result;
    }

public:
    static Present original;
    static volatile LONG init_owner;
    static Cursor cursor;

    void pulse(HWND window,bool eligible,bool (*context)()) {
        if(window_&&on_owner()&&(!eligible||window_!=window))clear_window();
        if(!eligible||!window||GetWindowThreadProcessId(window,nullptr)!=GetCurrentThreadId()||
           GetForegroundWindow()!=window)return;
        if(window_&&!on_owner())return;
        if(window_==window&&last_draw_&&GetTickCount64()-last_draw_<100)return;

        const auto engine=reinterpret_cast<uintptr_t>(GetModuleHandleW(L"Empire.Retail.dll"));
        uintptr_t backend{},table{},slot{};
        HWND renderer_window{};
        if(!engine||!remote131::read(engine+0x1c9a938,renderer_window)||renderer_window!=window||
           !remote131::read(engine+0x1c9a93c,backend)||!remote131::read(backend,table)||table!=engine+dx11_table||
           !remote131::read(table+present_slot,slot)||slot!=engine+present_rva){
            restore();
            return;
        }
        if(!original){
            if(!remote131::matches(slot,present_signature)||!apis())return;
            auto hook_owner=reinterpret_cast<LPCWSTR>(&present);
            HMODULE pinned{};
            if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
                                   hook_owner,&pinned))return;
            const auto init=MH_Initialize();
            if(init!=MH_OK&&init!=MH_ERROR_ALREADY_INITIALIZED)return;
            if(MH_CreateHook(reinterpret_cast<void*>(slot),reinterpret_cast<void*>(&present),
                             reinterpret_cast<void**>(&original))!=MH_OK)return;
            if(MH_EnableHook(reinterpret_cast<void*>(slot))!=MH_OK){
                MH_RemoveHook(reinterpret_cast<void*>(slot));original=nullptr;return;
            }
            InterlockedExchange(&init_owner,LONG(GetCurrentThreadId()));
        }
        if(DWORD(init_owner)!=GetCurrentThreadId())return;
        if(!window_){
            if(!set_subclass_(window,&window_proc,subclass_id,reinterpret_cast<DWORD_PTR>(this)))return;
            owner_=GetCurrentThreadId();
            window_=window;
        }
        context_=context;engine_=engine;backend_=backend;
    }
    void close() {
        if(on_owner())clear_window();
    }

private:
    bool render(void* backend) {
        const auto now=GetTickCount64();
        if(now<retry_draw_||!current()||reinterpret_cast<uintptr_t>(backend)!=backend_)return false;
        uintptr_t selected{};
        if(!remote131::read(engine_+0x1c9a93c,selected)||selected!=backend_)return false;
        IDXGISwapChain* chain{};
        if(!remote131::read(backend_+chain_member,chain)||!chain)return false;
        auto describe=remote130::method<GetDescription>(chain,12);
        DXGI_SWAP_CHAIN_DESC desc{};
        if(!describe||FAILED(describe(chain,&desc))||desc.OutputWindow!=window_)return false;
        CURSORINFO info{sizeof info};
        if(!GetCursorInfo(&info)||!deck194::visible_shape(info))return false;
        POINT at=info.ptScreenPos;
        RECT client{};
        if(!ScreenToClient(window_,&at)||!GetClientRect(window_,&client)||!cursor_position(at,client))return false;
        if(info.hCursor!=transparent_)native_=info.hCursor;
        if(!icon_.load(native_))return false;
        if(renderer_.draw_frame(chain,compiler_,icon_.place(at,client)))return true;
        retry_draw_=now+1000;
        renderer_.clear();
        return false;
    }
    static unsigned __fastcall present(void* backend,void*,uintptr_t first,uintptr_t second) {
        if(DWORD(init_owner)!=GetCurrentThreadId()||!cursor.on_owner())
            return original(backend,first,second);
        bool drew=false;
        try {drew=cursor.render(backend);} catch(...) {}
        const unsigned result=original(backend,first,second);
        if(drew&&(result&255)&&cursor.current()){
            cursor.last_draw_=GetTickCount64();
            SetCursor(cursor.transparent_);
        }else if(cursor.last_draw_)cursor.restore();
        return result;
    }
};
inline Present Cursor::original{};
inline volatile LONG Cursor::init_owner{};
inline Cursor Cursor::cursor;
}

