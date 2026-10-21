# v0.28 release promotion

v0.28 promotes accepted Test150–157 selection/roles, native camera/orbit/death/routing, wall navigation and mount continuity. Exact Test157 native/outputs closure retained; metadata/loader/installer predecessor handling only changes for release. See docs/releases/v0.28-technical.md and native shared actor_types152.h.

# Test157 development candidate

Test157 expands the shared actor_types152 soldier-role policy and actual-config exceptions in native_subject/native_wall/native_air/dismount/native_bow. Both fps_camera_actor read paths use the same role policy. New adapter/QTE verify_roles157 fixtures plus exact-cursor and original-native-camera role cases cover these changes. Details: docs/development/Test157.md.

# Test156 camera sources

view152.h and adapter.cpp retain manual third-person direction across QTEs; third_person152.h and melee_control.h retain third-person death mode/distance and omit FPS eyelids. camera_choice97.h shares the proved routing-selection policy and uses the numeric death update; first_person_interface.h retains the correct third-person up vector without an unused unit lookup. verify_view156.cpp and expanded verify_native_camera98.cpp cover production/native paths. Accepted155 wall/dismount code is byte-exact. Evidence: artifacts/Test156, docs/development/Test156.md.

# Test155 source

climb_control97.h uses original397e40/397e80 portal navigation locations instead of351e40 bearing poses, including the correct navigation surface on exit. dismount143.h reads native task slotsba0/ba4 with countba8. Expanded siege/dismount fixtures execute native cached-route validation and original task registration. Exact154 orbit, HUD/QTE/charge/orders/AI/movement retained.

# Test153 source

third_person152.h rebuilds ordinary pitched look basis before third-person validation; QTE native roll retained. climb_control97.h uses original351e40/3ba540/3df290 native path approach for F rather than a derived heading/direct bind. unit_detach_combat130.h scopes formation-task yielding to that live actor/current goal; native_charge148.h excludes pending wall approach. verify_view153.cpp runs original camera matrix/refresh; verify_siege97.cpp adds original native frame/parameters/path-order tests and retains W/S/crest/landing coverage. Exact Test152 QTE/shared/HUD and accepted leaders/equipment/eye/routing retained.

# Test152 source

actor_types152.h admits only native general351740/officer353d50 subtypes beyond ordinary humans; each affected camera/AI/combat/weapon identity boundary uses it. native_bow.h follows native391b50 personal action/projectile data. climb_control97.h aligns using original3936f0 before original3c1ba0. view152.h and third_person152.h implement bounded animated eyes and local P/wheel camera controls, native world obstruction and full model presentation. Native bridge/Session routing transitions reuse the selected pair and native preparation/retargeting, without QTE admission. Existing151 epoch/K/cursor and normal149 charging mechanics are retained. See docs/development/Test152.md.

# Test151 source additions

Adapter camera_pick150.h now reads originalbd4d70 normalized native mouse coordinates and applies335260's transform before original13660f0 unprojection; entry remains minimum-zoom/current selected-unit scoped. hud_visibility148.h reads native root+ca0, native_keyboard84.h leaves K native, and first_person_interface.h reapplies current10f1060 state on exit. Existing keyboard, F9 and interface lifecycle fixtures exercise these changes.

QTE battle_epoch151.h assigns process-local clock lifetime identities from fresh current native root/env/common/phase snapshots and confirms model tick resets. playable_runtime.cpp uses it for callbacks and read-only polls, with bounded numeric logging only. verify_battle_epoch151.cpp reproduces the saved pointer-key failure and tests actual Session admission across100 reused-address battles; qte_timeline.h and native combat/charge guards remain unchanged. Offline dump/index evidence is local in artifacts/Test151, excluded from publication.

# Test150 F9 entry

