# Contributor instructions

This is a native C++ Total War: SHOGUN 2 possession mod. Read README.md, BUILD_AND_TEST.md, SOURCE_MAP.md and VALIDATION.md before changing it.

v0.22 ships the exact Test124 gameplay: the Test123 adapter/audio guard, retained HUD, and QTE performance changes118 through122 plus124. Test116 clock and Test117 render-copy experiments are retained in Git history and excluded from this release. Do not reintroduce them accidentally.

Preserve native unit AI, formations, ranged fire, melee outcomes, camera control and siege behaviour. Identity filters may reject unrelated actors; they must never grant possession or reservation authority without fresh full proof. Preserve individual-read fallbacks.

Use existing indexed decoding and preserved sources. Keep dumps, raw input logs, game assets, local paths and credentials out of published source. Required local resources are documented in docs/LOCAL_DEPENDENCIES.md.

Run the relevant offline fixtures. Close the game before replacing native DLLs and preserve an exact rollback. Live tests require the user's READY cue, fresh loaded-file hashes and verified capture before asking them to play. Automatic recording runs independently of READY; preserve an existing watcher. Do not launch the game, start extra live profiling or change graphics settings without authorization.

Release packaging must retain the selected gameplay binaries, pin the complete source closure, verify payloads and pass installer upgrade/rollback tests. Publication requires an explicit user request. Distinguish user-reported performance improvement from measured FPS and from crash safety. v0.22 acceptance covers the user-reported smooth Large-unit 1v1/max-DX11 configuration. Ultra crowd slowdown and other crashes remain unresolved; no universal hardware limit or absence of mod overhead is established.
