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
    p.add_argument("--game", type=Path, required=True)
    p.add_argument("--assets", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    p.add_argument("--previous-bin", type=Path)
    p.add_argument("--previous159-bin", type=Path)
    p.add_argument("--previous-setup", type=Path)
    p.add_argument("--previous-release-bin", type=Path)
    p.add_argument("--previous029-bin", type=Path)
    p.add_argument("--previous030-bin", type=Path)
    p.add_argument("--previous031-bin", type=Path)
    a = p.parse_args()
    version=json.loads((a.build/'package-receipt.json').read_text())['version']
    assert digest(a.engine) == "022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5"
    a.output.mkdir(parents=True, exist_ok=False)
    helper_dir=a.output/'helper';helper_dir.mkdir()
    for name in ('game-integration.exe','dinput8.dll','marker.pack'):
        shutil.copyfile(a.build/'stage'/name,helper_dir/name)
    shutil.copytree(a.assets,helper_dir/'local-assets')
    helper = (helper_dir / 'game-integration.exe').resolve()
    results = []

    def fixture(name, raw=b'mod other.pack;\r\n'):
        root = a.output / name
        game, profile, app = root / "game", root / "profile", root / "app"
        (game / "data").mkdir(parents=True)
        (profile / "The Creative Assembly/Shogun2/scripts").mkdir(parents=True)
        (profile / "The Creative Assembly/Launcher").mkdir(parents=True)
        app.mkdir()
        steam=root/'synthetic-steam162'
        (steam/'config').mkdir(parents=True);(steam/'userdata/123/config').mkdir(parents=True)
        (steam/'.unit-possession-fixture162').write_bytes(b'synthetic only\n')
        (steam/'config/loginusers.vdf').write_text('"users" { "76561197960265851" { "MostRecent" "1" } }')
        (steam/'userdata/123/config/localconfig.vdf').write_text('"UserLocalConfigStore" { "Other" "keep" }\n')
        os.link(a.engine, game / "Empire.Retail.dll")
        for pack in ('data.pack','local_en.pack'):os.link(a.game/'data'/pack,game/'data'/pack)
        script = profile / "The Creative Assembly/Shogun2/scripts/user.script.txt"
        launcher = profile / "The Creative Assembly/Launcher/20190104-moddata.dat"
        script.write_bytes(raw)
        launcher.write_text('[{"uuid":"other.pack","game":"shogun2","active":true}]', encoding="utf-8")
        targets = [game / "DINPUT8.dll", game / "data/shogun2_possession_checkpoint45.pack", script, launcher]
        assets=game/'data/shogun2_possession_local_assets159';assets.mkdir()
        targets += [assets/'hud-assets159.s2fp',assets/'qte-assets159.s2fp']
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
        for target in f[4][4:]:shutil.copyfile(a.assets/target.name,target)

    if a.previous_bin:
        f=fixture('accepted158-upgrade')
        for name,target in zip(('dinput8.dll','marker.pack'),f[4][:2]):shutil.copyfile(a.previous_bin/name,target)
        baseline=[digest(t) for t in f[4]]
        assert baseline[0]=='936bb2cd09bfb57ba4a703a3dbdb7ca682d8df4e92a7feaf8b3738d8493a26c2'
        assert baseline[1]=='3db77b196bc9fb3ded51ac6e850a386a2b8cc30cf9e97011b1babf88d5e2753b'
        action('prepare',f);copy_payload(f);action('finish',f);action('rollback',f)
        assert baseline==[digest(t) for t in f[4]]
        results.append('exact accepted158 upgrade and complete rollback')

    if a.previous159_bin:
        f=fixture('accepted159-upgrade')
        for name,target in zip(('dinput8.dll','marker.pack'),f[4][:2]):shutil.copyfile(a.previous159_bin/name,target)
        for target in f[4][4:]:shutil.copyfile(a.assets/target.name,target)
        baseline=[digest(t) for t in f[4]]
        assert baseline[0]=='13e9097a3965d9a8a3fb054c1dd9dfe30c0591eba22fda678ab736b036251c44'
        action('prepare',f);copy_payload(f);action('finish',f);action('rollback',f)
        assert baseline==[digest(t) for t in f[4]]
        results.append('exact accepted159 upgrade and complete rollback')

    if a.previous_release_bin:
        f=fixture('former-release-upgrade')
        for name,target in zip(('dinput8.dll','marker.pack'),f[4][:2]):shutil.copyfile(a.previous_release_bin/name,target)
        for target in f[4][4:]:shutil.copyfile(a.assets/target.name,target)
        baseline=[digest(t) for t in f[4]]
        assert baseline[0]=='583b12f04734b8e5db59cad9f909b3a2f1c32773d5f5513008e4601d9f317af3'
        assert baseline[1]=='024eb40e5298e80fd817a95c13cf703de396ef741df2edb2aa1c46ac2d23573e'
        action('prepare',f);copy_payload(f);action('finish',f);action('rollback',f)
        assert baseline==[digest(t) for t in f[4]]
        results.append('exact former v1.0 upgrade and complete rollback')

    if a.previous029_bin:
        f=fixture('previous029-upgrade')
        for name,target in zip(('dinput8.dll','marker.pack'),f[4][:2]):shutil.copyfile(a.previous029_bin/name,target)
        for target in f[4][4:]:shutil.copyfile(a.assets/target.name,target)
        baseline=[digest(t) for t in f[4]]
        assert baseline[0]=='6e2bec13bdf74795e48281bdade07fa8a2c101ca7993ea2765679d7e5abd5af0'
        action('prepare',f);copy_payload(f);action('finish',f);action('rollback',f)
        assert baseline==[digest(t) for t in f[4]]
        results.append('exact v0.29 upgrade and complete rollback')

    if a.previous031_bin:
        f=fixture('previous031-upgrade')
        for name,target in zip(('dinput8.dll','marker.pack'),f[4][:2]):shutil.copyfile(a.previous031_bin/name,target)
        for target in f[4][4:]:shutil.copyfile(a.assets/target.name,target)
        baseline=[digest(t) for t in f[4]]
        assert baseline[0]=='9c1471e8dea6bae0f21dec152fc092e1858e680a878a74fa282d1f1b8bda1f64'
        assert baseline[1]=='7835ea6cab325d8823be27e1a19476e4c0cabf826db52c1258e7ce4c8dcf12ca'
        action('prepare',f);copy_payload(f);action('finish',f);action('rollback',f)
        assert baseline==[digest(t) for t in f[4]]
        results.append('exact v0.31 upgrade and complete rollback')

    if a.previous030_bin:
        f=fixture('previous030-upgrade')
        for name,target in zip(('dinput8.dll','marker.pack'),f[4][:2]):shutil.copyfile(a.previous030_bin/name,target)
        for target in f[4][4:]:shutil.copyfile(a.assets/target.name,target)
        baseline=[digest(t) for t in f[4]]
        assert baseline[0]=='19f7c9b197cb8128a160aa8bd7b4e930b2612f420a5468413690bea55e791ae7'
        assert baseline[1]=='a611fadf4369e7a415aab40099982822c32ba5107c357fea12aadc1c73e81623'
        action('prepare',f);copy_payload(f);action('finish',f);action('rollback',f)
        assert baseline==[digest(t) for t in f[4]]
        results.append('exact v0.30 upgrade and complete rollback')

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
        assert all(not f[4][i].exists() for i in (0,1,4,5))
        assert "other.pack" in f[4][3].read_text()
        if raw:
            clean = f[4][2].read_bytes()
            if name == "utf16le": clean = clean[2:].decode("utf-16-le")
            elif name == "utf16be": clean = clean[2:].decode("utf-16-be")
            else: clean = clean.decode("utf-8-sig")
            assert "mod other.pack;" in clean and "shogun2_possession" not in clean
        assert preserved == digest(f[1] / "Empire.Retail.dll")
        results.append(name + ": install, repeat repair, uninstall, repeat uninstall, engine preservation")

    for prefix in range(7):
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

    for index in (4,5):
        f=fixture('unknown-local-assets-'+str(index));f[4][index].write_bytes(b'unknown local resource')
        baseline=[digest(t) for t in f[4]];action('prepare',f,ok=False)
        assert baseline==[digest(t) for t in f[4]]
        results.append('unknown local asset refused without game changes: '+str(index))
    f=fixture('changed-local-after-install');action('prepare',f);copy_payload(f);action('finish',f)
    f[4][4].write_bytes(b'replacement resource');baseline=[digest(t) for t in f[4]]
    action('uninstall-check',f,ok=False);action('rollback',f,ok=False);assert baseline==[digest(t) for t in f[4]]
    results.append('post-install asset changes preserved and refused')

    # Run the real Inno engine against fixture paths, with its separate registry
    # identity. Never execute the production administrator installer in tests.
    f = fixture("inno-end-to-end")
    setup = (a.build / f"fixture-output/Shogun2-Unit-Possession-v{version}-Setup.exe").resolve()
    options = ["/VERYSILENT", "/SUPPRESSMSGBOXES", "/SP-", "/NORESTART", "/NOCLOSEAPPLICATIONS", "/NORESTARTAPPLICATIONS",
               f"/DIR={f[3].resolve()}", f"/GAME={f[1].resolve()}", f"/PROFILE={f[2].resolve()}", f"/STEAMROOT={f[0].resolve()/'synthetic-steam162'}"]
    for attempt in range(2):
        subprocess.run([str(setup), *options, f"/LOG={f[0].resolve() / ('install-' + str(attempt) + '.log')}"], check=True, timeout=60)
        assert (f[3] / "unins000.exe").exists()
        assert digest(f[3]/'shogun2-unit-possession-launcher.exe')==digest(a.build/'stage/shogun2-unit-possession-launcher.exe')
        configured=(f[0]/'synthetic-steam162/userdata/123/config/localconfig.vdf').read_text()
        assert 'LaunchOptions' in configured and '%command%' in configured and 'shogun2-unit-possession-launcher.exe' in configured
        assert (f[3]/'steam-launch162.state').exists()
        assert digest(f[3]/'steam-launch162.exe')==digest(a.build/'stage/steam-launch162.exe')
        assert digest(f[4][0]) == digest(a.build / "stage/dinput8.dll")
        for i in (4,5):assert digest(f[4][i])==digest(a.assets/f[4][i].name)
    key = r"Software\Microsoft\Windows\CurrentVersion\Uninstall\Shogun2FirstPerson-LocalAssetsFixture159_is1"
    with winreg.OpenKey(winreg.HKEY_CURRENT_USER, key) as k:
        assert winreg.QueryValueEx(k, "DisplayName")[0] == "Shogun 2 Unit Possession"
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
    assert all(not f[4][i].exists() for i in (0,1,4,5))
    assert not (f[3]/"shogun2-unit-possession-launcher.exe").exists()
    assert not (f[3]/"Steam-Launch-Options.txt").exists()
    assert 'LaunchOptions' not in (f[0]/'synthetic-steam162/userdata/123/config/localconfig.vdf').read_text()
    assert '"Other" "keep"' in (f[0]/'synthetic-steam162/userdata/123/config/localconfig.vdf').read_text()
    assert (f[3] / f"rollback-v{version}/manifest.json").exists()
    assert "other.pack" in f[4][3].read_text()
    results.append("real Inno uninstall removes fixture registration and mod, retains rollback and other mods")
    failed=fixture('inno-missing-original-pack')
    # Remove only this fixture's hardlink, never modify the real pack inode.
    (failed[1]/'data/local_en.pack').unlink()
    baseline=[digest(t) for t in failed[4]]
    bad_options=["/VERYSILENT","/SUPPRESSMSGBOXES","/SP-","/NORESTART",f"/DIR={failed[3].resolve()}",f"/GAME={failed[1].resolve()}",f"/PROFILE={failed[2].resolve()}",f"/STEAMROOT={failed[0].resolve()/'synthetic-steam162'}",f"/LOG={failed[0].resolve()/'failed.log'}"]
    result=subprocess.run([str(setup),*bad_options],timeout=60)
    assert result.returncode!=0 and baseline==[digest(t) for t in failed[4]]
    assert not (failed[3]/'unins000.exe').exists()
    try:
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER,key):pass
    except FileNotFoundError:pass
    else:raise AssertionError('Failed preparation registered an installation')
    results.append('real Inno missing original input fails before deployment/registration')

    if a.previous_setup:
        upgraded=fixture('real-preview-upgrade')
        opt=["/VERYSILENT","/SUPPRESSMSGBOXES","/SP-","/NORESTART",f"/DIR={upgraded[3].resolve()}",f"/GAME={upgraded[1].resolve()}",f"/PROFILE={upgraded[2].resolve()}",f"/STEAMROOT={upgraded[0].resolve()/'synthetic-steam162'}"]
        subprocess.run([str(a.previous_setup.resolve()),*opt],check=True,timeout=60)
        old_records={str(p.relative_to(upgraded[3])):digest(p) for p in (upgraded[3]/'rollback').rglob('*') if p.is_file()}
        assert old_records and digest(upgraded[4][0])=='13e9097a3965d9a8a3fb054c1dd9dfe30c0591eba22fda678ab736b036251c44'
        subprocess.run([str(setup),*opt],check=True,timeout=60)
        assert digest(upgraded[4][0])==digest(a.build/'stage/dinput8.dll')
        assert all(digest(upgraded[3]/p)==h for p,h in old_records.items())
        assert (upgraded[3]/f'rollback-v{version}/manifest.json').is_file()
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER,key) as k:assert winreg.QueryValueEx(k,'DisplayVersion')[0]==version
        subprocess.run([str((upgraded[3]/'unins000.exe').resolve()),'/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART'],check=True,timeout=60)
        for _ in range(100):
            try:
                with winreg.OpenKey(winreg.HKEY_CURRENT_USER,key):pass
            except FileNotFoundError:break
            time.sleep(.1)
        else:raise AssertionError('Upgrade fixture registration retained')
        assert all(not upgraded[4][i].exists() for i in (0,1,4,5))
        assert all(digest(upgraded[3]/p)==h for p,h in old_records.items())
        results.append('real Inno preview-to-release upgrade preserves old rollback and removes new mod safely')

    report = {"passed": True, "checks": results, "count": len(results), "real_game_modified": False,
              "production_setup_executed": False, "fixture_setup_sha256": digest(setup), "helper_sha256": digest(helper), "integration_source_sha256": digest(Path(__file__).parent / "game_integration.cpp")}
    (a.output / "verification.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))

if __name__ == "__main__":
    main()
