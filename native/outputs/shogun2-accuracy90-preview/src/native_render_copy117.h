#pragma once
#include <windows.h>
#include <bcrypt.h>
#include <emmintrin.h>
#include <cstdint>
#include <cstring>
#include "../vendor/include/MinHook.h"

// The supported engine sorts 0x138-byte render records with two scalar copy
// procedures. Preserve its fields, padding and overlapping-copy behavior;
// sorting, queue population and ownership remain entirely native.
namespace rendercopy117 {
using Copy = void*(__thiscall*)(void*, const void*);
static constexpr unsigned rvas[2] = {0x14b4530, 0x14b1880};
static constexpr unsigned lengths[2] = {753, 961};
static constexpr unsigned char digests[2][32] = {
    {0x50,0x9f,0xea,0x65,0x25,0xe7,0x18,0xbc,0x9f,0xfa,0x2b,0x4d,0xad,0x61,0x51,0xa7,0xce,0xa5,0x7d,0xcf,0x48,0xb2,0xc6,0xfe,0x22,0x9b,0x56,0x74,0x4d,0x21,0x1c,0x55},
    {0x83,0xd1,0xbb,0x0b,0xd0,0xda,0xe8,0xbb,0x0c,0xd9,0x07,0x8e,0xb4,0xe5,0x56,0xe0,0x2d,0xc8,0x4b,0x05,0x42,0x25,0xd1,0x5a,0xad,0x6a,0xfe,0x16,0xcf,0x4c,0xd9,0x97}
};
static Copy originals[2]{};
static void* targets[2]{};

static __forceinline void* copy(void* destination, const void* source, Copy original) {
    auto d = static_cast<unsigned char*>(destination);
    auto s = static_cast<const unsigned char*>(source);
    auto dd = reinterpret_cast<uintptr_t>(d), ss = reinterpret_cast<uintptr_t>(s);
    // Native forward stores propagate overlapping input differently from
    // memcpy/memmove. Use the corresponding original even for self-copy.
    if ((dd >= ss ? dd - ss : ss - dd) < 0x138) return original(destination, source);
    for (unsigned i = 0; i < 0x120; i += 16)
        _mm_storeu_si128(reinterpret_cast<__m128i*>(d + i),
                        _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i)));
    d[0x120] = s[0x120];
    _mm_storeu_si128(reinterpret_cast<__m128i*>(d + 0x124),
                    _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + 0x124)));
    d[0x134] = s[0x134];
    return destination;
}
static __declspec(noinline) void* __fastcall assignment(void* d, void*, const void* s) {
    return copy(d, s, originals[0]);
}
static __declspec(noinline) void* __fastcall construction(void* d, void*, const void* s) {
    return copy(d, s, originals[1]);
}

// Initialization only. Caller has already verified the complete engine file.
// Check complete loaded procedures too, rejecting other patches before touching
// either entry. These procedures contain no relocation-dependent instructions.
static bool signatures(uintptr_t base, unsigned image_size) {
    if (!base || image_size != 0x1d88000 ||
        !IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE)) return false;
    BCRYPT_ALG_HANDLE alg{};
    if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) return false;
    bool ok = true;
    for (unsigned i = 0; i < 2 && ok; ++i) {
        unsigned char digest[32]{};
        ok = BCryptHash(alg, nullptr, 0, reinterpret_cast<PUCHAR>(base + rvas[i]),
                        lengths[i], digest, sizeof digest) == 0 &&
             memcmp(digest, digests[i], sizeof digest) == 0;
    }
    BCryptCloseAlgorithmProvider(alg, 0);
    return ok;
}
static bool create(uintptr_t base, unsigned image_size) {
    if (targets[0] || targets[1] || !signatures(base, image_size)) return false;
    void* detours[] = {reinterpret_cast<void*>(&assignment), reinterpret_cast<void*>(&construction)};
    for (unsigned i = 0; i < 2; ++i) {
        auto target = reinterpret_cast<void*>(base + rvas[i]);
        if (MH_CreateHook(target, detours[i], reinterpret_cast<void**>(&originals[i])) != MH_OK) {
            // Created hooks are still disabled. Never leave half a pair behind.
            if (i) MH_RemoveHook(targets[0]);
            targets[0] = targets[1] = nullptr;
            originals[0] = originals[1] = nullptr;
            return false;
        }
        targets[i] = target;
    }
    // The adapter enables all prepared hooks together. No allocations, locks,
    // counters, logging or native identity reads occur in either copy callback.
    return true;
}
static bool disable() {
    bool ok = true;
    for (auto target : targets) if (target) {
        auto result = MH_DisableHook(target);
        if (result != MH_OK && result != MH_ERROR_DISABLED) ok = false;
    }
    // Match adapter lifetime: keep DLL and trampolines resident until exit so
    // a callback already in progress can finish safely after hook disable.
    return ok;
}
}
