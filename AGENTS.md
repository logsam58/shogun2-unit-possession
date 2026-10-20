# v0.26

v0.26/Test145 restores native animation-linked bow audio and retains user-confirmed bow strength, T/G abilities, friendly targeting, melee aiming dot, shared dismount continuity and Z corrections. Sound decoupling is deferred. Preserve exact HUD144 and QTE140 during this release; rebuild only the changed adapter and release wrapper. See docs/releases/v0.26-technical.md. Fresh closed-game checks, full exact144 installer rollback and post-install READY remain required.

# Test144

Test144 correction: user confirmed143 bow strength and friendly ability targeting; preserve. Bow audio rejected, melee dot requested; fresh CLOSED authorized offline fix/install. See docs/development/Test144.md and local receipts. Fresh READY/loaded144/capture gate required after installation. Published025 exact140 unchanged.

# Test143 correction candidate

Test142 was played without a formal loaded gate and rejected/partial: bow power, Inspire target/sound/layout, dismount possession and Z failed. User CLOSED authorized correction and install; added all-mounted dismount scope, RMB animation/LMB draw sound and release-time stop/release cue. Read docs/development/Test143.md and actual local artifacts/Test143 receipts. Preserve accepted Rapid/native1/Q/detachment/fire/camera/movement/siege and exact QTE140. Published v0.25 remains exact Test140; no publication requested. Fresh READY plus exact143 loaded hooks and actual recorder checks are required after installation.

# Test142 development (checkpoints3/4)

Published v0.25 retains accepted Test140, with charging marked buggy. Test142 adds native whole-unit T/G abilities and special ammunition, removes Tab ammo cycling, and adds bow-only LMB hold/release strength. It is not live-accepted. Read docs/development/Test142.md and both checkpoint notes. Preserve exact QTE140 and accepted140 rollback. Final validation/deployment status is in the local Test142 receipts. No automatic game launch or live trace; fresh READY and loaded/capture checks are still required for formal play.

# Contributor instructions

This is a native C++ first-person possession mod for Total War: SHOGUN 2. Read README.md, BUILD_AND_TEST.md, SOURCE_MAP.md and VALIDATION.md before changing it. Use the preserved native sources and existing indexed decoding.

## Current baseline

v0.25 retains the exact Test140 gameplay, accepted by the user with charging explicitly marked buggy. This is user-reported acceptance; no formal Test140 loaded-build gate was run. Exact accepted Test138/v0.24 remains recoverable. Preserve Test136 Rapid Advance, the native1 melee toggle, Q targeting, reversible squad detachment, independent formation firing, camera, HUD and QTE.

Ordinary native locomotion states0/4 pass unchanged. The accepted melee readiness handling and scoped advance-task detached-actor compatibility remain. This acceptance does not certify all animation assets, measured FPS or universal crash safety. First-person weapon/arm quality is deferred. Ultra crowd slowdowns and other crashes remain open.

Checkpoint2 is native infantry/cavalry charging. Published v0.25 retains Test140 charge-contact handling after Test139 locomotion worked but contact became a QTE. The user accepted it with a charging caveat. Read docs/development/Checkpoint2.md and docs/development/Test140.md before extending it. T/G abilities and variable bow draw are now the unaccepted Test143 correction candidate. Use native ability definitions, restrictions and original game icons/button artwork; no placeholders.

## Changes and validation

Preserve native unit AI, formations, ranged fire, melee outcomes, movement and siege behavior. Ownership filters must never grant authority without current full actor/unit/lease proof. Keep individual-read fallbacks. Do not reintroduce discarded Test116/117 experiments; their source remains in Git history.

Run the relevant offline fixtures. Keep work light while the user plays. Close the game before replacing native DLLs and preserve an exact rollback. Live tests require a fresh user READY/IN BATTLE cue, loaded-file hashes and actual authorized capture verification before formal play. Do not launch the game, start extra profiling or change graphics/CPU/affinity settings without authorization.

Ordinary recording starts independently of READY; preserve the existing watcher and its configuration. Honor explicit recorder-off instructions. Recording health and loaded identity do not establish gameplay acceptance.

## Releases

Retain accepted gameplay binaries during promotion; rebuild only release loader/installer metadata. Pin the complete native/tool source closure, verify embedded resources and run full installer upgrade/rollback checks, including the exact immediate predecessor, all interruption stages and repeat backups. Verify VERSION, marker and PE versions together.

Publication requires an explicit user request. Keep raw dumps/input logs, game assets, absolute local paths and credentials out of Git. Local requirements are in docs/LOCAL_DEPENDENCIES.md. Historical candidate notes describe their own scope; use the latest accepted baseline above.
