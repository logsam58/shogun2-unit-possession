#pragma once
#define HUD80_QTE_ONLY
#include "../src/hud_renderer.h"
namespace qte {
inline std::unique_ptr<hud80::Assets> renderer_assets;
inline bool renderer_failed=false;
inline void release_renderer(){renderer_assets.reset();renderer_failed=false;}
inline void render(Gdiplus::Graphics& g,float x,float y,const Snapshot& s){
    if(!s.active||renderer_failed)return;
    if(!renderer_assets){
        HMODULE owner=nullptr;
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&render),&owner)){renderer_failed=true;return;}
        auto assets=std::make_unique<hud80::Assets>();
        if(!assets->load(owner)){renderer_failed=true;return;}renderer_assets=std::move(assets);
    }
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    hud80::qte_meter(g,*renderer_assets,x,y,s);
}
}
