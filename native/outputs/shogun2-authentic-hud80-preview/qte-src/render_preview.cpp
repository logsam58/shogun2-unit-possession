#define NOMINMAX
#include "qte_render.h"
#include <iostream>
#include <memory>
int wmain(int argc,wchar_t** argv){
    using namespace Gdiplus;GdiplusStartupInput input;ULONG_PTR token;
    if(GdiplusStartup(&token,&input,nullptr)!=Ok)return 1;
    int result=1;
    {
        float scale=1.f;for(int n=2;n+1<argc;++n)if(!wcscmp(argv[n],L"--scale"))scale=float(_wtof(argv[n+1]));if(scale<.5f||scale>2.f)return 5;
        Bitmap bitmap(int(std::ceil(960*scale)),int(std::ceil(360*scale)),PixelFormat32bppARGB);Graphics g(&bitmap);g.Clear(Color(255,15,22,29));g.ScaleTransform(scale,scale);
        qte::UnitState neutral{qte::Required|qte::Strength,2,2,10,.5f,.8f,false};
        qte::UnitState strong{qte::Required|qte::Strength,0,0,12,1,1,false},weak{qte::Required|qte::Strength,5,7,8,0,.3f,false};
        qte::Pair pair{1,1,1,2,1,2};
        const bool results=argc>2&&!wcscmp(argv[2],L"--results");
        const bool interrupted=argc>2&&!wcscmp(argv[2],L"--interrupted");
        const bool timeout=argc>2&&!wcscmp(argv[2],L"--timeout");
        const bool timed=argc>3&&!wcscmp(argv[2],L"--at");
        const double age=timed?_wtof(argv[3]):0;
        if(timed&&(!std::isfinite(age)||age<0||age>3.1))return 4;
        for(unsigned i=0;i<3;++i){
            qte::Event event;auto own=i==0?strong:i==1?neutral:weak;auto enemy=i==0?weak:i==1?neutral:strong;
            auto action=i==1||(results&&i==2)?qte::Action::Defend:qte::Action::Attack;
            if(!event.begin(pair,action,own,enemy)){result=2;break;}
            auto s=event.snapshot();event.update(pair,own,enemy,timeout?qte::response_deadline(s)+.01:timed?age:s.ideal*(results?(i==2?.3:1.):.66),false);
            if(results)event.press(s.serial,action==qte::Action::Attack?qte::Button::Left:qte::Button::Right,true);
            auto shown=event.snapshot();if(interrupted){shown.result=qte::Result::Canceled;shown.active=true;}qte::render(g,160.f+320.f*i,180.f,shown);
        }
        UINT bytes,count;if(GetImageEncodersSize(&count,&bytes)==Ok){
            auto data=std::make_unique<unsigned char[]>(bytes);auto encoders=reinterpret_cast<ImageCodecInfo*>(data.get());
            if(GetImageEncoders(count,bytes,encoders)==Ok)for(unsigned i=0;i<count;++i)if(!wcscmp(encoders[i].MimeType,L"image/png")){
                result=bitmap.Save(argc>1?argv[1]:L"qte-preview.png",&encoders[i].Clsid,nullptr)==Ok?0:3;break;}
        }
    }
    qte::release_renderer();GdiplusShutdown(token);std::cout<<(result==0?"Rendered native QTE overlay preview.\n":"QTE render failed.\n");return result;
}
