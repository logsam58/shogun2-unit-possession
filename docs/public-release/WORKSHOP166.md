# Workshop storage update: Test166 / v0.32

This accepted build removes routine disk logging from the Workshop mod. The external
debugger, hardware recorder, profiling tools and crash-dump collector are not
part of the downloaded pack and are never installed by its runtime.

The adapter and QTE no longer open session/input JSONL files. Their periodic
formatting is skipped; live status channels and gameplay timing remain active.
The wall-HUD reset that was previously hidden in a logging helper still runs
at its existing cadence, and the fixed weapon-observation ring is drained.
Startup stages stay in memory instead of appending per-process autoload logs.
Lua no longer writes startup transcripts; native bridge errors do not create
per-process error files. Errors still fail the relevant initialization gate.

The small PID/start-time native-entry receipt has a 4 KiB maximum and a Windows
delete-on-close handle held for the process lifetime. Windows removes it on
normal exit or a crash/forced exit. A reader must open it with FILE_SHARE_DELETE
as well as FILE_SHARE_READ. Receipt creation does not gate gameplay startup.

Resource preparation discards child stdout/stderr in player builds. One stable
recipe directory holds the required HUD/QTE resources. Compatible builds use
hard links from their component caches; filesystems without hard-link support
fall back to verified copies. No resource format, artwork, voice crop, HUD
layout, gameplay hook, native ABI or shared status layout is changed.

A fixed resource staging directory bounds incomplete preparations. Completed,
hash-validated generated leftovers can be retired; unknown/corrupt files are
preserved and preparation refuses to create more copies until resolved. The
extractor cleans up its own known outputs on ordinary failure. The original
script uses one staging name and reuses an exact completed interrupted write.
Lua reuses an exact completed native-DLL stage and retires its own failed writes.

Normal repeated play therefore adds no persistent logs or receipt history.
Required resource/code caches remain. Old pre-update caches, game saves,
Steam-managed files, user archives and rollback backups are not automatically
deleted. Different gameplay versions retain separate code for loaded-module
safety; this update does not claim a global cap across arbitrary future updates.

Build components with the `workshop` target in tools/build.py, then run
tools/workshop/build_workshop165.py with --candidate 166 and the new component
bin directory. The publisher pack keeps its existing filename for updating
the same Workshop item rather than creating a duplicate. Never publish the
private fixture/build folders or their locally extracted game resources.

This page describes the implementation. Exact build/check results and live
validation status are recorded separately in TEST166_VALIDATION.json.
