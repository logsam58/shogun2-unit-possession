"""Exercise the standard installer only against an isolated synthetic game.

The real engine is hard-linked read-only as a fingerprint input, never executed.
The fixture installer uses a distinct HKCU uninstall identity and a mandatory
PROFILE override; the production setup does not accept that override.
"""
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import time
import winreg
from pathlib import Path

def digest(p):
    return hashlib.sha256(p.read_bytes()).hexdigest() if p.exists() else None

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--build", type=Path, required=True)
    p.add_argument("--engine", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    a = p.parse_args()
    assert digest(a.engine) == "022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5"
    a.output.mkdir(parents=True, exist_ok=False)
    helper = (a.build / "stage/game-integration.exe").resolve()
    results = []

    def fixture(name, raw=b'mod other.pack;\r\n'):
        root = a.output / name
        game, profile, app = root / "game", root / "profile", root / "app"
        (game / "data").mkdir(parents=True)
        (profile / "The Creative Assembly/Shogun2/scripts").mkdir(parents=True)
        (profile / "The Creative Assembly/Launcher").mkdir(parents=True)
        app.mkdir()
        os.link(a.engine, game / "Empire.Retail.dll")
        script = profile / "The Creative Assembly/Shogun2/scripts/user.script.txt"
        launcher = profile / "The Creative Assembly/Launcher/20190104-moddata.dat"
        script.write_bytes(raw)
        launcher.write_text('[{"uuid":"other.pack","game":"shogun2","active":true}]', encoding="utf-8")
        targets = [game / "DINPUT8.dll", game / "data/shogun2_possession_checkpoint45.pack", script, launcher]
        return root, game, profile, app, targets

    def action(command, f, ok=True):
        root, game, profile, app, _ = f
        run = subprocess.run([str(helper), command, str(game.resolve()), str(profile.resolve()),
                              str((app / "rollback").resolve()), str((root / "report.txt").resolve())], capture_output=True)
        assert (run.returncode == 0) == ok, (command, run.returncode, run.stderr.decode(errors="replace"))
        return run

    def copy_payload(f):
        for source, target in zip(("dinput8.dll", "marker.pack"), f[4][:2]):
            shutil.copyfile(a.build / "stage" / source, target)

    encodings = {
        "utf8": 'mod other.pack;\r\n// naïve comment\r\n'.encode(),
        "utf8-bom": b'\xef\xbb\xbf' + 'mod other.pack;\r\n'.encode(),
        "utf16le": b'\xff\xfe' + 'mod other.pack;\r\n'.encode("utf-16-le"),
        "utf16be": b'\xfe\xff' + 'mod other.pack;\r\n'.encode("utf-16-be"),
        "empty": b'',
    }
    for name, raw in encodings.items():
        f = fixture(name, raw)
        preserved = digest(f[1] / "Empire.Retail.dll")
        action("prepare", f); copy_payload(f); action("finish", f)
        installed = [digest(t) for t in f[4]]
        action("prepare", f); copy_payload(f); action("finish", f)
        assert installed == [digest(t) for t in f[4]], "Repeated setup changed bytes"
        action("uninstall-check", f); action("uninstall", f); action("uninstall", f)
        assert not f[4][0].exists() and not f[4][1].exists()
        assert "other.pack" in f[4][3].read_text()
        if raw:
            clean = f[4][2].read_bytes()
            if name == "utf16le": clean = clean[2:].decode("utf-16-le")
            elif name == "utf16be": clean = clean[2:].decode("utf-16-be")
            else: clean = clean.decode("utf-8-sig")
            assert "mod other.pack;" in clean and "shogun2_possession" not in clean
        assert preserved == digest(f[1] / "Empire.Retail.dll")
        results.append(name + ": install, repeat repair, uninstall, repeat uninstall, engine preservation")

    for prefix in range(5):
        f = fixture("rollback-prefix-" + str(prefix))
        baseline = [digest(t) for t in f[4]]
        action("prepare", f)
        for i in range(prefix):
            shutil.copyfile(f[3] / f"rollback/after-{i}.bin", f[4][i])
        action("rollback", f)
        assert baseline == [digest(t) for t in f[4]], "Rollback changed baseline"
        action("rollback", f)
        results.append(f"interrupted prefix {prefix}: exact baseline restored twice")

    f = fixture("unknown-dll")
    f[4][0].write_bytes(b"foreign mod")
    baseline = [digest(t) for t in f[4]]
    action("prepare", f, ok=False)
    assert baseline == [digest(t) for t in f[4]]
    results.append("foreign DINPUT8 refused without changes")

    f = fixture("unknown-pack")
    f[4][1].write_bytes(b"foreign pack")
    baseline = [digest(t) for t in f[4]]
    action("prepare", f, ok=False)
    assert baseline == [digest(t) for t in f[4]]
    results.append("foreign pack refused without changes")

    f = fixture("changed-after-install")
    action("prepare", f); copy_payload(f); action("finish", f)
    f[4][0].write_bytes(b"replacement mod")
    baseline = [digest(t) for t in f[4]]
    action("uninstall-check", f, ok=False); action("rollback", f, ok=False)
    assert baseline == [digest(t) for t in f[4]]
    results.append("unknown post-install binary blocks uninstall and rollback without changes")

    f = fixture("other-mod-added-later")
    action("prepare", f); copy_payload(f); action("finish", f)
    f[4][2].write_bytes(f[4][2].read_bytes() + b"\r\nmod later.pack;\r\n")
    rows = json.loads(f[4][3].read_text())
    rows.append({"uuid": "later.pack", "game": "shogun2", "active": True})
    f[4][3].write_text(json.dumps(rows))
    action("uninstall", f)
    assert b"later.pack" in f[4][2].read_bytes() and b"other.pack" in f[4][2].read_bytes()
    assert [r["uuid"] for r in json.loads(f[4][3].read_text())] == ["other.pack", "later.pack"]
    results.append("uninstall preserves script and launcher entries added afterward")

    f = fixture("adopt-existing-then-reinstall")
    copy_payload(f)
    baseline = [digest(t) for t in f[4]]
    action("prepare", f); copy_payload(f); action("finish", f)
    action("rollback", f)
    assert baseline == [digest(t) for t in f[4]]
    action("prepare", f); copy_payload(f); action("finish", f); action("uninstall", f)
    action("prepare", f); copy_payload(f); action("finish", f)
    assert len(list(f[3].glob("rollback-previous-*/manifest.json"))) == 1
    action("uninstall", f)
    results.append("adopt exact pre-existing mod, exact rollback, uninstall and reinstall with retained prior snapshot")

    f = fixture("malformed-launcher")
    f[4][3].write_bytes(b"unknown launcher data")
    baseline = [digest(t) for t in f[4]]
    action("prepare", f, ok=False)
    assert baseline == [digest(t) for t in f[4]]
    results.append("malformed launcher data refused without changes")

    f = fixture("changed-snapshot")
    action("prepare", f); copy_payload(f); action("finish", f)
    (f[3] / "rollback/before-2.bin").write_bytes(b"changed backup")
    baseline = [digest(t) for t in f[4]]
    action("uninstall-check", f, ok=False)
    assert baseline == [digest(t) for t in f[4]]
    results.append("changed rollback snapshot refused without game-file changes")

    # Run the real Inno engine against fixture paths, with its separate registry
    # identity. Never execute the production administrator installer in tests.
    f = fixture("inno-end-to-end")
    setup = (a.build / "fixture-output/Shogun2-First-Person-v0.28-Setup.exe").resolve()
    options = ["/VERYSILENT", "/SUPPRESSMSGBOXES", "/SP-", "/NORESTART", "/NOCLOSEAPPLICATIONS", "/NORESTARTAPPLICATIONS",
               f"/DIR={f[3].resolve()}", f"/GAME={f[1].resolve()}", f"/PROFILE={f[2].resolve()}"]
    for attempt in range(2):
        subprocess.run([str(setup), *options, f"/LOG={f[0].resolve() / ('install-' + str(attempt) + '.log')}"], check=True, timeout=60)
        assert (f[3] / "unins000.exe").exists()
        assert digest(f[4][0]) == digest(a.build / "stage/dinput8.dll")
    key = r"Software\Microsoft\Windows\CurrentVersion\Uninstall\Shogun2FirstPerson-PackagingFixture-20261021_is1"
    with winreg.OpenKey(winreg.HKEY_CURRENT_USER, key) as k:
        assert winreg.QueryValueEx(k, "DisplayName")[0] == "Shogun 2 First-Person Mod"
    results.append("real Inno install + repeat install + Windows uninstall registration")
    subprocess.run([str((f[3] / "unins000.exe").resolve()), "/VERYSILENT", "/SUPPRESSMSGBOXES", "/NORESTART",
                    f"/LOG={f[0].resolve() / 'uninstall.log'}"], check=True, timeout=60)
    for _ in range(50):
        try:
            with winreg.OpenKey(winreg.HKEY_CURRENT_USER, key):
                pass
        except FileNotFoundError:
            break
        time.sleep(.1)
    else:
        raise AssertionError("Fixture uninstall registration left behind")
    assert not f[4][0].exists() and not f[4][1].exists()
    assert (f[3] / "rollback/manifest.json").exists()
    assert "other.pack" in f[4][3].read_text()
    results.append("real Inno uninstall removes fixture registration and mod, retains rollback and other mods")
    report = {"passed": True, "checks": results, "count": len(results), "real_game_modified": False,
              "production_setup_executed": False, "fixture_setup_sha256": digest(setup), "helper_sha256": digest(helper)}
    (a.output / "verification.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))

if __name__ == "__main__":
    main()
