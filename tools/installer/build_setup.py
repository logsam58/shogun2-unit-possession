"""Package the accepted Test158 files with Inno Setup; never build gameplay.

Embedded game resources are not cleared for public redistribution. Consequently
this command requires --private-preview and creates no public upload artifact.
"""
import argparse
import hashlib
import json
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXPECTED = {
    "dinput8.dll": "936bb2cd09bfb57ba4a703a3dbdb7ca682d8df4e92a7feaf8b3738d8493a26c2",
    "marker.pack": "3db77b196bc9fb3ded51ac6e850a386a2b8cc30cf9e97011b1babf88d5e2753b",
    "possession-v46.dll": "6964fb76f4987423d13e460c1f17d411e1c14779efcc4a90c0992e99c825b4b8",
    "possession-hud.exe": "b85ce9beb74890db1626957b41e3f13bf7f51900b22cebfdd971a6f9fff87483",
    "qte-playable-v79.dll": "2dee668cc131959ba3ec544841b2423de001d5346acfb8d3ddd2b1458ce18210",
}

def sha(path):
    return hashlib.file_digest(Path(path).open("rb"), "sha256").hexdigest()

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--prepared-source", type=Path, required=True)
    p.add_argument("--bin-dir", type=Path, required=True)
    p.add_argument("--vcvars", type=Path, required=True)
    p.add_argument("--iscc", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    p.add_argument("--private-preview", action="store_true", required=True)
    p.add_argument("--fixture", action="store_true")
    args = p.parse_args()
    out = args.output.resolve()
    source = args.prepared_source.resolve()
    binaries = args.bin_dir.resolve()
    if out == binaries or out == source or binaries in out.parents or source in out.parents:
        p.error("Use a separate output folder; the finished build must remain unchanged")
    if (out / "Empire.Retail.dll").exists():
        p.error("The output must not be a game folder")
    for name, expected in EXPECTED.items():
        if sha(binaries / name) != expected:
            p.error(f"Accepted Test158 identity mismatch: {name}")
    out.mkdir(parents=True, exist_ok=True)
    stage = out / "stage"
    stage.mkdir(exist_ok=True)
    # No compiler/setup system installation. Pass a separately obtained ISCC.
    for value in (out, source, args.vcvars, ROOT):
        if any(c in str(value) for c in '%!&|<>^\r\n"'):
            p.error("Unsupported shell metacharacter in compiler path")
    helper = stage / "game-integration.exe"
    command = out / "compile-helper.cmd"
    command.write_text(
        '@echo off\n'
        f'call "{args.vcvars.resolve()}" >nul\n'
        'if errorlevel 1 exit /b 1\n'
        'cl /nologo /O2 /MT /EHsc /std:c++17 /DUNICODE /D_UNICODE '
        f'/I"{source / "native/work/installer-siege99"}" '
        f'/I"{source / "native/work/autoload99-preview"}" '
        f'"{ROOT / "tools/installer/game_integration.cpp"}" '
        f'/Fe:"{helper}" /Fo:"{out / "game-integration.obj"}" '
        '/link bcrypt.lib shell32.lib advapi32.lib ole32.lib comdlg32.lib user32.lib\n',
        encoding="utf-8",
    )
    subprocess.run(["cmd.exe", "/d", "/c", str(command)], check=True)
    for name in ("dinput8.dll", "marker.pack"):
        shutil.copyfile(binaries / name, stage / name)
    for name in ("LICENSE", "THIRD_PARTY_NOTICES.md"):
        shutil.copyfile(ROOT / name, stage / name)
    shutil.copyfile(ROOT / "native/outputs/shogun2-possession-cleanup-v46/vendor/LICENSE.txt", stage / "MinHook-LICENSE.txt")
    shutil.copyfile(ROOT / "tools/installer/PLAYING.md", stage / "PLAYING.md")
    (stage / "BeforeInstall.txt").write_text(
        "Install Shogun 2 First-Person Mod v0.28\n\n"
        "Close Shogun 2 completely. Select your original Windows Steam game folder on the next page.\n\n"
        "Setup preserves unrelated mods and keeps a snapshot of the files it changes. "
        "It does not change your saves, graphics, CPU settings, recorder or debugger.\n\n"
        "This copy is a private preview. Game artwork, fonts and audio remain their owners' material; "
        "this package is not cleared for public redistribution.\n",
        encoding="utf-8",
    )
    (stage / "AfterInstall.txt").write_text(
        "Installation complete\n\n"
        "Launch Shogun 2 normally through Steam. In a land battle, select a friendly unit and press F9. "
        "F10 returns to the RTS camera.\n\n"
        "To remove the mod, close the game and use Windows Settings > Apps > Installed apps > "
        "Shogun 2 First-Person Mod > Uninstall.\n\n"
        "Your rollback snapshots stay in the setup records folder after uninstall. "
        "The separate crash recorder is neither installed nor removed by this setup.\n",
        encoding="utf-8",
    )
    receipt = {
        "product": "Shogun 2 First-Person Mod", "version": "0.28", "candidate": "Test158",
        "framework": "Inno Setup 7.1.0", "private_preview": True,
        "public_resource_redistribution_cleared": False, "gameplay_rebuilt": False,
        "accepted_payload_sha256": EXPECTED,
        "packaging_sources": {name: sha(ROOT / "tools/installer" / name)
                              for name in ("game_integration.cpp", "Shogun2FirstPerson.iss", "build_setup.py", "PLAYING.md")},
        "integration_helper_sha256": sha(helper),
    }
    (stage / "payload-manifest.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    output = out / ("fixture-output" if args.fixture else "output")
    params = [str(args.iscc.resolve()), f"/DPayloadDir={stage}", f"/DOutputDir={output}"]
    if args.fixture:
        params.append("/DFixture=1")
    params.append(str(ROOT / "tools/installer/Shogun2FirstPerson.iss"))
    subprocess.run(params, check=True)
    setup = output / "Shogun2-First-Person-v0.28-Setup.exe"
    receipt.update(setup_sha256=sha(setup), setup_bytes=setup.stat().st_size, fixture=args.fixture)
    (out / "package-receipt.json").write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
    print(setup)

if __name__ == "__main__":
    main()
