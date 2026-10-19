# Test121 — current camera scope and committed pose reads

Test120 user result: “Cav was fixed, and large army combat got another improvement. It went for a bit then lagged hard under substantial army presence.” The user also suggested state accumulating across possessions. Cavalry success and partial crowd improvement are user observations; remaining severe crowd lag is unresolved. Unit size and exact lag timestamps were not specified.

Saved ordinary module inventories retrospectively confirm Test120 loader, adapter and QTE in game5404/creation134368595259507667. Session `D:/Shogun2Recordings/20261019-005846-pid5404` ended with recorded exit0. No menu/battle readiness gate preceded this play. See private `artifacts/Test120/live-result120.json`; no crash investigation was resumed.

## Evidence and carryover limits

The existing recording contains 771,497 registry lookups and exactly 771,497 probes: every lookup remained one probe, with no full table. Peak live encounters was699; live entries returned to0 at session end. Fifteen-second windows rose to about11,855 registry accesses/second under later combat. Those accesses include lifetime/removal/playback queries, not exclusively preparation/update calls.20 QTE acquisitions and20 resolutions, zero recorded cancellations and zero QTE faults. Cumulative counters are not queued work. This rules against deterioration of this registry's lookup cost in this recording, not against all engine or possession-state carryover. There are no synchronized user lag markers or Present measurements. Raw paths/hashes and windows are in `carryover-audit121.json`.

## Change

The exact v0.18/Test120 continuation batches current camera identity, mount and encounter backlink into one callback-local0x68c-byte snapshot. The same current lease, pointer chain, numeric actor identity and unit identity remain required. A single local participant snapshot serves camera and mount membership. Potential mounted ownership still gets the original fresh reciprocal proof. Both gates in a full callback remain; no old negative result is reused at a later gate.

Committed pose refresh batches table/serial/unit/position into a current0x218-byte read, then reads the unit serial separately. Failed snapshots retain the original field-read path, including partial output behavior and readable groups at inaccessible page boundaries. No cross-callback identity cache, time grace, new hooks, writes, timing/control/artwork/settings change, or blanket removal of safety checks.

## Validation

All43 QTE fixtures and2 loader fixtures passed. New differential coverage includes6002 scope states,6000 pose result/output cases,10,000 possession/camera transitions, forced bulk-read failures and real inaccessible camera/witness tails. Earlier8004 combat identity and6000 mounted overlap/24000 release cases remain passing. An additional retained private fixture ran5000 acquisition/release/removal cycles through one production Session, alternating admission removal and lease reset, with one native replay per cycle and no pending-key/error accumulation. Those native actions are fixtures, not a game soak test.

Five alternating runs per build, production dispatch with native actions stubbed:

| Callback | Test120 reads | Test121 reads | Test120 median | Test121 median |
|---|---:|---:|---:|---:|
| Infantry,20,000 calls |21|17|197.399ms|162.212ms|
| Infantry with held pair,20,000 calls |22|18|211.439ms|172.063ms|
| Mounted,10,000 calls |25|19|115.597ms|88.5393ms|
| Mounted with held pair,10,000 calls |28|22|128.611ms|101.883ms|

Committed pose refresh7→2 reads. The unchanged Test120 implementation passes functional comparisons and fails the new read budget deliberately. These measurements show roughly18–23% lower callback CPU time, not18–23% higher game FPS or a demonstrated whole-lag fix. Read `measure121.json`, `baseline-rejection121.log`, `repeated-session121.json` and build logs.

## Deployment and next check

Installed Test121/cache`integrated-4f85022c6873b244` with game verified closed. Source was built in isolated `artifacts/Test121/v018-source` from the verified v0.18 archive plus118/119/120/121 changes. Maintained checkout remains intentionally dirty at HEAD451ec6a127e8cf80ba3d6ebd32675633817adcda; do not build current main as this backport. Later clock116/render-copy117 remain absent. Exact v0.18 adapter/HUD retained.

- Loader `84c7c115b1ed2d6e64990e3ac43b74fdb3edc05658d75db923a664f3bb0c1444`
- Marker `1026d3def6c4f8d49ec1d37e8f213f12aed2a6d05d8048d6254a9bc765858689`
- QTE `95b37877e3090c4c4b8e952a3665d0401d761b2946861f85dc1bfbbcfea8dfeb`
- Adapter `06d8895e4532644d8fab96ccaf50e8fb6f2cb9ba109704e3e2ae34efb9ef48fe`
- HUD `141906cc0991b52e65783db0084af3d57b900540af85dec5eec0b5ace27655d3`

Ten exact rollback-Test120 files preserved;24 protected files unchanged. Four interrupted-prefix transaction recoveries plus success/stage-collision checks passed. Installed and embedded hashes independently verified; recorder identity baseline updated last. No cleanup, game/settings/graphics changes, launch, recorder restart, Git commit/push/tag or publication. Original watcher9880/creation134368525366282010 remains freshly verified ARMED_NO_GAME, no stop flag, no active session.

NEXT: user launches normally through Steam, stays MAIN MENU, replies READY. Run private `verify_live121.py menu` against the actual fresh process; then user IN BATTLE and `verify_live121.py battle` verifies phase, actual loaded components and crash/OS/native input/memory/resources before formal play. No previous readiness reuse, focused profiler/hardware trace or crash investigation. Test121 loaded=false/live_accepted=false until new evidence.
