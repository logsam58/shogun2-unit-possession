#define wmain extractor_entry159
#include "prepare_local_assets159.cpp"
#undef wmain
#include <iostream>
int wmain(int argc,wchar_t** argv){try{
    need159(argc==2,"Use font checks <owned-game>");auto fonts=local_fonts167(argv[1]);
    need159(fonts.size()==20,"Missing native fonts");
    for(const auto& font:fonts)need159(locale167::font(font),"Original native font rejected");
    auto bytes=fonts.front();
    for(unsigned size:{0u,4u,0x20020u})need159(!locale167::font(Bytes159(bytes.begin(),bytes.begin()+size)),"Truncated native font accepted");
    for(unsigned offset:{0u,26u,28u,32u+L'?'*2u}){
        auto bad=bytes;bad[offset]=255;bad[offset+1]=255;need159(!locale167::font(bad),"Malformed font layout/glyph accepted");
    }
    for(auto name:{L"local_fr.pack",L"local_zh.pack",L"local_ru.pack"})need159(locale167::pack_name(name),"Native localization filename rejected");
    for(auto name:{L"foreign.pack",L"local_../x.pack",L"local_x.exe"})need159(!locale167::pack_name(name),"Unrelated locale candidate accepted");
    std::cout<<"PASS 20 original fonts, malformed bounds/glyphs and localization filename policy.\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
