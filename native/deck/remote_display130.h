#pragma once
#include "device192.h"
#include "../outputs/shogun2-possession-cleanup-v46/src/game_window105.h"
#include <algorithm>
#include <cstddef>
#include <cstring>

namespace remote131 {

constexpr int deck_width = 1280, deck_height = 800;
constexpr UINT_PTR timer_id = 0x53323131;

template<class T> inline bool read(uintptr_t address, T& value) {
    SIZE_T received{};
    return address && ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(address),
        &value, sizeof value, &received) && received == sizeof value;
}

template<size_t N> inline bool matches(uintptr_t address, const unsigned char (&expected)[N]) {
    unsigned char actual[N]{};
    return read(address, actual) && std::memcmp(actual, expected, N) == 0;
}

// Warscape stores the committed dimensions separately from its pending mode.
// Only the game’s renderer thread may dispatch a display change.
struct Mode {
    uintptr_t table;
    unsigned reserved;
    int width, height, viewport_width, viewport_height;
    unsigned char pending, padding[3], windowed, desktop_size;
};
static_assert(offsetof(Mode, pending) == 0x18 && offsetof(Mode, windowed) == 0x1c);
struct Size { int width, height; };
using SetWindowed = void (__thiscall*)(void*, bool);
using RequestSize = void (__thiscall*)(void*, const Size*);
using Apply = void (__thiscall*)(void*);
using Ready = bool (__thiscall*)(void*);

struct Engine {
    uintptr_t renderer{}, mode{}, set_windowed{}, request_size{}, apply{};

    bool bind(uintptr_t base, HWND window, Mode& original) {
        constexpr unsigned char set_bytes[]{0x8b,0x01,0xff,0x90,0x08,0x03,0x00,0x00,0x8b,0xc8,0xe9,0x21,0x62,0x00,0x00};
        constexpr unsigned char size_bytes[]{0x8b,0x54,0x24,0x04,0xc6,0x41,0x18,0x01,0x8b,0x02,0x89,0x41,0x20,0x8b,0x42,0x04,0x89,0x41,0x24,0xc2,0x04,0x00};
        constexpr unsigned char apply_bytes[]{0xe9,0x5b,0xaa,0xfe,0xff};
        DWORD owner{};
        HWND actual{};
        uintptr_t table{}, backend{}, backend_table{}, ready{}, method{};
        set_windowed = base + 0x141eae0;
        request_size = base + 0x141f3f0;
        apply = base + 0x1416cf0;
        if (!base || !read(base + 0x1c9a940, owner) || owner != GetCurrentThreadId() ||
            !read(base + 0x1c9a938, actual) || actual != window ||
            !read(base + 0x17c7560, renderer) ||
            !read(renderer, table) || table != base + 0x1754574 ||
            !read(table + 0xa8, method) || method != set_windowed ||
            !read(table + 0xb0, method) || method != apply ||
            !read(table + 0x308, method) || method != base + 0x13dbce0 ||
            !read(base + 0x1c9a924, mode) || !read(mode, original) ||
            original.table != base + 0x1754560 || original.pending || original.desktop_size ||
            original.windowed > 1 || original.width < 640 || original.height < 360 ||
            original.width > 16384 || original.height > 16384 ||
            !matches(set_windowed, set_bytes) || !matches(request_size, size_bytes) ||
            !matches(apply, apply_bytes) ||
            !read(base + 0x1c9a93c, backend) || !read(backend, backend_table))
            return false;
        if (backend_table == base + 0x17666f8) ready = base + 0x1456710;
        if (backend_table == base + 0x1766c68) ready = base + 0x1467d20;
        return ready && read(backend_table + 0x70, method) && method == ready &&
               reinterpret_cast<Ready>(ready)(reinterpret_cast<void*>(backend));
    }

