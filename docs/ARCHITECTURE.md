# Architecture

## Runtime components

The current v0.33 release packages Test173. The enabled pack
replaces the front-end root Lua entry. Its first-party wrapper stages and checks
its embedded native bridge, then calls that bridge to recover the original UI
script from the player's owned game. The original script runs before the wrapper
requests gameplay startup. See the [bridge design](public-release/WORKSHOP165.md)
and [Test166 storage changes](public-release/WORKSHOP166.md).

The bridge checks the host and shared engine contract, pins its module,
and starts the shared worker once. The worker verifies embedded component
fingerprints, prepares or reuses validated local resources, and starts the
possession adapter, QTE runtime and HUD from a versioned cache. Workshop starts
in third person; P changes perspective. No installer, DInput bootstrap, camera
chooser or Steam launch-option edit participates in this route.

The possession adapter and QTE DLL run as x86 native code in the game process. The HUD is a separate process consuming numeric publications. MinHook supplies native trampolines. Native hook addresses and structures target the original x86 engine family. A shared contract admits the stock/reviewed Bugfix image or a coherently compatible protected-region basis; exact loaded hook prefixes are checked separately. See [engine compatibility](public-release/WORKSHOP172.md).

The [source map](../SOURCE_MAP.md) and [native guide](../native/README.md) locate
these entry points. Historical directory suffixes are not product versions.

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

The Workshop runtime keeps status channels and numeric diagnostics, but disables
routine gameplay, input, QTE and startup disk logs. Its small live-entry receipt
uses a delete-on-close handle. Code/resource caches remain; compatible builds
share verified resources where supported. Unknown files and old caches are not
automatically removed.

The optional [external recorder](../tools/diagnostics/README.md) is a separate
developer tool, absent from the player pack and inactive unless explicitly
started. Once started, it watches launches, attaches a debugger and records
available core resources/input; native channels can become available later.
Backing-file hashes identify loaded images, while a dump preserves patched
runtime memory. A native fault address establishes where the failure surfaced,
not which component caused the preceding bad state.

## Build and release

Builds use explicit inputs and isolated source/output snapshots. The Workshop
builder checks payload closure and pack contents, then exercises matching
float-Lua startup and synthetic resource/storage fixtures. The distributable
`package` directory is separate from private fixtures containing locally
recovered scripts and resources. See [build and test](../BUILD_AND_TEST.md).

The current release distributes the offline-checked Test173 pack and matching source;
game scripts, artwork, fonts and voice are prepared locally rather than shipped.
No publication or installation is implied by a build. See the [release
procedure](public-release/RELEASING.md), [validation](../VALIDATION.md), and
[native contracts](public-release/NATIVE_CONTRACTS.md).

## Historical installer route

The installer verifies the retail engine and payload, refuses unrelated same-name injection files, updates managed files and mod selection transactionally, and records flushed identified snapshots before mutation. Uncommitted killed-process recovery restores prior files and registry; committed recovery completes backup retention. Unknown edits stop recovery. Cache cleanup removes only recognized unchanged caches; locked, modified and unknown items are deferred.

That older route used a game-local DINPUT8.dll forwarding bootstrap and, in
later setup revisions, a prelaunch camera chooser. Its retained sources and
transaction tests remain useful, but its installation steps do not apply to
Workshop. Do not run the old loader alongside the Workshop mod.
