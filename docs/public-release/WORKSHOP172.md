# Test172 — shared engine compatibility

This is an unreleased candidate built on the accepted local Test171 source.
It does not change the published v0.32/Test166 identity or authorize installation.

Test171 required a new whole-file checksum for each reviewed replacement
Empire.Retail.dll. Test172 separates file identity from a conservative binary
dependency contract. All current startup consumers use the same implementation:
Workshop entry, delayed worker, adapter, playable QTE, HUD and resource extractor.
The standalone observer also uses it. Historical injection/setup helpers remain
historical; they are not in the Workshop payload.

## Admission and binding

`native/common/engine_contract172.h` identifies the original x86 engine family
using its PE layout and normalized header contract. Timestamp, PE checksum and
bounded unmapped certificate/overlay metadata do not determine the native ABI.
Section addresses, permissions, imports, entry point and initialized data retain
their checked layout. Only the reviewed eight-byte .rdata virtual-size variation
is allowed. This is not support for relocated or rebuilt engine families.

The stock and exact reviewed ModDB image retain their known-good fingerprint
path. An unlisted identity must match one coherent protected-region basis,
stock or reviewed Bugfix. It cannot mix protected fragments from the two bases.
Changes wholly outside that protected set can pass without adding a new file
hash. Changes to a required region fail with its RVA, contract ID and engine
fingerprint. This is compatibility checking, not an assessment of a mod's trust.

Successful admission returns schema 172, a contract ID and the actual complete
file fingerprint. A bounded in-memory cache retains read-only file handles,
preventing later disk replacement of an accepted binding. Before installation,
the 7 adapter, 10 QTE and Lua hook guards consume that result, verify the loaded
module's file identity and check exact 16-byte prefixes. Existing production
signature, actor, lease, lifetime, role and thread checks remain in place.
No foreign jump is chained automatically. No engine code or file is patched
by the validator itself. There are no new polling threads, recording, recurring
logs or dumps; failures use the existing bounded startup error path.

## Registry and conservative coverage

`tools/data/engine-contract172.json` and the generated C++ header are maintained
inputs. The registry pins 196 source dependencies, including the fresh-resource
extractor. Both native and Workshop builders run the source audit before their
isolated build. A changed/missing dependency, unregistered candidate address or
changed generated contract refuses the build until reviewed and regenerated.

The generator reuses the preserved exact-build native index and Ghidra assembly
exports, verifying every consumed instruction against the owned stock image.
It does not run a Rust rebuild, engine entry point or gameplay function.

The source scan supplies 611 conservative numeric seed records. Some are scalar
false positives; they are not described as 611 proven native APIs. Additional
roots cover address-taken routines and routines referencing native data through
relocations, followed by the indexed direct-call closure. This protects 62,643
recognized functions and all unrecognized executable bytes. The other 2,931
recognized functions are extension candidates; shared chunks required by a
protected function stay protected. Every source candidate must remain covered.

The resulting 3,306 protected file regions cover 25,328,935 bytes. This first
implementation deliberately protects most of the engine. It can still refuse a
compatible modification inside that conservative set; that needs a narrower
reviewed contract or integration, rather than blindly admitting a new DLL.
Indirect behavior and every possible mod interaction are not formally proved.

To regenerate, obtain both owned/reviewed engine files and the preserved native
analysis separately. No engine bytes or decompilation database ship in source:

    python tools/workshop/generate_engine_contract172.py --stock <stock-DLL> --modified <reviewed-Bugfix-DLL> --export <preserved-native-export> --output <new-private-evidence-directory> --write
    python tools/workshop/check_engine_contract172.py

Regeneration is a review operation. Source hashes alone do not prove an edited
member offset or new native call is correct. Inspect the private provenance and
changed registry before building. Ordinary builds use the checked-in hashes and
require no decoder or third-party source download.

## Verification boundaries

The isolated contract controls cover independent code/metadata edits, combined
reviewed edits, critical code and table changes, malformed headers, profile
mixing, stale/foreign binding results, retained file identity and occupied live
hooks. Actual compiled QTE startup tests distinguish disk admission from a
changed in-memory hook, stopping before any engine/gameplay call.

Fresh local-resource tests use empty output directories for reviewed and
unlisted compatible engines; generated HUD/QTE bundles must be byte-identical
to stock. The old independent stock-only extractor gate has been removed.

Optional Loader checks support pinned author sources 0.5.1 and 0.5.2. They run
only the reviewed patch installers in owned, uninitialized mapped engines,
including the stock/Bugfix combination. They do not run the author's entry
point, engine functions, hook bodies, affinity changes, crash handler or stats
thread. Third-party code is neither installed nor shipped by these checks.

See TEST172_VALIDATION.json for final receipts and the separate live status.
Offline success is not gameplay acceptance, universal DLL compatibility or
permission to update Workshop. Test171 remains installed until a separately
authorized rollback-backed swap; recording and the debugger remain off.
