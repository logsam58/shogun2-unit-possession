#define wmain extractor_entry159
#include "prepare_local_assets159.cpp"
#undef wmain
template<class F>static void rejects159(F f){bool rejected=false;try{f();}catch(...){rejected=true;}need159(rejected,"Malformed input was accepted");}
int wmain(){try{
    auto root=std::filesystem::temp_directory_path()/(L"s2fp-assets159-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetTickCount64()));
    need159(std::filesystem::create_directory(root),"Cannot create test directory");
    Bytes159 pixel(22);pixel[2]=2;pixel[12]=1;pixel[14]=1;pixel[16]=32;pixel[17]=32;pixel[18]=10;pixel[19]=20;pixel[20]=30;pixel[21]=128;
    auto image=tga159(pixel);need159(image.w==1&&image.h==1&&image.pixels==Bytes159({10,20,30,128}),"TGA channels/origin changed");
    for(unsigned n=0;n<22;++n)rejects159([&]{tga159(Bytes159(pixel.begin(),pixel.begin()+n));});
    auto rle=pixel;rle[2]=10;rle.insert(rle.begin()+18,128);need159(tga159(rle).pixels==image.pixels,"RLE differs");rle[18]=129;rejects159([&]{tga159(rle);});
    auto large=pixel;large[12]=255;large[13]=255;rejects159([&]{tga159(large);});
    Bundle159 q(2);q.add(101,Bytes159{1,2,3,4},1,1);for(unsigned i=201;i<=220;++i)q.add(i,Bytes159{1,2,3});q.save(root/L"valid.s2fp");
    assets159::Store store;need159(store.load((root/L"valid.s2fp").wstring(),2),"Valid bundle failed");
    need159(!store.load((root/L"valid.s2fp").wstring(),1),"Foreign bundle kind accepted");
    for(unsigned position:{0u,8u,12u,16u,20u,84u,96u,100u,104u,168u}){
        auto changed=q.bytes;changed[position]^=1;auto file=root/(std::to_wstring(position)+L".s2fp");write159(file,changed);
        need159(!store.load(file.wstring(),2),"Corrupt bundle accepted");
    }
    auto short_file=root/L"short.s2fp";auto changed=q.bytes;changed.pop_back();write159(short_file,changed);need159(!store.load(short_file.wstring(),2),"Truncated bundle accepted");
    need159(!store.load((root/L"missing.s2fp").wstring(),2),"Missing bundle accepted");
    Bytes159 header(32);memcpy(header.data(),"PFH3",4);unsigned count=1,size=8;memcpy(header.data()+16,&count,4);memcpy(header.data()+20,&size,4);
    header.insert(header.end(),{3,0,0,0,'a','b','c',0,1,2,3});auto pack=root/L"valid.pack";write159(pack,header);Pack159 native(pack);
    auto hash=assets159::sha(header.data()+40,3);Input159 spec{L"", "abc",3,hash.c_str()};need159(native.get(spec)==Bytes159({1,2,3}),"Pack extraction differs");
    spec.sha="wrong";rejects159([&]{native.get(spec);});spec.entry="absent";rejects159([&]{native.get(spec);});
    for(unsigned offset:{0u,8u,12u,16u,20u,32u}){auto invalid=header;invalid[offset]=255;auto path=root/(std::to_wstring(offset)+L".pack");write159(path,invalid);rejects159([&]{Pack159 broken(path);});}
    std::cout<<"PASS: native TGA/RLE, bounded PFH3 reads, source hash/missing entry rejection, bundle kind/recipe/hash/bounds/truncation/missing rejection. Synthetic files retained at test directory.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
