## v0.24 release

The current gameplay is exact accepted Test138, including Rapid136, the native1 unit toggle, enemy-priority Q targeting, mode artwork, ordinary native locomotion and scoped detached-advance compatibility. Promotion changes only native/work packaging and tools metadata. See docs/releases/v0.24-technical.md. Native charging is a separate prepared checkpoint; historical candidate entries below do not supersede the current baseline.

## Test138 / v0.24

`melee_presentation137.h` now limits synchronization to native readiness requests; ordinary locomotion0/4 passes unchanged through the animation hook and equipment boundary. `verify_melee_presentation137.cpp` compares original native calls with the hook. The retained advance-task compatibility is independent. See docs/development/Test138.md and Checkpoint2.md.

Test137: melee_presentation137.h preserves native personal melee state across generic task resets for supported projectile families; equipment_native81.h also synchronizes entry into an already-melee unit. unit_detach_combat130.h scopes native advance-task context and unit_detach130.h retains it until rejoin. Two new original-code fixtures record the boundaries. Rapid136, native toggle134 and HUD/QTE remain exact.

Test136 adds adapter rapid_pace136.h: bounded read-only same-unit native velocity reference and exact-active-Rapid Shift ceiling. adapter.cpp invokes it after terrain and clamps acceleration carryover; existing log rows publish numeric reference. native_movement.h restores direct manual Shift selection, removing135 order-gait selection. verify_rapid136.cpp covers cap/normal/other-ability/ownership/expiry/failure cases. See docs/development/Test136.md.

Test135 keeps compatibility filenames: native_movement.h selects original18c120 unit gait during exact Rapid; native_commands100.h and native_fatigue.h/native_speed_modifiers.h share it. verify_rapid132.cpp compares original gait/speed; verify_fatigue.cpp isolates gait selection for hook costs. See docs/development/Test135.md.

# Source map

Test132 adds body_look132.h (bounded numeric turn goal) and rapid_advance132.h (read-only exact active-effect lookup), with the production integration in adapter.cpp/native_movement.h. verify_rapid132.cpp executes original native modifier/lookup and the requested Shift policy. The revised verify_head131.cpp now checks model-led visible yaw and restored pitch/QTE responsiveness.


The discarded Test117 render-copy experiment is retained in Git history, not the active build. See [Test117](docs/development/Test117.md) for its historical scope and limits.

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

Unpublished Test131 adds `head_look131.h` for mouse rotation limits and `unit_weapon131.h` for native unit weapon orders with acknowledgment. Existing movement, context and equipment files contain their integration. The four `verify_*131.cpp` fixtures cover speed parity, head-look, Q targeting and weapon-mode requests. Charges, T/G abilities and variable bow draw are later checkpoints.

Read adapter.cpp for hook setup, input service and lifecycle coordination. Follow native_bow.h and manual_dispersion.h for projectile launch/accuracy, the equipment headers for weapon selection/ammunition/crouch, and the siege/climb headers for wall state and ownership. Fixture names identify the corresponding behavior under test.

For QTE work, start with playable_runtime.cpp, qte_core.h, qte_timeline.h and qte_presentation93.h. Native pair preparation and observation are separate from the visual response clock. For the HUD, start with hud.cpp and its state policy; preserve the shared numeric layouts when changing either producer or consumer.

Read [architecture](docs/ARCHITECTURE.md) for component data flow and [local dependencies](docs/LOCAL_DEPENDENCIES.md) for assets, exact engine fixtures and optional indexed native research. docs/source-import.json and docs/import-validation.json record initial provenance and validation; they are engineering receipts rather than the player feature guide.


Test100 development changes are in the maintained adapter's `native_advance100.h` (Q to stock window messages and native cursor cleanup), `native_commands100.h` (native Z/C/fire packets and same-unit command-fire scope), `command_policy100.h`, `command_input100.h`, `command_protocol100.h` (optional 80-byte numeric status), and `self_visibility100.h` plus `soldier_pose.h` (render-only head/neck hiding and raw native head-cache preservation). The main HUD's renderer has the new key labels; formation graphics are entirely native. `hud_surface100.h` reuses the actual DIB/DC at the prior HUD cadence. No QTE source changed. Isolated weapon fixtures explicitly substitute inactive command mode; the new full-adapter fixture exercises active command ownership and original native mouse/cursor primitives.

