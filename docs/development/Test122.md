# Test122 — restore v0.20 features and reduce foreign selector checks

User reports Test121 “again, an improvement. Keep doing what youre doing,” then requests rolling forward to the latest version with “the shameful display and updated HUD etc.” This ends the deliberate v0.18 feature comparison. It does not establish a complete lag fix or request publication.

## Restored feature base

Candidate122 uses published v0.20/Test115, source9de65d7bc0c12c6b5842f5129b2539507e377b84. All442 archived members were checked against exact Git blob identities. The exact released adapter and HUD were extracted as resources from the verified setup file, which was never executed. This restores the updated HUD layout, cropped friendly-fire callout and Test113 QTE presented-image/input work.310 other published native source members remain byte-identical;25 private UI/audio inputs are hash-verified.

Test118 through121 performance changes are carried into this source. The later116 clock-identity and117 render-copy experiments remain excluded. The maintained checkout still contains those intentional uncommitted/earlier committed developments; the actual candidate is `artifacts/Test122/v020-source`, not a build of current main.

The retained HUD's actual WAVE301 resource hashes to `b11e637701714d311b51529522a8a818127591b7ce214bdce12f03c6158014ce`,136754 bytes,1.55-second PCM. Its original3.25–4.8-second crop excludes the routing lead-in. Resource inspection loaded it as data and played no sound. Read `audio-resource122.json`.

## New optimization

Within a coordinated native preparation, the selector still proved full physical ownership for every unrelated fight. A current camera/lease check and bounded participant-address filter now exclude unrelated actors first. A possible camera or mount match repeats the original full physical proof and re-reads the participants. No snapshot can grant ownership. Failed bulk reads retain readable individual entries, and an unreadable mount field falls back conservatively to the original proof. Hook forwarding, argument/return values, native role pointers, selected outcomes and replay handling are retained.

Possible player hits pay the extra filter before the original proof; the intended saving is on the much more frequent foreign preparations. There is no cross-callback identity cache or extra wait, no new hook/native write, and no change to QTE rules or controls.

## Fresh checks and measurement

- 44 existing QTE executables passed in the full build; the new45th selector fixture was compiled/run separately. It checks10000 frozen-predicate selector outcomes/roles, actual native-port/selector forwarding, identity/participant mutation after filtering and partial unreadable participant lists. Existing6002 scope/6000 pose-output cases and10000 camera/lease transitions also pass.
- Five HUD fixtures and the native friendly-fire impact fixture passed freshly, including exact cropped PCM and200 native ABI forwards. Other unchanged adapter checks are retained from the exact release, not claimed freshly rerun.
- Two loader checks and transactional deployment checks passed. The separate selector fixture initially exposed a test counter not reset after the added10000 cases; its failed log is preserved. Resetting the fixture counter corrected it; no production change was needed.

Five alternating runs compare the rolled-forward v0.20+Test121 runtime with only the new selector change toggled. For10000 full foreign preparation callbacks including the production selector wrapper:

| Path | Before reads | After reads | Before median | After median |
|---|---:|---:|---:|---:|
| Infantry |38|25|183.063ms|120.189ms|
| Mounted |53|27|256.727ms|130.862ms|

The earlier update paths retain17/18 infantry and19/22 mounted reads; their benchmark timings varied without a read-count change. Old selector code passes functional comparisons but deliberately fails the new preparation read budget. This is roughly34%/49% less synthetic callback CPU time, not game FPS or proof that all lag is fixed. Native actions are stubs. See `measure122.json`, `source-audit122.json`, `selector-build122.json`, `qte-build122.json`, `restored-feature122.json` and `loader-build122.json`.

## Current installation

Installed Test122/v0.20/cache`integrated-d9fa303f73de6348` with game closed. Ten exact Test121 rollback files preserved;32 protected files unchanged, including both published release archives. Four interrupted-prefix recoveries, success and stage-collision cases passed. Installed and embedded hashes independently verified; recorder baseline updated last. No old installer, settings change, cache cleanup, game launch, recorder restart or Git publication.

- Loader `1a61fd598f7fc28b89f6ee47a0774a581df00c851e8ae4fe50da69091d326f45`
- Marker `ffea580f82656de54600256cc76e3005f444ae598b0f04428372a8376a1e2b54`
- QTE `e303557a600491800bb4d487714a880be6b6f36d920cf5719aed9c5a79f3c6f3`
- Released v0.20 adapter `460efc036c95d9329205a9661837c2c33ed825b836bb9e4a299ab73f66e92dd3`
- Released v0.20 HUD `2904d8930d536a9d8c0412a662f2aeed969330392e332e5ffe531070d67e67b1`

Test121 loaded identity was confirmed retrospectively from ordinary recording `D:/Shogun2Recordings/20261019-012149-pid27380`, game creation134368609097170665, recorded exit0. No pre-play menu/battle gate was completed. User improvement is recorded in `artifacts/Test121/live-result121.json`; no crash investigation resumed.

Test122 loaded=false/live_accepted=false. Original watcher9880/creation134368525366282010 remains freshly verified ARMED_NO_GAME with no stop flag and no active session. NEXT: user Steam launch, MAIN MENU, READY; `verify_live122.py menu`; then user IN BATTLE and `verify_live122.py battle` against the same current process, with actual loaded hashes, native battle phase and ordinary capture checked before formal play. No old readiness reuse or new focused profiler/hardware tracing.

## Release selection

Subsequent menu and battle gates passed. The player reported "worked great. but crashed" and explicitly selected this checkpoint for v0.21 publication. The active source was aligned to the actual isolated Test122 tree for release; Test116/117 experiments remain in Git history. Release packaging retains exact gameplay binaries. Crash diagnosis is separately authorized and remains open.
