# Workshop-only investigation

The requested player flow is **subscribe, enable, launch normally**. Native
entry from an enabled local pack is now verified after the timestamp correction.
The complete Workshop gameplay build is not implemented or validated. The
Test163 installer remains intact. Nothing has been uploaded or made public.

The native gameplay, HUD and QTE need a native startup entry; a marker pack alone
does not load them. Shogun 2's pinned engine initializes Lua's dynamic-library
loader. Its front-end constructor at RVA `e7dd20` loads
`data/frontend_scripted.lua` through the archive file manager. This is a promising
menu-time entry, not live proof that an enabled Workshop pack can supply it.

The earlier CoreUtils replacement caused a controlled Custom Battle regression.
It stays retired. The first two private Test164 probes appended a protected
initializer to the original `frontend_scripted.lua`. In a freshly cued game
process, the game recorded the second pack as its only enabled mod, but no probe
log/cache/DLL appeared at either the main menu or Custom Battle setup. Custom
Battle opened normally. This did not demonstrate native startup.

A bounded read-only inspection then identified the running front-end root's
callbacks and their original bytecode source. Candidate 3 targets that observed
`ui/frontend ui/layout_scripts/root.luac`. Its private wrapper executes the exact
owned-game bytecode in the original environment, then attempts the inert probe.
The matching float Lua fixture preserves all 37 exported callback bytecodes,
environments and initialization counts, verifies startup occurs afterward and
contains native-load failure. UI/native loading is mocked in that wrapper
fixture; a separate standalone Lua check actually loads the probe DLL.

No candidate replaces CoreUtils or loads gameplay. The inert DLL records entry
and verifies the host engine; it installs no hooks. Candidate 3 did not produce
an entry receipt. Its raw pack paths used forward slashes; candidate 4 corrected
only those four index bytes to the stock backslash spelling. The native DLL and
Lua payload remained exact. This correction also did not establish entry.

**The private diagnostic pack includes a locally copied stock script. Do not
upload or distribute it.** The eventual public pack must contain first-party
bootstrap code only and recover original scripts/resources from each player's
game. HUD artwork, cards, fonts, ability icons and voice remain local resources.
No public Workshop package is ready yet.

The offline build verifies original script bytes, matching float Lua syntax, actual
`package.loadlib` in a standalone Lua host, one-time startup, non-game rejection,
contained denied writes/missing loader, preservation of foreign cache bytes and
pack roundtrip. These checks do not establish execution in Shogun 2. The native
probe's Lua-side cache creation is not an exclusive filesystem transaction; it
is limited to this private experiment and must not become the release loader.

The next gate is a reversible menu-entry test, followed by Custom Battle and
campaign entry checks. A full gameplay test must later exclude the installed
DInput bootstrap, otherwise it cannot establish Workshop-only functionality.
Disable/restart, subscription delivery, resource preparation, updates, cleanup,
and a clean second installation also need validation before release claims.

Workshop scripts execute after the game starts. A Workshop-only guide/camera
choice would therefore be an in-game menu flow. Preserving a chooser that runs
before the game starts would require a separate startup mechanism.

Build the private probe with Python 3.12, MSVC x86 and the preserved float Lua 5.1
interpreter/compiler:

    python tools/workshop/build_probe164.py --entry ui-root --game <owned-game> --output <new-output> --vcvars <vcvars32.bat> --lua <lua51.exe> --luac <luac51.exe>

`tools/workshop/stage_probe164.py prepare` creates a backup/journal without game
writes. Actual `stage --closed` requires fresh human CLOSED confirmation and
checks the process state again. `rollback` preserves foreign edits, removes only
the owned directive and retires the exact probe pack. An explicitly invoked
`watch` rolls it back after the next game process exits; it never launches or
terminates the game. These operator tools are not player installation steps.

