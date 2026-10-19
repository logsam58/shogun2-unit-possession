# Contributor instructions

This is a native C++ Total War: SHOGUN 2 possession mod. Read README.md, BUILD_AND_TEST.md, SOURCE_MAP.md and VALIDATION.md before changing it.

v0.23 retains exact accepted Test130 gameplay, including reversible squad detachment, Test127 voice and Test126 camera/entry, with the prior audio guard, HUD and QTE improvements. Test116 clock and Test117 render-copy experiments are retained in Git history and excluded from this release. Do not reintroduce them accidentally.

Preserve native unit AI, formations, ranged fire, melee outcomes, camera control and siege behaviour. Identity filters may reject unrelated actors; they must never grant possession or reservation authority without fresh full proof. Preserve individual-read fallbacks.

Use existing indexed decoding and preserved sources. Keep dumps, raw input logs, game assets, local paths and credentials out of published source. Required local resources are documented in docs/LOCAL_DEPENDENCIES.md.

Run the relevant offline fixtures. Close the game before replacing native DLLs and preserve an exact rollback. Live tests require the user's READY cue and fresh loaded-file hashes before formal play. Honor explicit recorder/debugger-off instructions; otherwise verify authorized capture. Automatic recording runs independently of READY; preserve an existing watcher. Do not launch the game, start extra live profiling or change graphics settings without authorization.

Release packaging must retain the selected gameplay binaries, pin the complete source closure, verify payloads and pass installer upgrade/rollback tests. Publication requires an explicit user request. Distinguish user-reported performance improvement from measured FPS and from crash safety. v0.22 acceptance covers the user-reported smooth Large-unit 1v1/max-DX11 configuration. Ultra crowd slowdown and other crashes remain unresolved; no universal hardware limit or absence of mod overhead is established.

Test130 gameplay was accepted after its loaded battle gate, but that session also crashed. Keep its cause unresolved. Preserve exact gameplay during v0.23 promotion.
