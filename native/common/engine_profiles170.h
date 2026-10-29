#pragma once
#include <string_view>

// Known reviewed identities, used as a fast path by engine_contract172.h.
// An unlisted identity now needs that shared same-layout contract; it never
// bypasses exact occupied-hook checks. This helper alone is not admission.
namespace engine170 {
inline constexpr char stock_sha[]="022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5";
inline constexpr char bugfix_sha[]="c4d91d74ce9ff8338cf61d5b021b03a2776830177a590fcac205509a42239545";
inline bool supported(std::string_view sha){return sha==stock_sha||sha==bugfix_sha;}
}