Primary references: [CA's Shogun 2 Workshop guide](https://wiki.totalwar.com/w/Steam_Workshop_General_Usage_Guide_(TWS2).html),
[Valve's delivery model](https://partner.steamgames.com/doc/features/workshop/implementation),
and [Lua 5.1 dynamic loading](https://www.lua.org/manual/5.1/manual.html#pdf-package.loadlib).
Generic platform capability does not establish this game's startup behavior.

## Current diagnostic retry

In candidate 4's cued process, the main menu and Custom Battle opened normally.
The game's archive root-script map listed the probe before data.pack, but no
native probe cache, module or receipt appeared. Read-only inspection of all 390
registered Lua nodes did not find the stock root callbacks seen in the earlier
legacy probe. This suggests root initialization failed; it does not identify
the failing operation or prove why. No engine calls or memory writes were made.

The same pack is staged again after fresh CLOSED, with only the native
`log_lua_errors_to_file;` and `log_lua_output_to_file;` directives added for
diagnosis. The staging helper backs up existing Lua logs and restores only its
owned directives on rollback, preserving preexisting enabled flags and unrelated
edits. Native game-written log contents remain as evidence. Isolated checks
passed for absent/preexisting flags, prior-log backup, running-process refusal
and rollback with a later comment. A backup-variable fixture failure was fixed
before real staging; its failed evidence remains distinct.

The logging retry in `artifacts/Test164/live-probe-5` kept both menus healthy,
but the log remained empty through exit and no native entry appeared. Runtime
logging does not cover every native script-load rejection. That retry rolled
back with its exact retired pack verified.
Recorder and debugger remain unchanged. Native logging is documented by
[CA's Battle XML guide](https://wiki.totalwar.com/w/Battle_XML_Documentation.html);
CA documents the Lua output under the Shogun2 profile's logs directory in its
[Battle Script guide](https://wiki.totalwar.com/w/Battle_Script_Documentation).

## Timestamp correction awaiting live proof

Native archive inspection exposed a zero timestamp on the probe's root entry;
the original game's entry carries data.pack's nonzero header timestamp. In the
pinned file reader at RVA12c3730, a missing loose file begins with FILETIME zero.
The archive branch at12c38b2 accepts only a strictly older loose timestamp. With
a zero archive timestamp they compare equal, so the code can fall through to an
empty file at12c3952 rather than selecting the registered archive. Actual Windows
CompareFileTime checks reproduce 0 for zero/zero and -1 for zero/stock.

`retimestamp_probe164.py` corrects only header bytes24-31 using the owned stock
pack's timestamp. Raw index, complete payload, all entry offsets/sizes, native
DLL and Lua bytes remain exact; fresh comparison checks passed. Its first
invocation stopped on an isolated-runtime import path before creating output;
that path was corrected and the failed receipt preserved. The future builder
also preserves this nonzero timestamp, but was not rerun to produce this probe.

Candidate5 is staged in `artifacts/Test164/live-probe-6` with temporary Lua
logging and automatic exit rollback. Fresh native/Lua fixture suites are not
claimed: they are retained exact from candidate3. The corrected pack subsequently loaded the exact inert native DLL in PID10596.
Its PID-specific receipt verified the supported engine and no gameplay/hooks;
the Lua log recorded all three startup stages. The owner confirmed main and
Custom Battle menus work. The former failing/corrected packs differ only in
those eight timestamp bytes. This establishes startup for that comparison,
not full Workshop subscription delivery or gameplay.

## Loader-free entry check and chooser removal

The owner requested removing the First/Third selection. The old installed
Test160 DInput loader supplies that popup. For the next controlled entry test,
fresh CLOSED authorized excluding it while keeping the working probe identical.
Its exact hash is pinned, copied to a verified journal backup and moved out of
the game root. The exit watcher restores that historical installation after
the test; future Workshop gameplay must omit the chooser and retain P switching.

The operator staging tool's optional exclusion passed isolated checks across
UTF8 and both UTF16 BOM forms, with/without exclusion, preserving later comments
and enabled logging flags. Running-process/changed-loader refusals, restoration
after an injected partial stage, and foreign target/excluded-file preservation
passed. Engine/process providers were mocked only in those synthetic fixtures.
Real staging independently verified the actual pinned installed loader.

Current journal: `artifacts/Test164/live-probe-7`. Await fresh READY and verify
that no mod DInput loaded, the chooser stayed absent and a fresh native entry
receipt matches the game PID. The first native success still had historical
DInput present and therefore is not the clean-loader test. No ordinary recorder
capture or gameplay-hook validation is inferred from these inert checks.