Test101 keeps the compatibility filenames/protocols above. `native_advance100.h` now feeds the crosshair while FPS mouse-look remains active. `command_policy100.h` follows translation/travel rather than gaze. `first_person_presentation101.h`, `first_person_interface.h` and `soldier_pose.h` implement owned near-plane restoration and eye-relative foot weapon-look; `native_bow.h` adds identity to the private eye sample without changing published shot/status layouts. `verify_presentation101.cpp` and expanded native projection/command/camera fixtures cover those changes.

Test102 / v0.16 extends the existing command files. `command_policy100.h` contains bounded tap, follow gait and catch-up policy; `command_input100.h` carries only private numeric tap/anchor data. `native_commands100.h` sets the original native move-order run flag for Z/C and samples current same-unit formation position. `native_advance100.h` maps a second Q tap to the original RMB double-click sequence, leaving contextual attack/move choice in the game. Shared status version100/80-byte layout is unchanged.

Test103 adds `native_recovery103.h` for current native prone/animation gating and `native_context103.h` for guarded camera cycling and own-unit cursor-hit filtering. The main adapter adds bounded private release snapshots and suppresses the existing native camera-input dispatcher only with full ownership. `verify_context103.cpp` executes production guards with original native cycling/hit/hover/animation-clock routines and explicitly substituted providers.

Test104 restricts `camera_choice97.h` roster overrides to an explicit native FPS update/constructor and resolves the current exact holder. `native_context103.h` guards only the selected/wavering UI identity comparison before its followed-actor write; other native identity accessor calls forward unchanged. `animation_look104.h`, `mouse_look101` and `adjust_eye` implement bounded two-axis head-look through combat/handoff. Existing camera/landing/siege/mounted fixtures and new `verify_selection104.cpp` / `verify_look104.cpp` document native/provider boundaries. Private morale/roster/head-look diagnostics do not change shared numeric layouts.

Test105 retains the `throw_stop_scope83.h` compatibility filename while generalizing the scoped reset guard to currently owned infantry personal animations; `verify_animation_ownership105.cpp` executes the original native reset/clock with production identity. `native_bow.h` applies current aim-frame lease checks to all consumers. QTE `qte_timeline.h` reconciles prediction at actual native ticks and `verify_drift105.cpp` covers sustained scheduling drift. Shared `shogun2-possession-cleanup-v46/src/game_window105.h` serves HUD/QTE window recovery, with actual Win32/DIB cases in `verify_hud_window105.cpp`. No new native hooks or numeric protocol layouts.


Test106 presentation is in `native/outputs/shogun2-accuracy90-preview/src/native_overlay106.h`; optional numeric/pixel transport is in `native/outputs/shogun2-possession-cleanup-v46/src/overlay_surface106.h`. Existing HUD/QTE DIB rendering publishes those pixels; the adapter composites them at native DirectX Present. `verify_directx106.cpp`, `verify_overlay106.cpp`, `verify_overlay_identity106.cpp` and QTE `verify_presented106.cpp` exercise actual graphics state/pixels, channel ownership and submitted-image judgement. Development scope/limitations are in `docs/development/Test106.md`.


Test107 timing correction is in QTE `qte_session.h` publication and paused-time accounting. Private distinct-submission counters are in `playable_runtime.cpp`; `verify_publication107.cpp` exercises the production coordinator/transport boundary, and `verify_cadence107.cpp` verifies the production counter's unique-frame semantics. Test106 adapter/HUD source and binaries remain exact.


Test108 presentation helpers are `hud_layout108.h` and `qte_meter108.h` in the shared possession-cleanup-v46/src directory. Test109 personal projectile tagging/passive contact observation is in adapter `friendly_fire109.h`, with the optional numeric `friendly_fire_protocol109.h` in that shared directory. HUD `audio_feedback109.h` consumes notifications and plays the private cropped resource. `tools/prepare_audio109.py` and `tools/audio109.json` define the repeatable local audio input; no game voice bytes are tracked.

