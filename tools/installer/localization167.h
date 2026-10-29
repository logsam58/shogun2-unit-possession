#pragma once
// Validate locally installed CUF fonts before allowing localized variants.
// No font pixels, system-font replacement or translated voice crop is shipped.
namespace locale167 {
inline bool font(const std::vector<unsigned char>& bytes) {
    if(bytes.size()<0x20024||bytes.size()>32u*1024*1024||memcmp(bytes.data(),"CUF0",4))return false;
    auto u16=[&](size_t p){return unsigned(bytes[p])|(unsigned(bytes[p+1])<<8);};
    auto u32=[&](size_t p){return uint64_t(u16(p))|(uint64_t(u16(p+2))<<16);};
    unsigned count=u16(26);uint64_t start=0x20020ull+uint64_t(count)*8;
    uint64_t end=start+u32(28);
    if(!count||start>bytes.size()||end+4>bytes.size())return false;
    unsigned kern=u16(size_t(end)),skip=u16(size_t(end)+2);
    if(uint64_t(kern)+skip>count||end+4+uint64_t(kern)*kern!=bytes.size())return false;
    for(unsigned i=0;i<count;++i){
        size_t d=0x20020+size_t(i)*4;
        uint64_t offset=u32(0x20020+size_t(count)*4+size_t(i)*4);
        if(offset+uint64_t(bytes[d+2])*bytes[d+3]>end-start)return false;
    }
    // The current HUD uses Latin labels; '?' must also be a safe fallback.
    for(unsigned c=32;c<=126;++c)if(u16(32+c*2)>=count)return false;
    return true;
}
inline bool pack_name(std::wstring name) {
    std::transform(name.begin(),name.end(),name.begin(),towlower);
    if(name.size()<11||name.size()>40||name.rfind(L"local_",0)||name.substr(name.size()-5)!=L".pack")return false;
    for(size_t i=6;i<name.size()-5;++i)if(!((name[i]>=L'a'&&name[i]<=L'z')||name[i]==L'_'||name[i]==L'-'))return false;
    return true;
}
}
