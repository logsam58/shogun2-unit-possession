// Runs during setup, outside the game. All input packs are opened read-only.
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlobj.h>
#include <wrl/client.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include "../../native/common/local_assets159.h"
#include "asset_recipe159.generated.h"
using Microsoft::WRL::ComPtr;
using Bytes159=std::vector<unsigned char>;
static void need159(bool value,const char* why){if(!value)throw std::runtime_error(why);}
static Bytes159 read159(const std::filesystem::path& p,size_t limit){std::ifstream f(p,std::ios::binary);need159(bool(f),"Cannot read local game input");auto n=std::filesystem::file_size(p);need159(n<=limit,"Local input exceeds bounded size");Bytes159 b(size_t(n),0);need159(bool(f.read((char*)b.data(),b.size()))||b.empty(),"Truncated local input");return b;}
static void write159(const std::filesystem::path& p,const Bytes159& b){
    HANDLE f=CreateFileW(p.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,nullptr);
    need159(f!=INVALID_HANDLE_VALUE,"Existing output preserved or staging file unavailable");DWORD written=0;
    bool ok=b.size()<=MAXDWORD&&WriteFile(f,b.data(),DWORD(b.size()),&written,nullptr)&&written==b.size()&&FlushFileBuffers(f);
    CloseHandle(f);need159(ok,"Cannot write local resource staging file");
}
static void plain159(const std::filesystem::path& p){
    auto absolute=std::filesystem::absolute(p).lexically_normal();need159(absolute.wstring().size()>3&&absolute.wstring()[1]==L':',"Use a local drive path");
    std::filesystem::path current;for(auto& part:absolute){current/=part;auto attr=GetFileAttributesW(current.c_str());
        if(attr!=INVALID_FILE_ATTRIBUTES)need159(!(attr&FILE_ATTRIBUTE_REPARSE_POINT),"Reparse resource path refused; original preserved");
        else need159(GetLastError()==ERROR_FILE_NOT_FOUND||GetLastError()==ERROR_PATH_NOT_FOUND,"Cannot inspect resource path");}
}
static unsigned u16_159(const unsigned char* b){return unsigned(b[0])|(unsigned(b[1])<<8);}
struct Pack159 {
    std::filesystem::path path;std::map<std::string,std::pair<uint64_t,unsigned>> entries;
    explicit Pack159(std::filesystem::path p):path(p){
        std::ifstream f(path,std::ios::binary);need159(bool(f),"Required Shogun 2 pack is missing");unsigned char h[32];need159(bool(f.read((char*)h,32)),"Truncated pack header");
        unsigned flags=assets159::u32(h+4),count=assets159::u32(h+16),size=assets159::u32(h+20);auto total=std::filesystem::file_size(path);
        need159(!memcmp(h,"PFH3",4)&&(flags==0||flags==1||flags==3)&&!assets159::u32(h+8)&&!assets159::u32(h+12)&&count<=100000&&size<=16*1024*1024&&32ull+size<=total,"Unsupported pack index; original preserved");
        Bytes159 index(size);need159(bool(f.read((char*)index.data(),size)),"Truncated pack index");size_t cursor=0;uint64_t offset=32ull+size;std::set<std::string> seen;
        for(unsigned i=0;i<count;++i){need159(cursor+4<=size,"Truncated pack entry");unsigned bytes=assets159::u32(index.data()+cursor);cursor+=4;auto start=cursor;while(cursor<size&&index[cursor])++cursor;need159(cursor<size,"Unterminated pack path");
            std::string name((const char*)index.data()+start,cursor-start);++cursor;std::replace(name.begin(),name.end(),'\\','/');std::string key=name;std::transform(key.begin(),key.end(),key.begin(),[](unsigned char x){return char(tolower(x));});
            need159(!name.empty()&&name.front()!='/'&&name.find(':')==name.npos&&name.find("//")==name.npos&&name.find("../")==name.npos&&name.find("/..") == name.npos&&name.find("./")==name.npos&&seen.insert(key).second&&offset+bytes<=total,"Unsafe/duplicate/out-of-range pack entry");
            entries.emplace(name,std::make_pair(offset,bytes));offset+=bytes;
        }
        need159(cursor==size,"Pack index was not fully consumed");
    }
    Bytes159 get(const Input159& input){auto it=entries.find(input.entry);need159(it!=entries.end()&&it->second.second==input.bytes&&input.bytes<32*1024*1024,"Required game resource is missing or changed");std::ifstream f(path,std::ios::binary);f.seekg(it->second.first);Bytes159 b(input.bytes);need159(bool(f.read((char*)b.data(),b.size())),"Truncated resource");need159(assets159::sha(b.data(),b.size())==input.sha,"Original game resource hash differs; repair the supported game files");return b;}
};
struct Picture159 {unsigned w{},h{};Bytes159 pixels;};
static Picture159 tga159(const Bytes159& b){
    need159(b.size()>=18,"Truncated TGA");unsigned cmap=b[1],type=b[2],start=u16_159(b.data()+3),colors=u16_159(b.data()+5),palette_bits=b[7],w=u16_159(b.data()+12),h=u16_159(b.data()+14),bits=b[16],desc=b[17];
    need159(w&&h&&w<=4096&&h<=4096&&(type==1||type==2||type==3||type==9||type==10||type==11),"Unsupported TGA image");size_t p=18+b[0];need159(p<=b.size(),"Truncated TGA identifier");
    auto color=[&](unsigned depth){need159((depth==8||depth==16||depth==24||depth==32)&&p+depth/8<=b.size(),"Invalid TGA pixel");unsigned c=0;if(depth==8){c=0xff000000u|unsigned(b[p])*0x10101u;}else if(depth==16){unsigned x=u16_159(b.data()+p);c=0xff000000u|((x&31)*255/31)|(((x>>5)&31)*255/31<<8)|(((x>>10)&31)*255/31<<16);}else{c=unsigned(b[p])|(unsigned(b[p+1])<<8)|(unsigned(b[p+2])<<16)|(unsigned(depth==32?b[p+3]:255)<<24);}p+=depth/8;return c;};
    std::vector<unsigned> palette;if(cmap){need159(cmap==1&&colors&&colors<=65536,"Invalid TGA palette");for(unsigned i=0;i<colors;++i)palette.push_back(color(palette_bits));}
    Picture159 out{w,h,Bytes159(size_t(w)*h*4)};size_t done=0;
    auto pixel=[&](){if(type==1||type==9){need159((bits==8||bits==16)&&p+bits/8<=b.size(),"Invalid TGA index");unsigned n=bits==8?b[p]:u16_159(b.data()+p);p+=bits/8;need159(n>=start&&n-start<palette.size(),"TGA index outside palette");return palette[n-start];}return color(bits);};
    while(done<size_t(w)*h){unsigned n=1;bool repeat=false;if(type>=9){need159(p<b.size(),"Truncated TGA RLE");unsigned packet=b[p++];n=(packet&127)+1;repeat=(packet&128)!=0;}need159(n<=size_t(w)*h-done,"TGA RLE exceeds image");unsigned c=repeat?pixel():0;
        for(unsigned i=0;i<n;++i){if(!repeat)c=pixel();unsigned x=unsigned(done%w),y=unsigned(done/w);if(desc&16)x=w-1-x;if(!(desc&32))y=h-1-y;memcpy(out.pixels.data()+4*(size_t(y)*w+x),&c,4);++done;}}
    return out;
}
static void put_u32_159(Bytes159& b,unsigned x){auto p=(unsigned char*)&x;b.insert(b.end(),p,p+4);}
struct Bundle159 {Bytes159 bytes;unsigned count{};Bundle159(unsigned kind){bytes.insert(bytes.end(),{'S','2','F','P','A','S','1',0});put_u32_159(bytes,159);put_u32_159(bytes,kind);put_u32_159(bytes,0);bytes.insert(bytes.end(),assets159::recipe,assets159::recipe+64);}
    void add(unsigned id,const Bytes159& data,unsigned w=0,unsigned h=0){put_u32_159(bytes,id);put_u32_159(bytes,w);put_u32_159(bytes,h);put_u32_159(bytes,unsigned(data.size()));put_u32_159(bytes,w?1:0);auto hash=assets159::sha(data.data(),data.size());bytes.insert(bytes.end(),hash.begin(),hash.end());bytes.insert(bytes.end(),data.begin(),data.end());++count;}
    void save(const std::filesystem::path& p){memcpy(bytes.data()+16,&count,4);write159(p,bytes);}
};
static Bytes159 wave159(const std::filesystem::path& mp3){
    struct Platform {bool com{},mf{};~Platform(){if(mf)MFShutdown();if(com)CoUninitialize();}} platform;
    need159(SUCCEEDED(CoInitializeEx(nullptr,COINIT_MULTITHREADED)),"Windows audio initialization failed");platform.com=true;
    need159(SUCCEEDED(MFStartup(MF_VERSION)),"Windows Media Foundation is unavailable; Windows N editions need the Media Feature Pack");platform.mf=true;
    ComPtr<IMFSourceReader> reader;need159(SUCCEEDED(MFCreateSourceReaderFromURL(mp3.c_str(),nullptr,&reader)),"Windows could not decode the original advisor MP3");
    reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS,FALSE);reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM,TRUE);
    ComPtr<IMFMediaType> format;need159(SUCCEEDED(MFCreateMediaType(&format)),"Cannot create Windows audio format");format->SetGUID(MF_MT_MAJOR_TYPE,MFMediaType_Audio);format->SetGUID(MF_MT_SUBTYPE,MFAudioFormat_PCM);format->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE,16);format->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS,1);format->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND,44100);
    need159(SUCCEEDED(reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM,nullptr,format.Get())),"Windows audio conversion to mono 44.1kHz PCM failed");
    ComPtr<IMFMediaType> actual;need159(SUCCEEDED(reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM,&actual)),"Cannot inspect decoded audio format");UINT32 rate=0,ch=0,bits=0;actual->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND,&rate);actual->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS,&ch);actual->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE,&bits);need159(rate==44100&&ch==1&&bits==16,"Unexpected decoded voice format");
    Bytes159 pcm;for(unsigned i=0;i<10000;++i){DWORD flags=0;LONGLONG stamp=0;ComPtr<IMFSample> sample;need159(SUCCEEDED(reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM,0,nullptr,&flags,&stamp,&sample)),"Cannot decode advisor audio");
        if(sample){ComPtr<IMFMediaBuffer> buffer;need159(SUCCEEDED(sample->ConvertToContiguousBuffer(&buffer)),"Cannot read advisor PCM");BYTE* bytes=nullptr;DWORD size=0;need159(SUCCEEDED(buffer->Lock(&bytes,nullptr,&size)),"Cannot lock advisor PCM");pcm.insert(pcm.end(),bytes,bytes+size);buffer->Unlock();need159(pcm.size()<4*1024*1024,"Advisor decode exceeded its bound");}
        if(flags&MF_SOURCE_READERF_ENDOFSTREAM)break;need159(i<9999,"Advisor decode did not terminate");}
    // Same 3.25s..4.80s native phrase, without the routing sentence. No audio
    // sample is shipped. Windows decoder rounding may differ from FFmpeg.
    const unsigned begin=143325*2,length=68355*2;need159(pcm.size()>=begin+length,"Decoded advisor is too short");Bytes159 out;
    auto tag=[&](const char* s){out.insert(out.end(),s,s+4);};tag("RIFF");put_u32_159(out,36+length);tag("WAVE");tag("fmt ");put_u32_159(out,16);out.insert(out.end(),{1,0,1,0});put_u32_159(out,44100);put_u32_159(out,88200);out.insert(out.end(),{2,0,16,0});tag("data");put_u32_159(out,length);out.insert(out.end(),pcm.begin()+begin,pcm.begin()+begin+length);return out;
}
int wmain(int argc,wchar_t** argv){try{
    need159(argc==3,"Use prepare-local-assets <game-folder> <new-staging-folder>");std::filesystem::path game=argv[1],out=argv[2];
    need159(game.is_absolute()&&out.is_absolute(),"Use absolute local paths");plain159(game);plain159(out);plain159(game/L"Empire.Retail.dll");
    auto engine=read159(game/L"Empire.Retail.dll",64*1024*1024);need159(assets159::sha(engine.data(),engine.size())=="022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5","Unsupported original Shogun 2 installation");
    need159(!std::filesystem::exists(out),"Existing resource output preserved; use a new directory");auto g=std::filesystem::weakly_canonical(game).wstring(),o=std::filesystem::weakly_canonical(out.parent_path()).wstring();
    std::transform(g.begin(),g.end(),g.begin(),towlower);std::transform(o.begin(),o.end(),o.begin(),towlower);need159(o!=g&&o.rfind(g+L"\\",0)!=0,"Resource staging must be outside the game directory");
    std::filesystem::create_directories(out);std::map<std::wstring,std::unique_ptr<Pack159>> packs;std::map<unsigned,Bytes159> inputs;std::map<unsigned,Picture159> images;
    auto input=[&](unsigned i)->const Bytes159&{auto found=inputs.find(i);if(found!=inputs.end())return found->second;auto& spec=inputs159[i];auto& p=packs[spec.pack];if(!p){plain159(game/L"data"/spec.pack);p=std::make_unique<Pack159>(game/L"data"/spec.pack);}return inputs.emplace(i,p->get(spec)).first->second;};
    auto picture=[&](unsigned i)->const Picture159&{auto found=images.find(i);if(found==images.end())found=images.emplace(i,tga159(input(i))).first;return found->second;};
    Bundle159 hud(1),qte(2);
    auto atlas=[&](Bundle159& bundle,const Image159* rows,size_t count,unsigned number,unsigned limit){unsigned w=0,h=0;for(size_t i=0;i<count;++i)if(rows[i].atlas==int(number)){w=std::max(w,unsigned(rows[i].x+rows[i].w+2));h=std::max(h,unsigned(rows[i].y+rows[i].h+2));}need159(w&&h&&w<=limit&&h<=limit,"Atlas placement exceeds accepted dimensions");Picture159 result{w,h,Bytes159(size_t(w)*h*4)};
        for(size_t i=0;i<count;++i){auto& row=rows[i];if(row.atlas!=int(number))continue;auto& image=picture(row.input);need159(image.w==unsigned(row.lw)&&image.h==unsigned(row.lh),"Native image dimensions changed");for(int y=0;y<row.h;++y)for(int x=0;x<row.w;++x){int sx=row.ox+x,sy=row.oy+y;if(sx>=0&&sy>=0&&unsigned(sx)<image.w&&unsigned(sy)<image.h)memcpy(result.pixels.data()+4*(size_t(row.y+y)*w+row.x+x),image.pixels.data()+4*(size_t(sy)*image.w+sx),4);}}
        bundle.add(101+number,result.pixels,w,h);};
    for(unsigned i=0;i<3;++i)atlas(hud,hud159,std::size(hud159),i,2048);atlas(qte,qte159,std::size(qte159),0,512);
    for(unsigned i=0;i<20;++i){auto& b=input(fonts159[i]);need159(b.size()>0x20020&&!memcmp(b.data(),"CUF0",4),"Invalid local font");hud.add(201+i,b);qte.add(201+i,b);}
    for(unsigned i=0;i<2;++i){auto& image=picture(abilities159[i]);hud.add(430+i,image.pixels,image.w,image.h);}
    auto mp3=out/L"advisor-source.tmp.mp3";write159(mp3,input(voice159));auto wav=wave159(mp3);std::filesystem::remove(mp3);hud.add(301,wav);
    hud.save(out/L"hud-assets159.s2fp");qte.save(out/L"qte-assets159.s2fp");assets159::Store a,b;need159(a.load((out/L"hud-assets159.s2fp").wstring(),1)&&b.load((out/L"qte-assets159.s2fp").wstring(),2),"Generated resources failed validation");
    std::cout<<"Prepared local Shogun 2 artwork, cards, 20 fonts and cropped advisor voice. All "<<inputs.size()<<" source entries hash-verified. Game files unchanged.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
