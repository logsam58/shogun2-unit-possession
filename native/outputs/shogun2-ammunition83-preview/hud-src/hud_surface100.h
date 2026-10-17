#pragma once
#include <windows.h>
// Reuse the desktop/DIB surface at the existing HUD cadence. Native formation
// placement is rendered by the game. Dimensions change atomically.
class HudSurface100 {
    HDC screen_{},memory_{};HBITMAP bitmap_{};HGDIOBJ previous_{};void* pixels_{};int width_{},height_{};
public:
    HudSurface100()=default;HudSurface100(const HudSurface100&)=delete;HudSurface100& operator=(const HudSurface100&)=delete;
    ~HudSurface100(){reset();}
    void reset(){
        if(memory_&&previous_)SelectObject(memory_,previous_);
        if(bitmap_)DeleteObject(bitmap_);if(memory_)DeleteDC(memory_);if(screen_)ReleaseDC(nullptr,screen_);
        screen_=memory_=nullptr;bitmap_=nullptr;previous_=nullptr;pixels_=nullptr;width_=height_=0;
    }
    bool ensure(int width,int height){
        if(width<1||height<1||width>16384||height>16384)return false;
        if(bitmap_&&width==width_&&height==height_)return true;
        HDC screen=GetDC(nullptr),memory=screen?CreateCompatibleDC(screen):nullptr;
        BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=width;info.bmiHeader.biHeight=-height;
        info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;void* pixels=nullptr;
        HBITMAP bitmap=memory?CreateDIBSection(screen,&info,DIB_RGB_COLORS,&pixels,nullptr,0):nullptr;
        if(!screen||!memory||!bitmap||!pixels){if(bitmap)DeleteObject(bitmap);if(memory)DeleteDC(memory);if(screen)ReleaseDC(nullptr,screen);return false;}
        HGDIOBJ previous=SelectObject(memory,bitmap);
        if(!previous||previous==HGDI_ERROR){DeleteObject(bitmap);DeleteDC(memory);ReleaseDC(nullptr,screen);return false;}
        reset();screen_=screen;memory_=memory;bitmap_=bitmap;previous_=previous;pixels_=pixels;width_=width;height_=height;return true;
    }
    HDC screen()const{return screen_;}HDC memory()const{return memory_;}void* pixels()const{return pixels_;}
};
