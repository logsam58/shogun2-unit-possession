# Test118: reduce unrelated melee-check work

Subsequent live result: the user reported substantial improvement and no initial lag, with lag still occurring later. This is ordinary-play feedback rather than a controlled frame-rate benchmark. Test119 continues this work; see [Test119](Test119.md). The installation and launch status further below describes this candidate's original preparation snapshot.

Crowded first-person play can run the mod's melee interception for fights that do not involve the possessed soldier. The complete foreign update callback repeated camera and actor ownership checks even when a valid infantry identity had already excluded that soldier. The native selector also read player identity outside the replay scope, where that result was discarded.

Test118 removes those duplicate checks. It proves the current camera identity once, tests participant membership using a local 80-byte read, and retains the original individual reads if the full read fails. Possible mounted subjects still require the existing reciprocal ownership proof. No identity is cached between callbacks. The original native selector is still called once with the same arguments and its return value is preserved outside replay.

This is a measured reduction in mod overhead, not a confirmed explanation or fix for the entire battle slowdown. The user reported that the exact v0.18 rollback also lagged after F9 against two enemy armies. Its earlier capture contained no QTE acquisitions, so a visible prompt is not required for that slowdown. Melee interception still runs without a visible prompt.

## Candidate source and installed state

The installed candidate uses **v0.18 / Test107** as its base. It was built from the retained v0.18 source archive after checking 389 members against commit `cf1dfaad9c43592dcee5632addaa54de3af6dcad`. Only the two QTE source corrections and their new regression fixture were applied. The adapter and HUD are the exact published v0.18 binaries. QTE timing, controls and artwork remain at v0.18.

The same changes are present, uncommitted, in the maintained checkout. That checkout's HEAD is `451ec6a127e8cf80ba3d6ebd32675633817adcda` and contains later gameplay changes; rebuilding it does **not** reproduce this installed candidate. The exact isolated build is retained under private `artifacts/Test118/v018-source`, with its source and asset hashes in `backport-source118.json`. No release, tag, push or version promotion was performed.

Test118 is installed locally, **not loaded or live accepted**. Cache identity is `ffec5cbf0a2d11db`; it will be extracted on the next user launch. Private deployment and final-verification receipts record the installed hashes and exact v0.18 rollback.

## Validation

The new fixture runs the production dispatch and selector code with an instrumented native forwarding stub. It compares the scope predicate with its frozen previous implementation across 6,002 identity, membership, mounted alias, malformed-input and inaccessible-page cases. Native engine actions are stubbed; this is not a running-game benchmark.

| Measurement | Previous callback | Test118 callback |
| --- | ---: | ---: |
| Memory reads per unrelated update | 107 | 27 |
| Bytes read per update | 404 | 254 |
| Unrelated selector identity reads | 37 | 0 |
| 20,000 callbacks, measured CPU time | 865.842 ms | 219.077 ms |

The old implementation deliberately fails the new read-budget assertion. The corrected v0.18 build passes all **37 QTE fixtures** and both loader checks. The time figures describe this synthetic callback only; they do not predict whole-game frame rate. The adapter and HUD were retained rather than rebuilt.

Deployment used the existing SHA-guarded transaction, not a historical installer. Fresh checks covered recovery at all four committed prefixes, successful replacement and stage-collision rejection. Ten rollback files were saved, 24 protected files remained identical, and the installed loader's embedded adapter/HUD/QTE resources were independently rehashed. The recorder baseline was replaced last, after both game files verified. No settings, graphics caches or other runtime caches were changed.

## Next live check

The original automatic recorder remains armed. No game is currently being captured, and no new focused trace is authorized. After the user's fresh launch and READY, run private `artifacts/Test118/verify_live118.py menu`. After IN BATTLE, run it with `battle`; that gate also requires a native battle phase, loaded component hashes, ready mappings and actual native input/memory recording. Do not reuse a prior process's readiness.

The user explicitly redirected work away from crash analysis to the lag. Do not resume crash investigation or another profiling exercise merely because ordinary capture remains enabled. The preceding interrupted comparison was not a completed, cued RTS/FPS experiment and does not establish a performance result for this candidate.
