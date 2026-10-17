#pragma once
#include "seated_weapon_geometry.h"
// Bounded visual blending at RMB edges only. The steady native held sample,
// horse, pelvis, legs, torso and head remain outside this arm/equipment blend.
namespace mountedge {
struct Key {uintptr_t actor,holder,binding,asset;unsigned lease;bool operator==(const Key& b)const{
    return actor==b.actor&&holder==b.holder&&binding==b.binding&&asset==b.asset&&lease==b.lease;}};
struct Cache {
    bool valid=false,aim=false,blending=false;Key key{};unsigned count=0;uint64_t tick=0;float elapsed=0;
    int32_t parents[256]{};float last[256*12]{},from[256*12]{};
    void reset(){valid=false;blending=false;}
};
inline bool rotation(const float* m,float* q,float* scale){
    float r[9];
    for(unsigned c=0;c<3;++c){float v=0;for(unsigned a=0;a<3;++a)v+=m[a*4+c]*m[a*4+c];
        scale[c]=std::sqrt(v);if(!std::isfinite(scale[c])||scale[c]<.000001f||scale[c]>8)return false;
        for(unsigned a=0;a<3;++a)r[a*3+c]=m[a*4+c]/scale[c];}
    for(unsigned a=0;a<3;++a)for(unsigned b=0;b<3;++b){float dot=0;for(unsigned c=0;c<3;++c)dot+=r[c*3+a]*r[c*3+b];
        if(std::abs(dot-(a==b?1.f:0.f))>.05f)return false;}
    float det=r[0]*(r[4]*r[8]-r[5]*r[7])-r[1]*(r[3]*r[8]-r[5]*r[6])+r[2]*(r[3]*r[7]-r[4]*r[6]);
    if(det<.9f)return false;
    float trace=r[0]+r[4]+r[8];
    if(trace>0){float s=2*std::sqrt(trace+1);q[3]=s*.25f;q[0]=(r[7]-r[5])/s;q[1]=(r[2]-r[6])/s;q[2]=(r[3]-r[1])/s;}
    else {unsigned i=r[0]>r[4]?(r[0]>r[8]?0:2):(r[4]>r[8]?1:2),j=(i+1)%3,k=(j+1)%3;
        float s=2*std::sqrt(std::max(0.f,1+r[i*3+i]-r[j*3+j]-r[k*3+k]));if(s<.000001f)return false;
        q[i]=s*.25f;q[j]=(r[i*3+j]+r[j*3+i])/s;q[k]=(r[i*3+k]+r[k*3+i])/s;q[3]=(r[k*3+j]-r[j*3+k])/s;}
    float norm=0;for(unsigned i=0;i<4;++i)norm+=q[i]*q[i];if(!std::isfinite(norm)||norm<.000001f)return false;
    for(unsigned i=0;i<4;++i)q[i]/=std::sqrt(norm);return true;
}
inline bool interpolate(const float* a,const float* b,float t,float* out){
    if(t<=0){memcpy(out,a,48);return true;}if(t>=1){memcpy(out,b,48);return true;}
    float qa[4],qb[4],sa[3],sb[3];if(!rotation(a,qa,sa)||!rotation(b,qb,sb))return false;
    float dot=0;for(unsigned i=0;i<4;++i)dot+=qa[i]*qb[i];if(dot<0)for(float& v:qb)v=-v;
    float q[4],length=0;for(unsigned i=0;i<4;++i){q[i]=qa[i]+(qb[i]-qa[i])*t;length+=q[i]*q[i];}
    if(!std::isfinite(length)||length<.000001f)return false;for(float& v:q)v/=std::sqrt(length);
    float x=q[0],y=q[1],z=q[2],w=q[3];float r[9]={1-2*(y*y+z*z),2*(x*y-z*w),2*(x*z+y*w),
        2*(x*y+z*w),1-2*(x*x+z*z),2*(y*z-x*w),2*(x*z-y*w),2*(y*z+x*w),1-2*(x*x+y*y)};
    for(unsigned i=0;i<3;++i){for(unsigned c=0;c<3;++c)out[i*4+c]=r[i*3+c]*(sa[c]+(sb[c]-sa[c])*t);
        out[i*4+3]=a[i*4+3]+(b[i*4+3]-a[i*4+3])*t;}
    return true;
}
inline bool apply(Cache& cache,const Key& key,float* m,unsigned n,const int32_t* parents,unsigned hips,
                  unsigned left,unsigned right,const unsigned* weapons,bool aim,uint64_t tick,bool paused){
    auto reject=[&](){cache.reset();return false;};
    if(!m||!parents||!weapons||!key.actor||!key.holder||!key.binding||!key.asset||!n||n>256||hips>=n||left>=n||right>=n)return reject();
    if(parents[hips]!=-1)return reject();
    for(unsigned i=0;i<n;++i)if(parents[i]>=int32_t(i)||parents[i]<-1)return reject();
    for(unsigned i=0;i<3;++i)if(weapons[i]>=n||parents[weapons[i]]!=-1)return reject();
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(m[i])||std::abs(m[i])>100000)return reject();
    float inverse[12];if(!affine_pose_inverse(m+hips*12,inverse))return reject();
    bool fresh=!cache.valid||!(cache.key==key)||cache.count!=n||memcmp(cache.parents,parents,n*sizeof(int32_t))||tick<cache.tick||tick-cache.tick>250;
    if(fresh){cache.reset();cache.key=key;cache.count=n;cache.aim=aim;cache.tick=tick;memcpy(cache.parents,parents,n*sizeof(int32_t));}
    else if(aim!=cache.aim){memcpy(cache.from,cache.last,n*48);cache.elapsed=0;cache.blending=true;cache.aim=aim;cache.tick=tick;}
    else if(cache.blending&&!paused)cache.elapsed+=std::min<uint64_t>(tick-cache.tick,50)*.001f;
    cache.tick=tick;
    float progress=std::clamp(cache.elapsed/.16f,0.f,1.f);progress=progress*progress*(3-2*progress);
    float out[256*12];memcpy(out,m,n*48);
    for(unsigned i=0;i<n;++i){bool owned=bone_descends(i,left,parents,n)||bone_descends(i,right,parents,n)||i==weapons[0]||i==weapons[1]||i==weapons[2];
        if(!owned)continue;
        if(cache.blending&&progress<1){float source[12];affine_product(m+hips*12,cache.from+i*12,source);
            if(!interpolate(source,m+i*12,progress,out+i*12))return reject();}
    }
    for(unsigned i=0;i<n*12;++i)if(!std::isfinite(out[i]))return reject();
    for(unsigned i=0;i<n;++i)affine_product(inverse,out+i*12,cache.last+i*12);
    memcpy(m,out,n*48);cache.valid=true;if(progress>=1)cache.blending=false;return true;
}
}
