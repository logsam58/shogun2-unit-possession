# Crash diagnostics

The Python and native source in source/ supports automatic capture of SHOGUN 2 sessions and offline dump inspection. It is a developer/investigation tool, separate from the player installer.

## Available capture

The watcher detects game launches and starts core capture immediately, independent of pause, battle state, mod availability or a READY message. It remains waiting between launches. Available channels include process CPU/memory/handles/address-space resources, GPU/VRAM counters, game-focused OS input and optional native actor/input/mod telemetry. Coverage depends on OS/provider availability; this is not an instruction-by-instruction trace, physical display-latency measurement or video recorder.

The x86 external debugger preserves normal native exception handling apart from the attach break and exact legacy thread-naming case. Severe first-chance faults and unhandled exceptions can produce full dumps. A recovered first-chance event must not be labeled a final crash. The debugger can detach without terminating the game.

Diagnostics110 adds bounded lifecycle sampling from launch and native phase/selected-unit/actor/command/effects history. Its experimental processor breakpoints caused two real startup single-step crashes and are withdrawn. A verified capture-only debugger is installed; read-only capture remains. Source permits hardware tracing only for toy fixtures. Diagnostics111 adds stricter build and deployment isolation as described below. See [startup recovery](../../docs/development/Diagnostics110-startup-recovery.md) and [Diagnostics110](../../docs/development/Diagnostics110.md). These tools are separate from the pinned v0.19 player release and do not claim a fix for the longstanding native battle crashes.

## Setup limitations

Diagnostics112 adds independent pre-dump fault reports and bounded, explicitly labeled readable/minimal dump fallbacks. It addresses observed evidence loss when DbgHelp cannot read process memory; it does not fix the underlying game fault. See [Diagnostics112](../../docs/development/Diagnostics112.md) for verified coverage and limits.

Diagnostics111 separates the default capture-only executable from fingerprint-locked toy tracing. Its installer/deployment gate checks the compiled capability and PE imports before mutation. The offline build and failure-injection workflow is described in [Diagnostics111](../../docs/development/Diagnostics111.md). The active installation is not changed merely by building these tools.

A portable diagnostics installer is not supplied. Source includes deployment helpers with machine-specific path assumptions that must be reviewed/configured before use on another computer. Supply a local Python runtime, build the x86 debugger and configure output paths/baseline component hashes separately. Runtime/config/baseline files are not tracked. Do not run source/install.py unchanged as a general-purpose installer.

Do not start duplicate watchers or core recorders. Check both the actual watcher process and fresh armed health, then actual debugger/resource/input/native samples before claiming capture is running. Optional mod telemetry can arrive after core recording starts. An unavailable mod channel must not prevent crash capture.

## Inspect a dump

```powershell
py tools/diagnostics/source/inspect_minidump.py C:\path\to\local.dmp --output artifacts/dump-report.json
py -m unittest discover -s tests -v
```

The reader supports FullMemory Memory64, MemoryList and embedded ThreadList stack storage, including a zero ThreadList stack RVA when memory exists in another stream. Exception context takes precedence for the faulting thread. Missing stacks are explicit. Use a new output path to retain original reports.

Stack-word address candidates are not an unwound call stack. Loaded backing-file hashes do not prove patched runtime pages equal stock; use runtime bytes in a dump for that comparison. An exception inside a native module does not determine who caused the earlier corrupt state.

Full dumps and raw input/session records may contain private data. Keep them outside Git and sanitize reports before sharing. The seven synthetic format cases run without private game dumps; real captured dumps are not test fixtures in this repository.
