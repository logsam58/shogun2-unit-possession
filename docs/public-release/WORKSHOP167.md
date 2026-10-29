# Test167 startup-compatibility candidate

This is an uninstalled, unpublished correction under test. The current product
release remains v0.32/Test166. It does not establish the reporting player's
actual failure cause or universal language support.

The previous extractor required 20 English native fonts and the English voice.
Missing local_en.pack stopped the whole resource build. Test167 retains exact
English font fingerprints when that pack exists. Otherwise it searches bounded,
regular local_*.pack candidates for the required native CUF font entries and
validates layout, bitmap/kerning bounds and Latin HUD glyphs. No system fonts or
placeholder artwork are introduced. HUD text remains English. Incompatible
localizations receive an explicit native startup warning rather than a promise
of unsupported font/translation support.

The English advisor clip and decoder are optional. Missing/failed audio omits
resource301 while preserving validated HUD/QTE resources. The bundle reader
accepts this defined optional entry; the worker skips playback safely. System
Media Foundation libraries are preflighted and delay-imported. A new shared
resource namespace prevents old runtimes from consuming voice-optional caches.
Existing English resource files compare byte for byte with accepted Test166.

The Lua bootstrap still uses the normal native uint64 reader. On an early
failure it attempts to restore the stock UI directly from owned data.pack using
base-65536 offset limbs, an 8MiB index bound, and two exact seeks below signed
2GiB. The normal native path still supports larger offsets. Original callbacks
are never initialized twice after a partial invocation. No stock script is
redistributed. A missing/corrupt vanilla pack or an unsupported fallback offset
cannot be repaired by this code.

Native failures show one warning per game process. If the bridge cannot load,
Lua can leave one exclusively created first-error note at
%LOCALAPPDATA%/S2UP-Workshop-startup-error.txt, at most4KiB. Existing files are
preserved. This is failure-only; normal success creates no session/input/QTE
logs. No recorder, debugger, profiler or crash collector is added.

Offline checks reproduced old-build loss of menu callbacks on bootstrap failure
and old missing-English extraction failure. The correction preserved all37
callbacks and their environments under cache, native-load, prepare and start
faults. Locale fixtures, native regressions, resource drawing, storage checks
and package closure passed. The localized inputs are private owned-font fixtures,
not a real non-English installation. See TEST167_VALIDATION.json for exact
results and limitations. Owner/reporting-player live acceptance remains pending.

Build with tools/build.py workshop --tests --candidate167, then the Workshop
builder with --candidate167. Isolated locale checks are in
 tools/workshop/verify_locales167.py; never distribute their owned-game fixtures.
A real install/staging needs fresh CLOSED and verified rollback. A live test
needs fresh READY and loaded hashes/hooks. No upload, visibility change or
new product release is implied by this build.
