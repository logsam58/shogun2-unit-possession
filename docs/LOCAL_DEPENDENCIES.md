# Local dependencies retained outside Git

The original development machine retains these logical dependencies:

- FP99-20261017: the preserved Test99 source/resource closure and exact installed-payload build references.
- FP98 and earlier candidates, plus the accepted v0.14 checkpoint79 release: immutable rollback/evidence inputs, not a history to regenerate or relabel.
- The previous native-mod workspace: current installation/rollback manifests, work/check_live99.py, current-state.json, validation receipts and raw automatic recordings/dumps.
- Shogun2Diagnostics: the existing installed persistent watcher/private Python runtime/debugger. It is not replaced by importing its source into tools/diagnostics/source.
- The original shared research workspace: the indexed understanding SQLite database and full native C/ASM exports. Use these preserved read-only dependencies; do not rebuild or upload the full decoding.
- Installed retail SHOGUN 2 and its native UI resources; MSVC/Windows SDK and a Python runtime.

Absolute paths and machine-local input configuration belong in ignored tools/local.json. That file is created by tools/configure.py and is not a portable dependency bundle. Local UI input hashes are in tools/local-assets.json. A new clone without those UI inputs can still inspect the source and build the adapter; styled HUD/QTE builds require the matching local resource closure. Historical full installer upgrade tests also need the ignored predecessor loader/marker inputs.

Future source edits belong in this Git repository. Original candidate roots remain historical build/evidence baselines; installed payload/readiness checks must continue pinning their actual hashes until a separately validated candidate is installed.
