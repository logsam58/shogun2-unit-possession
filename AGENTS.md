# Native Shogun 2 repository

Read README.md, docs/STATUS.md, BUILD_AND_TEST.md, SOURCE_MAP.md and VALIDATION.md before implementation. Current human requests override historical handoffs. This is the maintained Git source tree imported from Test99; use native/ here for future edits, not the frozen FP99 source snapshot or the earlier Rust reconstruction.

Preserve working infantry/firearms, native movement/fatigue, independent unit AI/fire, numeric HUD/equipment/ammunition/Accuracy90/Climb97 protocols, QTE commitment/timing, RTS return and installed startup/rollback behavior. Keep strict current actor/unit/lease/holder/camera/serial/death/routing identities; do not substitute stale pointers or time grace. Use existing indexed native decoding at the local dependency paths, never rebuild the research or resume Rust. Decompilation is analysis output, not recovered original game source.

Accepted release is v0.14 checkpoint79, not this Test99 import. User confirmed Test99 climb/crest/landing worked during extended play; the session crashed in a native effects-table dereference. The preceding state change and native-only versus mod-induced cause remain unresolved. Do not claim universal unit/map acceptance or a crash fix. Read docs/KNOWN_ISSUES.md.

Production files were imported unchanged; docs/source-import.json records original hashes. Future edits may intentionally differ; retain that manifest as historical import provenance. Build into ignored artifacts/ and never overwrite frozen accepted releases, installed files or old candidate snapshots to validate a build. Do not create a checkpoint79 tag pointing to Test99.

Keep game binaries/assets, full decompilation, raw recordings/dumps, local paths/config, credentials and runtimes out of Git. Preserve third-party licenses. Before pushing, review staged files and run tools/audit_repository.py. Do not invent a project license; none has been selected.

Live installation/testing is separate from offline builds. Native DLL replacement requires the game closed and a fresh normal user Steam launch. Prompt for READY and verify exact loaded backing paths/hashes plus actual debugger/input/native-memory/resource/GPU capture before asking the user to play. Automatic recording itself never waits for READY, pause, battle or mod readiness. The existing persistent watcher must remain armed; do not start a duplicate recorder or restart/stop it just because a test ended. Read actual watcher process/image/creation plus fresh health before claiming readiness. Do not launch the game simply to inspect this repository.

No sub-agent delegation is requested or authorized by this file. Keep changes scoped and report what was actually tested; source import, offline checks and live acceptance are different states.
