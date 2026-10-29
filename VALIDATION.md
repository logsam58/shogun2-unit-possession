# Validation

**v0.33 packages the exact Test173 build and its matching source.** No native
gameplay code was rebuilt for publication. Test173 removes F12's disable,
release and camera-yield paths from the accepted Test172 implementation.

The retained Test173 build passed 100 adapter, 50 QTE and 8 HUD executables,
resource extraction/format/render checks, and 16 Workshop build/package checks.
These include all 37 stock Lua callbacks, menu failure recovery, resource and
receipt lifetimes, UI-mod coexistence, actual compiled QTE startup and 48 shared
engine-contract controls. Eight isolated update/rollback checks passed.

Stock, reviewed Bugfix and controlled compatible engine edits passed offline
checks; critical engine edits and occupied hooks were refused. Test172 also
passed fresh-resource comparisons and isolated Loader source-patch checks.
See [engine details](docs/public-release/WORKSHOP172.md), the
[Test173 record](docs/public-release/TEST173_VALIDATION.json) and the
[release record](docs/public-release/V0_33_VALIDATION.json).

The owner reported Test172 worked in game. Its formal loaded-hash/hook gate
was not completed, and the report does not establish exhaustive feature or
long-term stability coverage. **Test173 has no live acceptance yet.** Offline
font fixtures are not real foreign-language installations; engine checks are
not universal DLL/mod compatibility or a security assessment of another mod.

Steam's Test172 upload and exact subscribed download were verified separately.
No Steam upload, game launch, game/settings change, recording or debugger start
is part of this GitHub release. Live checks require fresh readiness and exact
loaded identities/hooks. Old Test165/166 gates and versioned installer records
apply only to those historical builds.

Release checks cover Python tests, generated metadata, the exact Git source,
source/pack asset exclusions, archive closure and downloaded release hashes.
They do not establish all unit/map/driver cases, race freedom, leak freedom,
physical display latency or absence of crashes. Charging/climbing, clipping and
Ultra multi-army slowdown remain open.
