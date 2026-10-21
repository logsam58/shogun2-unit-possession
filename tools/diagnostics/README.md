# Optional crash diagnostics

Diagnostics is separate from the player installer. Normal build/configuration/import/help commands do not start a watcher, attach a debugger or change autorun. Existing installed recording is untouched by this branch.

Use the public Python/MSVC prerequisites from the main build guide. Build into a new directory:

    py -3.12 tools/diagnostics/build111.py --vcvars 'C:\BuildTools\VC\Auxiliary\Build\vcvars32.bat' --output-dir 'D:\Build\diagnostics'

Default binaries are capture-only. Experimental processor tracing is compiled only when explicitly requested for fingerprint-locked toy fixtures; it is not a supported game-debugging mode.

Stage into another new directory with explicit paths:

    py -3.12 tools/diagnostics/source/install.py --game 'D:\Steam Library\Total War SHOGUN 2' --output-dir 'D:\Diagnostics Staged' --recordings 'D:\Recordings' --debugger 'D:\Build\diagnostics\crash-recorder.exe'
    py -3.12 tools/diagnostics/check_stage.py --stage 'D:\Diagnostics Staged'

Staging checks the engine, capture-only binary and local import closure; includes accuracy_read90.py and shared role constants; writes hashes/config/baseline/Python receipt; and does not start anything or enable autorun. An optional baseline can pin additional expected loaded-module hashes. Actual native telemetry needs compatible mod channels and discovered module cache; absent/unknown channels stay explicit. Specialist telemetry now follows the same final-human policy as adapter/HUD.

Only after separate authorization may an operator arm the staged watcher with watcher.py --run or control.py start. Avoid duplicate watchers. No autorun installer is provided. A READY reply gates formal testing, not automatic ordinary capture. Verify actual worker/debugger attachment and fresh input/resources/native samples before claiming recording.

## Offline checks

    py -3.12 -m unittest discover -s tests -v
    py -3.12 tools/diagnostics/source/verify_lifecycle110.py
    & 'D:\Build\diagnostics\verify-ownership111.exe'
    py -3.12 tools/diagnostics/source/verify_capture112.py --build-dir 'D:\Build\diagnostics' --support-root 'D:\Diagnostics Staged' --output-dir 'D:\Build\capture checks'

Capture checks create toy processes with a generic fixture name. They test normal exit, handled and unhandled exceptions, safe detach, eight dump/cache fallback cases and toy loader corruption. They do not use the real game or its watcher. Never rename a toy as shogun2.exe to test detection against an active installation.

The recorder can sample available CPU/memory/address-space/GPU/input/native/lifecycle channels. This is not instruction-by-instruction capture, video or physical display-latency measurement. Missing providers do not mean zero load. Dump fallback quality is explicitly labeled.

Inspect a private dump:

    py -3.12 tools/diagnostics/source/inspect_minidump.py 'D:\Private\crash.dmp' --output 'D:\Private\report.json'

Stack-word candidates are not an unwound call stack. A native-module fault is not attribution of root cause. Full dumps/input logs can contain private information and stay out of Git. Historical watcher-integration/deployment helpers are excluded from the public kit; do not run legacy machine-specific scripts as current setup.
