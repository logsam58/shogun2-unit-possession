#pragma once
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
namespace deck181 {
inline constexpr unsigned campaign_slots[]={0x94,0x98,0x9c,0xa0,0x108,0x10c};
inline constexpr unsigned campaign_functions[]={0x8c09e0,0x8c08a0,0x8c0d10,0x8c0b20,0x8ef030,0x8f0490};
template<class Read>uintptr_t campaign_camera(uintptr_t base,Read read){
 // Native 11499f0/1135690 -> manager+a98, stack+14 -> 6a2810.
 // 1bc8fe4 is a MUTABLE pointer slot, never treated as immutable code.
 auto global=read(base+0x1bc8fe4);if(!global)return 0;
 auto manager=read(global+0xa98),stack=manager?read(manager+0x14):0;if(!stack)return 0;
 auto index=read(stack+0x10),array=read(stack+0xc);if(index>63||!array)return 0;
 auto node=read(array+index*4);if(!node)return 0;
 auto count=read(node+4),cameras=read(node+8);if(!count||count>64||!cameras)return 0;
 auto camera=read(cameras+(count-1)*4);if(!camera||read(camera)!=base+0x16209f8||!read(camera+0x138))return 0;
 for(unsigned i=0;i<6;++i)if(read(base+0x16209f8+campaign_slots[i])!=base+campaign_functions[i])return 0;
 return camera;
}
struct CampaignStep {uintptr_t camera{};float x{},y{},yaw{},zoom{};uint64_t tick{};};
inline uint64_t campaign_clock_us(){
 using namespace std::chrono;
 return duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}
struct CampaignRate {
 float x{},y{},yaw{},wheel{};
 bool moving()const{return x!=0||y!=0||yaw!=0||wheel!=0;}
};
// The worker publishes a held rate. Advance it using elapsed UI time so the number of
// Steam Input polls between rendered frames does not change the next camera step.
class CampaignMotion {
 CampaignRate rate_{},legacy_{};
 uintptr_t camera_{};
 uint64_t sample_{},frame_{};
 static constexpr uint64_t stale_us=100000,min_frame_us=4000,max_frame_us=50000;
 bool fresh(uint64_t now)const{return sample_&&now>=sample_&&now-sample_<=stale_us;}
 bool due(uint64_t now)const{return !frame_||now<frame_||now-frame_>=min_frame_us;}
 float elapsed(uint64_t now){
  if(!due(now))return 0;
  auto previous=frame_;frame_=now;
  if(!previous||now<previous||now-previous>stale_us)return 0;
  return float(std::min(now-previous,max_frame_us))*1e-6f;
 }
public:
 void clear(){*this={};}
 void publish(CampaignRate rate,uint64_t now){
  if(!rate.moving()||!now){clear();return;}
  if(!fresh(now))clear();
  rate_=rate;sample_=now;
 }
 bool ready(uint64_t now)const{return fresh(now)&&due(now);}
 CampaignStep consume(uintptr_t camera,uint64_t now){
  if(!fresh(now)||!camera){clear();return {};}
  if(camera_!=camera){camera_=camera;frame_=now;legacy_={};return {};}
  const float dt=elapsed(now);
  if(!dt)return {};
  return {camera,std::clamp(rate_.x*dt,-4.f,4.f),std::clamp(rate_.y*dt,-4.f,4.f),
          std::clamp(rate_.yaw*dt,-.4f,.4f),rate_.wheel*dt,now/1000};
 }
 // Older Lua callers can still drain the documented fixed steps.
 bool legacy_step(unsigned axis,int sign,uint64_t now){
  if(!fresh(now)){clear();return false;}
  const float dt=elapsed(now);
  auto queue=[&](float& pending,float rate,float limit){
   if(pending*rate<0)pending=0;
   pending=std::clamp(pending+rate*dt,-limit,limit);
  };
  queue(legacy_.x,rate_.x,4.f);queue(legacy_.y,rate_.y,4.f);queue(legacy_.yaw,rate_.yaw,.4f);
  auto& pending=axis==0?legacy_.x:axis==1?legacy_.y:legacy_.yaw;
  const float quantum=axis==2?.01f:.125f;
  if(pending*sign<quantum)return false;
  pending-=sign*quantum;return true;
 }
};
inline bool fresh_campaign(const CampaignStep& step,uintptr_t current,uint64_t now){return current&&step.camera==current&&step.tick&&now>=step.tick&&now-step.tick<=100;}
template<class Apply>bool apply_campaign(const CampaignStep& step,uintptr_t current,uint64_t now,Apply apply){
 if(!fresh_campaign(step,current,now)||!std::isfinite(step.x)||!std::isfinite(step.y)||!std::isfinite(step.yaw)||!std::isfinite(step.zoom)||std::fabs(step.x)>4||std::fabs(step.y)>4||std::fabs(step.yaw)>.4f||std::fabs(step.zoom)>14.4f)return false;
 if(step.y)apply(step.y>0?0x94:0x98,std::fabs(step.y));
 if(step.x)apply(step.x>0?0x9c:0xa0,std::fabs(step.x));
 if(step.yaw)apply(0x10c,step.yaw);return true;
}
}
