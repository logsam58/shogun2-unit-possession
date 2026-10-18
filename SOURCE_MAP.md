# Source map

The mod is native C++ with Python build and diagnostic tools. VERSION is the product version. Numeric suffixes in paths and filenames are stable component/interface identifiers; use the active paths below when changing code.

| Component | Active path | Responsibilities |
| --- | --- | --- |
| Possession adapter | native/outputs/shogun2-accuracy90-preview/src | Actor ownership, camera, movement, weapons, accuracy, equipment, unit orders, siege traversal and native regression fixtures |
| Adapter hooks | native/outputs/shogun2-accuracy90-preview/vendor | MinHook and x86 trampoline/disassembler support |
| Melee QTE runtime | native/outputs/shogun2-authentic-hud80-preview/qte-src | Native pair preparation, combat state, response clock, input queue, interruption and renderer fixtures |
| Styled QTE renderer | native/outputs/shogun2-authentic-hud80-preview/src | Shared native-style surface, fonts, artwork/resource definitions |
| Contextual HUD | native/outputs/shogun2-ammunition83-preview/hud-src | Out-of-process HUD, state policy, reticles and equipment/ammo/climb display |
| Equipment/ammunition interfaces | native/outputs/shogun2-ammunition83-preview/src | Numeric publication layouts and state definitions |
| Identity/status interfaces | native/outputs/shogun2-possession-cleanup-v46 | Shared identity/status definitions and vendor closure |
| Shared QTE/HUD headers | native/outputs/shogun2-possession-qte55-preview; shogun2-melee-qte63-preview; shogun2-melee-meter79-preview under native/outputs | Retained header dependencies; not additional production adapter targets |
| DInput loader | native/work/autoload99-preview | Bootstrap, embedded payload preparation, fingerprints and cache startup |
| Installer | native/work/installer-siege99 | Wizard/CLI, exact predecessor recognition, transactional rollback, cache retention and isolated fixtures |
| Prior cache identities | native/work/installer-siege99/previous_cache_catalog.h | Fingerprints distinguishing known caches from unrelated files |
| Build/configuration | tools/build.py; tools/configure.py; tools/local-assets.json | x86 builds, local resource import and hash manifest |
| Release tooling | tools/package_release.py; tools/github_release.py | Verified package creation and explicitly requested publication |
| Diagnostics | tools/diagnostics/source | Launch watcher, debugger, telemetry and minidump inspection |
| Diagnostic format tests | tests/test_minidump.py | Synthetic dump-layout cases |

## Starting points

Read adapter.cpp for hook setup, input service and lifecycle coordination. Follow native_bow.h and manual_dispersion.h for projectile launch/accuracy, the equipment headers for weapon selection/ammunition/crouch, and the siege/climb headers for wall state and ownership. Fixture names identify the corresponding behavior under test.

For QTE work, start with playable_runtime.cpp, qte_core.h, qte_timeline.h and qte_presentation93.h. Native pair preparation and observation are separate from the visual response clock. For the HUD, start with hud.cpp and its state policy; preserve the shared numeric layouts when changing either producer or consumer.

Read [architecture](docs/ARCHITECTURE.md) for component data flow and [local dependencies](docs/LOCAL_DEPENDENCIES.md) for assets, exact engine fixtures and optional indexed native research. docs/source-import.json and docs/import-validation.json record initial provenance and validation; they are engineering receipts rather than the player feature guide.


Test100 development changes are in the maintained adapter's `native_advance100.h` (Q to stock window messages and native cursor cleanup), `native_commands100.h` (native Z/C/fire packets and same-unit command-fire scope), `command_policy100.h`, `command_input100.h`, `command_protocol100.h` (optional 80-byte numeric status), and `self_visibility100.h` plus `soldier_pose.h` (render-only head/neck hiding and raw native head-cache preservation). The main HUD's renderer has the new key labels; formation graphics are entirely native. `hud_surface100.h` reuses the actual DIB/DC at the prior HUD cadence. No QTE source changed. Isolated weapon fixtures explicitly substitute inactive command mode; the new full-adapter fixture exercises active command ownership and original native mouse/cursor primitives.

Test101 keeps the compatibility filenames/protocols above. `native_advance100.h` now feeds the crosshair while FPS mouse-look remains active. `command_policy100.h` follows translation/travel rather than gaze. `first_person_presentation101.h`, `first_person_interface.h` and `soldier_pose.h` implement owned near-plane restoration and eye-relative foot weapon-look; `native_bow.h` adds identity to the private eye sample without changing published shot/status layouts. `verify_presentation101.cpp` and expanded native projection/command/camera fixtures cover those changes.

Test102 / v0.16 extends the existing command files. `command_policy100.h` contains bounded tap, follow gait and catch-up policy; `command_input100.h` carries only private numeric tap/anchor data. `native_commands100.h` sets the original native move-order run flag for Z/C and samples current same-unit formation position. `native_advance100.h` maps a second Q tap to the original RMB double-click sequence, leaving contextual attack/move choice in the game. Shared status version100/80-byte layout is unchanged.

Test103 adds `native_recovery103.h` for current native prone/animation gating and `native_context103.h` for guarded camera cycling and own-unit cursor-hit filtering. The main adapter adds bounded private release snapshots and suppresses the existing native camera-input dispatcher only with full ownership. `verify_context103.cpp` executes production guards with original native cycling/hit/hover/animation-clock routines and explicitly substituted providers.
