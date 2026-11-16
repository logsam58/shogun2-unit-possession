#pragma once
// Validate locally installed CUF fonts before allowing localized variants.
#include "../../native/deck/proton_paths174.h"
#include <fstream>
#include <string_view>
namespace locale167 {
inline bool font(const std::vector<unsigned char>& bytes) {
    constexpr size_t glyph_table_start = 32 + 65536 * 2;
    if(bytes.size()<glyph_table_start+4||bytes.size()>32u*1024*1024||memcmp(bytes.data(),"CUF0",4))return false;
    auto u16=[&](size_t p){return unsigned(bytes[p])|(unsigned(bytes[p+1])<<8);};
    auto u32=[&](size_t p){return uint64_t(u16(p))|(uint64_t(u16(p+2))<<16);};
    unsigned count=u16(26);uint64_t start=uint64_t(glyph_table_start)+uint64_t(count)*8;
    uint64_t end=start+u32(28);
    if(!count||start>bytes.size()||end+4>bytes.size())return false;
    unsigned kern=u16(size_t(end)),skip=u16(size_t(end)+2);
    if(uint64_t(kern)+skip>count||end+4+uint64_t(kern)*kern!=bytes.size())return false;
    for(unsigned i=0;i<count;++i){
        size_t d=glyph_table_start+size_t(i)*4;
        uint64_t offset=u32(glyph_table_start+size_t(count)*4+size_t(i)*4);
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

struct Installed {
    std::wstring active;
    std::vector<std::filesystem::path> packs;
};
inline Installed installed(const std::filesystem::path& data) {
    Installed result;
    const auto language = data / L"language.txt";
    if (std::filesystem::is_regular_file(language)) {
        try {
            deck174::paths::read_file(language);
            std::ifstream input(language, std::ios::binary);
            char bytes[17]{};
            input.read(bytes, 16);
            if (input.gcount() < 16) {
                std::string_view code(bytes, size_t(input.gcount()));
                if (code.substr(0, 3) == "\xef\xbb\xbf") code.remove_prefix(3);
                const auto end = code.find_last_not_of(" \t\r\n");
                if (end != code.npos && end == 1) {
                    for (unsigned char c : code.substr(0, 2)) {
                        if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
                        if (c >= 'a' && c <= 'z') result.active.push_back(c);
                    }
                    if (result.active.size() != 2) result.active.clear();
                }
            }
        } catch (const std::exception&) {
            // A missing or unreadable selection leaves the default pack order.
        }
    }
    const std::wstring preferred = L"local_" + result.active + L".pack";
    for (const auto& entry : std::filesystem::directory_iterator(data)) {
        const auto path = entry.path();
        if (!pack_name(path.filename().wstring())) continue;
        try {
            deck174::paths::read_file(path);
            if (entry.is_regular_file() && result.packs.size() < 32) result.packs.push_back(path);
        } catch (const std::exception&) {
            // Optional font packs can be ignored if unsafe or inaccessible.
        }
    }
    auto rank = [&](const std::filesystem::path& path) {
        auto name = path.filename().wstring();
        std::transform(name.begin(), name.end(), name.begin(), towlower);
        return name == preferred ? 0 : name == L"local_en.pack" ? 1 : 2;
    };
    std::sort(result.packs.begin(), result.packs.end(), [&](const auto& a, const auto& b) {
        const auto left = rank(a), right = rank(b);
        return left != right ? left < right : a < b;
    });
    return result;
}
}
