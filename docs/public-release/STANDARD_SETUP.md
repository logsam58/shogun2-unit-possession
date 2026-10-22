# Standard Windows setup

The private v0.28 preview uses **Inno Setup 7.1.0**, with a normal setup wizard
and Windows uninstall entry. It packages the accepted Test158 payload without
rebuilding the loader, adapter, HUD or QTE component. This is an unsigned mod
installer, not a Microsoft/Creative Assembly certified product.

## What it installs

The single player download is **`Shogun2-First-Person-v0.28-Setup.exe`**.

Inno Setup copies the existing loader to `DINPUT8.dll` beside the game's
executable and the existing marker pack to
`data/shogun2_possession_checkpoint45.pack`. The loader already contains the
runtime components. The standard installer also stores its helper, notices,
playing guide, manifest and uninstaller in its own program folder.

The integration helper enables only this pack in the current Windows profile's
`user.script.txt` and Shogun 2 launcher selection. It reuses the existing
fingerprint, encoding, JSON and safe-file primitives. It does **not** run the old
custom wizard or `install_cli.exe`. Inno owns the UI, payload copying and Windows
uninstall registration; the helper handles the game-specific ownership checks,
activation and rollback snapshots.

It doesn't launch the game, start/stop a recorder or debugger, prune runtime
caches, change CPU/graphics settings, touch saves or alter Steam prerequisite
flags. Run it from the Windows account used to play; administrator access is
requested for protected game/program folders.

## Backup and removal

Before game-file deployment, the helper saves identified, hash-checked before
and after images for the two mod files and two activation files. Snapshot intent
is flushed before Inno copies anything into the game. Failed installation
attempts restore the previous bytes when every affected file still has an
expected before/after identity. Unknown edits stop restoration and are preserved.

After interruption, rerunning setup uses that identified snapshot to recover or
repair the same installation. An incomplete snapshot without its final identity
is preserved rather than guessed at; no game file is changed during snapshot
creation. Choose a fresh setup records directory or inspect the old record in
that case. Different game/profile bindings are rejected.

Use **Windows Settings → Apps → Installed apps → Shogun 2 First-Person Mod →
Uninstall**, with the game closed. Removal deletes only unchanged current mod
files, removes this pack's activation statements/launcher entries, and preserves
other mods, including entries added after setup. Unknown replacement binaries
stop uninstall before game-file changes. Uninstall does not reinstall an older
mod version. Earlier snapshots remain in the setup folder's `rollback` directory
for explicit recovery; caches and the separate diagnostics installation remain.

## Build the wrapper

Obtain the compiler from the [official Inno Setup download page](https://jrsoftware.org/isdl.php).
The tested compiler was the publisher-signed 7.1.0 x64 distribution, prepared in
portable mode. Python 3.12 and the existing MSVC x86 Build Tools compile only the
new packaging helper. No Inno/Python/compiler runtime is required on a player's
machine.

From the repository root, supply your completed, isolated Test158 build:

```powershell
python tools/installer/build_setup.py `
  --prepared-source '<finished-build>/source' `
  --bin-dir '<finished-build>/bin' `
  --vcvars '<BuildTools>/VC/Auxiliary/Build/vcvars32.bat' `
  --iscc '<InnoSetup>/ISCC.exe' `
  --output '<separate-packaging-output>' `
  --private-preview
```

The command pins the accepted payload hashes, refuses a changed payload and
writes `output/Shogun2-First-Person-v0.28-Setup.exe` plus a package receipt. Output
must be separate from the finished source and binaries. The script deliberately
requires `--private-preview`: the embedded artwork/fonts/audio have unresolved
public redistribution status. It is not a replacement for the source-kit public
resource gate.

## Validation scope

The final package passed [19 grouped checks](STANDARD_SETUP_VALIDATION.json).

`tools/installer/verify_setup.py` checks the helper in synthetic game/profile
folders using the supported engine only as a read-only fingerprint input. A
separately compiled `--fixture` setup has a distinct per-user uninstall identity
and mandatory isolated profile override. The production setup has no profile
override and is not run by these checks.

Checks cover five script encodings/initial states; install and repeat repair;
uninstall and repeat uninstall; exact rollback at every four-file mutation
prefix; foreign DLL/pack refusal; post-install unknown edits; preservation of
later mod selections; and real Inno installation, repeat installation, Windows
registration and removal. These prefix checks simulate interrupted states; they
are not a claim of power-loss or killed-process coverage for this new wrapper.

The original Test158 game binaries retain their earlier offline validation and
the owner's completed-battle acceptance. That is separate from this wrapper's
fixture tests. No new real-game installation, launch, loaded-hook verification
or gameplay test is claimed for this setup.

## Files and distribution

Packaging source lives under `tools/installer/`. New executables, compiler tools,
fixture folders and logs belong in ignored `artifacts/standard-setup/`, not Git.
The original accepted build, previous installer, source ZIP and published v0.28
assets are not overwritten. See [publication handoff](PUBLICATION_HANDOFF.md).
