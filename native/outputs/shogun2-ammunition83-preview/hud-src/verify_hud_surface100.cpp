#include "../../shogun2-possession-cleanup-v46/src/identity.h"
#include "hud_surface100.h"
int main(){try{
    unsigned before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS),peak=before;
    {HudSurface100 surface;require(surface.ensure(1280,720),"Initial HUD surface failed");auto first=surface.pixels();auto dc=surface.memory();
        for(unsigned i=0;i<1000;++i){require(surface.ensure(1280,720)&&surface.pixels()==first&&surface.memory()==dc,"HUD reallocates a reused frame");memset(surface.pixels(),int(i&255),1280u*720u*4u);}
        for(unsigned i=0;i<400;++i){int width=640+int(i%5)*16,height=360+int(i%7)*8;require(surface.ensure(width,height),"HUD resize failed");memset(surface.pixels(),0,size_t(width)*height*4);peak=std::max<DWORD>(peak,GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS));}
        require(!surface.ensure(0,720),"Invalid surface size accepted");require(surface.ensure(1280,720),"Valid resize after rejection failed");}
    unsigned after=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);require(peak<=before+8&&after<=before+2,"HUD resize/cleanup leaked GDI resources");
    std::cout<<"PASS: actual DIB/DC reuse1000 frames,400 resizes, invalid-size recovery and bounded GDI cleanup\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
