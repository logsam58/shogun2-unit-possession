"""Capture-only checks; no game launch, installed-file writes or hardware tracing."""
import argparse,json,shutil,subprocess,sys,time
from pathlib import Path
SOURCE=Path(__file__).resolve().parent;sys.path.insert(0,str(SOURCE))
from binary_policy111 import require_capture_only111
from inspect_minidump import analyze
FLAGS=0x08000000
def rows(path):return [json.loads(x) for x in path.read_text().splitlines() if x.strip()]
def main():
 p=argparse.ArgumentParser();p.add_argument('--build-dir',type=Path,required=True);p.add_argument('--support-root',type=Path,required=True);p.add_argument('--output-dir',type=Path,required=True);o=p.parse_args()
 build=o.build_dir.resolve();out=o.output_dir.resolve();out.mkdir(parents=True,exist_ok=False);tests=[]
 def result(name,**extra):
  tests.append({'case':name,'passed':True,**extra});print(json.dumps(tests[-1]),flush=True)
 core=build/'crash-recorder.exe';require_capture_only111(core);result('no-instrumentation-binary-policy')
 unit=out/'unit';unit.mkdir()
 r=subprocess.run([str(build/'verify-dump112.exe'),str(unit),str(build/'diagnostics-fixture.exe')],capture_output=True,text=True,creationflags=FLAGS,timeout=20)
 assert r.returncode==0,r.stderr;result('eight-fallback-cache-snapshot-cases',output=r.stdout.strip())
 attempts=rows(unit/'attempts.jsonl')
 assert any(x['event']=='dump_attempt112' and x.get('error')==0x8007012b for x in attempts)
 for name,full in [('mode-0.dmp',True),('mode-1-readable.dmp',True),('mode-2-minimal.dmp',False)]:
  report=analyze(unit/name);assert (9 in report['streams'])==full
  result('valid-'+name,bytes=(unit/name).stat().st_size,full_memory=full)
 snap=json.loads((unit/'independent.snapshot.json').read_text());assert snap['cached_module_offset']=='toy.dll+0x123' and not snap['context_captured'] and snap['context_error']
 stage=out/'stage';stage.mkdir()
 for src in o.support_root.iterdir():
  if src.is_file() and src.suffix in ['.py','.json','.ps1']:shutil.copy2(src,stage/src.name)
 shutil.copy2(SOURCE/'verify.py',stage/'verify.py');(stage/'native').mkdir()
 for src in [core,build/'diagnostics-fixture.exe']:shutil.copy2(src,stage/'native'/src.name)
 with (out/'core-sessions.log').open('w') as log:r=subprocess.run([sys.executable,str(stage/'verify.py')],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS,timeout=120)
 assert r.returncode==0,(out/'core-sessions.log').read_text();result('four-full-core-session-cases')
 snapshots=list((stage/'fixture-results/final').glob('*/*.snapshot.json'));assert len(snapshots)==3
 for path in snapshots:
  s=json.loads(path.read_text());assert s['context_captured'] and s['code']=='0xc0000005'
  assert s['cached_module_offset'].startswith('diagnostics-fixture.exe+') and s['stack_bytes']['bytes_read'] and s['code_bytes']['bytes_read']
 result('native-fault-register-stack-cache-evidence',snapshots=len(snapshots))
 folder=out/'loader-corruption';folder.mkdir()
 with (folder/'console.log').open('w') as log:
  toy=subprocess.Popen([str(build/'diagnostics-fixture.exe'),'loader-fault112'],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS)
  debugger=subprocess.Popen([str(core),'--fixture',str(toy.pid),'0',str(folder)],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS)
  toy.wait(timeout=15);debugger.wait(timeout=15)
 assert toy.returncode==0 and debugger.returncode==0,(toy.returncode,debugger.returncode)
 events=rows(folder/f'exceptions-{toy.pid}.jsonl');faults=[x for x in events if x['event']=='exception'];assert len(faults)==1 and faults[0]['first_chance']
 saved=list(folder.glob('*.snapshot.json'));assert len(saved)==1
 s=json.loads(saved[0].read_text());assert s['context_captured'] and s['cached_module_offset'].startswith('diagnostics-fixture.exe+') and s['stack_bytes']['bytes_read']
 result('real-toy-loader-corruption-evidence-survives',native_error_attempts=sum(x['event']=='dump_attempt112' and not x['success'] for x in events),snapshot_context=True)
 report={'passed':True,'groups':tests,'game_launched':False,'installed_files_changed':False,'hardware_tracing':False,'injection_limit':'Forced writer read errors test retry policy; damaged toy loader tests real context/cache survival. Neither establishes why the real game dump reads failed.'}
 (out/'verification112.json').write_bytes((json.dumps(report,indent=2)+'\n').encode());print('CAPTURE112 PASS',flush=True)
if __name__=='__main__':main()
