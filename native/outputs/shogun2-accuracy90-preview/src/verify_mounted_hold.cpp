#include "identity.h"
#include "mounted_hold_geometry.h"
#include <limits>
static void same(const float* a,const float* b,const char* why){for(unsigned i=0;i<12;++i)require(std::abs(a[i]-b[i])<.00003f,why);}
int main(){try{
    float reference[48*12],m[48*12],input[48*12];memcpy(reference,SEATED_BOW_MODEL,sizeof reference);
    int32_t parents[48];memcpy(parents,SEATED_BOW_PARENTS,sizeof parents);
    const unsigned weapons[3]={1,2,3};heldpose::Key key{1,2,3,1,1};heldpose::Cache cache;
    memcpy(m,reference,sizeof m);require(heldpose::apply(cache,key,m,48,parents,7,weapons),"Cannot hold decoded native rider palette");
    for(unsigned i=0;i<48;++i)same(m+i*12,reference+i*12,"Initial held pose changed");
    for(unsigned step=0;step<150;++step){
        float movement[12]={1,0,0,0,0,1,0,.001f*step,0,0,1,.002f*step};
        for(unsigned i=0;i<48;++i)affine_product(movement,reference+i*12,input+i*12);
        // Native upper interpolation changes while the simulation frame stays held.
        for(unsigned i=0;i<48;++i)if(cache.mask[i]){input[i*12+3]+=.06f*std::sin(float(step));input[i*12+11]-=.04f*std::cos(float(step));}
        memcpy(m,input,sizeof m);require(heldpose::apply(cache,key,m,48,parents,7,weapons),"Hold rejected native pelvis motion");
        for(unsigned i=0;i<48;++i){
            float expected[12];if(cache.mask[i])affine_product(movement,reference+i*12,expected);else memcpy(expected,input+i*12,48);
            same(m+i*12,expected,"Held arms/equipment jittered, or native lower body was modified");
        }
        float repeat[48*12];memcpy(repeat,m,sizeof m);require(heldpose::apply(cache,key,m,48,parents,7,weapons),"Repeated hold rejected");
        for(unsigned i=0;i<48;++i)same(m+i*12,repeat+i*12,"Held transforms accumulate on repeated calls");
    }
    ++key.draw;memcpy(m,input,sizeof m);require(heldpose::apply(cache,key,m,48,parents,7,weapons),"New draw not captured");
    for(unsigned i=0;i<48;++i)same(m+i*12,input+i*12,"New draw retained prior pose");
    ++key.lease;m[1*12+3]+=.2f;memcpy(input,m,sizeof m);require(heldpose::apply(cache,key,m,48,parents,7,weapons),"New actor lease not captured");
    for(unsigned i=0;i<48;++i)same(m+i*12,input+i*12,"Old lease pose leaked into new actor");
    cache.reset();require(!cache.valid,"Release did not retire palette");
    memcpy(m,reference,sizeof m);parents[7]=999;memcpy(input,m,sizeof m);
    require(!heldpose::apply(cache,key,m,48,parents,7,weapons)&&!memcmp(m,input,sizeof m),"Invalid rig wrote output");parents[7]=SEATED_BOW_PARENTS[7];
    m[3]=std::numeric_limits<float>::quiet_NaN();memcpy(input,m,sizeof m);
    require(!heldpose::apply(cache,key,m,48,parents,7,weapons)&&!memcmp(m,input,sizeof m),"Nonfinite rig wrote output");
    memcpy(m,reference,sizeof m);for(unsigned i=0;i<12;++i)m[i]=0;memcpy(input,m,sizeof m);
    require(!heldpose::apply(cache,key,m,48,parents,7,weapons)&&!memcmp(m,input,sizeof m),"Singular pelvis wrote output");
    std::cout<<"PASS: decoded native mounted held palette rejects upper interpolation jitter, preserves pelvis/legs/horse motion, no transform accumulation, new draw/lease recapture and malformed/nonfinite/singular guards.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
