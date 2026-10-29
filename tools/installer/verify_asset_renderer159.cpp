#ifdef HUD80_QTE_ONLY
#include "../../native/outputs/shogun2-authentic-hud80-preview/src/hud_renderer.h"
#else
#include "../../native/outputs/shogun2-ammunition83-preview/hud-src/hud_renderer.h"
#endif
#include <iostream>
#include <stdexcept>
int main(){ULONG_PTR token{};Gdiplus::GdiplusStartupInput input;try{
    if(Gdiplus::GdiplusStartup(&token,&input,nullptr)!=Gdiplus::Ok)throw std::runtime_error("GDI+ unavailable");
    {hud80::Assets assets;if(!assets.load(nullptr))throw std::runtime_error("Production asset/font load failed");
    for(auto& sprite:hud80::sprites){auto b=assets159::resource(nullptr,101+sprite.atlas);
        if(sprite.x<0||sprite.y<0||sprite.x+sprite.w>int(b.width)||sprite.y+sprite.h>int(b.height))throw std::runtime_error("Sprite outside local atlas");
        auto tile=assets.logical_tile(sprite.key);if(!tile||tile->GetLastStatus()!=Gdiplus::Ok)throw std::runtime_error("Production sprite draw failed");
    }
    Gdiplus::Bitmap canvas(640,240,PixelFormat32bppARGB);Gdiplus::Graphics graphics(&canvas);
    assets.image(graphics,hud80::sprites[0].key,Gdiplus::RectF(0,0,100,100));
#ifndef HUD80_QTE_ONLY
    for(auto size:{std::pair<int,int>{1280,720},{1920,1080}}){
        hud80::View stock{};stock.unit=hud80::unit_art(L"Inf_Missile_Bow_Ashigaru");
        stock.owner=hud80::faction_art(L"oda");stock.native_unit_key=L"Inf_Missile_Bow_Ashigaru";
        stock.men=stock.initial=120;stock.ranged=true;
        auto custom=stock;custom.unit=nullptr;custom.native_unit_key=L"Armored_Addon_Unit_169";
        Gdiplus::Bitmap normal(size.first,size.second,PixelFormat32bppARGB),addon(size.first,size.second,PixelFormat32bppARGB);
        {Gdiplus::Graphics g(&normal);g.Clear(Gdiplus::Color(0,0,0,0));hud80::render(g,assets,float(size.first),float(size.second),stock);}
        {Gdiplus::Graphics g(&addon);g.Clear(Gdiplus::Color(0,0,0,0));hud80::render(g,assets,float(size.first),float(size.second),custom);}
        Gdiplus::Rect area(0,0,size.first,size.second);Gdiplus::BitmapData a{},b{};
        if(normal.LockBits(&area,Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&a)!=Gdiplus::Ok)throw std::runtime_error("Stock rendered HUD unavailable");
        if(addon.LockBits(&area,Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&b)!=Gdiplus::Ok){normal.UnlockBits(&a);throw std::runtime_error("Custom rendered HUD unavailable");}
        unsigned visible=0;bool same=true;
        for(int y=0;y<size.second;++y){
            auto p=reinterpret_cast<const unsigned*>(static_cast<const char*>(a.Scan0)+y*a.Stride);
            auto q=reinterpret_cast<const unsigned*>(static_cast<const char*>(b.Scan0)+y*b.Stride);
            for(int x=size.first*2/3;x<size.first;++x){same&=p[x]==q[x];visible+=(p[x]>>24)>16;}
        }
        normal.UnlockBits(&a);addon.UnlockBits(&b);
        if(!same||visible<100)throw std::runtime_error("Custom artwork lookup removed or changed the visible command HUD");
    }
#endif
    assets.clear();}
    Gdiplus::GdiplusShutdown(token);std::cout<<"PASS: production external atlas, every sprite, native fonts and drawing. No window/game/audio started.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';if(token)Gdiplus::GdiplusShutdown(token);return 1;}}
