# Test117: faster native render-record copies

This v0.20 development candidate targets one measured CPU cost during crowded first-person play. Two native render-record copy procedures accounted for about 18% of sampled main-thread CPU in a slow 25-second interval. They also appeared in faster intervals at lower shares. Those scenes differed, so the trace does not establish why possession produced more work or prove that copying explains all the lag.

The adapter replaces the supported engine's assignment and construction procedures at RVAs `14b4530` and `14b1880`. SSE2 moves copy the same fields of each 312-byte record. Six padding bytes remain untouched. Overlapping records, including self-copy, go through their corresponding native trampoline because the original forward stores have different semantics from `memmove`.

The native sorting routines, comparisons, ordering, queue population and object ownership stay in place. The callbacks use no locks, shared scratch buffers, allocation, logging or native identity reads. Preparation requires SSE2, the existing complete engine-file check and SHA-256 checks of both entire loaded procedures. A failed second hook creation removes the first disabled hook. F12 disables both hooks; the DLL and trampolines remain resident until process exit, following the adapter's existing shutdown policy.

The optimization applies to these render-copy calls whenever the adapter is enabled, including RTS view. It does not depend on possession state or bypass actor, camera, death, routing, QTE or unit-AI checks. The candidate retains the exact Test116 QTE and HUD binaries.

## Offline validation

The production hooks run against a private mapped copy of the hash-verified retail engine. Its entry point is never called. The original engine's copy and sort procedures provide the comparison results; game machine code is not included in the repository.

- 9,440 differential cases cover unaligned pointers, randomized fields, untouched padding, source preservation, all overlapping offsets and inaccessible guard-page boundaries.
- 100 full native sort cases compare every output byte, including ordering, across two sorting routines, ten array sizes and five key patterns.
- Eight ABI cases check stack cleanup, return pointer and nonvolatile x86 registers, including the overlapping fallback.
- Eight workers perform 400,000 copies while the production shutdown disables the hooks. Complete function fingerprints match again after shutdown.
- Initialization checks reject changed function tails, invalid image identity, duplicate preparation and a forced second-hook collision without leaving a partial pair.

The copy benchmark measures calls through the actual MinHook entries, alternating native and optimized runs and reporting seven-run medians. This is a copy-speed measurement, **not a whole-game frame-rate improvement**. The full adapter, loader and installer results and exact binary/source identities are in [the validation receipt](Test117-validation.json).

## Scope and testing state

This candidate is built for offline review; it is not installed, live accepted or published by this preparation. Existing Test116, earlier releases and rollback files are preserved. Recording and debugging remain disabled at the user's request, including the persistent stop flag. Building or installing this candidate must not silently resume them.

Any later installation requires a closed game and a fresh user launch. Verify the loaded candidate and restored/active hook state before evaluating crowded combat. Respect the recorder-off comparison; additional capture needs separate user authorization. No live performance or crash fix is claimed. Native post-verdict QTE start losses, the separate graphics/audio crashes and mounted-bow presentation remain unresolved.

For a source build, pass `--candidate 117` to the existing loader build command. The render regression accepts `SHOGUN2_GAME` as the engine directory, with the standard Steam path as fallback. Complete installer regression also requires the separately retained exact Test116 loader and marker under `native/work/autoload-test116-baseline/`.
