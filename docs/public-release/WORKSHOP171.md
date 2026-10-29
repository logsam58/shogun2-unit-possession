# Test171: active QTE engine gate correction

Test170 accepted the reviewed bugfix in the loader, worker, adapter and HUD,
but missed an independent stock-only check in the active playable QTE worker.
Changing observer.cpp did not change that worker. The owner's screenshot and
an isolated test of the actual Test170 QTE DLL reproduced the resulting
Unsupported engine refusal for the exact reviewed ModDB DLL.

The production correction changes one predicate in playable_runtime.cpp to the
same reviewed-image policy already used elsewhere. No gameplay, hooks, actor
ownership, camera, movement, weapons, charge or pack-reader logic changes.
A source-path audit covers the actual four compiled runtime entry points.
The source map now identifies the playable QTE entry point explicitly.

New regression coverage loads the actual compiled QTE DLL into an owned helper
with an uninitialized mapped engine and deliberately no possession adapter.
Both exact supported engines must pass the real engine gate and stop at
Possession module unavailable, before hooks, renderers or engine functions.
A private timestamp-edited unknown image must still stop at Unsupported engine.
All ten hook prefixes remain unchanged. Test170's actual DLL reproduces the
old mismatch; Test171's actual DLL passes stock, reviewed and unknown controls.

All 50 QTE regressions and shared resource/font/renderer checks passed afresh.
The adapter and HUD are exact reused Test170 binaries with prior 100/8 checks;
only the QTE production native source differs. The package passed all 12 checks,
including actual float-Lua/menu recovery, coexistence, storage and the three
actual-QTE startup cases. These are not combined gameplay acceptance.

The builder accepts --compat-engine pointing to the separately obtained exact
reviewed ModDB image. It verifies its hash and stages it only in private test
outputs. No game DLL is included in the distributable pack or source kit.
Development input remains the stock owned engine, supplied here through a
private read-only fixture while the actual game retains its installed bugfix.

The local update preserves the launcher filename shogun2_unit_possession_test170
and replaces only its owned pack. The installed bugfix DLL and thumbnail remain
unchanged. A new independent stock backup and the prior local pack are verified
before replacement. Latest manual rollback restores stock and removes only the
owned local test pack/preview, preserving settings, subscription and quarantine.
There is no recorder/debugger, game launch, Workshop upload or GitHub change.
See TEST171_VALIDATION.json for build evidence and separate installation state.
