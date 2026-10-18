# Contributor and automation instructions

Current product version is v0.18, promoting the exact Test107 gameplay accepted on 18 October 2026 after recorded 1920Ã—1080 fullscreen DirectX11 play. Read docs/releases/v0.18.md and its validation receipt. Adapter, HUD and QTE binaries and their native gameplay inputs remain exact; release changes are metadata, predecessor recognition and installer fixture diagnostics. No gameplay rebuild or new native hook/protocol change. Separate freshly run release loader/installer checks from retained Test106 adapter/HUD and Test107 QTE checks. User acceptance is scoped to the tested build; physical scanout, broad death-overlay/roster/device coverage and the effects crash remain unresolved.

Unpublished Test109 combines the Test108 HUD layout with the explicitly requested friendly-fire voice callout. Read docs/development/Test109.md and its validation receipt, using explicit --candidate 109 builds. Its adapter adds one passive impact observer; preserve native forwarding and all existing gameplay/QTE authority. Test108 is retained as a separate offline candidate. Publication, installation and live acceptance remain separate facts.

Published v0.17 and older assets/receipts are immutable. Source packaging, publication, installation and loaded state are separate facts. Preparing/publishing v0.18 does not replace installed Test107 or its recorder baseline. Private installation/readiness/publication receipts establish those states. The release flag retains implementation revision107 while removing the development label.

Read README.md, BUILD_AND_TEST.md, SOURCE_MAP.md, VALIDATION.md, docs/STATUS.md and docs/KNOWN_ISSUES.md before changing the project. Work in this maintained source checkout. VERSION defines the product release; directory suffixes and protocol versions are compatibility identifiers.

Preserve native movement/fatigue, independent unit AI and firing, equipment/ammunition, effective-accuracy spread, saved RTS camera return, QTE timing/commitment, HUD and siege traversal. Keep strict current actor/unit/lease/holder/camera/serial/death/routing validation. Never replace current identity proof with a stale pointer or a time-based grace period. Keep numeric shared protocols compatible unless an explicit coordinated version change is required.

Use existing indexed native decoding when available as a local research dependency. Decoding and decompilation are analysis, not original game source. Keep archived releases, reference inputs, installed files and raw evidence immutable during normal source work. docs/source-import.json is a historical provenance manifest; do not rewrite it to conceal later changes.

Build into ignored artifacts/. Game binaries/assets, full decoding databases, recordings/dumps, local paths/configuration, credentials and runtimes stay outside Git. Preserve third-party licenses and do not invent a project-wide license. Before pushing, inspect staged changes and run tools/audit_repository.py.

Offline checks, live gameplay and release publication are separate operations. Report what actually ran. Finite fixture coverage does not prove every unit/map works or exclude mod involvement in a native crash. Keep the unresolved effects crash and mounted-bow limitations visible.

Do not install or launch the game merely to inspect/build source. Replacing native gameplay DLLs requires a closed game and fresh user Steam launch. For a formal live test, prompt for READY, then verify exact loaded backing paths/hashes and actual debugger/input/native-memory/resource/GPU capture before asking for play. Automatic recording never waits for READY, pause, battle or mod availability. Preserve any existing persistent watcher; do not stop it when a test ends or create duplicate recorders. Verify its actual process and fresh health before claiming capture is armed.

Release assets belong in ignored artifacts/releases/. Publish only when explicitly requested, using a reviewed commit, tag and hash-verified package. Do not change repository visibility or replace published versions without authorization. No agent delegation is authorized by this file.
