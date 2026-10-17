#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <cwchar>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#ifdef HUD80_QTE_ONLY
#include "qte-sprites.generated.h"
#else
#include "sprites.generated.h"
#endif
#include "unit_art.generated.h"
#ifdef HUD80_QTE_ONLY
#include "../qte-src/qte_core.h"
#else
#include "../../shogun2-melee-meter79-preview/src/qte_core.h"
#endif
#include "native_font.h"
#include "hud_state89.h"
#include "accuracy_geometry90.h"
#include "../src/ammunition_protocol83.h"
namespace hud80 {
using namespace Gdiplus;
struct View {
    const UnitArt* unit{};
    const FactionArt* owner{};
    std::wstring faction=L"Faction unavailable",mon;
    unsigned men{},initial{},morale{},fatigue{};
    int veterancy=-1;
    bool ranged{},bow{},aiming{},routing{},following{},fixture{},boshin{};
    unsigned aim_pose{},weapon_phase{},kind{},equipment_error{};
    bool equipment_known{},has_ranged{},crouch_supported{},crouched{},pending{};
    unsigned slot{},styles{};std::wstring ammunition;
    bool ammunition_known{};ammo83::Status ammunition_state{};
    float ammo{},reload{},speed{};
    bool accuracy_known{};float cone_half_angle{},projection_y{};
    unsigned climb_mode{};
};
class Assets {
    std::vector<IStream*> streams;
    std::vector<std::unique_ptr<Bitmap>> atlases;
    mutable std::map<std::wstring,std::unique_ptr<Bitmap>> tiles;
public:
    bool load(HMODULE module){
        if(!native_fonts.load(module))return false;
        for(int i=0;i<atlas_count;++i){
            HRSRC r=FindResourceW(module,MAKEINTRESOURCEW(101+i),MAKEINTRESOURCEW(10));
            if(!r)return false;
            DWORD size=SizeofResource(module,r);void* bytes=LockResource(LoadResource(module,r));
            if(!bytes||!size)return false;
            HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,size);if(!memory)return false;
            void* copy=GlobalLock(memory);if(!copy){GlobalFree(memory);return false;}
            memcpy(copy,bytes,size);GlobalUnlock(memory);IStream* stream=nullptr;
            if(CreateStreamOnHGlobal(memory,TRUE,&stream)!=S_OK){GlobalFree(memory);return false;}
            streams.push_back(stream);auto bitmap=std::make_unique<Bitmap>(stream);
            if(bitmap->GetLastStatus()!=Ok)return false;
            atlases.push_back(std::move(bitmap));
        }return true;
    }
    void clear(){tiles.clear();atlases.clear();for(auto stream:streams)stream->Release();streams.clear();native_fonts.clear();}
    ~Assets(){clear();}
    Bitmap* logical_tile(const wchar_t* key)const{
        const Sprite* s=sprite(key);if(!s)return nullptr;
        auto found=tiles.find(key);
        if(found==tiles.end()){
            auto bitmap=std::make_unique<Bitmap>(s->logical_w,s->logical_h,PixelFormat32bppPARGB);
            {Graphics tile_graphics(bitmap.get());tile_graphics.Clear(Color(0,0,0,0));
                tile_graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
                tile_graphics.DrawImage(atlases[s->atlas].get(),Rect(s->offset_x,s->offset_y,s->w,s->h),s->x,s->y,s->w,s->h,UnitPixel);}
            found=tiles.emplace(key,std::move(bitmap)).first;
        }
        return found->second.get();
    }
    void image(Graphics& g,const wchar_t* key,RectF target,float opacity=1.f,bool isolated=false)const{
        const Sprite* s=sprite(key);if(!s)return;
        float sx=target.Width/s->logical_w,sy=target.Height/s->logical_h;
        RectF bounds(target.X+s->offset_x*sx,target.Y+s->offset_y*sy,s->w*sx,s->h*sy);
        ImageAttributes attributes;ColorMatrix alpha={1,0,0,0,0, 0,1,0,0,0, 0,0,1,0,0, 0,0,0,std::clamp(opacity,0.f,1.f),0, 0,0,0,0,1};
        attributes.SetColorMatrix(&alpha);
        if(isolated){auto bitmap=logical_tile(key);if(!bitmap)return;
            attributes.SetWrapMode(WrapModeTileFlipXY);
            g.DrawImage(bitmap,target,0.f,0.f,float(s->logical_w),float(s->logical_h),UnitPixel,&attributes);
        }else g.DrawImage(atlases[s->atlas].get(),bounds,float(s->x),float(s->y),float(s->w),float(s->h),UnitPixel,&attributes);
    }
    void tile(Graphics& g,const wchar_t* key,RectF bounds,float scale=1.f,WrapMode wrap=WrapModeTile)const{
        auto bitmap=logical_tile(key);if(!bitmap)return;
        TextureBrush texture(bitmap,wrap);texture.ScaleTransform(scale,scale);
        texture.TranslateTransform(bounds.X,bounds.Y,MatrixOrderAppend);g.FillRectangle(&texture,bounds);
    }

};
inline Color ink(){return Color(255,38,31,24);}
inline Color red(){return Color(255,135,36,29);}
inline Color muted(){return Color(255,91,79,61);}
inline void text(Graphics& g,const std::wstring& value,float x,float y,float w,float h,float size,Color color=ink(),bool bold=false){
    (void)bold;native_fonts.paint(g,value,RectF(x,y,w,h),size,color);
}
inline void round_rect(GraphicsPath& path,RectF r,float radius){float d=radius*2;
    path.AddArc(r.X,r.Y,d,d,180,90);path.AddArc(r.GetRight()-d,r.Y,d,d,270,90);
    path.AddArc(r.GetRight()-d,r.GetBottom()-d,d,d,0,90);path.AddArc(r.X,r.GetBottom()-d,d,d,90,90);path.CloseFigure();}