Diagnostics110 session history is in tools/diagnostics/source/lifecycle110.py. Experimental processor tracing in native/targeted_trace110.h is withdrawn after real startup failures and restricted to toy fixtures; a verified capture-only debugger is installed. Earlier verify_targeted110.py synthetic passes do not establish real-game safety. See docs/development/Diagnostics110-startup-recovery.md and Diagnostics110.md; this follow-up is separate from the pinned v0.19 player package.

Diagnostics111 isolates default capture in native/capture_only111.h; native/trace_ownership111.h preserves retired binding ownership, and native/fixture_identity111.h rejects non-pinned toy targets. source/binary_policy111.py gates recorder PE imports/capability. tools/diagnostics/build111.py builds isolated artifacts and source/verify_hardening111.py runs deterministic context-restoration, concurrency, native exception and failure-injection checks. All paths under native/ here refer to tools/diagnostics/source/native, not gameplay. See docs/development/Diagnostics111.md; this work does not replace the active capture-only installation.

Diagnostics112 adds native/crash_capture112.h for independent fault snapshots, load-event module caching and bounded dump fallbacks. native/verify_dump112.cpp and source/verify_capture112.py check actual context evidence, dump formats, output failure and toy loader corruption. See docs/development/Diagnostics112.md; only diagnostics change.

Test113 QTE input uses qte_presented113.h and playable_runtime.cpp to resolve the latest native image acknowledgement independently of render-worker polling. verify_input113.cpp reproduces the older-cache judgement error and exercises real shared-channel acknowledgement, bounded concurrent history, identity and expiry. This uninstalled candidate retains the accepted adapter/HUD and does not resolve the separately reported possession slowdown.


v0.20 packages the exact Test115 runtime. QTE `qte_presentation93.h` / `playable_runtime.cpp` retain the direct latest-native-acknowledgement input history from Test113. `native_reservation.h` and `verify_crowd115.cpp` contain the bounded foreign-encounter snapshot/fallback and differential identity cases. The product version and release defaults advance independently of those stable filenames and numeric protocols. No animation-lock correction is included.

## v0.21

Active gameplay source matches the tested Test122 tree. The v0.20 adapter/HUD are retained. QTE `native_combat_actor.h`, `native_reservation.h`, `strict_melee_gate.h` and `playable_runtime.cpp` contain performance changes118-122, with `verify_scope118.cpp` through `verify_scope122.cpp` and `verify_identity120.cpp`. Test116/117 experiments are excluded from active source and retained in Git history. See [technical notes](docs/releases/v0.21-technical.md).

## v0.22

Active gameplay source matches Test124. Adapter `miles_zero123.h`, its integration in `adapter.cpp` and `verify_miles123.cpp` provide the pinned Miles zero-count guard and native differential fixture. QTE `native_selection_prefix.h` and `verify_preparation124.cpp` add the rejection-only initial encounter filter with current positive validation and individual-read fallback. Other gameplay inputs match the retained Test122 source. See [technical notes](docs/releases/v0.22-technical.md).

## v0.23

Active native output source and exact runtime binaries match accepted Test130. `possession_entry125.h`, `combat_focus125.h` and `combat_turn126.h` implement entry facing/gait and animation-led look. `follow_voice125.h` contains accepted Test127 acknowledgements. `unit_detach130.h` separates squad participation from unit/stat ownership; `unit_detach_combat130.h` preserves personal combat reads and deferred rejoin. Earlier readiness headers remain complementary. New fixtures describe original-code/provider boundaries. See [technical notes](docs/releases/v0.23-technical.md).

Test133 restores Test130 adapter camera source and its mounted/animation helpers. `native_movement.h` owns the shared Rapid sprint decision also used by `native_commands100.h`; `hud-src/weapon_mode133.h` selects original stock mode art from the validated native unit flag read in `hud.cpp`. See docs/development/Test133.md.

Test134: `unit_weapon131.h` now delegates the original native UI action10dfb80/cases18-19, while `equipment_input81.h` binds1 as the single toggle. `equipment_native81.h` separates unit action from personal animation readiness. No custom mode packet or native mode-flag writes. See docs/development/Test134.md.
