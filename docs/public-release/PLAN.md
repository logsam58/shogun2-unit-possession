> **Historical record:** This page describes source-kit engineering and v0.30/v0.31 planning.
> The current release is v0.32 / Test166 Workshop. Use the
> [developer guide](../README.md), [release identity](../current-release.json),
> and [release procedure](RELEASING.md) for current work. Commands, pending
> tasks and approvals below apply only to the recorded session.

# Current v0.31 update

The owner requested a quick-start control schematic in the prelaunch chooser, then authorized committing and releasing v0.31 privately. See [V0_31_VALIDATION.json](V0_31_VALIDATION.json) for fresh startup/installer checks, retained exact gameplay evidence and live-test limits.

# Historical v0.30 work

The owner requested the Shogun 2 Unit Possession rename, startup camera chooser and explicit naval/multiplayer/bug limitations. This new Test160 build is awaiting live acceptance. Current checks are in [V0_30_VALIDATION.json](V0_30_VALIDATION.json). The earlier source-kit engineering checklist below records historical work, not a claim of live acceptance for Test160.

# Source-kit engineering history

Baseline: immutable v0.28/Test157 at 24d31a2f5e5c2cf84a42c459a69bdcd9bd86c600. The engineering work began on public-release. The owner subsequently accepted Test159 and authorized promoting it to v0.30 on main as the only branch. No game/install/active-recorder/settings/CPU changes are part of release packaging.

## Implemented with offline evidence

- [x] F5: shared ten-role policy across adapter, production HUD and diagnostic snapshot; liveness/serial/camera/unit/army negatives retained.
- [x] F6: durable identified installer snapshots and ready/commit/resolved records; safe reconciliation of files and two fixed registry values; 15 actual owned-child kills, repeated recovery/repair and unknown-file preservation.
- [x] F7: explicit portable diagnostic stage, complete local imports including accuracy_read90, five safe entry points, capture-only gate.
- [x] F1: original-game resource path using bounded PFH3 reads, pinned image/packing/audio tools and exact inputs/geometry/pixels/fonts/voice validation.
- [x] F4: current mandatory tests independent of private history; optional 169-file historical archive inventory/preflight/runner with explicit skips.
- [x] F8: current-release.json is authoritative; concise current guides replace obsolete machine/session guidance. Historical evidence stays in Git, outside the source kit.
- [x] Explicit supported game/tool/build/resource paths, x86 ABI, spaces and separate output snapshots; defaults do not install or start capture.
- [x] Owner-approved MIT first-party scope, vendor notices, provenance, contribution guidance and excluded game payloads.
- [x] Native ABI/offset/ownership/thread/reentrancy/forwarding/shutdown contracts, source-generated index and explicit unknowns.
- [x] Derived roles/resources/headers/index reproducible from declared canonical inputs; retained golden observations explicitly distinguished from new native sampling.
- [x] Curated source-kit export, source/content hashes, secret/personal-path/binary audit and CRC/hash verification.

## Engineering validation

Fresh-source and local test results are recorded in VALIDATION_RESULTS.json, including the non-default read-only legitimate game input view, separate build outputs and exact tool versions. A source export and ordinary checkout use the same entry points. No private prepared resource tree or historical payload archive is used by mandatory validation.

Independent resource runs produced 32 byte-identical prepared files. This is a resource comparison, not a claim of bit-identical complete executable builds. Missing optional historical inputs are a skip, never a pass.

v0.30 release checks are recorded separately in [V0_30_VALIDATION.json](V0_30_VALIDATION.json).

## Separate gates and limitations

- [ ] Full formal live gate: not completed before play. The owner subsequently accepted Test158 after one completed battle; existing recorder inventory matched all four component hashes and capture metadata. This is user acceptance with retrospective evidence, not a formal native-hook gate. Future testing needs a fresh cue.
- [x] Owner approval to promote main and release v0.30 inside the private repository. Changing public visibility remains an owner decision.
- [ ] Permission for public binaries containing game artwork/fonts/voice: unresolved; those binaries are excluded. The owner already chose the supported source/local-resource route, so no additional format decision is pending.

Crashes/crowd slowdown/POV remain open. Native provider boundaries, unknown races and incomplete historical sampler re-capture inputs remain documented limits. Formatting canonical records is not fresh decoding. Process-kill recovery is not a physical power-loss guarantee. Normal Windows path-length limits remain.

Owner decisions already received: full implementation on a separate branch, CLOSED for repository work, MIT, source/local-resource kit, original native status icons for that public profile. Private accepted icons and published v0.28 remain unchanged. The owner later authorized a safe local swap to Test158; exact Test157 rollback and old caches are preserved.
