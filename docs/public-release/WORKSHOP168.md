# Test168: compatibility candidate

This candidate is built for offline review. It is not installed, uploaded or
accepted in gameplay. The published v0.32/Test166 release remains unchanged.
Test167's separate candidate and all rollback material are preserved.

## What changed

Test167's startup corrections are retained: compatible native localization
fonts can be prepared without local_en.pack, missing English advisor audio or
its decoder does not disable possession, and native startup failure attempts
to preserve all 37 stock menu callbacks. The original English resources remain
byte-identical to Test166. The locale tests use private fixtures derived from
owned fonts; they are not real non-English installation tests.

Test168 also addresses shared startup behavior:

- It reads the launcher's active `used_mods.txt`, including its UTF16 format,
  quoted search directories and comments. Ordinary unit/database packs do not
  replace the menu. When exactly one enabled UI pack supplies a different
  front-end root, its script is executed in the existing UI environment with
  the original arguments. Identical duplicate roots are accepted. Different
  competing roots or duplicate filenames in different search directories are
  reported; no arbitrary script merge or load-order guess is made.
- The menu handoff is temporary, bounded to 256 KiB, exclusively created and
  deleted when the last handle closes, including forced exits. Unknown existing
  files are preserved. The Lua reader uses `rbD`: the supported engine's mode
  dispatcher maps D (0x44) to its temporary-file flag at RVA 0x151f234. This was
  checked against the owned image with read-only disassembly, then exercised
  using the actual matching float Lua and native file guard. Microsoft's
  [CRT mode documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen)
  documents D as delete-on-close.
- F9 cannot arm possession while the coordinator is still starting the
  components or after startup fails. The barrier is process-local and leaves
  standalone adapter behavior unchanged when absent. Once all existing
  component startup gates pass, normal possession is allowed.
- Startup is serialized per process. Existing adapter modules and control
  mappings are checked before loading. A later failure requests a stop only
  for components loaded by this coordinator, with matching protocol/process
  fields, while leaving unrelated hooks and mappings alone. No hot unload is
  attempted. Duplicate startup failures do not overwrite another coordinator's
  readiness barrier.
- A bounded first-party error port carries the adapter/QTE's specific reason
  into the startup warning. Combat-hook failures identify the engine RVA.
  Selection files are bounded before reading and malformed UTF16 is rejected.

The existing exact engine fingerprint, hook signatures, ABI/role contracts and
Miles audio guard remain. This does not accept arbitrary replacement engines,
blindly chain another native hook or change combat/movement to hide a conflict.
No third-party DLL is added to the player package.

## Loading-mod investigation

The reported link is [Shogun2 Loader](https://www.nexusmods.com/totalwarshogun2/mods/28).
The author's [0.5.1 source](https://github.com/Dacadey/shogun2-loader/tree/371a351bb960ac11d6ab732dcfddcf5e1ce49a51)
uses a version.dll proxy and patches the engine in memory. It does not replace
Empire.Retail.dll on disk. Our disk fingerprint alone therefore does not
reject that technique. The reporting player's actual file/version was not
received; the screenshot's mention of a replacement DLL may describe another
release or setup. No specific player-side cause is asserted.

The optional `tools/workshop/check_loader168.py` fixture requires the separately
obtained, fingerprinted author source and an owned supported engine. It maps
the image without running its entry point, then invokes the author's actual
thread/loading/render patch installers at eight sites. It checks our actual
production signature readers, Lua API bytes and 311 unchanged 16-byte windows
at source RVA literals. A deliberately conflicting motion-hook edit must be
rejected; it also exercises the real F9 request with synthetic status.

This check passed. It is stronger than comparing two lists of offsets, but it
does not execute battles, the engine entry point, hook bodies, the other mod's
DllMain/Install, affinity changes, optional crash handlers or statistics threads.
Those behavioral interactions and combined gameplay remain unverified. The
source/RVA inventory is not a proof of every runtime memory dependency.

## Compatibility limits

| Combination | Candidate behavior / evidence |
| --- | --- |
| Enabled unit/database packs with no front-end root | Preserved; synthetic pack coexistence checked |
| One enabled front-end root replacement | Preserved when our bootstrap wins and the pack is in the launcher's active list; actual float-Lua fixture passed |
| Multiple different root replacements | Explicit conflict; no script merge |
| Another UI mod wins the bootstrap entry | Our code may never start; not solved by this candidate |
| Automatic movie packs or manual scripts absent from used_mods | Not resolved by the selected-root reader |
| Current Shogun2 Loader 0.5.1 source patches | Isolated signature/location check passed; joint gameplay pending |
| Unknown on-disk engine changes or overlapping native hooks | Refused; needs a separately investigated, verified compatibility profile |
| Real non-English installation / Windows N | Still needs actual-system testing |

No new CPU affinity, thread-count, graphics, Steam or launcher preference is
applied. Recorder/debugger remain off. Normal startup writes no session/QTE
logs. Test167's one exclusive first-error note remains possible on failure,
at most 4 KiB, preserving any existing file. Necessary local code/resources
still occupy storage; unknown caches are not automatically pruned.

## Verification and next gate

Fresh components passed 99 adapter, 50 QTE and 8 HUD executables, plus resource
format/font/renderer checks. The final pack checks actual float-Lua/native
startup, all 37 original callbacks, startup failure recovery, another enabled
UI script, repeated startup, owned resource preparation, temporary-file
lifetime and synthetic mod/status coexistence. Optional localized/missing
voice checks and the external-source patch fixture passed separately.

See TEST168_VALIDATION.json for exact artifacts, hashes and limits. Private
build folders contain owned-game fixture resources/scripts and are never
distribution folders. The player pack contains first-party code/notices only.
The public source kit contains the optional fixture driver, not the other mod's
source, binaries or the original engine.

No game install, live process/memory gate, Workshop upload or GitHub publication
was performed. An approved rollback-backed staging and a fresh readiness cue
are still needed for a loaded-build check and combined gameplay acceptance.
