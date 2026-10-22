#pragma once
namespace hud80 {
struct Sprite { const wchar_t* key; int atlas,x,y,w,h,logical_w,logical_h,offset_x,offset_y; };
inline const Sprite sprites[]={
{L"ui/frames/data__subpanel_paper_fill.png",0,2,2,200,200,200,200,0,0},
{L"ui/frames/data__big_panel_top.png",0,206,2,150,33,150,36,0,3},
{L"ui/frames/data__big_panel_bottom.png",0,360,2,150,32,150,36,0,0},
{L"ui/bars/data__unit_strength_bar_green.png",0,360,38,58,16,58,16,0,0},
{L"ui/bars/data__unit_strength_bar_red.png",0,422,38,58,16,58,16,0,0},
{L"ui/frames/data__big_panel_left.png",0,484,38,24,50,36,50,12,0},
{L"ui/frames/data__big_panel_tl.png",0,206,39,36,36,36,36,0,0},
{L"ui/frames/data__big_panel_tr.png",0,246,39,36,36,36,36,0,0},
{L"ui/frames/data__big_panel_bl.png",0,286,39,34,36,36,36,2,0},
{L"ui/frames/data__big_panel_right.png",0,324,39,23,50,36,50,0,0},
{L"ui/frames/data__big_panel_br.png",0,206,79,34,36,36,36,0,0},
};
inline constexpr int atlas_count=1;
inline const Sprite* sprite(const wchar_t* key){for(const auto& s:sprites)if(wcscmp(s.key,key)==0)return &s;return nullptr;}
}
