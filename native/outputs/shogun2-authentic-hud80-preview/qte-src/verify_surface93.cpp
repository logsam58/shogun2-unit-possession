#include "playable_runtime.cpp"
int main(){try{
 DWORD before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);Surface93 cache;
 require(cache.ensure(272),"Initial production surface");auto bitmap=cache.bitmap;
 for(unsigned i=0;i<1000;++i){require(cache.ensure(272)&&cache.bitmap==bitmap,"Production renderer reallocates an unchanged surface");memset(cache.pixels,0,272*272*4);}
 for(unsigned i=0;i<400;++i)require(cache.ensure(i%2?232:320),"Viewport resize surface");cache.clear();
 require(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)<=before+1,"Production surface leaked GDI handles across resize/clear");
 require(!cache.ensure(0),"Invalid render target accepted");cache.clear();
 std::cout<<"PASS: actual styled production surface reused1000frames, resized400times and released GDI handles; invalid-size failure is bounded. No game or foreground window used.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
