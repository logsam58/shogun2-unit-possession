"""Build a standard code-only setup; game resources are prepared on the player's PC."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[2]
VERSION=(ROOT/'VERSION').read_text().strip()
sys.path.insert(0,str(Path(__file__).parent))
from audit_local_payload159 import audit
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    p=argparse.ArgumentParser(description=__doc__)
    for name in ('build','vcvars','iscc','output'):p.add_argument('--'+name,type=Path,required=True)
    p.add_argument('--fixture',action='store_true')
    p.add_argument('--verified-fixture',type=Path,help='Reuse the exact helper that passed isolated setup verification')
    a=p.parse_args()
    build=a.build.resolve();out=a.output.resolve();source=build/'source';binaries=build/'bin'
    assert not out.exists() and out!=build and build not in out.parents
    assert not any((x/'Empire.Retail.dll').exists() for x in (out,*out.parents)), 'Never package into a game directory'
    inputs=json.loads((build/'build-inputs.json').read_text())
    result=json.loads((build/'build-results.json').read_text())
    assert inputs['local_assets'] and inputs['native_build_passed'] and result['candidate']>=160
    assert all(r.get('passed',True) for r in result['results'])
    assert {'local-assets-format-render-tests','adapter-tests','qte-tests','hud-tests','loader-tests','installer-current-tests','startup-view-tests'} <= {r['name'] for r in result['results']}
    checked=audit(binaries)
    out.mkdir(parents=True);stage=out/'stage';stage.mkdir()
    for value in (out,source,a.vcvars,ROOT):
        assert not any(c in str(value) for c in '%!&|<>^\r\n"'), 'Unsupported shell path'
    helper=stage/'game-integration.exe';batch=out/'compile-helper.cmd'
    batch.write_text('@echo off\n'
        f'call "{a.vcvars.resolve()}" >nul\nif errorlevel 1 exit /b 1\n'
        'cl /nologo /O2 /MT /EHsc /std:c++17 /DUNICODE /D_UNICODE /DNOMINMAX /DS2FP_LOCAL_ASSETS '
        f'/I"{source / "native/work/installer-siege99"}" /I"{source / "native/work/autoload99-preview"}" '
        f'"{ROOT / "tools/installer/game_integration.cpp"}" /Fe:"{helper}" /Fo:"{out / "game-integration.obj"}" '
        '/link bcrypt.lib shell32.lib advapi32.lib ole32.lib comdlg32.lib user32.lib\n',encoding='utf-8')
    if a.verified_fixture:
        verified=json.loads((a.verified_fixture/'verification.json').read_text())
        fixture_helper=a.verified_fixture/'helper/game-integration.exe'
        assert verified['passed'] and verified['helper_sha256']==sha(fixture_helper)
        assert verified['integration_source_sha256']==sha(ROOT/'tools/installer/game_integration.cpp')
        shutil.copyfile(fixture_helper,helper)
    else:
        subprocess.run(['cmd.exe','/d','/c',str(batch)],check=True)
    for name in ('dinput8.dll','marker.pack','prepare-local-assets159.exe'):shutil.copyfile(binaries/name,stage/name)
    for name in ('LICENSE','THIRD_PARTY_NOTICES.md'):shutil.copyfile(ROOT/name,stage/name)
    shutil.copyfile(ROOT/'native/outputs/shogun2-possession-cleanup-v46/vendor/LICENSE.txt',stage/'MinHook-LICENSE.txt')
    shutil.copyfile(ROOT/'tools/installer/PLAYING.md',stage/'PLAYING.md')
    (stage/'BeforeInstall.txt').write_text(f'Shogun 2 Unit Possession v{VERSION}\n\nClose Shogun 2. Select your original Steam installation. Setup reads artwork, unit cards, ability icons, fonts and the advisor voice from your own game. It does not download or ship those resources. No Python or developer tools are needed.\n\nWindows N editions need the Media Feature Pack. Saves, graphics, CPU settings and other mods are preserved. The separate recorder/debugger is not installed or changed.\n',encoding='utf-8')
    (stage/'AfterInstall.txt').write_text('Launch through Steam and choose your default camera in the startup window. P switches views during possession. In a land battle, select a friendly unit and press F9. F10 returns to RTS.\n\nTo remove the mod, close the game and use Windows Settings > Apps > Shogun 2 Unit Possession. Rollback snapshots remain in the setup records folder.\n',encoding='utf-8')
    receipt={'version':VERSION,'candidate':result['candidate'],'game_resources_shipped':False,'assets_prepared_at_install':True,
             'asset_audit':checked,'fixture':a.fixture,'installed_or_launched':False,'signed':False,
             'payloads':{x.name:sha(x) for x in stage.iterdir()},
             'packaging_sources':{x:sha(ROOT/'tools/installer'/x) for x in ('build_local_setup159.py','Shogun2LocalAssets159.iss','game_integration.cpp','audit_local_payload159.py')}}
    (stage/'payload-manifest.json').write_text(json.dumps(receipt,indent=2)+'\n')
    output=out/('fixture-output' if a.fixture else 'output')
    args=[str(a.iscc.resolve()),f'/DPayloadDir={stage}',f'/DOutputDir={output}',f'/DProductVersion={VERSION}',f'/DProductFileVersion={VERSION}.0.0']
    if a.fixture:args.append('/DFixture=1')
    subprocess.run([*args,str(ROOT/'tools/installer/Shogun2LocalAssets159.iss')],check=True)
    setup=output/f'Shogun2-Unit-Possession-v{VERSION}-Setup.exe'
    receipt.update(setup_sha256=sha(setup),setup_bytes=setup.stat().st_size)
    (out/'package-receipt.json').write_text(json.dumps(receipt,indent=2)+'\n')
    print(setup)
if __name__=='__main__':main()