    bool change(bool windowed, Size size) const {
        reinterpret_cast<SetWindowed>(set_windowed)(reinterpret_cast<void*>(renderer), windowed);
        reinterpret_cast<RequestSize>(request_size)(reinterpret_cast<void*>(mode), &size);
        reinterpret_cast<Apply>(apply)(reinterpret_cast<void*>(renderer));
        Mode current{};
        return read(mode, current) && !current.pending && (current.windowed != 0) == windowed &&
               current.width == size.width && current.height == size.height;
    }
};

inline bool client_is(HWND window, Size size) {
    RECT client{};
    return GetClientRect(window, &client) && client.right == size.width && client.bottom == size.height;
}

inline bool position(HWND window, Size size) {
    MONITORINFO monitor{sizeof monitor};
    RECT frame{0, 0, size.width, size.height};
    const DWORD style = DWORD(GetWindowLongW(window, GWL_STYLE));
    const DWORD exstyle = DWORD(GetWindowLongW(window, GWL_EXSTYLE));
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor) ||
        !AdjustWindowRectEx(&frame, style, GetMenu(window) != nullptr, exstyle))
        return false;
    const int width = frame.right - frame.left;
    const int height = frame.bottom - frame.top;
    const RECT area = monitor.rcWork;
    const int left = area.left + std::max(0L, (area.right - area.left - width) / 2);
    const int top = area.top + std::max(0L, (area.bottom - area.top - height) / 2);
    if (IsZoomed(window)) ShowWindow(window, SW_RESTORE);
    return SetWindowPos(window, nullptr, left, top, width, height, SWP_NOACTIVATE | SWP_NOZORDER) &&
           client_is(window, size);
}

inline bool apply_display(HWND window, const Engine& engine, const Mode& original) {
    constexpr Size target{deck_width, deck_height};
    if (original.windowed && original.width == target.width && original.height == target.height &&
        client_is(window, target)) return true;

    WINDOWPLACEMENT placement{sizeof placement};
    if (!GetWindowPlacement(window, &placement)) return false;

    // The renderer samples client dimensions before releasing the old device.
    // Leaving fullscreen can change the window during that release.
    if (position(window, target)) {
        bool committed = engine.change(true, target);
        Mode settled{};
        if (!committed && read(engine.mode, settled) && settled.windowed && !settled.pending &&
            !IsIconic(window) && position(window, target)) {
            // A driver can restore the old bounds before the windowed mode is sampled.
            // Retry once after exclusive fullscreen has been released.
            committed = engine.change(true, target);
        }
        if (committed) {
            if (!IsIconic(window) && !client_is(window, target)) position(window, target);
            return true;
        }
    }

    SetWindowPlacement(window, &placement);
    Mode after{};
    if (read(engine.mode, after) && (after.pending || after.windowed != original.windowed ||
        after.width != original.width || after.height != original.height))
        engine.change(original.windowed != 0, {original.width, original.height});
    return false;
}

inline void CALLBACK apply_on_window_thread(HWND window, UINT, UINT_PTR id, DWORD) {
    KillTimer(window, id);
    if (!gamewindow105::belongs(window, GetCurrentProcessId()) ||
        !IsWindowVisible(window) || IsIconic(window) ||
        !remote130::direct_sessions(GetModuleHandleW(L"steam_api.dll")).size)
        return;
    Engine engine;
    Mode original{};
    if (engine.bind(reinterpret_cast<uintptr_t>(GetModuleHandleW(L"Empire.Retail.dll")), window, original))
        apply_display(window, engine, original);
}


inline void queue() {
    static HWND scheduled{};
    if ((scheduled && IsWindow(scheduled)) || !device192::streaming()) return;
    const HWND window = gamewindow105::find(GetCurrentProcessId());
    if (!window || !IsWindowVisible(window) || IsIconic(window) ||
        GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) return;

    // A queued WM_TIMER may outlive the Lua root that requested it.
    HMODULE module{};
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&apply_on_window_thread), &module) &&
        SetTimer(window, timer_id, USER_TIMER_MINIMUM, &apply_on_window_thread))
        scheduled = window;
}

} // namespace remote131
