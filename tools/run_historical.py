"""Run the retained optional suite only after exact archive preflight."""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from check_historical import inspect
from game_packs import require_game

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--fixtures',type=Path);p.add_argument('--build-dir',type=Path,required=True)
    p.add_argument('--game',type=Path,required=True);p.add_argument('--vcvars',type=Path,required=True)
    p.add_argument('--output-dir',type=Path,required=True);p.add_argument('--require',action='store_true');o=p.parse_args()
    report=inspect(o.fixtures);out=o.output_dir.resolve()
    if out.exists() or out==ROOT or ROOT.is_relative_to(out) or out.is_relative_to(o.game.resolve()):p.error('Use a new output directory outside the game')
    out.mkdir(parents=True)
    if report['status']!='prerequisites-verified':
        (out/'historical-result.json').write_text(json.dumps(report,indent=2)+'\n')
        print(report['status'].upper()+': historical suite unavailable; see historical-result.json')
        raise SystemExit(1 if report['changed'] or o.require else 0)
    require_game(o.game)
    build=o.build_dir.resolve();source=build/'source/native/work/installer-siege99/verify.cpp'
    if not source.is_file() or not (build/'obj/installer.res').is_file():p.error('Supply a completed current all build with its source snapshot')
    # Preserve the retained relative archive layout. Never execute an archived
    # installer or game; compile only the current VERIFY_FIXTURE executable.
    fixture=Path(tempfile.mkdtemp(prefix='s2fp history '));report['fixture']=str(fixture)
    manifest=json.loads((ROOT/'tools/data/historical-fixtures.json').read_text())
    for row in manifest['files']:
        target=fixture/row['path'];target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(o.fixtures/row['path'],target)
    payload=fixture/'native/outputs/shogun2-siege99-preview';payload.mkdir(parents=True,exist_ok=True)
    for name in ('possession-v46.dll','possession-hud.exe','qte-playable-v79.dll'):shutil.copy2(build/'bin'/name,payload/name)
    cwd=fixture/'native/work/installer-siege99';cwd.mkdir(parents=True,exist_ok=True);exe=cwd/'verify-historical.exe'
    command=['cl','/nologo','/std:c++17','/EHsc','/O2','/MT',str(source),str(build/'obj/installer.res'),'/Fo'+str(out/'history.obj'),'/Fe'+str(exe),'/link','bcrypt.lib','shell32.lib','advapi32.lib','comdlg32.lib','user32.lib','gdi32.lib']
    for arg in [str(o.vcvars),*command]:
        if any(c in arg for c in '\r\n%&|<>^!'):raise ValueError('Unsupported shell argument')
    batch=out/'history.cmd';batch.write_text('@echo off\ncall '+subprocess.list2cmdline([str(o.vcvars)])+'\nif errorlevel 1 exit /b %errorlevel%\n'+subprocess.list2cmdline(command)+'\nif errorlevel 1 exit /b %errorlevel%\n'+subprocess.list2cmdline([str(exe)])+'\n')
    env=dict(os.environ,SHOGUN2_GAME_DIR=str(o.game.resolve()))
    with (out/'history.log').open('w') as log:r=subprocess.run(['cmd.exe','/d','/c',str(batch)],cwd=cwd,env=env,stdout=log,stderr=subprocess.STDOUT)
    report.update(status='passed' if r.returncode==0 else 'failed',suite_executed=True,returncode=r.returncode)
    (out/'historical-result.json').write_text(json.dumps(report,indent=2)+'\n');raise SystemExit(r.returncode)

if __name__=='__main__':main()
