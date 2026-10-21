# Test146: native charge participation

Unpublished v0.26 development candidate. The player reported three games with charge running but unreliable target acquisition, kills and cavalry leaps; QTE still intercepted the approach. This reopens checkpoint 2. Published v0.26 remains unchanged.

The rejected bridge required W+Shift, copied a peer's individual target, kept manual movement involved and called the base charge constructor for horses. Detachment also removed the player from the native squad planner that assigns individual charge tasks. The preserved decoder and animation companion were reused read-only; no research archive was rebuilt.

Test146 temporarily restores primary squad participation only after current actor/unit/lease proof and a native formation charge. The native planner receives the possessed human and reciprocal horse, chooses their individual target/task and controls movement. Manual movement, weapon and command input yield during this interval; F10 and normal exit/focus handling remain available. Native completion, interruption or invalid ownership releases the handoff and restores ordinary detachment, preserving independent formation arrival and firing. Unit ownership, master rosters and unit statistics are retained.

The QTE runtime accepts a proved current native charge task before its visual charge pose, or a fresh numeric adapter handoff beside independently verified current camera/actors. It forwards original encounter preparation, selection, targets and updates. Original kill, failure, counterattack, jump eligibility and authored clips remain native decisions. No defense animation, forced kill, target-radius rule or charge minigame is added.

The bounded peer search remains at most 32 records per 100 ms with a shared 2,000 target-entry budget. Active source checks use the remembered current roster slot. The fixed numeric handoff is one record; no event queue or new crowd scan is added.

Offline fixtures execute original squad insertion/removal, formation firing waits, human/horse charge construction, rider task propagation and the native jump factory. Native unit planning/target choice, scene/path data, encounter selection/preparation and rendered clips have explicit providers. They cannot certify a real charge kill or leap. The charge/QTE fixture tests native result/attacker/defender preservation for foot and mounted actors, early contact, replaced-task handoffs, stale/foreign/torn observations and repeated updates without rerolling selection.

Required final checks: fresh adapter86, QTE47, loader2, full installer exact Test145 plus every prior predecessor, all seven interruption rollbacks, repeat backups and Python7. Retain exact HUD144/7. Preserve exact Test145 binaries/source and rollback. Installation requires the closed-game guard. A fresh READY/IN BATTLE cue, exact loaded hashes/hooks and actual crash/input/memory/CPU/GPU recording are required before formal testing.

Charging remains unaccepted until live infantry/cavalry comparison confirms target lock, approach, native engagement/outcome and return to normal possession. Bow audio decoupling and POV quality remain deferred; Ultra crowd slowdowns and other crashes remain open.
