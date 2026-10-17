# Automatic crash-recorder source

The preserved recorder source is in source/. It watches for Shogun launches, attaches its x86 external debugger, records resources/GPU/game-focused OS input and optionally native actor/input telemetry, then preserves severe first-fault and unhandled dumps. Normal native exception handling is preserved apart from the attach break and exact legacy thread-name exception handling. Stack-word candidates are not a full unwind.

This is a source import. The currently installed watcher/private runtime remains at the original local Shogun2Diagnostics location and is unchanged. source/install.py and some historical helpers retain the original machine paths; do not run them as a portable installer. Local config.json/baseline.json, Python runtime and native debugger executable are not tracked. They must be configured/built separately for another computer. No recorder install/restart is part of the repository build.

The corrected inspect_minidump.py can run standalone against a locally held x86 dump:

```powershell
py tools/diagnostics/source/inspect_minidump.py C:\path\to\local.dmp --output artifacts/dump-report.json
py -m unittest discover -s tests -v
```

It supports FullMemory Memory64, MemoryList and embedded ThreadList stack storage. Missing stacks are explicit, the exception context is preferred and the optional output path preserves original reports. Private dumps and real input/memory/session logs must stay outside Git.
