#pragma once
#include "native214.h"
// Included after native_ui185.h. All edits execute on its current root UI thread.
namespace deck214 {
struct Geometry {uintptr_t widget{},state{};float width{},height{},scaled_width{},scaled_height{};};
struct Font {uintptr_t state{};int original{},scaled{};};
inline uintptr_t owner_root{};inline std::vector<Geometry> geometry;inline std::vector<Font> fonts;
struct Node {uintptr_t widget{},state{};float x{},y{},width{},height{};unsigned depth{};};
inline bool node(uintptr_t at,Node& n){
 uintptr_t table{};n.widget=at;
 if(!native214::value(at,table)||(table!=deck185::native_ui185.base+0x16e2aac&&table!=deck185::native_ui185.base+0x1704cdc)||!native214::value(at+0xb8,n.state)||!n.state)return false;
 float wh[2]{};if(!native214::read(n.state+0x28,wh,sizeof wh)||!std::isfinite(wh[0])||!std::isfinite(wh[1])||wh[0]<0||wh[1]<0||wh[0]>16384||wh[1]>16384)return false;
 n.width=wh[0];n.height=wh[1];return true;
}
inline bool linked(uintptr_t widget){
 uintptr_t root{};if(!native214::value(deck185::native_ui185.base+0x1bbe364,root)||root!=owner_root)return false;
 for(unsigned n=0;widget&&widget!=root&&n<16;++n){uintptr_t parent{};if(!native214::value(widget+0x94,parent)||parent==widget)return false;widget=parent;}
 return widget==root;
}
inline bool original_position(Node& n){
 __try{float xy[2]{};reinterpret_cast<float*(__thiscall*)(void*,float*)>(deck185::native_ui185.base+0xf955b0)(reinterpret_cast<void*>(n.widget),xy);n.x=xy[0];n.y=xy[1];return std::isfinite(n.x)&&std::isfinite(n.y)&&std::fabs(n.x)<32768&&std::fabs(n.y)<32768;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool resize(Node n,float w,float h){
 Node live;if(!linked(n.widget)||!node(n.widget,live)||live.state!=n.state)return false;
 unsigned char flags[2]{};if(!native214::read(n.widget+0xed,flags,2)||flags[0]>1||flags[1]>1||w<=0||h<=0)return false;
 __try{
  *reinterpret_cast<unsigned char*>(n.widget+0xed)=1;*reinterpret_cast<unsigned char*>(n.widget+0xee)=1;
  __try{reinterpret_cast<deck185::Resize187>(deck185::native_ui185.base+0xf98900)(reinterpret_cast<void*>(n.widget),w,h,false,true);}
  __finally{*reinterpret_cast<unsigned char*>(n.widget+0xed)=flags[0];*reinterpret_cast<unsigned char*>(n.widget+0xee)=flags[1];}
  return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool move(uintptr_t widget,float x,float y){
 Node live;if(!linked(widget)||!node(widget,live))return false;
 __try{float xy[2]{x,y};reinterpret_cast<void(__thiscall*)(void*,const float*)>(deck185::native_ui185.base+0xf920a0)(reinterpret_cast<void*>(widget),xy);return true;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool font(uintptr_t widget,uintptr_t state,float scale){
 uintptr_t owner{};int normal{},current{};
 if(!linked(widget)||!native214::value(state+4,owner)||owner!=widget||!native214::value(state+0xe4,normal)||!native214::value(state+0xd0,current))return false;
 // Native f82540 loads the font's point size into+e4; f9ce40 uses+d0
 // as its per-state override. No font family/resource pointer is replaced.
 if(normal<6||normal>48)return true;
 auto item=std::find_if(fonts.begin(),fonts.end(),[&](const Font& f){return f.state==state;});
 if(item==fonts.end()){if(fonts.size()>=1024)return false;fonts.push_back({state,normal,std::min(64,int(std::lround(normal*scale)))});item=fonts.end()-1;}
 else if(normal!=item->original&&normal!=item->scaled){item->original=normal;item->scaled=std::min(64,int(std::lround(normal*scale)));}
 if(normal==item->scaled&&current==item->scaled)return true;
 __try{
  *reinterpret_cast<int*>(state+0xe4)=item->scaled;*reinterpret_cast<int*>(state+0xd0)=item->scaled;
  reinterpret_cast<void(__thiscall*)(void*)>(deck185::native_ui185.base+0xf87480)(reinterpret_cast<void*>(state));return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline bool decorate(uintptr_t root,uintptr_t target,bool corner){
 const auto image=deck185::native_ui185.base;
 if(!deck185::native_ui185.ready||!native214::admit(image,5)||!root||!target||target==root)return false;
 uintptr_t ancestor=target;
 for(unsigned n=0;ancestor!=root&&n<16;++n){uintptr_t p{};if(!native214::value(ancestor+0x94,p)||!p||p==ancestor)return false;ancestor=p;}
 if(ancestor!=root)return false;
 if(owner_root!=root){owner_root=root;geometry.clear();fonts.clear();}
 Node first;if(!node(target,first)||!original_position(first)||first.width<1||first.height<1)return false;
 auto view=deck185::active_ui185();if(!deck185::ui_size_valid(view))return false;
 const float scale=corner?1.15f:1.5f;
 // Whole tooltip fits the current logical viewport. The small end-turn corner
 // keeps its bottom/right anchor and never resizes a campaign modal or map.
 const float factor=std::min({scale,(float(view.width)-16)/first.width,(float(view.height)-16)/first.height});
 if(factor<1.f)return false;
 std::vector<Node> nodes{first};
 for(unsigned i=0;i<nodes.size();++i){
  unsigned count{};uintptr_t children{};const auto parent=nodes[i];
  if(!native214::value(parent.widget+0x9c,count)||count>256||!native214::value(parent.widget+0xa0,children)||(count&&!children)||parent.depth>12)return false;
  for(unsigned n=0;n<count;++n){uintptr_t at{};Node next;
   if(nodes.size()>=256||!native214::value(children+n*4,at)||!node(at,next)||!original_position(next)||std::any_of(nodes.begin(),nodes.end(),[&](const Node& q){return q.widget==at;}))return false;
   next.depth=parent.depth+1;nodes.push_back(next);
  }
 }
 auto found=std::find_if(geometry.begin(),geometry.end(),[&](const Geometry& g){return g.widget==target&&g.state==first.state;});
 bool fresh=found==geometry.end();
 float anchor_x=first.x,anchor_y=first.y;
 if(fresh){
  if(geometry.size()>=64){geometry.clear();fonts.clear();}
  if(corner){anchor_x=std::max(8.f,first.x+first.width-first.width*factor);anchor_y=std::max(8.f,first.y+first.height-first.height*factor);}
  for(const auto& n:nodes)if(n.width>0&&n.height>0){if(!resize(n,n.width*factor,n.height*factor)||!move(n.widget,anchor_x+(n.x-first.x)*factor,anchor_y+(n.y-first.y)*factor))return false;}
  geometry.push_back({target,first.state,first.width,first.height,first.width*factor,first.height*factor});
 }
 for(const auto& n:nodes){
  unsigned count{};uintptr_t states{};
  if(!native214::value(n.widget+0xb0,count)||count>64||!native214::value(n.widget+0xb4,states)||(count&&!states))return false;
  for(unsigned i=0;i<count;++i){uintptr_t s{};if(!native214::value(states+i*4,s)||!s||!font(n.widget,s,factor))return false;}
 }
 Node after;if(!node(target,after)||!original_position(after))return false;
 // Native text wrapping may change a tooltip's final dimensions. Keep its
 // native close/hover geometry aligned and its whole frame on-screen.
 float x=std::clamp(after.x,8.f,std::max(8.f,float(view.width)-after.width-8));
 float y=std::clamp(after.y,8.f,std::max(8.f,float(view.height)-after.height-8));
 if(x!=after.x||y!=after.y)return move(target,x,y);
 return true;
}
} // deck214
