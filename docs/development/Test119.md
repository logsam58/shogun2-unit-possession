# Test119: reduce crowd checks during a reserved QTE

Subsequent result: the user reported a small further improvement, immediate cavalry lag, and lag near large enemy groups with other units. They exited afterward. Existing automatic module inventories confirm Test119 was loaded, retrospectively; the menu/battle gates were not completed before that play. See [Test120](Test120.md) for the next mounted-path correction. Installation status below is the original preparation snapshot.

The user reported a substantial improvement with Test118, including no lag at the beginning of the battle, but still experienced lag later. This is user feedback from ordinary play, not a matched-scene frame-rate measurement. Test119 continues the same reduction of repeated native-memory reads.

Two changes are applied to the exact v0.18/Test118 source:

- The reservation filter checks participant addresses before proving actor identity. Unrelated infantry encounters need one bounded participant read; possible matches still receive the original current identity checks. This brings the previously tested Test114/115 reservation optimization into the v0.18 backport, where it was absent. Mounted aliases retain the existing reciprocal checks, and failed bulk reads retain individual-field fallback.
- Camera identity reads use one current 536-byte actor prefix in place of separate reads for its type, serial, unit pointer and alive flag. The camera pointer chain, shared possession lease and unit serial are still verified each time. A failed prefix read falls back to the previous field reads. Nothing is cached across callbacks.

These changes reduce system-call count at the cost of reading more bytes for the camera identity. They add no hooks, allocations, time grace, native writes, timing changes or rendering changes. The v0.18 adapter and HUD remain byte-identical. Later clock116 and render-copy117 changes are not included.

The complete production update wrapper is exercised both with and without a reserved player/enemy pair. Native forwarding functions are instrumented stubs; this fixture does not execute a running battle. It checks native return values, exactly-once forwarding, reservation preservation, scope decisions and lease results. The existing 6,002 scope cases now also compare camera identity with its frozen Test118 implementation in 6,000 states. Existing reservation fixtures cover 5,152 differential/identity/fallback cases, mounted aliases, identity mutation and a 100,000-encounter flood.

The exact build is retained in private `artifacts/Test119/v018-source`, with inputs in `source119.json`. The maintained checkout also has the camera correction and new fixture, but contains later gameplay code: rebuilding that checkout does not reproduce this backport. Private build, transaction and live-verification receipts establish which stages have completed. No GitHub publication or release promotion is part of this work. Live performance must be evaluated separately from offline read counts.

## Completed checks and deployment

All 40 QTE fixtures and both loader checks passed. Five alternating runs of the complete callback fixture produced these medians for 20,000 calls:

| Case | Test118 reads/call | Test119 reads/call | Test118 time | Test119 time |
| --- | ---: | ---: | ---: | ---: |
| Unrelated fight, no reserved pair | 27 | 21 | 226.370 ms | 187.716 ms |
| Unrelated fight, reserved pair | 120 | 22 | 977.339 ms | 194.897 ms |

Unrelated contact checks decreased from 93 reads to one; unrelated release checks decreased from 14 reads to zero. The unchanged Test118 implementation deliberately fails the new read budget. These timings measure the fixture's callback work, not a game's frame rate. The initial single-run result was noisy; `measure119.json` retains all alternating measurements and executable hashes.

Test119 was installed with the game confirmed closed. The exact ten-file Test118 rollback is retained under private `deployment/rollback-Test118`. Fresh transaction checks passed all four interrupted prefixes, successful replacement and an existing-stage collision. The independent final check verified installed files, all three embedded runtime payloads, ten backups and 24 unchanged protected files. The recorder baseline was updated after the installed game files verified.

Cache identity: `3c9a0c8ed1a53da1`. The candidate is installed, **not yet loaded or live accepted**. The original watcher remains armed for the next user launch; no game or extra profiler was started. After READY at the menu, use `artifacts/Test119/verify_live119.py menu`, followed by its `battle` gate after IN BATTLE. It verifies actual process generations, loaded backing hashes, capture and native battle phase. Test118's earlier readiness cannot be reused for Test119.
