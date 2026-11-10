# Shogun 2 Unit Possession — testing branch

The combined PC/Steam Deck source is the maintained implementation. Read README.md,
BUILD_STEAM_DECK.md and docs/current-release.json first. Native/deck contains the
combined runtime and build-time adaptations; older numerical suffixes are historical.
Use owned game resources and isolated output directories. Never commit extracted
assets, engine DLLs, compiled builds, credentials, local connection profiles or dumps.

Offline work and tests may run while the owner plays. Actual installation or staging
requires explicit approval, exact rollback and a closed receiving game and launcher.
A live SSH/memory check needs readiness and verified process/build identity; keep it
read-only and bounded. Publishing GitHub source does not upload Steam Workshop.
Preserve repository identity, visibility, prior tags/releases and the existing
Workshop item 3825625050. No installer downloads. Keep the MIT and vendor notices.

Use evidence from native decoding and original instructions for engine changes.
Keep build/fixture results separate from real gameplay acceptance. The current
revision34 enemy-target acquisition still fails the owner's live trial; do not
claim it fixed. Charge, climbing, camera and infantry/cavalry animation acceptance
are separate, scoped results. The latest source includes those targeting attempts.
Do not resume targeting development or live testing just because source is published.
No subagents unless the owner explicitly requests them.

Revision36 corrects the Deck campaign setup clipping parent; the owner accepted
the Start Game footer fix. Preserve PC device bypass and the normal native
button/content geometry. This acceptance does not resolve enemy acquisition.

Revision39 adds native cleanup of excluded QTE candidates' combat reservations
and retains revision38's current-possession guard and revision37 renderer
coexistence. The full fresh build and PC/Deck rollback/readback passed. The owner
reported “success” after installation; the exact scenario was not specified and
the formal revision39 loaded-hash/hook gate was not completed. Keep this scoped
manual report separate from all-siege or stability proof. Enemy acquisition
while separated remains unresolved. See V0_34_MELEE39_VALIDATION.json.

# Private naval Revision 127 staged locally on PC

Owner requested offline Ghidra implementation and explicitly approved staging on
completion. Revision127 is staged in the existing PC workshop165 entry, pack SHA
1f3fa364cbff8a1d4c0cf3112d413968f1419b0237b9e6992cda6f7bed3af15f.
Receiving game/launcher were closed; 22 protected paths remained exact. Verified
Revision126 rollback is in D:/Shogun2Deck/promotion/naval-crew127-pc-stage-1.
No Workshop upload, GitHub publication, Deck transfer or new live inspection.

Scope: PC single-player normal marine/seaman movement, mouse camera, native
return to retained duty. Select one friendly ship, F9/Caps, WASD/Shift, F10 return.
Start Battle first. Personal naval combat, boarding, crew switching, naval HUD
and Deck naval controls remain unimplemented. Do not claim live naval success.

Ghidra verified dispatcher/request/transport/station paths against stock and
reviewed Bugfix. 22 native contract and 16 production-header fixture cases per
engine passed, including repeated same-sailor return and peer crew progression.
Projection/path submission are explicit fixture providers: live deck pathfinding
and moving-ship behavior still need owner testing. Broad regressions passed
100 adapter, 51 QTE, 8 HUD cases, plus resource/package/Lua/campaign checks.
Four preexisting weapon-test stubs needed the optional type-output argument;
original failed results are preserved and resumed verification is documented.

Final build, source snapshots, full evidence and TESTING.txt are at
D:/Shogun2Deck/promotion/naval-crew127-final. FINAL-VALIDATION.json links all
receipts. Canonical source changes remain local/uncommitted. Earlier preliminary
naval-crew127 output is superseded and must not be staged. Preserve all accepted
land behavior and the no-live-check instruction until fresh owner readiness.


# Private naval Revision 128 staged locally on PC

