#pragma once
#include <cmath>
#include <cstdint>
namespace rts90 {
struct Pose {float eye[3]{},target[3]{},up[3]{},fov{};};
inline bool valid(const Pose& p){
 float n=0,look=0;for(unsigned i=0;i<3;++i){if(!std::isfinite(p.eye[i])||!std::isfinite(p.target[i])||!std::isfinite(p.up[i])||std::abs(p.eye[i])>100000||std::abs(p.target[i])>100000)return false;
  n+=p.up[i]*p.up[i];look+=(p.target[i]-p.eye[i])*(p.target[i]-p.eye[i]);}
 return n>.0001f&&n<4&&look>.000001f&&std::isfinite(p.fov)&&p.fov>.1f&&p.fov<3.14f;
}
struct Identity {uintptr_t environment{},common{},phase{},manager{},overhead{};};
inline bool same(const Identity&a,const Identity&b){return a.environment&&a.environment==b.environment&&a.common==b.common&&a.phase==b.phase&&a.manager==b.manager&&a.overhead==b.overhead;}
struct Cache {unsigned char override_matrix{};float aspect{};float view[16]{},projection[16]{};};
struct Saved {Identity identity{};Pose camera{},manager{};Cache camera_cache{},manager_cache{};bool active{};};
}