inline void key_image(Graphics& g,const wchar_t* label,RectF r){
    GraphicsPath shape;round_rect(shape,r,3);SolidBrush paper(Color(255,231,226,205));Pen rim(Color(255,95,80,57),1.1f);
    g.FillPath(&paper,&shape);g.DrawPath(&rim,&shape);Pen highlight(Color(255,255,251,233),1.f);
    g.DrawLine(&highlight,r.X+4,r.Y+3,r.GetRight()-4,r.Y+3);g.DrawLine(&rim,r.X+4,r.GetBottom()-3,r.GetRight()-4,r.GetBottom()-3);
    text(g,label,r.X+6,r.Y+2,r.Width-11,r.Height-4,14,ink());
}
inline void mouse_image(Graphics& g,RectF r,bool left,bool active=true){
    GraphicsPath shape;round_rect(shape,r,r.Width*.43f);SolidBrush paper(Color(255,228,223,201)),pressed(active?red():muted());
    g.FillPath(&paper,&shape);GraphicsState clip=g.Save();g.SetClip(&shape,CombineModeIntersect);
    g.FillRectangle(&pressed,left?r.X:r.X+r.Width*.5f,r.Y,r.Width*.5f,r.Height*.49f);g.Restore(clip);
    Pen line(Color(255,61,50,34),1.1f);g.DrawPath(&line,&shape);
    g.DrawLine(&line,r.X+r.Width*.5f,r.Y+2,r.X+r.Width*.5f,r.Y+r.Height*.49f);
    g.DrawLine(&line,r.X+1,r.Y+r.Height*.49f,r.GetRight()-1,r.Y+r.Height*.49f);
    SolidBrush wheel(Color(255,220,209,170));g.FillRectangle(&wheel,r.X+r.Width*.5f-1.5f,r.Y+r.Height*.14f,3.f,r.Height*.18f);
}
inline void panel(Graphics& g,const Assets& a,RectF r){
    // Keep every nine-slice join on one device-pixel grid. Corner and edge
    // thickness must share the same scale instead of rounding independently.
    Matrix transform;g.GetTransform(&transform);float elements[6];transform.GetElements(elements);
    float device=std::hypot(elements[0],elements[1]);if(!std::isfinite(device)||device<=0)return;
    auto snap=[&](float value){return std::round(value*device)/device;};
    float right=snap(r.GetRight()),bottom=snap(r.GetBottom());r.X=snap(r.X);r.Y=snap(r.Y);
    r.Width=right-r.X;r.Height=bottom-r.Y;
    const float c=snap(18.f),edge_scale=c/36.f,inset=snap(8.f);
    RectF inside(r.X+inset,r.Y+inset,r.Width-2*inset,r.Height-2*inset);
    SolidBrush paper(Color(248,225,219,195));g.FillRectangle(&paper,inside);
    a.tile(g,L"ui/frames/data__subpanel_paper_fill.png",inside);
    // Mirror only across the narrow axis: transparent outer padding must not
    // wrap back into the opaque inner edge and produce a detached hairline.
    a.tile(g,L"ui/frames/data__big_panel_top.png",RectF(r.X+c,r.Y,r.Width-2*c,c),edge_scale,WrapModeTileFlipY);
    a.tile(g,L"ui/frames/data__big_panel_bottom.png",RectF(r.X+c,r.GetBottom()-c,r.Width-2*c,c),edge_scale,WrapModeTileFlipY);
    a.tile(g,L"ui/frames/data__big_panel_left.png",RectF(r.X,r.Y+c,c,r.Height-2*c),edge_scale,WrapModeTileFlipX);
    a.tile(g,L"ui/frames/data__big_panel_right.png",RectF(r.GetRight()-c,r.Y+c,c,r.Height-2*c),edge_scale,WrapModeTileFlipX);
    // Isolate native logical sprites before bicubic scaling; atlas neighbours
    // cannot bleed through corner cutaways. Original offsets are preserved.
    a.image(g,L"ui/frames/data__big_panel_tl.png",RectF(r.X,r.Y,c,c),1.f,true);
    a.image(g,L"ui/frames/data__big_panel_tr.png",RectF(r.GetRight()-c,r.Y,c,c),1.f,true);
    a.image(g,L"ui/frames/data__big_panel_bl.png",RectF(r.X,r.GetBottom()-c,c,c),1.f,true);
    a.image(g,L"ui/frames/data__big_panel_br.png",RectF(r.GetRight()-c,r.GetBottom()-c,c,c),1.f,true);
}

