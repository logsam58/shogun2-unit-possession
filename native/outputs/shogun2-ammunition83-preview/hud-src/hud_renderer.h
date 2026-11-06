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
#ifdef S2FP_LOCAL_ASSETS
#include "../../../common/local_assets159.h"
#ifdef HUD80_QTE_ONLY
#include "../../../common/qte_sprites159.h"
#else
#include "../../../common/hud_sprites159.h"
#endif
#else
#ifdef HUD80_QTE_ONLY
#include "qte-sprites.generated.h"
#else
#include "sprites.generated.h"
#endif
#endif
#include "unit_art.generated.h"
#include "../../../common/image_fit184.h"
#include "../../../common/card_image217.h"
#ifdef HUD80_QTE_ONLY
#include "../qte-src/qte_core.h"
#else
#include "../../shogun2-melee-meter79-preview/src/qte_core.h"
#endif
#include "native_font.h"
#include "../../shogun2-possession-cleanup-v46/src/hud_layout108.h"
#include "hud_state89.h"
#include "../../../common/unit_activity215.h"
#include "weapon_mode133.h"
#include "../../shogun2-accuracy90-preview/src/ability_protocol141.h"
#include "../../shogun2-accuracy90-preview/src/ability_catalog141.h"
#include "accuracy_geometry90.h"
#include "../../shogun2-accuracy90-preview/src/command_protocol100.h"
#include "../src/ammunition_protocol83.h"
namespace hud80 {
using namespace Gdiplus;
struct View {
    const UnitArt* unit{};
    std::wstring native_unit_key;
    std::shared_ptr<card217::Picture> native_card217;
    const FactionArt* owner{};
    std::wstring faction=L"Faction unavailable",mon;
    unsigned men{},initial{},morale{},fatigue{};
    int veterancy=-1;
    bool ranged{},bow{},aiming{},routing{},following{},fixture{},boshin{};
    unsigned aim_pose{},weapon_phase{},kind{},equipment_error{};
    bool equipment_known{},has_ranged{},crouch_supported{},crouched{},pending{};
    bool unit_mode_known{},unit_melee{};
    bool hidden{};
    unsigned slot{},styles{};std::wstring ammunition;
    bool ammunition_known{};ammo83::Status ammunition_state{};
    float ammo{},reload{},speed{};unsigned ammo_capacity{};
    bool unit_activity_known{};unitactivity215::Activity unit_activity{};
    bool accuracy_known{};float cone_half_angle{},projection_y{};
    unsigned climb_mode{};
    bool commands_known{};command100::Status commands{};
    bool abilities_known{};ability141::Status abilities{};
};
class Assets {
    std::vector<IStream*> streams;
    std::vector<std::unique_ptr<Bitmap>> atlases;
    mutable std::map<std::wstring,std::unique_ptr<Bitmap>> tiles;
public:
    bool load(HMODULE module){
        if(!native_fonts.load(module))return false;
#ifdef S2FP_LOCAL_ASSETS
        for(int i=0;i<atlas_count;++i){
            auto b=assets159::resource(module,101+i);if(!b.bytes||!b.width)return false;
            auto bitmap=std::make_unique<Bitmap>(b.width,b.height,b.width*4,PixelFormat32bppARGB,const_cast<BYTE*>(b.bytes));
            if(bitmap->GetLastStatus()!=Ok)return false;atlases.push_back(std::move(bitmap));
        }
#ifndef HUD80_QTE_ONLY
        for(unsigned id:{30u,31u}){
            auto b=assets159::resource(module,400+id);if(!b.bytes||!b.width)return false;
            auto bitmap=std::make_unique<Bitmap>(b.width,b.height,b.width*4,PixelFormat32bppARGB,const_cast<BYTE*>(b.bytes));
            if(bitmap->GetLastStatus()!=Ok)return false;tiles.emplace(L"native-ability141-"+std::to_wstring(id),std::move(bitmap));
        }
#endif
#else
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
        }
#ifndef HUD80_QTE_ONLY
        for(unsigned id:{30u,31u}){
            HRSRC r=FindResourceW(module,MAKEINTRESOURCEW(400+id),MAKEINTRESOURCEW(10));if(!r)return false;
            auto size=SizeofResource(module,r);auto bytes=LockResource(LoadResource(module,r));HGLOBAL memory=GlobalAlloc(GMEM_MOVEABLE,size);if(!memory)return false;
            auto copy=GlobalLock(memory);if(!copy){GlobalFree(memory);return false;}memcpy(copy,bytes,size);GlobalUnlock(memory);
            IStream* stream=nullptr;if(CreateStreamOnHGlobal(memory,TRUE,&stream)!=S_OK){GlobalFree(memory);return false;}streams.push_back(stream);
            auto bitmap=std::make_unique<Bitmap>(stream);if(bitmap->GetLastStatus()!=Ok)return false;
            tiles.emplace(L"native-ability141-"+std::to_wstring(id),std::move(bitmap));
        }
#endif

#endif
                return true;
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
    RectF fitted(const wchar_t* key,RectF slot)const{
        float w=0,h=0;
        if(const auto* s=sprite(key)){w=float(s->logical_w);h=float(s->logical_h);}
        else {auto tile=tiles.find(key);if(tile!=tiles.end()){w=float(tile->second->GetWidth());h=float(tile->second->GetHeight());}}
        const auto r=imagefit184::within({slot.X,slot.Y,slot.Width,slot.Height},w,h);
        return RectF(r.x,r.y,r.w,r.h);
    }
    void image(Graphics& g,const wchar_t* key,RectF target,float opacity=1.f,bool isolated=false)const{
        const Sprite* s=sprite(key);if(!s){auto extra=tiles.find(key);if(extra!=tiles.end())g.DrawImage(extra->second.get(),target);return;}
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
inline std::wstring display_name169(const View& v){
    if(v.unit)return v.unit->name;
    return v.native_unit_key.empty()?L"Unit identity unavailable":v.native_unit_key;
}
#include "../../shogun2-possession-cleanup-v46/src/qte_meter108.h"

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
inline bool melee_dot144(const View& v){
    return v.unit_mode_known&&v.unit_melee&&v.equipment_known&&v.slot==1&&!v.pending&&!v.equipment_error;
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
    const hudlayout108::Layout layout(width,height);
    const float scale=layout.zoom;
    GraphicsState all=g.Save();g.ScaleTransform(scale,scale);width/=scale;height/=scale;
    if(v.hidden){
        // Native QTE meter includes the required mouse-button indicator.
        // The live QTE renderer is independent and never reads this HUD flag.
        if(qte&&qte->active)qte_meter(g,a,width*.5f,hudlayout108::qte_anchor_y(height),*qte);
        g.Restore(all);return;
    }
    // Compact bottom-left identity card, with ability and orders spaced down
    // the right. The redundant raised/fire instruction panel is omitted.
    const auto& identity=layout.soldier;const float x=identity.x,y=identity.y;
    panel(g,a,RectF(x,y,identity.w,identity.h));
    text(g,display_name169(v),x+22,y+16,390,29,22,ink(),true);
    text(g,v.faction,x+22,y+46,94,21,14,muted());
    if(!v.mon.empty())a.image(g,v.mon.c_str(),RectF(x+422,y+19,40,40));
    const wchar_t* portrait=portrait_for(v.unit,v.owner);
    const wchar_t* backing=L"ui/panel_backgrounds/data__unit_card_background.png";
    const RectF slot(x+22,y+67,92,136);
    auto fitted217=v.native_card217?imagefit184::within({slot.X,slot.Y,slot.Width,slot.Height},float(v.native_card217->w),float(v.native_card217->h)):imagefit184::Rect{};
    const RectF card=v.native_card217?RectF(fitted217.x,fitted217.y,fitted217.w,fitted217.h):a.fitted(*portrait?portrait:backing,slot);
    if(card.Width>0&&card.Height>0){
        a.image(g,backing,card);
        if(v.native_card217){Bitmap actual(v.native_card217->w,v.native_card217->h,v.native_card217->w*4,PixelFormat32bppARGB,v.native_card217->pixels.data());g.DrawImage(&actual,card);}
        else if(*portrait)a.image(g,portrait,card);
        else text(g,L"No portrait",card.X+3,card.Y+45,card.Width-6,22,12,muted());
        Pen portrait_rim(Color(255,108,96,72),1.f);g.DrawRectangle(&portrait_rim,card);
    }
    text(g,class_name(v.unit),x+126,y+51,336,24,16,ink(),true);
    std::wstring strength=std::to_wstring(v.men)+L" / "+std::to_wstring(v.initial)+L" men";
    text(g,strength,x+126,y+79,176,26,20,ink(),true);
    if(v.has_ranged){
        text(g,L"Ammo",x+327,y+73,134,20,13,muted());
        RectF meter(x+327,y+96,135,10);a.image(g,L"ui/bars/data__unit_strength_bar_red.png",meter);
        const float fraction=unitactivity215::ammunition(v.ammo,v.ammo_capacity);
        if(fraction>0){auto clipped=g.Save();g.SetClip(RectF(meter.X,meter.Y,meter.Width*fraction,meter.Height),CombineModeIntersect);a.image(g,L"ui/bars/data__unit_strength_bar_yellow.png",meter);g.Restore(clipped);}
    }
    RectF bar(x+126,y+115,336,11);a.image(g,L"ui/bars/data__unit_strength_bar_red.png",bar);
    if(v.initial){GraphicsState clip=g.Save();g.SetClip(RectF(bar.X,bar.Y,bar.Width*std::min(1.f,float(v.men)/v.initial),bar.Height),CombineModeIntersect);a.image(g,L"ui/bars/data__unit_strength_bar_green.png",bar);g.Restore(clip);}
    text(g,L"Unit morale",x+126,y+132,131,23,15,muted());text(g,morale_name(v.morale),x+270,y+132,192,23,16,v.morale>=5?red():ink(),true);
    text(g,L"Unit fatigue",x+126,y+156,131,23,15,muted());text(g,fatigue_name(v.fatigue),x+270,y+156,192,23,16,v.fatigue>=4?red():ink(),true);
    if(v.veterancy>0&&v.veterancy<=9){wchar_t key[96];swprintf_s(key,L"ui/experience/data__experience_%d.png",v.veterancy);a.image(g,key,RectF(x+110,y+181,10,25));}
    // Activity excludes the player; the adjacent mode is the formation's order.
    const wchar_t* status=v.routing?L"Routing":v.unit_activity_known?unitactivity215::label(v.unit_activity):L"Unit status";
    const wchar_t* icon=v.routing?L"ui/icons/data__status_routing.png":v.unit_activity_known?unitactivity215::icon(v.unit_activity):nullptr;
    if(icon)a.image(g,icon,RectF(x+126,y+183,24,24));
    text(g,status,x+157,y+184,130,24,15,muted());
    const auto mode=weaponmode133::resolve(v.unit_mode_known,v.unit_melee);
    if(mode.icon)a.image(g,mode.icon,RectF(x+294,y+183,24,24));
    text(g,mode.label,x+328,y+184,134,24,14,v.unit_mode_known?ink():muted());
    const float middle=width*.5f;
    const auto& context=layout.context;
    if(v.climb_mode&&!(qte&&qte->active)){
        const float cx=context.x,cy=context.y;
        panel(g,a,RectF(cx,cy,context.w,context.h));
        {
            if(v.climb_mode<=2){key_image(g,L"F",RectF(cx+22,cy+20,30,26));text(g,L"Climb wall",cx+65,cy+21,242,26,17,ink(),true);}
            else if(v.climb_mode==5)text(g,L"Cannot climb here",cx+22,cy+23,290,26,17,ink(),true);
            else if(v.climb_mode==6)text(g,L"Approaching wall",cx+22,cy+23,290,26,17,ink(),true);
            else if(v.climb_mode==4)text(g,L"Climbing over",cx+22,cy+23,290,26,17,ink(),true);
            else {key_image(g,L"W",RectF(cx+27,cy+17,28,25));text(g,L"Ascend",cx+65,cy+18,107,25,16);
                key_image(g,L"S",RectF(cx+174,cy+17,28,25));text(g,L"Descend",cx+211,cy+18,100,25,16);
                text(g,L"Release to hold",cx+22,cy+48,290,23,14,muted());}
        }
    }
    const auto& command=layout.commands;const float right=command.x,top=command.y;
    panel(g,a,RectF(right,top,command.w,command.h));
    const wchar_t* command_title=L"Unit orders";
    if(v.commands_known&&v.commands.placing)command_title=L"UNIT PLACEMENT";
    else if(v.commands_known&&GetTickCount64()<v.commands.feedback_until){
        const wchar_t* titles[]={L"COMMANDS",L"ADVANCE ORDERED",L"FORMATION ORDERED",L"UNIT STOPPED",L"HOLDING HERE",L"FOLLOWING YOU",L"FOLLOW ENDED",L"HOLD FIRE",L"FIRE AT WILL",L"FIRE ON COMMAND",L"NO GROUND TARGET",L"COMMAND UNAVAILABLE",L"COMMAND UNAVAILABLE"};
        if(v.commands.feedback<sizeof titles/sizeof *titles)command_title=titles[v.commands.feedback];
    }
    text(g,command_title,right+23,top+17,224,27,wcscmp(command_title,L"Unit orders")==0?16.f:13.f,ink(),true);
    const bool placing=v.commands_known&&v.commands.placing;
    const wchar_t* fire_mode=!v.commands_known?L"Fire mode unknown":v.commands.fire_mode==0?L"Hold fire":v.commands.fire_mode==1?L"Fire at will":L"Fire on command";
    const wchar_t* keys[]={L"Q",L"Z",L"C",L"R"};
    const wchar_t* labels[]={L"Move / attack",L"Hold here",L"Follow me",fire_mode};
    for(unsigned i=0;i<4;++i){float row=top+50+i*28.f;
        key_image(g,keys[i],RectF(right+24,row,34,25));
        bool active=i==1?v.commands_known&&v.commands.move_mode==3:i==2?v.following:i==3&&v.commands_known;
        text(g,labels[i],right+76,row+1,170,26,16,active?red():ink());
    }
    Pen command_rule(Color(150,108,96,72),1.f);g.DrawLine(&command_rule,right+24,top+167,right+246,top+167);
    key_image(g,L"ALT",RectF(right+24,top+178,44,23));text(g,L"Slow motion",right+84,top+178,162,24,15,ink());
    key_image(g,L"F10",RectF(right+24,top+207,44,22));text(g,L"RTS view",right+84,top+207,162,24,15,muted());
    if(v.abilities_known&&v.abilities.count&&v.abilities.selected<v.abilities.count){
        const auto& item=v.abilities.items[v.abilities.selected];auto art=ability141::art(item.id);
        if(art){float ax=layout.ability.x,ay=layout.ability.y;panel(g,a,RectF(ax,ay,layout.ability.w,layout.ability.h));
            a.image(g,art->icon,RectF(ax+24,ay+22,40,40),(item.flags&ability141::Available)?1.f:.45f);
            native_fonts.paint(g,art->label,RectF(ax+81,ay+18,layout.ability.w-105,27),18,ink(),true);
            wchar_t status[64];if(v.abilities.error)wcscpy_s(status,item.flags&ability141::Targeted?L"Aim at an ally":L"Unavailable");else if(v.abilities.pending)wcscpy_s(status,L"Ordered");
            else if(item.cooldown>.05f)swprintf_s(status,L"Ready in %.0fs",std::ceil(item.cooldown));
            else wcscpy_s(status,item.flags&ability141::Active?L"Active":item.flags&ability141::Available?(item.flags&ability141::Targeted?L"Aim at an ally":L"Ready"):L"Unavailable");
            const bool ready=!v.abilities.error&&!v.abilities.pending&&item.cooldown<=.05f&&(item.flags&ability141::Available);
            native_fonts.paint(g,status,RectF(ax+81,ay+48,layout.ability.w-105,25),17,v.abilities.error?red():ready?Color(255,85,105,25):muted(),true);
            key_image(g,L"T",RectF(ax+24,ay+84,29,25));text(g,L"Next",ax+67,ay+85,87,25,15,ink());
            key_image(g,L"G",RectF(ax+203,ay+84,29,25));text(g,item.flags&ability141::Active?L"End":L"Use",ax+245,ay+85,91,25,15,ink());
        }
    }
    auto reticle=weapon_state89(v);
    if(!placing&&!v.climb_mode&&!(qte&&qte->active)&&melee_dot144(v)){
        float x=width*.5f,y=height*.5f,r=2.f/scale;
        SolidBrush edge(Color(210,30,26,20)),core(Color(235,243,232,202));
        g.FillEllipse(&edge,x-r,y-r,r*2,r*2);r=1.f/scale;g.FillEllipse(&core,x-r,y-r,r*2,r*2);
    }
    if(!placing&&!v.climb_mode&&!(qte&&qte->active)&&reticle.reticle==hudstate89::Reticle::Drop)drop_reticle89(g,width*.5f,height*.5f,reticle.fire_prompt);
    if(!placing&&!v.climb_mode&&!(qte&&qte->active)&&reticle.reticle==hudstate89::Reticle::Firearm&&v.accuracy_known){float radius=0;
        if(accuracy90::projected_radius(v.cone_half_angle,v.projection_y,physical_height,radius))firearm_reticle90(g,width*.5f,height*.5f,radius/scale,reticle.fire_prompt);}
    (void)offline;
    if(qte&&qte->active)qte_meter(g,a,middle,hudlayout108::qte_anchor_y(height),*qte);
    if(offline)text(g,L"OFFLINE HUD PREVIEW | fixture data",30,22,700,30,18,Color(255,239,232,211));
    g.Restore(all);
}
}
