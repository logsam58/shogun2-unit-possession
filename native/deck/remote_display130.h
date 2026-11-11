#pragma once
#include "device192.h"
#include "../outputs/shogun2-possession-cleanup-v46/src/game_window105.h"

namespace remote130 {

constexpr int deck_width = 1280, deck_height = 800;

// Use the stock window-resize path. The engine dispatches WM_SIZE to its
// renderer. Exclusive fullscreen and desktop display modes remain Steam's
// responsibility. This object owns only the placement it changed.
class Display {
    HWND window_{};
    WINDOWPLACEMENT original_{sizeof(WINDOWPLACEMENT)};
    RECT applied_{};
    uint64_t next_check_{};
    bool changed_{};
    bool applied_seen_{};

    static bool usable(HWND window) {
        return gamewindow105::belongs(window, GetCurrentProcessId()) &&
               IsWindowVisible(window) && !IsIconic(window) &&
               (GetWindowLongW(window, GWL_STYLE) & WS_CAPTION) == WS_CAPTION;
    }

    static bool target(HWND window, RECT& result) {
        const DWORD style = DWORD(GetWindowLongW(window, GWL_STYLE));
        const DWORD exstyle = DWORD(GetWindowLongW(window, GWL_EXSTYLE));
        MONITORINFO monitor{sizeof(MONITORINFO)};
        RECT frame{0, 0, deck_width, deck_height};
        if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor) ||
            !AdjustWindowRectEx(&frame, style, GetMenu(window) != nullptr, exstyle))
            return false;
        const int width = frame.right - frame.left;
        const int height = frame.bottom - frame.top;
        const RECT area = monitor.rcWork;
        if (width > area.right-area.left || height > area.bottom-area.top) return false;
        const int x = area.left + (area.right-area.left-width)/2;
        const int y = area.top + (area.bottom-area.top-height)/2;
        result = {x, y, x+width, y+height};
        return true;
    }

public:
    ~Display() { restore(); }

    void restore() {
        RECT current{};
        // Honor a manual resize or window mode change while streaming.
        if (changed_ && usable(window_) && GetWindowRect(window_, &current) &&
            EqualRect(&current, &applied_)) {
            original_.flags |= WPF_ASYNCWINDOWPLACEMENT;
            SetWindowPlacement(window_, &original_);
        }
        window_ = nullptr;
        changed_ = applied_seen_ = false;
    }

    void update(bool connected, HWND window) {
        if (!connected || (window_ && window_ != window)) restore();
        if (!connected || !usable(window)) return;
        if (window_ == window) {
            RECT current{};
            if (GetWindowRect(window, &current) && EqualRect(&current, &applied_))
                applied_seen_ = true;
            else if (applied_seen_) changed_ = false;
            return;
        }
        RECT client{};
        if (!GetClientRect(window, &client) || client.right < 640 || client.bottom < 360 ||
            !target(window, applied_) || !GetWindowPlacement(window, &original_)) return;
        window_ = window;
        if (client.right == deck_width && client.bottom == deck_height) return;
        if (IsZoomed(window) && !ShowWindowAsync(window, SW_RESTORE)) return;
        changed_ = SetWindowPos(window, nullptr, applied_.left, applied_.top,
            applied_.right-applied_.left, applied_.bottom-applied_.top,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS) != FALSE;
    }

    void tick(uint64_t now) {
        if (now < next_check_) return;
        next_check_ = now + 500;
        const bool connected = direct_sessions(GetModuleHandleW(L"steam_api.dll")).size != 0;
        update(connected, connected ? gamewindow105::find(GetCurrentProcessId()) : nullptr);
    }
};

} // namespace remote130
