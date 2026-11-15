#pragma once

namespace locale167 {
// Rasterize a system serif at the same pixel sizes as the native HUD fonts.
// The temporary bitmaps use the existing CUF reader and are never shipped.
inline std::vector<unsigned char> system_font(unsigned size) {
    using Bytes = std::vector<unsigned char>;
    struct ScreenFont {
        HDC dc = CreateCompatibleDC(nullptr);
        HFONT font{};
        HGDIOBJ previous{};
        ~ScreenFont() {
            if (previous) SelectObject(dc, previous);
            if (font) DeleteObject(font);
            if (dc) DeleteDC(dc);
        }
    } context;
    if (!context.dc) throw std::runtime_error("Cannot initialize the HUD fallback font");
    context.font = CreateFontW(-int(size), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        VARIABLE_PITCH | FF_ROMAN, L"Times New Roman");
    if (context.font) context.previous = SelectObject(context.dc, context.font);
    if (!context.previous || context.previous == HGDI_ERROR) {
        context.previous = nullptr;
        throw std::runtime_error("Cannot select the HUD fallback font");
    }

    struct Glyph {unsigned char top, advance, width, height;};
    constexpr size_t table_start = 32 + 65536 * 2;
    Bytes result(table_start, 0xff), bitmaps;
    std::vector<Glyph> metrics;
    std::vector<unsigned> offsets;
    memcpy(result.data(), "CUF0", 4);
    auto put16 = [&](size_t pos, unsigned value) {
        result[pos] = static_cast<unsigned char>(value);
        result[pos+1] = static_cast<unsigned char>(value >> 8);
    };
    auto put32 = [&](size_t pos, unsigned value) {
        put16(pos, value);
        put16(pos + 2, value >> 16);
    };
    const MAT2 transform = {{0,1}, {0,0}, {0,0}, {0,1}};
    auto capture = [&](wchar_t character) {
        WORD glyph{};
        if (GetGlyphIndicesW(context.dc, &character, 1, &glyph, GGI_MARK_NONEXISTING_GLYPHS) == GDI_ERROR ||
                glyph == 0xffff) return false;
        GLYPHMETRICS m{};
        const auto format = GGO_GRAY8_BITMAP | GGO_GLYPH_INDEX;
        const DWORD length = GetGlyphOutlineW(context.dc, glyph, format, &m, 0, nullptr, &transform);
        if (length == GDI_ERROR || length > 65536 || m.gmBlackBoxX > 255 || m.gmBlackBoxY > 255 ||
                m.gmptGlyphOrigin.y < -128 || m.gmptGlyphOrigin.y > 127 ||
                m.gmCellIncX < 0 || m.gmCellIncX > 255) return false;
        const unsigned stride = (m.gmBlackBoxX + 3u) & ~3u;
        const unsigned width = length ? m.gmBlackBoxX : 0;
        const unsigned height = length ? m.gmBlackBoxY : 0;
        Bytes gray(length);
        if (length && (stride * height > length ||
                GetGlyphOutlineW(context.dc, glyph, format, &m, length, gray.data(), &transform) != length))
            return false;
        put16(32 + size_t(character)*2, unsigned(metrics.size()));
        metrics.push_back({static_cast<unsigned char>(m.gmptGlyphOrigin.y),
            static_cast<unsigned char>(m.gmCellIncX), static_cast<unsigned char>(width),
            static_cast<unsigned char>(height)});
        offsets.push_back(unsigned(bitmaps.size()));
        for (unsigned y = 0; y < height; ++y)
            for (unsigned x = 0; x < width; ++x)
                bitmaps.push_back(static_cast<unsigned char>(std::min(64u, unsigned(gray[y*stride+x])) * 255 / 64));
        return true;
    };
    for (wchar_t c = 32; c <= 126; ++c)
        if (!capture(c)) throw std::runtime_error("The HUD fallback font is missing basic glyphs");
    for (wchar_t c = 160; c <= 0x024f; ++c) capture(c);
    for (wchar_t c = 0x0400; c <= 0x052f; ++c) capture(c);
    for (wchar_t c : {L'\u2013', L'\u2014', L'\u2018', L'\u2019', L'\u201c', L'\u201d', L'\u2026'}) capture(c);

    put16(26, unsigned(metrics.size()));
    put32(28, unsigned(bitmaps.size()));
    result.resize(table_start + metrics.size()*8);
    for (size_t i = 0; i < metrics.size(); ++i) {
        const auto& g = metrics[i];
        const auto at = table_start + i*4;
        result[at] = g.top;
        result[at+1] = g.advance;
        result[at+2] = g.width;
        result[at+3] = g.height;
        put32(table_start + metrics.size()*4 + i*4, offsets[i]);
    }
    result.insert(result.end(), bitmaps.begin(), bitmaps.end());
    result.resize(result.size() + 4, 0);
    if (!font(result)) throw std::runtime_error("Generated HUD fallback font failed validation");
    return result;
}
}
