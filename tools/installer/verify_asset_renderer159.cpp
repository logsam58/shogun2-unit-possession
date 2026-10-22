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
    assets.image(graphics,hud80::sprites[0].key,Gdiplus::RectF(0,0,100,100));assets.clear();}
    Gdiplus::GdiplusShutdown(token);std::cout<<"PASS: production external atlas, every sprite, native fonts and drawing. No window/game/audio started.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';if(token)Gdiplus::GdiplusShutdown(token);return 1;}}
