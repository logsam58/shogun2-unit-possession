# Diagnostics110: session history and targeted lifetime tracing

This is a diagnostic-only update, separate from the pinned v0.19 player package and accepted Test109 gameplay. It adds no game DLL, patch, order or audio hook. The existing automatic watcher starts the updated session and its single external debugger on the next normal game launch. Recording does not wait for READY, battle or possession.

## Captured history

The read-only sampler starts immediately and waits for the supported engine independently of the mod. It checks the native root/environment/phase at 200ms intervals through setup, battle and cleanup. Phase IDs and object transitions are recorded as native facts; menu/deployment/post-battle names require live correlation, rather than invented enum meanings.

Available battle data includes selected unit serials, raw morale/routing fields, guard/fire settings and the followed soldier's position, velocity, animation and identity. Command100 state is sampled when its optional mapping exists; Q/E/Z/C/R/F/Shift join existing game-focused input recording. Existing native command, combat, presentation and possession-release logs are preserved. This is selected-unit/followed-actor coverage, not an understanding of every AI decision or an exhaustive decoded order queue.

Effects snapshots cover at most256 global entries and reread headers/records to flag churn. Repeated empty/changed active owners and pose discontinuities produce candidate markers, not automatic blame. Address reuse observations do not prove allocator generations. A bounded120-second in-memory history is saved on candidate triggers, up to16 times with30-second spacing. The continuous history has a128MiB/session budget; reaching it is reported and earlier files are never deleted. The original core resource/input/crash channels continue separately.

## Targeted debugger

The x86 debugger verifies the retail engine hash and original destructor prefix, then watches RVA0x1448700. If that destructor begins while an active effects entry references its ECX owner, it records registers, stack evidence and matching entries. A data watch then follows that owner's first word, including destruction/reuse writes, for up to16 writes or replacement by another matched owner.

For the exact verified system DirectSound DLL, a one-time execution scout at RVA0x5DEE0 locates the first observed wrapper/interface. Data watches cover the wrapper's interface pointer and the interface's first word across existing and newly created threads. A write records the stopped thread, registers, instruction bytes and stack evidence. The instruction pointer for a data trap is after the writing instruction. Subsequent pointer changes rebind the interface watch. Other wrappers and arbitrary heap allocations are outside this four-slot trace; the Miles negative-count producer is not continuously watched.

There are no instruction patches or game-memory writes. The debugger owns processor debug registers and its execution-trap resume flag, preserves pre-existing registers, restores them on detach, and forwards unrelated exceptions normally. Debug-event handles remain Windows-owned until exit; closing them early caused a reproduced synthetic concurrency failure. The corrected final suite includes100 repeated concurrent-write cases.

The trace stops its hardware observations if its per-second handler-work/hit budget or32MiB event budget is exceeded. That reduction is explicit in targeted110-health.json; sampling and original crash capture continue. Handler measurements do not include all kernel/debug-event scheduling overhead. Breakpoints can perturb timing; real gameplay performance is still a live check.

## Files and verification

- `lifecycle110.jsonl`, `lifecycle110-health.json`: phases, selected units, actor/command/effects state, bounds and sampling health.
- `lifecycle110-pretrigger-*.jsonl`: bounded preceding history for a candidate anomaly.
- `targeted110.jsonl`, `targeted110-health.json`: actual destruction/write traps, stacks, thread bindings, coverage and budgets.
- Existing exception dumps, resource/CPU/GPU/VRAM/input/module/native logs remain independent.

Run `verify_lifecycle110.py` for synthetic reader/budget cases. Build `native/crash_recorder.cpp` and `native/trace_fixture110.cpp` as x86 with MSVC, then run `verify_targeted110.py --build-dir <binaries> --output-dir <new-evidence-folder> --repeat-normal 100`. The existing `verify.py` additionally exercises full-session normal/handled/unhandled/detach behavior and lifecycle startup without mod/engine channels. Real preserved dump decoding is separate retained private evidence.

`deploy110.py` requires explicit installed root, built recorder and a new evidence directory. It verifies an idle actual watcher, preserves exact rollback, replaces four diagnostic files atomically per file, checks imports and leaves watcher/config/baseline/autorun unchanged. It never launches the game. Installation and synthetic passes do not establish a live crash fix or universal capture coverage. Prompt a fresh user Steam launch and READY at the main menu; verify actual capture and trace health there, then verify loaded gameplay/HUD/QTE identities when battle initializes before formal play.

## Research informing this implementation

A [10 March2017 firsthand Shogun2 report](https://community.pcgamingwiki.com/topic/2011-total-war-crash-mss32dll-etc) contains `mss32.dll`, timestamp0x4BF2900C, C0000005 and offset0x16EB9. The local Miles file has that timestamp and a captured fault at the same offset. This is a historical immediate-signature match, not proof of the same corrupting write, negative entry count or common cause with DirectSound/effects crashes.

[Microsoft data-breakpoint documentation](https://learn.microsoft.com/en-us/windows-hardware/drivers/debuggercmds/ba--break-on-access-) supports targeted memory-write observation. [Debug-event ownership](https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-continuedebugevent) defines automatic handle closure. [PageHeap guidance](https://learn.microsoft.com/en-us/windows-hardware/drivers/devtest/application-verifier-debugging-application-verifier-stops) describes substantial full-mode memory cost and custom-heap limits. [Time Travel Debugging](https://learn.microsoft.com/en-us/windows-hardware/drivers/debuggercmds/time-travel-debugging-overview) offers deeper history but typically substantial slowdown; it remains an option for a short isolated reproduction, not enabled here. Neither registry heap verification nor game graphics/audio settings are changed.
