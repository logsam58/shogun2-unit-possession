# Architecture

## Runtime components

The game loads the mod through a game-local DINPUT8.dll bootstrap that forwards the Windows DInput interface. The integrated loader checks the selected mod and supported engine, verifies embedded component fingerprints and starts the possession adapter, QTE runtime and HUD from a versioned cache. Players launch through Steam after setup; no separate manual companion startup is required.

The possession adapter and QTE DLL run as x86 native code in the game process. The HUD is a separate process consuming numeric publications. MinHook supplies native trampolines. Native hook addresses and structure assumptions target one fingerprinted Empire.Retail.dll build; a changed engine requires analysis and validation before support can be added.

## Ownership and native forwarding

Possession owns one current logical actor through a validated lease. Checks include battle/camera identity, actor serial and holder, reciprocal actor/unit relationships, selection, routing, death and mount state. Address reuse is not identity. Invalid current ownership causes cleanup rather than permitting stale pointers to remain active.

Hook behavior is scoped to the owned actor and intended native call context. Unrelated actors and calls forward to original game behavior. Personal ranged-task completion changes must not globally complete other soldiers' tasks or replace their AI targets. Native ammo, casualty and unit-order policies remain authoritative.

Movement feeds native intent and preserves unit speed, fatigue, collision and terrain integration. Siege traversal uses native registered wall resources, path nodes, climb parameters and animation progression. Crest-to-landing ownership retains the current live actor without overriding native physics/contact or extending identity by a time grace. Ground/weapon commands wait for native contact.

## Weapons and accuracy

The adapter recognizes bounded native projectile records and weapon keys, supplies a muzzle direction and circular spread, and invokes native projectile creation. Effective native unit accuracy drives cone width. Game projectile calibration and physics remain inputs; the circular conversion differs from stock rectangular scatter.

Rendering publishes projection scale for the firearm reticle. Accuracy sampling occurs at the owned native simulation boundary. Reusing a paused sample requires matching ownership and native tick. Equipment/ammo changes are sequenced around native shot/reload/commitment state, with support determined by actual records, unlocks and animation availability.

## QTE scheduling and input

Native pair selection/preparation is retained, including valid deferred preparation in crowds. A response event begins only when registration/backlinks and current participant facts are ready. Preparation has a native waiting limit; it is not a response event with a running player timer.

An unanswered event expires only after both three active-simulation seconds and one complete phase cycle. Pause freezes age and phase; speed updates preserve accumulated progress. Timestamped input is judged against matching submitted marker/zone geometry up to 50 ms old while the live simulation deadline remains authoritative. An expired event cannot be revived by a stale click. Canceled admitted events retain enough numeric context to display INTERRUPTED without dereferencing former participants.

Input polling requests 1 ms pacing; native ownership/publication services request 8 ms and urgent focus/stop/stale-presentation work is serviced promptly. Rendering uses monitor-rate pacing and reuses its DIB/DC surface. Requested pacing is not a guaranteed scheduler interval or physical display-latency measurement. The fixed-capacity registry uses backward-shift deletion to avoid accumulating tombstones during actor churn.

## Publications and diagnostics

Shared HUD/status, equipment, ammo, accuracy and climb messages carry numeric data. Protocol sizes/versions are compatibility contracts shared by producers and consumers; internal filenames can retain older suffixes across product releases. QTE Publication91 is internal DLL storage, not a named mapping. Private telemetry records additional timing, ownership, ranged-task and traversal diagnostics without changing public layouts.

The optional external recorder is independent of mod readiness. It watches launches, attaches a debugger and records available core resources/input immediately; native channels can become available later. Capture never depends on pause or a READY response. Backing-file hashes identify loaded images, while a dump preserves patched runtime memory. A native fault address establishes where the failure surfaced, not which component caused the preceding bad state.

## Setup and release

The installer verifies the retail engine and payload, refuses unrelated same-name injection files, updates managed files and mod selection transactionally, and retains one previous managed snapshot. Interrupted stages restore prior files and registry state. Cache cleanup removes only recognized unchanged caches; locked, modified and unknown items are deferred.

Build artifacts, proprietary inputs and real recordings stay outside Git. Release packaging pins the reviewed source commit and component hashes, checks embedded resources and versions, then assembles player downloads. Publication uploads a draft, downloads and verifies every asset, and only then publishes the requested version. It does not install or launch the game.
