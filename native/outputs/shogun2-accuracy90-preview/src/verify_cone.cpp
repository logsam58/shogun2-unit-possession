#include "identity.h"
#include "cone_geometry.h"
int main(){try{
    uint32_t seed=9123;double sum_x=0,sum_y=0,sum_c=0;unsigned quadrants[4]{};
    for(unsigned i=0;i<100000;++i){
        float u=native_random_sample(seed),v=native_random_sample(seed);
        for(float pitch:{-1.55334f,0.0f,1.55334f}){
            float direction[3]={0,std::sin(pitch)*50,std::cos(pitch)*50};
            float good[3],poor[3];require(cone_velocity(direction,.025f,u,v,good)&&cone_velocity(direction,.25f,u,v,poor),"Cone at near-vertical aim");
            float norm=0,cgood=0,cpoor=0;for(unsigned k=0;k<3;++k){norm+=poor[k]*poor[k];cgood+=good[k]*direction[k]/2500;cpoor+=poor[k]*direction[k]/2500;}
            require(std::abs(norm-2500)<.002f&&cpoor>=std::cos(.25f)-.000001f&&cgood>=cpoor-.000001f,"Native speed and monotonic accuracy bounds");
            if(pitch==0){sum_x+=poor[0];sum_y+=poor[1];sum_c+=cpoor;++quadrants[(poor[0]>0?1:0)|(poor[1]>0?2:0)];}
        }
    }
    require(std::abs(sum_x/100000)<.05&&std::abs(sum_y/100000)<.05,"Cone has no directional bias");
    require(std::abs(sum_c/100000-(1+std::cos(.25))/2)<.0002,"Uniform solid-angle distribution");
    for(auto q:quadrants)require(q>24000&&q<26000,"All cone quadrants populated");
    float initial[3]={1,2,3},out[3]={9,8,7};
    require(cone_velocity(initial,0,.5f,.5f,out)&&std::abs(out[0]-1)<.000001f,"Zero spread keeps solved direction");
    out[0]=9;require(!cone_velocity(initial,NAN,.5f,.5f,out)&&out[0]==9,"Invalid input makes no write");
    std::cout<<"PASS: 300000 cone samples, speed conservation, accuracy monotonicity, vertical aiming, angular bounds, unbiased uniform distribution, zero/invalid spread.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