Owner explicitly confirmed PC game and launcher closed and requested staging.
Actual closure verified before preparation and atomic swap. Installed pack SHA
601f6ea87e11e9f7365d261926be82164be9e4d52a6b3e7118071bc934c2f94b.
All 22 protected paths unchanged. Exact Revision127 rollback and safeguards
passed; backup and journal: D:/Shogun2Deck/promotion/naval-crew128-pc-stage-1.
Authoritative build: D:/Shogun2Deck/promotion/naval-crew128-final-aslr.
Earlier naval-crew128-final failed its relocated-engine startup check and is
superseded; never stage it. Revision128 corrects the native naval camera update,
return barrier and relocation-aware exact hook verification. Offline native
camera/task/contract and broad regressions passed on owned stock and reviewed
Bugfix fixtures. Live Revision128 gameplay acceptance remains pending.
No Deck transfer, Workshop upload, GitHub publication, game launch or new live
inspection was performed. PC single-player sailor movement/camera/duty return
only; naval weapons, boarding, crew switching, naval HUD and Deck naval input
remain unimplemented. Launch a fresh game to test; preserve accepted land work.


# Private naval Revision 129 staged locally on PC

Owner accepted Revision128 sailor entry but reported repeated stutter, wrong
facing and sideways walking with camera-relative W. A bounded ready-authorized
30-second read-only capture verified128 and observed repeated requests/rejected
projection and ~10Hz actor positions against ~110Hz camera callbacks. No live
capture or debugger remains running.

Revision129 uses the shared native movement integrator with land steering/facing,
one initial walking context, native locomotion setters and native rendered pose
interpolation for the camera. No repeated waypoint stream or actor teleport.
Ghidra exports and 311 function byte comparisons agree on stock/reviewed Bugfix.
Only adapter.cpp (two-line naval preparation handoff) and naval_control127.h
changed among native production files versus128; land implementations exact.

Offline checks passed:18 native control cases per engine (including40 direction
combinations,200 input changes,120 sub-tick rendered poses and100 same-sailor
returns),7 camera integration cases,69 relocated-engine startup cases per engine,
100 adapter/51 QTE/8 HUD programs,resources/package/Lua/campaign/maintained checks.
Exact backed-up128 reproduces wrong facing and stale camera in the new fixture.
Synthetic worlds do NOT prove collision broadphase, actual deck pathfinding or
visual gameplay. Revision129 live acceptance remains pending.

Owner freshly confirmed PC game/launcher closed. Actual closure checked before
preparation and atomic swap. Existing local workshop165 entry is now pack SHA
8e11c9ff1abf28ea697f1abbe5a9c04c5dd982b352910bed905651bd91d32f31.
All22 protected paths unchanged. Exact128 rollback and accepted126 backup are in
D:/Shogun2Deck/promotion/naval-crew129-pc-stage-1. Full candidate/checklist/evidence:
D:/Shogun2Deck/promotion/naval-crew129-final. No Workshop upload, GitHub publication,
Deck transfer, game launch or new live check. Production source stays uncommitted.
Scope remains PC single-player normal sailor movement/camera/duty return only;
naval personal combat,boarding,crew switching,naval HUD and Deck naval controls
are unimplemented. Fresh launch required. Keep readiness/build identity gates
for any future live inspection; do not infer gameplay success from offline tests.


# Revision129 accepted and reserved for Testing branch

Owner confirmed stutter removed, correct directions, clean movement on a moving
ship, and possession/release/repossess. Record scoped manual acceptance only;
no new loaded-hash gate or all-feature/long-term claim. Owner requested saving
this experiment to Testing branch and restoring the main mod to the uploaded
Workshop build. Stop naval development here until another task.

The current Steam download is exact Revision126, pack64054dc25361ee7e21c70ddddcb526dd56d6f789125ea7abf5202d819f70249d,
manifest7228153338856273762. Stable main stays897bc56411df42135c982cc4fa1cab04c3031964;
testing holds the naval source. This owner request supersedes the older main-only
branch policy. Preserve all tags, releases, repository visibility and Workshop IDs.
Exact129 pack is reserved for the existing Testing branch launcher filename and
its existing labelled thumbnail. No Workshop upload or Deck transfer requested.
Local save/restore journal is D:/Shogun2Deck/promotion/naval129-testing-save-1.

Both PC pack swaps completed with exact readback and21 protected paths unchanged.
Main is exact Workshop126; Testing is exact accepted129; both preview PNGs and
launcher selection preserved. Owner was done playing and actual receiving game
plus launcher closure was verified. No Deck change or Workshop upload.
