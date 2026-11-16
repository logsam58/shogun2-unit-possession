#pragma once
#include <windows.h>
#include <cstdint>

namespace remote130 {

template<class Function>
Function method(void* object, unsigned slot) {
    void** table{};
    void* address{};
    SIZE_T bytes{};
    HANDLE process = GetCurrentProcess();
    if (!object ||
        !ReadProcessMemory(process, object, &table, sizeof table, &bytes) ||
        bytes != sizeof table || !table ||
        !ReadProcessMemory(process, table + slot, &address, sizeof address, &bytes) ||
        bytes != sizeof address) return nullptr;
    MEMORY_BASIC_INFORMATION page{};
    if (!VirtualQuery(address, &page, sizeof page) || page.State != MEM_COMMIT ||
        (page.Protect & (PAGE_GUARD | PAGE_NOACCESS)) ||
        !(page.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
                          PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))) return nullptr;
    return reinterpret_cast<Function>(address);
}

inline void* get_interface(HMODULE steam, const char* version) {
    if (!steam) return nullptr;
    using Handle = int(__cdecl*)();
    using Client = void*(__cdecl*)();
    using Generic = void*(__cdecl*)(void*, int, int, const char*);
    auto user = reinterpret_cast<Handle>(GetProcAddress(steam, "SteamAPI_GetHSteamUser"));
    auto pipe = reinterpret_cast<Handle>(GetProcAddress(steam, "SteamAPI_GetHSteamPipe"));
    auto client = reinterpret_cast<Client>(GetProcAddress(steam, "SteamClient"));
    auto generic = reinterpret_cast<Generic>(GetProcAddress(steam, "SteamAPI_ISteamClient_GetISteamGenericInterface"));
    if (!user || !pipe || !client || !generic) return nullptr;
    __try {
        const int u = user(), p = pipe();
        auto c = client();
        return u && p && c ? generic(c, u, p, version) : nullptr;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

constexpr int deck_type = 14; // ESteamInputType::k_ESteamInputType_SteamDeck
struct Sessions {
    uint32_t ids[16]{};
    unsigned size{};
    bool contains(uint32_t id) const {
        if (!id) return false;
        for (unsigned i = 0; i < size; ++i) if (ids[i] == id) return true;
        return false;
    }
};

// Valve SDK165 SteamRemotePlay004. Earlier interfaces have different slots
// and lack BSessionRemotePlayTogether.
inline Sessions direct_sessions(HMODULE steam) {
    void* api = get_interface(steam, "STEAMREMOTEPLAY_INTERFACE_VERSION004");
    auto count = method<uint32_t(__thiscall*)(void*)>(api, 0);
    auto id = method<uint32_t(__thiscall*)(void*, int)>(api, 1);
    auto together = method<bool(__thiscall*)(void*, uint32_t)>(api, 2);
    if (!count || !id || !together) return {};
    __try {
        Sessions sessions;
        const auto n = count(api);
        if (n > 16) return {};
        for (unsigned i = 0; i < n; ++i) {
            const auto session = id(api, int(i));
            if (session && !together(api, session) && !sessions.contains(session))
                sessions.ids[sessions.size++] = session;
        }
        return sessions;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return {};
    }
}

// Probe the actual forwarded controller. Device names can be changed by the
// user and screen size is shared by other devices. The normal Deck input
// worker takes ownership after this bounded probe releases Steam Input.
inline bool has_deck(HMODULE steam, const Sessions& sessions) {
    if (!sessions.size) return false;
    void* api = get_interface(steam, "SteamInput006");
    auto init = method<bool(__thiscall*)(void*, bool)>(api, 0);
    auto shutdown = method<bool(__thiscall*)(void*)>(api, 1);
    auto frame = method<void(__thiscall*)(void*, bool)>(api, 3);
    auto connected = method<int(__thiscall*)(void*, uint64_t*)>(api, 6);
    auto type = method<int(__thiscall*)(void*, uint64_t)>(api, 37);
    auto session = method<uint32_t(__thiscall*)(void*, uint64_t)>(api, 45);
    if (!init || !shutdown || !frame || !connected || !type || !session) return false;

    bool owned = false;
    bool unique = false;
    __try {
        owned = init(api, true);
        if (!owned) return false;
        const auto deadline = GetTickCount64() + 750;
        do {
            frame(api, true);
            uint64_t handles[16]{};
            const int count = connected(api, handles);
            if (count < 0 || count > 16) break;
            unsigned matches = 0;
            for (int i = 0; i < count; ++i) {
                if (handles[i] && type(api, handles[i]) == deck_type &&
                    sessions.contains(session(api, handles[i]))) ++matches;
            }
            if (matches) {
                unique = matches == 1;
                break;
            }
            if (GetTickCount64() >= deadline) break;
            Sleep(15);
        } while (true);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        unique = false;
    }
    __try {
        if (owned && !shutdown(api)) unique = false;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        unique = false;
    }
    return unique;
}

} // namespace remote130