Test150 adds precise F9 selection at the stock RTS camera minimum zoom. It performs one native sorted center-ray query at entry; high camera retains the exact prior selection policy. The ray uses original342a40 render-service/virtual18 context and13660f0 unprojection, including cached matrices. Original29a450/a0b60 reads the live minimum, matching293cd0 camera+180 clamp. Original158970 mask30b includes actors and world occluders. The first physical hit wins; an eligible current selected-unit human or proved reciprocal horse maps to the exact existing holder. Foreign/dead/unsupported hits and missing/invalid context retain prior selection. Existing selection/identity/thread/heartbeat/roster guards and native camera constructor remain. No input/cursor/selection order mutation, shared ABI, hook, recurring crowd scan or new asset.

The focused production entry fixture passes 1000 repeated calls, native getter/unprojection/query dispatch, original high view, live minimum changes, world occluders, dead/foreign hits, bounded results, cached matrices/nonfinite input, reciprocal horse and broken links. Geometry/render device/sorting/allocation/camera construction are explicit providers. Negative control fails without the hit preference. Twelve indexed exports are instruction-byte verified. The first private fixture had swapped actor-grid provider argument labels and lacked a valid custom camera aspect; fixed only providers, failures retained. A preliminary pose-based ray was replaced before source freeze by the original render unprojection path.

Exact accepted Test149 charge/K/order/movement/weapon/ability source is retained except the F9 entry call/preference. Exact HUD148 and QTE146 source/binaries remain. v0.27/433815b release/tag/main and six published assets stay unchanged. Full fresh adapter89/loader2/full installer exact149+published027+allprior/seven interruptions/repeatbackup/Python7 required. Test150 is development0.27 and not accepted/published. Install only under actual closed/armed guard with exact149 rollback; then fresh READY/menu or IN BATTLE and exact150 loaded hashes plus actual crash/input/memory/CPU/GPU recording before formal play. No automatic launch or extra profiling. Enemy acquisition/POV/Ultra/other crashes remain open.

# v0.27 / charging checkpoint complete

v0.27 retains Test149 adapter and native_charge148.h via native_charge146.h, proximity147.h, native keyboard K continuity, exact HUD148 and QTE146. Version/build/embedded metadata and installer predecessor catalog/tests promote accepted gameplay to a regular release. See docs/releases/v0.27-technical.md.

# Test149

native_keyboard84.h tracks native K in RTS/FPS. native_charge148.h retains W+Shift acquisition with native ground/separation/actual-pose gates and numeric observations. adapter.cpp gives proved pair alignment priority over approach-only charge yield. verify_charge148.cpp executes original entry/pose/contact/intent with explicit scene/path/pace providers; keyboard84 covers RTS transitions. QTE146/HUD148 remain byte-exact.

# Test148

native_charge148.h replaces active147 charging; native_charge146.h includes it. verify_charge148.cpp exercises native perception/eligibility/human/horse/rider dispatch and queued contact. proximity147.h retains membership with drain/retirement corrections. hud_visibility148.h in common and adapter carries K preference; native_keyboard84.h mirrors DIK_K; ammunition83 HUD consumes it. QTE146 source/binary stays exact.

# Test147

proximity147.h manages real kind2/3 native groups and distance transitions at existing unit callbacks. unit_detach130.h retains legacy fallback/rider restoration; ranged_position128.h and formation_arrival129.h supply full scoped aggregate ownership. native_charge147.h hooks actual native charge assignment; verify_proximity147.cpp executes native subgroup pools, targeting, original unit target picker/scheduler, foot/horse handoff and restoration. verify_charge139.cpp retains native personal entry/contact/jump coverage.

# v0.26

v0.26/Test145 removes experimental bow-sound hooks, actor audio TLS and LMB cue/handle bookkeeping. native_audio143.h retains only original per-ability sound selection. bow_input142.h retains hold/release strength. weapon_animation.h/native_bow.h keep the native animation/projectile paths. Exact HUD144 aiming dot and QTE140 are retained.

# Test144