inline const wchar_t* morale_name(unsigned n){static const wchar_t* names[]={L"Impetuous",L"Eager",L"Confident",L"Steady",L"Shaken",L"Wavering",L"Broken",L"Shattered"};return n<8?names[n]:L"Unavailable";}
inline const wchar_t* fatigue_name(unsigned n){static const wchar_t* names[]={L"Fresh",L"Active",L"Winded",L"Tired",L"Very tired",L"Exhausted"};return n<6?names[n]:L"Unavailable";}
inline std::wstring class_name(const UnitArt* u){
    if(!u)return L"Unknown unit type";std::wstring s=u->kind;
    std::replace(s.begin(),s.end(),L'_',L' ');if(!s.empty())s[0]=wchar_t(towupper(s[0]));return s;
}
inline void qte_meter(Graphics& g,const Assets& a,float x,float y,const qte::Snapshot& s){
    if(!s.active)return;
    panel(g,a,RectF(x-150,y-80,300,156));
    text(g,s.result==qte::Result::Pending?(s.action==qte::Action::Charge?L"CHARGE":qte::attacking(s.action)?L"ATTACK":L"DEFEND"):
        s.result==qte::Result::Success?L"WELL TIMED":L"MISSED",x-122,y-66,244,24,17,ink(),true);
    mouse_image(g,RectF(x+94,y-65,25,38),qte::attacking(s.action));
    constexpr float span=244,height=20;float left=x-span*.5f,top=y-5;
    GraphicsPath track;round_rect(track,RectF(left,top,span,height),2.5f);
    SolidBrush failure(Color(255,151,48,37)),success(Color(255,139,153,56));
    g.FillPath(&failure,&track);
    GraphicsState state=g.Save();
    g.SetClip(&track,CombineModeIntersect);
    g.FillRectangle(&success,RectF(left+qte::zone_left(s)*span,top,s.zone_width*span,height));g.Restore(state);
    Pen track_rim(Color(255,85,67,40),1.2f),track_light(Color(255,221,208,163),1.f);
    g.DrawPath(&track_rim,&track);g.DrawLine(&track_light,left+3,top+1, left+span-3,top+1);
    float marker=left+qte::reticle_position(s)*span;
    Pen shadow(Color(255,40,31,20),5),pointer(Color(255,255,248,224),2);
    g.DrawLine(&shadow,marker,top-5,marker,top+height+5);g.DrawLine(&pointer,marker,top-5,marker,top+height+5);
    SolidBrush time_track(Color(255,127,111,82)),remaining(Color(255,71,59,40));
    g.FillRectangle(&time_track,left,top+30,span,3.f);
    g.FillRectangle(&remaining,left,top+30,span*std::clamp(qte::response_remaining(s)/float(qte::response_deadline(s)),0.f,1.f),3.f);
    const wchar_t* status=s.paused?L"Paused":s.result==qte::Result::Success?L"Good timing":s.result==qte::Result::Timeout?L"Time ran out":
        s.result==qte::Result::WrongButton?L"Wrong button":s.result!=qte::Result::Pending?L"Outside green":L"Stop in green";
    text(g,status,left,y+36,span,20,13,muted());
}
inline void ammunition_indicator83(Graphics& g,const Assets& a,float middle,float bottom,const View& v){
    const auto& s=v.ammunition_state;
    if(!v.ammunition_known||!s.count){text(g,L"Ammo unavailable",middle-168,bottom-91,336,24,16,muted());return;}
    const auto& current=s.items[s.current];
    a.image(g,ammo83::image(ammo83::Icon(current.icon)),RectF(middle-168,bottom-88,22,22));
    text(g,current.label,middle-136,bottom-89,205,26,17,ink(),true);
    key_image(g,L"Tab",RectF(middle-168,bottom-63,44,23));
    unsigned first=s.current>=6?s.current-5:0,last=std::min(s.count,first+7);
    for(unsigned i=first;i<last;++i){const auto& item=s.items[i];float x=middle-108+(i-first)*31.f;
        bool selected=i==s.current,available=(item.flags&ammo83::Available)!=0;
        RectF box(x-3,bottom-65,27,27);
        if(selected){SolidBrush fill(Color(255,218,205,163));g.FillRectangle(&fill,box);Pen rim(red(),1.5f);g.DrawRectangle(&rim,box);}
        a.image(g,ammo83::image(ammo83::Icon(item.icon)),RectF(x,bottom-62,21,21),selected||available?1.f:.30f);
        if(!selected&&!available){Pen line(Color(220,116,108,90),1.f);g.DrawLine(&line,x,bottom-42,x+20,bottom-61);}
    }
    if(s.count==1)text(g,L"Single type",middle-72,bottom-62,190,22,13,muted());
    else {std::wstring position=std::to_wstring(s.current+1)+L" / "+std::to_wstring(s.count);text(g,position,middle+115,bottom-62,55,22,12,muted());}
}
inline const wchar_t* weapon_name89(const View& v){
 if(v.kind==1)return L"Bow";
 if(v.kind==3&&v.ammunition_known&&v.ammunition_state.count){auto icon=v.ammunition_state.items[v.ammunition_state.current].icon;
  if(icon==ammo83::Mortar)return L"Mortar";if(icon==ammo83::Rocket)return L"Rocket";return L"Thrown";}
 return L"Ranged";
}
inline hudstate89::State weapon_state89(const View& v){
    unsigned icon=v.ammunition_known&&v.ammunition_state.count&&v.ammunition_state.current<v.ammunition_state.count?
        v.ammunition_state.items[v.ammunition_state.current].icon:0;
    bool known=v.ammunition_known&&v.ammunition_state.count&&v.ammunition_state.current<v.ammunition_state.count&&
        (v.ammunition_state.items[v.ammunition_state.current].flags&ammo83::Selected)!=0;
    return hudstate89::resolve({v.equipment_known,v.ranged&&v.slot==2,known,
        v.pending||v.ammunition_state.pending!=0,v.aiming,v.kind,v.weapon_phase,v.aim_pose,icon,
        v.equipment_error|v.ammunition_state.error,v.ammo,v.reload});
}
inline void drop_reticle89(Graphics& g,float x,float y,bool ready){
    Pen shadow(Color(215,30,26,20),3.f),stroke(ready?Color(245,243,232,202):Color(180,207,198,173),1.2f);
    for(unsigned i=0;i<4;++i){float half=hudstate89::drop_width[i]*.5f,top=y+hudstate89::drop_y[i];
        g.DrawLine(&shadow,x-half,top,x+half,top);g.DrawLine(&stroke,x-half,top,x+half,top);}
}
inline void firearm_reticle90(Graphics& g,float x,float y,float radius,bool ready){
    float gap=std::max(3.f,radius),length=7.f;
    Pen shadow(Color(215,30,26,20),3.f),stroke(ready?Color(245,243,232,202):Color(180,207,198,173),1.2f);
    for(auto pen:{&shadow,&stroke}){g.DrawLine(pen,x-gap-length,y,x-gap,y);g.DrawLine(pen,x+gap,y,x+gap+length,y);
        g.DrawLine(pen,x,y-gap-length,x,y-gap);g.DrawLine(pen,x,y+gap,x,y+gap+length);}
}
inline void render(Graphics& g,const Assets& a,float width,float height,const View& v,const qte::Snapshot* qte=nullptr,bool offline=false){
    native_fonts.boshin=v.boshin;
    g.SetSmoothingMode(SmoothingModeAntiAlias);g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    float physical_height=height;
    float scale=std::min(width/1920.f,height/1080.f);
    if(width>=1280&&height>=720)scale=std::max(.85f,scale);
    scale*=.85f; // User-approved compact HUD:15% smaller, same anchored layout.
    GraphicsState all=g.Save();g.ScaleTransform(scale,scale);width/=scale;height/=scale;
    const float bottom=height-26,x=26,y=bottom-302;
    panel(g,a,RectF(x,y,466,302));
    text(g,v.unit?v.unit->name:L"Unit identity unavailable",x+27,y+23,344,32,21,ink(),true);
    text(g,v.faction,x+27,y+55,322,22,14,muted());
    if(!v.mon.empty())a.image(g,v.mon.c_str(),RectF(x+380,y+26,54,54));
    a.image(g,L"ui/panel_backgrounds/data__unit_card_background.png",RectF(x+28,y+84,146,184));
    const wchar_t* portrait=portrait_for(v.unit,v.owner);
    if(*portrait)a.image(g,portrait,RectF(x+27,y+81,148,190));
    else text(g,L"No portrait",x+34,y+154,132,24,15,muted());
    Pen portrait_rim(Color(255,108,96,72),1.f);g.DrawRectangle(&portrait_rim,x+27,y+83,148.f,188.f);
    text(g,class_name(v.unit),x+192,y+86,245,24,16,ink(),true);
    std::wstring strength=std::to_wstring(v.men)+L" / "+std::to_wstring(v.initial)+L" men";
    text(g,strength,x+192,y+116,244,28,21,ink(),true);
    RectF bar(x+191,y+148,238,13);a.image(g,L"ui/bars/data__unit_strength_bar_red.png",bar);
    if(v.initial){GraphicsState clip=g.Save();g.SetClip(RectF(bar.X,bar.Y,bar.Width*std::min(1.f,float(v.men)/v.initial),bar.Height),CombineModeIntersect);a.image(g,L"ui/bars/data__unit_strength_bar_green.png",bar);g.Restore(clip);}
    Pen condition_rule(Color(110,108,96,72),1.f);g.DrawLine(&condition_rule,x+192,y+166,x+430,y+166);
    text(g,L"Unit morale",x+192,y+171,102,23,14,muted());text(g,morale_name(v.morale),x+300,y+171,134,23,16,v.morale>=5?red():ink(),true);
    text(g,L"Unit fatigue",x+192,y+199,102,23,14,muted());text(g,fatigue_name(v.fatigue),x+300,y+199,134,23,16,v.fatigue>=4?red():ink(),true);
    if(v.veterancy>0&&v.veterancy<=9){wchar_t key[96];swprintf_s(key,L"ui/experience/data__experience_%d.png",v.veterancy);a.image(g,key,RectF(x+176,y+239,12,34));}
    const wchar_t* status=v.routing?L"Routing":v.crouched?L"Kneeling":v.ranged&&v.aiming?L"Aiming":v.speed>.1f?L"Moving":L"Standing";
    const wchar_t* icon=v.routing?L"ui/icons/data__status_routing.png":v.ranged&&v.aiming?L"ui/restored/status_firing-hd-v1.png":qte&&qte->active?L"ui/restored/status_melee-hd-v1.png":v.speed>.1f?L"ui/icons/data__status_walk.png":nullptr;
    if(icon)a.image(g,icon,RectF(x+193,y+239,32,32));
    text(g,qte&&qte->active?L"In melee":status,x+235,y+242,190,24,15,muted());
    float middle=width*.5f;bool ammo_panel=v.ranged||(v.equipment_known&&v.has_ranged);
    float extra=0.f;
    float center_height=ammo_panel||v.equipment_known?136.f:100.f;
    panel(g,a,RectF(middle-(ammo_panel?195.f:173.f),bottom-center_height,ammo_panel?390.f:346.f,center_height));
    if(v.equipment_known){
        key_image(g,L"1",RectF(middle-143,bottom-117-extra,23,23));text(g,L"Melee",middle-113,bottom-115-extra,57,23,14,v.slot==1?red():muted());
        if(v.has_ranged){key_image(g,L"2",RectF(middle-49,bottom-117-extra,23,23));text(g,weapon_name89(v),middle-19,bottom-115-extra,65,23,14,v.slot==2?red():muted());}
        if(v.crouch_supported){key_image(g,L"Ctrl",RectF(middle+49,bottom-117-extra,42,23));text(g,L"Kneel",middle+98,bottom-115-extra,49,23,14,v.crouched?red():muted());}
    }
    if(v.ranged){
        auto personal=weapon_state89(v);
        text(g,personal.label,middle+78,bottom-87,90,23,12,personal.fire_prompt?ink():muted());
        ammunition_indicator83(g,a,middle,bottom,v);
        wchar_t ammo[100];if(v.reload>.05f){swprintf_s(ammo,L"Unit ammo: %.0f    Reload: %.1fs",v.ammo,v.reload);text(g,ammo,middle-168,bottom-32,336,22,14,muted());}
        else{swprintf_s(ammo,L"Unit ammo: %.0f",v.ammo);text(g,ammo,middle-168,bottom-32,140,22,14,muted());
            mouse_image(g,RectF(middle-6,bottom-35,14,22),false);text(g,v.bow?L"Draw":L"Aim",middle+14,bottom-32,43,22,14,muted());
            mouse_image(g,RectF(middle+76,bottom-35,14,22),true);text(g,v.kind==1?L"Release":v.kind==3&&v.ammunition_known&&v.ammunition_state.count&&
                (v.ammunition_state.items[v.ammunition_state.current].icon==ammo83::Bomb||v.ammunition_state.items[v.ammunition_state.current].icon==ammo83::BlindBomb)?L"Throw":L"Fire",middle+97,bottom-32,65,22,14,personal.fire_prompt?ink():Color(145,108,98,78));}
    }else{
        if(ammo_panel){ammunition_indicator83(g,a,middle,bottom,v);text(g,L"Melee",middle+78,bottom-87,90,23,12,muted());}
        else text(g,L"Melee combat",middle-143,bottom-79-extra,286,23,18,ink(),true);
        text(g,L"Respond to the highlighted button",middle-143,bottom-(ammo_panel?32.f:48.f),286,22,14,muted());
    }
    float right=width-350;panel(g,a,RectF(right,bottom-160,324,160));
    text(g,L"UNIT COMMANDS",right+28,bottom-141,268,24,17,ink(),true);
    key_image(g,L"F5",RectF(right+28,bottom-108,36,24));text(g,L"Hold position",right+76,bottom-108,185,24,16);
    a.image(g,L"ui/buttons/data__button_halt_active.png",RectF(right+269,bottom-110,27,27));
    key_image(g,L"F6",RectF(right+28,bottom-77,36,24));text(g,L"Advance",right+72,bottom-77,87,24,14);
    key_image(g,L"F7",RectF(right+165,bottom-77,36,24));text(g,v.following?L"Following":L"Follow",right+209,bottom-77,87,24,14);
    key_image(g,L"F10",RectF(right+28,bottom-45,44,24));text(g,L"Return to RTS mode",right+84,bottom-45,212,24,14,red(),true);
    if(v.climb_mode){
        float cy=height*.65f;
        panel(g,a,RectF(middle-206,cy,412,88));
        if(v.climb_mode<=2){key_image(g,L"F",RectF(middle-176,cy+24,42,36));text(g,v.climb_mode==2?L"Climb down wall":L"Climb wall",middle-119,cy+26,289,32,21,ink(),true);}
        else if(v.climb_mode==4)text(g,L"Climbing over",middle-174,cy+28,348,32,21,ink(),true);
        else {key_image(g,L"W",RectF(middle-175,cy+15,36,29));text(g,L"Ascend",middle-127,cy+17,118,27,18,ink(),true);
            key_image(g,L"S",RectF(middle+8,cy+15,36,29));text(g,L"Descend",middle+56,cy+17,126,27,18,ink(),true);
            text(g,L"Release W / S to hold",middle-174,cy+52,348,24,15,muted());}
    }
    auto reticle=weapon_state89(v);
    if(!v.climb_mode&&!(qte&&qte->active)&&reticle.reticle==hudstate89::Reticle::Drop)drop_reticle89(g,width*.5f,height*.5f,reticle.fire_prompt);
    if(!v.climb_mode&&!(qte&&qte->active)&&reticle.reticle==hudstate89::Reticle::Firearm&&v.accuracy_known){float radius=0;
        if(accuracy90::projected_radius(v.cone_half_angle,v.projection_y,physical_height,radius))firearm_reticle90(g,width*.5f,height*.5f,radius/scale,reticle.fire_prompt);}
    (void)offline;
    if(qte&&qte->active)qte_meter(g,a,middle,bottom-210-extra,*qte);
    if(offline)text(g,L"OFFLINE HUD PREVIEW | fixture data",30,22,700,30,18,Color(255,239,232,211));
    g.Restore(all);
}
}
