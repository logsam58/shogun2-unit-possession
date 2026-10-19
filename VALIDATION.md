# Validation and evidence

The unpublished [Test117 candidate](docs/development/Test117.md) adds a production native render-copy regression to the adapter suite. Its [receipt](docs/development/Test117-validation.json) separates fresh adapter/loader/installer checks from the exact retained Test116 QTE and HUD. Offline copy speed is not a whole-game performance result.

## What the suites check

| Suite | Coverage |
| --- | --- |
| Adapter: 61 executables (retained from Test109) | Native forwarding, strict identity/lifetime, movement, camera return, weapons/accuracy, equipment, siege path/speed/crest/landing and cleanup |
| Styled QTE: 39 executables (retained from Test115) | Combat preparation, clocks/cycles, input/deadlines, interruption feedback, registry churn, pacing and actual reusable styled surface |
| HUD: 5 executables (retained from Test109) | Consistent contextual state, visibility and presentation policy |
| Loader: 2 executables | Bootstrap, embedded payload integrity and integration |
| Installer fixture | Supported predecessor upgrades, seven interruption stages, rollback, reinstall, collisions, cache retention and isolated HKCU handling |
| Dump reader: 7 Python cases | Embedded, MemoryList and Memory64 stack layouts, exception-context preference and explicit missing-stack handling |

Some fixtures execute bounded original native functions from the supported retail engine with synthetic records and substituted external providers. They are stronger than a pure policy mock, but do not reproduce the full game's collision, AI, asset and casualty lifecycles. Exact fixture limits belong alongside the tested function. No count here proves every unit or siege map is supported.

## Recorded results

[docs/import-validation.json](docs/import-validation.json) records a complete source-checkout build: 50 adapter and 32 styled QTE executables, HUD policy, two loader fixtures, full installer regression and seven dump-reader format cases passed. It includes initial dependency/CWD failures and subsequent successful stages. [docs/source-import.json](docs/source-import.json) pins the 310 original imported source files for provenance; intentional later changes need not match that historical manifest.

[The v0.15 receipt](docs/releases/v0.15-validation.json) identifies exact retained adapter/QTE/HUD component hashes and freshly run release loader/installer checks. Release packaging verifies PE version/resources, payload hashes, marker metadata and source commit. Retaining an already verified gameplay binary is not reported as a new gameplay test run.

## Offline and live evidence

Builds/tests write ignored artifacts/ and do not install or launch the game. Installer fixtures use temporary game/user directories and an isolated registry fixture. The maintenance CLI's `--check-only` mode validates a target and embedded payload without writing the installation.

A live check requires a closed-game update, fresh launch, exact loaded backing paths/hashes and appropriate capture. Test movement, weapon shots and peer volleys, QTE timing/interruption, camera return, death, mounted behavior and siege entry/crest/landing for the affected scenario. Record map, unit, ammunition, battle speed and actual results. A stable short run does not establish long-run stability.

An extended siege session completed climbing/crest/landing but later crashed in native effects processing. The failure remains unresolved, including possible mod involvement. [Known issues](docs/KNOWN_ISSUES.md) records what the dump establishes and what it does not.

Before a source push, run `py tools/audit_repository.py`. It checks staged blob contents for excluded binaries/assets/evidence, oversized files and recognized credential patterns. Binary release assets are distributed separately with checksums; private real dumps are never part of that package.


[Test100 development validation](docs/development/Test100-validation.json) records 52 adapter executables, a final native input/modifier follow-up, both HUD and loader checks, and final-payload installer regression. Four corrected HUD images were rendered and two inspected; formation rendering remains in the game. The unchanged published QTE binary and its earlier 32-case receipt are retained. No live acceptance or crash fix is claimed.

[Test101 correction validation](docs/development/Test101-validation.json) records the final 53 adapter executables, exact native projection/camera-cache and stationary-follow/Q regressions, loader integrity and Test100 transactional upgrade/rollback. HUD/QTE sources and binaries are retained rather than claimed as newly rendered or retested. Test100 was live rejected for Q/Follow Me and rifle/arm clipping; Test101 still needs live acceptance.

[Test102 / v0.16 validation](docs/development/Test102-validation.json) pins the final adapter, native gait/tap regressions, loader integrity and full Test101 upgrade/interruption/rollback payload. The release-build receipt additionally pins the tracked native/tool source closure in [v0.16-validation.json](docs/releases/v0.16-validation.json). HUD/QTE binaries are retained; their suites and visuals are not reported as freshly rerun. New catch-up, native contextual attack/targeting and formation gait still require recorded live play.

[Test103 correction validation](docs/development/Test103-validation.json) records native contextual targeting, current input/camera ownership and full active-animation recovery-lock cases. Test102 user feedback is mixed; remaining release causes and actual attack/recovery behavior require recorded live validation. Exact Test102 upgrade and interruption rollback are included in the new installer payload checks.

[Test104 validation](docs/development/Test104-validation.json) covers original native selected/wavering followed-actor writes, scoped camera/holder lookup across roster compaction/duplicate indices, ordinary native query preservation, production head-look and post-QTE handoff. Formation AI/path recovery, real morale transitions and authored combat visuals still need recorded user validation; reproducing an overwrite path is not attribution of every live exit.