Test144: native_audio143.h adds narrow292620 authored-note hook and correctd3/d4 cue IDs. verify_audio143.cpp executes original292620/cd56e0 with explicit providers. hud_renderer.h adds discreet melee center dot; bow input/power and ally targeting code unchanged.

# Test143 source additions

See ability_target143.h for native ally-targeted Inspire, dismount143.h for same-human mounted-to-foot continuity, native_audio143.h for native ability sound selection and owned LMB draw/stop/release audio. bow_input142.h and the real window handler share release edges and verified asset timing. verify_input143/ability143/dismount143/audio143 plus expanded verify_context103 exercise the production boundaries. Existing charge140, Rapid136, native mode134, detachment130 and QTE140 remain.

## Test142 development

Ability input/catalog/protocol/native141 and bow draw/input142 extend the accepted adapter; the HUD reads separate ability telemetry and reuses native art. Native charge/QTE source remains unchanged. See docs/development/Test142.md.

## v0.25 / accepted Test140

Promote exact Test140 gameplay with charging marked buggy. Release metadata uses revision140; retain adapter80/QTE47/HUD6 and run fresh loader2, complete installer upgrades/rollback and Python7. See docs/releases/v0.25-technical.md.

## Test140 charge contact

QTE `native_charge_contact140.h` bridges original charge encounters around prompt acquisition while reusing the adapter alignment/playback channel. `verify_charge140.cpp` exercises production dispatch/selector/target boundaries and removed-lifetime cleanup; `native_registry94.h::matches140` proves an existing generation without creating an entry. Adapter `native_subject.h` recognizes the original horse jump task; `adapter.cpp` yields manual translation and `verify_charge139.cpp` exercises its original factory and lease/motion handoff. See docs/development/Test140.md for provider and live limits.

## v0.24 release

The current gameplay is exact accepted Test138, including Rapid136, the native1 unit toggle, enemy-priority Q targeting, mode artwork, ordinary native locomotion and scoped detached-advance compatibility. Promotion changes only native/work packaging and tools metadata. See docs/releases/v0.24-technical.md. Native charging is a separate prepared checkpoint; historical candidate entries below do not supersede the current baseline.

## Test138 / v0.24

`melee_presentation137.h` now limits synchronization to native readiness requests; ordinary locomotion0/4 passes unchanged through the animation hook and equipment boundary. `verify_melee_presentation137.cpp` compares original native calls with the hook. The retained advance-task compatibility is independent. See docs/development/Test138.md and Checkpoint2.md.

Test137: melee_presentation137.h preserves native personal melee state across generic task resets for supported projectile families; equipment_native81.h also synchronizes entry into an already-melee unit. unit_detach_combat130.h scopes native advance-task context and unit_detach130.h retains it until rejoin. Two new original-code fixtures record the boundaries. Rapid136, native toggle134 and HUD/QTE remain exact.

Test136 adds adapter rapid_pace136.h: bounded read-only same-unit native velocity reference and exact-active-Rapid Shift ceiling. adapter.cpp invokes it after terrain and clamps acceleration carryover; existing log rows publish numeric reference. native_movement.h restores direct manual Shift selection, removing135 order-gait selection. verify_rapid136.cpp covers cap/normal/other-ability/ownership/expiry/failure cases. See docs/development/Test136.md.

Test135 keeps compatibility filenames: native_movement.h selects original18c120 unit gait during exact Rapid; native_commands100.h and native_fatigue.h/native_speed_modifiers.h share it. verify_rapid132.cpp compares original gait/speed; verify_fatigue.cpp isolates gait selection for hook costs. See docs/development/Test135.md.

# Source map

Test139 adds `native_charge139.h` and `verify_charge139.cpp` under the active adapter source. The motion boundary bridges current native formation charge descriptors into the original personal task, with bounded roster/target proof, native speed and cancellation. Exact Test138 HUD/QTE are retained. See docs/development/Test139.md for scope and fixture providers; live charge acceptance is pending.

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
