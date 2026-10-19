# Test120: reduce repeated horse and rider checks

The Test119 play report separates two remaining problems: cavalry lagged immediately, while other units ran well until large enemy groups were nearby. The user then exited. Saved automatic module inventories confirm that the session loaded Test119; no menu/battle readiness gate was completed before that play. The report describes perceived performance, not a controlled frame-rate comparison.

Test120 continues the exact v0.18/Test119 backport with three changes:

- Mounted scope checks first exclude witnesses containing neither the current camera actor nor its mount. A possible mount match still goes through the full current reciprocal horse/rider proof.
- Reservation checks use a callback-local rider-address filter. Unrelated participants can be excluded before proving both actors repeatedly. Failed reads fall back to full validation; possible rider matches still require current identity, a single rider, reciprocal links, matching unit/side/environment and the original special-model restrictions.
- Combat identity reads batch the actor's type, alive flag, unit pointer, serial, side and height into one current 536-byte prefix, then read the unit serial separately. All validation rules remain. If the prefix cannot be read, the original individual-field path runs.

The filters never grant ownership. No identities or aliases persist across callbacks. There are no new hooks, native writes, timing changes or lock changes. The exact v0.18 adapter/HUD and all previous backport corrections remain; later clock116/render-copy117 changes are still absent.

New tests exercise the full production dispatch on a reciprocal synthetic horse/rider pair, both with and without a reserved QTE. They also compare reservation and release decisions against the frozen Test119 implementation through malformed groups, reused identities, death, dismount and multi-rider states. A separate 8,004-case fixture compares combat identity results and output bytes against Test119, including failed bulk reads and an inaccessible-page boundary. Native actions in these fixtures are stubs; the results are not game FPS measurements.

Exact source inputs and assets are pinned under private `artifacts/Test120/source120.json`; the isolated build lives in `artifacts/Test120/v018-source`. The maintained checkout contains later gameplay code and cannot be substituted for that backport build. Build/deployment receipts establish completed stages. This work does not publish or promote a release.

## Results and deployment

All 42 QTE fixtures and both loader checks passed. Five alternating benchmark runs per build produced these medians:

| Unrelated encounter | Reads, Test119 → Test120 | Calls per run | Test119 time | Test120 time |
| --- | ---: | ---: | ---: | ---: |
| Infantry | 21 → 21 | 20,000 | 193.201 ms | 184.340 ms |
| Infantry, pair reserved | 22 → 22 | 20,000 | 202.517 ms | 195.082 ms |
| Mounted | 125 → 25 | 10,000 | 503.033 ms | 108.201 ms |
| Mounted, pair reserved | 371 → 28 | 10,000 | 1,464.520 ms | 120.841 ms |

Mounted unrelated-contact reads decreased from 246 to three; unrelated release checks decreased from 49 to two. A valid combat identity now takes two reads rather than seven. Infantry's unchanged read counts are not evidence of a new crowd FPS gain. `measure120.json` records all runs and executable hashes. The unchanged Test119 implementation deliberately fails the new mounted read budget.

Test120 is installed, **not yet loaded or live accepted**. The game was confirmed closed before replacement. Ten exact Test119 rollback files and 24 unchanged protected files were independently verified, as were the installed loader/marker and three embedded runtime payloads. Fresh transaction checks passed all four interrupted prefixes, success and stage collision. The baseline was updated only after the game files verified. No settings or existing cache directories changed.

Cache identity is `0bfecffe95030f27`. The original automatic watcher remains armed with no active game session. After the user launches and replies READY, use private `artifacts/Test120/verify_live120.py menu`, then its `battle` gate after IN BATTLE. Do not reuse Test119 process readiness or claim that this fixture result resolves the remaining in-game lag.
