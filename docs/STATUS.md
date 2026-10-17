# Development and acceptance status

Recorded17 October2026.

- Accepted release: v0.14 checkpoint79, retained as an immutable local release. This repository's first commit imports Test99 and must not be labeled checkpoint79.
- Test87: targeting accepted, mounted-bow presentation rejected. Test88/89 do not gain acceptance through this source import.
- Installed candidate: Test99. User-launched PID23208 passed exact loaded-hash/cache/HUD/QTE/Climb97 and actual capture verification. The user confirmed climbing/crest/landing worked during extended play, then the process crashed. This is scoped positive siege feedback, not universal map/unit or crash-free release acceptance.
- Current game session: exited. Both first-fault and unhandled full dumps were preserved locally. The persistent recorder remains installed and armed, independent of any READY/pause/battle/mod-readiness gate.
- Current crash: C0000005 reading2C at Empire.Retail.dll RVA14A4011 in the native effects-table path. The inconsistency is established; its originating lifecycle/write and mod involvement remain unresolved. No speculative native crash patch is included.
- Test99 source retains Test90 native effective-accuracy conical spread and saved RTS camera, duration/minimum-cycle QTE, native crowd preparation, larger input prompts/INTERRUPTED feedback, timestamped submitted-frame judgement, registry/service performance correction, scoped ranged barriers, vertical look and native wall/crest/landing ownership.
- The automatic recorder's post-exit dump reader was corrected for Memory64 stacks with ThreadList RVA0. Seven synthetic cases and both actual full dumps passed. Original historical reports/dumps were retained. Watcher/debugger/gameplay code was not restarted or changed for that parser repair.

The Git import does not change installed files, start a live test or resolve the outstanding crash. New builds have fresh backing hashes and require separate installation/readiness validation before play.