[v0.17 validation](docs/releases/v0.17-validation.json) promotes the exact Test104 gameplay binaries after the user reported "works great" following verified loaded components and actual recording. The existing 56 adapter fixtures and retained QTE/HUD evidence are not represented as fresh runs. Only release loader/installer checks are freshly run, including exact Test104 upgrade, seven interruption rollbacks and repeat install. Product/marker metadata, five embedded payloads, source closure and package hashes are checked separately. User acceptance is scoped to the tested build and does not settle the known effects crash or full roster/map coverage.

[Test105 validation](docs/development/Test105-validation.json) is separate from that published receipt. It adds an original-native frame-zero reset/clock regression, sustained QTE clock-drift cases and shared HUD/QTE Win32 display-window/DIB lifecycle checks. Its complete fresh component suites and final payload receipts are recorded there. Offline display surfaces and submission cadence do not establish exclusive-fullscreen visibility, physical scanout or live symptom causation.


Unpublished Test106 adds actual private DirectX9/11 device and backbuffer tests at720p/1080p/2160p with1x/4x/8x sampling, premultiplied pixel readback, native state/target restoration, upload reuse, resize/reset lifetime and exact graphics-hook restoration. Numeric/pixel-channel concurrency/ownership and the production acknowledged-QTE-image click path are covered. See `docs/development/Test106-validation.json` for final fresh suite counts and hashes. No desktop display-mode change or game launch occurs in those fixtures; maximum-graphics fullscreen visibility and scanout/input latency remain live validation work.


Test107 adds a production Session/publication regression with staggered native combat callbacks and frequent clock polls. It reproduces a74.8768% repeated-marker case before correction; the corrected27 phase/speed/refresh cases have no repeated/held samples. Pause/native-callback/input-level and precise expiry checks preserve authority. A separate cadence fixture distinguishes CPU acknowledgement polling from distinct submitted images. See `docs/development/Test107-validation.json` for fresh QTE/loader/installer checks and separately retained byte-identical Test106 adapter/HUD evidence. These are software/fixture measurements, not physical scanout or live acceptance.

## v0.18 acceptance and release checks

[v0.18 validation](docs/releases/v0.18-validation.json) promotes byte-identical Test107 gameplay after the user reported "worked great" in a session with verified loaded hashes, native1920×1080 fullscreen DirectX11 presentation and actual automatic debugger/input/native-memory/resources/GPU/VRAM capture. Retained gameplay evidence comprises the60 adapter and3 HUD executables from Test106 and36 QTE executables from Test107; these are not fresh promotion runs. New release checks cover both loader executables and the full final-payload installer, including exact Test107/published-v0.17 upgrades, all seven interrupted rollback stages for each and repeated installation. The receipt records any failed attempt and subsequent investigation separately.

Release metadata, PE version, five embedded payloads, complete native/tool source closure, package checksums and source archive Git blobs are verified independently. The release-specific loader/cache is separate from the installed accepted Test107. Acceptance covers the tested behavior; death-overlay/all-driver coverage, physical scanout/input latency, complete roster/map lifecycles and the unresolved effects crash remain outside that claim.

## v0.19 acceptance and release checks

[v0.19 validation](docs/releases/v0.19-validation.json) promotes the exact Test109 adapter/HUD and Test108 QTE. Retained evidence comprises61 adapter,5 HUD and36 QTE executables, twelve HUD previews/eight viewport checks and Test109 pixel equivalence. Only release loader and final installer checks are freshly run, including exact Test109/published-v0.18 upgrade, seven interruption stages each and repeat-install retention. Embedded payloads, PE0.19, native/tool source closure and package/source-archive hashes are verified independently.

The player accepted Test109 without sending READY before play. Automatic recording still captured exact module/HUD identities,1920x1080 fullscreen native presentation, input/native memory and CPU/GPU/VRAM. This is retrospective evidence, not a claimed pre-play check. The session crashed with blood disabled; first-chance and unhandled dumps agree on an invalid DirectSound virtual call. Its originating corruption/lifetime, relation to earlier effects/Miles faults and mod involvement remain unresolved. v0.19 includes no crash patch or universal roster/device/ally-hit certification.

## v0.20 release evidence

v0.20 retains all three exact Test115 gameplay binaries and all tracked native output source inputs. Retained evidence covers Test109 adapter61/HUD5 and the full Test115 QTE39 suite, including the Test113 acknowledged-image correction and 5,152 differential/current-identity/fallback crowd cases plus 100,000 encounters. Gameplay suites are retained, not described as rerun for metadata-only packaging.

Release loader2 and the complete final installer suite are freshly run. Exact Test115 and local v0.19 predecessors join the historical upgrade fixtures, each with seven interruption rollbacks and repeat-install backup retention. The release receipt pins current source closure, payload/PE versions and fresh/retained results separately.

The Test115 five-minute reference has 305.07 seconds of target CPU samples, 299.07 seconds of frames, 19,021 frames and 44 sampled visible QTE serials. User feedback was no significant lag; recorded loss/property-error counters were zero. Earlier severe slowdowns were not reproduced in this reference. Sampled lock frames also occur during this smooth session and do not quantify all waiting or establish a cause. Native crashes, physical display latency and complete roster/map coverage remain unresolved. Raw recordings, paths, dumps and process data stay outside Git.
