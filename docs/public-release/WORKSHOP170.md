# Test170: reviewed engine and legacy-pack compatibility

This local trial adds the exact 2 October 2026 ModDB bugfix engine profile,
SHA-256 c4d91d74ce9ff8338cf61d5b021b03a2776830177a590fcac205509a42239545.
The original engine profile is retained. The bridge also checks that its loaded
engine file and game-folder engine have the same digest. The worker, adapter,
QTE observer and external HUD use the same profile policy. Runtime identity
messages use the actual selected digest. Native hook-byte checks are unchanged.
This does not permit arbitrary engine modifications or establish compatibility
with every loader, language, platform or future bugfix version.

The reviewed DLL differs from stock by 875 bytes across 70 regions. A private
mapped-image helper passed the eight production adapter signature readers,
289 heuristic source-address windows, 17 additional combat/Lua windows and the
mitigation pointer, with relocation normalization and occupied-hook controls.
It did not run engine initialization, functions or hook bodies. Preserved stock
Ghidra/decode evidence was consulted with its partial-coverage limits recorded.
The known image is a trial profile; these checks are not combined gameplay.

The enabled-mod menu reader now supports the bounded PFH2/PFH3 layouts with
or without 64-bit per-entry timestamps. It validates the dependency index and
keeps existing payload bounds, path/duplicate checks, stock-root fingerprint,
self identification and conflicting-root refusal. Unknown flags, compressed or
encrypted layouts remain unsupported. Errors identify the selected filename.
Twenty format/type combinations were exercised for database and UI content,
including real selector use; malformed dependency/truncation/traversal/duplicate
cases still refuse. Actual float-Lua startup also passed with the reviewed
engine file and timestamped PFH2 UI/database fixtures, preserving all 37 stock
callbacks plus the addon callback and one initialization.

Test169's custom-unit HUD correction, locale/font fallback, optional voice,
menu recovery, startup barrier, bounded error reporting and storage behavior
are retained. No movement, combat, charge, wall-climb or order mechanics were
changed. The Miles audio guard is retained. There is no recorder or debugger.

The full native checks passed 100 adapter, 50 QTE and 8 HUD executables plus
resource/font/renderer checks. All nine Workshop checks and nine isolated
installation/rollback safety scenarios passed. See TEST170_VALIDATION.json.
Local installation is tracked separately; gameplay acceptance remains pending.

For the local trial the pack is named shogun2_unit_possession_test170.pack so
the owner can enable it while keeping older possession/checkpoint entries off.
The existing subscribed rollback and old publisher copies are preserved.
The stock engine must be backed up and verified before the owner-requested
bugfix DLL replacement. Manual rollback restores that engine and removes only
the two exact local Test170 files, preserving later settings and subscription.
The downloaded third-party engine is never included in the distributable pack
or portable source kit. Development builds still use the stock owned engine.
There is no Workshop/GitHub upload or release authorization in this local task.
