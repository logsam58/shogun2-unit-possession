"""Offline-only debugger regression, isolation and failure-injection checks."""
import argparse,json,shutil,subprocess,sys,time
from pathlib import Path
SOURCE=Path(__file__).resolve().parent;sys.path.insert(0,str(SOURCE))
from binary_policy111 import require_capture_only111
FLAGS=0x08000000
def wait(test,seconds=12):
 end=time.monotonic()+seconds
 while time.monotonic()<end:
  if test():return
  time.sleep(.01)
 raise RuntimeError('Offline fixture readiness timeout')
def rows(p):
 try:return [json.loads(x) for x in p.read_text().splitlines()]
 except FileNotFoundError:return []
def main():
 p=argparse.ArgumentParser();p.add_argument('--build-dir',type=Path,required=True);p.add_argument('--output-dir',type=Path,required=True);p.add_argument('--support-root',type=Path,required=True);p.add_argument('--baseline-debugger',type=Path);o=p.parse_args()
 out=o.output_dir.resolve();out.mkdir(parents=True,exist_ok=False);build=o.build_dir.resolve();results=[]
 def record(name,**details):
  result={'case':name,'passed':True,**details};results.append(result)
  (out/'progress.json').write_text(json.dumps(results,indent=2));print(json.dumps(result),flush=True)
 core=build/'crash-recorder.exe';experimental=build/'crash-recorder-fixture111.exe'
 policy=require_capture_only111(core)
 try:require_capture_only111(experimental)
 except ValueError as error:record('binary-isolation',production_forbidden_imports=policy['forbidden_imports'],experimental_rejected=str(error))
 else:raise AssertionError('Experimental debugger accepted as production')
 malformed=out/'invalid-pe.bin';malformed.write_bytes(b'MZ'+b'\0'*62)
 try:require_capture_only111(malformed)
 except ValueError:record('malformed-binary-rejected')
 else:raise AssertionError('Malformed binary accepted')
 blocked=out/'must-not-deploy'
 result=subprocess.run([sys.executable,str(SOURCE/'deploy110.py'),'--installed-root',str(o.support_root),'--built-recorder',str(experimental),'--evidence',str(blocked)],capture_output=True,text=True,creationflags=FLAGS,timeout=10)
 assert result.returncode!=0 and 'invasive imports' in result.stderr and not blocked.exists(),result.stderr
 (out/'deployment-rejection.txt').write_text(result.stdout+result.stderr);record('experimental-deployment-rejected-before-mutation')
 result=subprocess.run([str(build/'verify-ownership111.exe')],capture_output=True,text=True,creationflags=FLAGS,timeout=10)
 assert result.returncode==0,result.stderr;record('ownership-policy',output=result.stdout.strip())
 def trace(mode,name,debugger):
  folder=out/name;folder.mkdir()
  with (folder/'console.log').open('w') as log:
   target=subprocess.Popen([str(build/'trace-fixture110.exe'),str(folder),mode],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS)
   wait(lambda:(folder/'targeted110-fixture.txt').exists())
   debug=subprocess.Popen([str(debugger),'--fixture',str(target.pid),'0',str(folder)],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS)
   wait(lambda:(folder/'targeted110-health.json').exists());(folder/'trace-go').touch()
   target.wait(timeout=20);debug.wait(timeout=20)
  events=rows(folder/'targeted110.jsonl');faults=[x for x in rows(folder/f'exceptions-{target.pid}.jsonl') if x['event']=='exception']
  return target.returncode,debug.returncode,events,faults
 if o.baseline_debugger:
  code,debugcode,events,faults=trace('context-restore','baseline-context-restore',o.baseline_debugger.resolve())
  assert code and any(x['event']=='unowned_single_step' and x['programmed3']=='0x0' and int(x['dr3'],16) for x in events)
  assert any(x['code']=='0x80000004' and not x['first_chance'] for x in faults)
  record('baseline-defect-reproduced',toy_exit=code,debugger_exit=debugcode)
 for n,mode in enumerate(['context-restore']*12+['retirement-race']*12+['history-limit']):
  code,debugcode,events,faults=trace(mode,mode+'-'+str(n),experimental)
  assert code==0 and debugcode==0,(mode,code,debugcode)
  assert not any(x['code']=='0x80000004' for x in faults),(mode,faults)
  assert not any(x['event']=='unowned_single_step' for x in events),(mode,events)
  if mode=='context-restore':
   assert len(faults)==1 and faults[0]['code']=='0xe0421111' and faults[0]['first_chance']
   assert any(x['event']=='retired_binding_restored' for x in events)
  if mode=='history-limit':assert any(x['event']=='ownership_history_full' for x in events)
  record(mode+'-'+str(n),toy_exit=code,debugger_exit=debugcode,restored_contexts=sum(x['event']=='retired_binding_restored' for x in events))
 legacy=out/'retained-hardware-cases'
 with (out/'retained-hardware.log').open('w') as log:
  r=subprocess.run([sys.executable,str(SOURCE/'verify_targeted110.py'),'--build-dir',str(build),'--recorder',str(experimental),'--output-dir',str(legacy)],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS,timeout=100)
 assert r.returncode==0,(out/'retained-hardware.log').read_text()
 record('retained-hardware-regressions',fresh_cases=6)
 # Only private copies of the existing read-only/session helpers are exercised.
 stage=out/'core-session';stage.mkdir()
 for src in o.support_root.iterdir():
  if src.is_file() and src.suffix in ['.py','.ps1','.json']:shutil.copy2(src,stage/src.name)
 shutil.copy2(SOURCE/'verify.py',stage/'verify.py');(stage/'native').mkdir()
 for src in [core,build/'diagnostics-fixture.exe']:shutil.copy2(src,stage/'native'/src.name)
 with (out/'core-session.log').open('w') as log:
  r=subprocess.run([sys.executable,str(stage/'verify.py')],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS,timeout=120)
 assert r.returncode==0,(out/'core-session.log').read_text();record('full-core-session-regressions',fresh_cases=4)
 # Failure injection never touches a game process or installed recorder file.
 modes=['handled-single-step','handled-breakpoint','handled-custom','unhandled-illegal','thread-churn','debugger-exit','duplicate-attach','log-open-failure','dump-write-failure','health-write-failure','bad-output','experimental-wrong-target','experimental-real-mode']
 for mode in modes:
  folder=out/mode;folder.mkdir();targetmode=mode if mode in modes[:5] else 'handled' if mode=='dump-write-failure' else 'normal'
  with (folder/'console.log').open('w') as log:
   target=subprocess.Popen([str(build/'diagnostics-fixture.exe'),targetmode],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS)
   if mode=='log-open-failure':(folder/f'exceptions-{target.pid}.jsonl').mkdir()
   if mode=='dump-write-failure':(folder/f'first-chance-{target.pid}-1.dmp').mkdir()
   if mode=='health-write-failure':(folder/'targeted110-health.json.tmp').mkdir()
   use=experimental if mode.startswith('experimental-') else core
   output=folder/'absent' if mode=='bad-output' else folder
   flag='--watch-auto' if mode=='experimental-real-mode' else '--fixture'
   debug=subprocess.Popen([str(use),flag,str(target.pid),'0',str(output)],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS)
   if mode in ['debugger-exit','duplicate-attach']:
    wait(lambda:any(x.get('event')=='attached' for x in rows(folder/f'exceptions-{target.pid}.jsonl')))
    if mode=='debugger-exit':debug.terminate()
    else:
     other=subprocess.run([str(core),'--fixture',str(target.pid),'0',str(folder)],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS,timeout=10)
     assert other.returncode!=0;(folder/'crash.stop').touch()
   target.wait(timeout=15);debug.wait(timeout=15)
  faults=[];file=folder/f'exceptions-{target.pid}.jsonl'
  if file.is_file():faults=rows(file)
  if mode=='unhandled-illegal':assert target.returncode==0xc000001d and any(x.get('code')=='0xc000001d' and not x['first_chance'] for x in faults)
  else:assert target.returncode==0,(mode,target.returncode)
  if mode.startswith('handled-'):assert len([x for x in faults if x['event']=='exception' and x['first_chance']])==1
  if mode=='dump-write-failure':assert any(x['event']=='dump_error' for x in faults)
  if mode in ['log-open-failure','bad-output','experimental-wrong-target','experimental-real-mode']:assert debug.returncode!=0
  elif mode!='debugger-exit':assert debug.returncode==0,(mode,debug.returncode)
  if mode.startswith('experimental-'):assert not any(x['event']=='attached' for x in faults)
  record(mode,toy_exit=target.returncode,debugger_exit=debug.returncode)
 # Exact-path and fingerprint gates are independent of a caller's --fixture flag.
 for mismatch in ['path','fingerprint']:
  folder=out/('identity-'+mismatch);folder.mkdir();targetdir=folder/'tools';targetdir.mkdir()
  targetpath=targetdir/'trace-fixture110.exe'
  shutil.copy2(build/('trace-fixture110.exe' if mismatch=='path' else 'diagnostics-fixture.exe'),targetpath)
  use=experimental
  if mismatch=='fingerprint':use=targetdir/experimental.name;shutil.copy2(experimental,use)
  with (folder/'console.log').open('w') as log:
   args=[str(targetpath),str(folder),'normal'] if mismatch=='path' else [str(targetpath),'normal']
   target=subprocess.Popen(args,stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS)
   if mismatch=='path':wait(lambda:(folder/'targeted110-fixture.txt').exists())
   debug=subprocess.run([str(use),'--fixture',str(target.pid),'0',str(folder)],stdout=log,stderr=subprocess.STDOUT,creationflags=FLAGS,timeout=10)
   assert debug.returncode!=0
   (folder/'trace-go').touch();target.wait(timeout=15)
  assert target.returncode==0
  assert not any(x['event']=='attached' for x in rows(folder/f'exceptions-{target.pid}.jsonl'))
  text=(folder/'console.log').read_text();assert ('adjacent pinned toy' if mismatch=='path' else 'fingerprint differs') in text,text
  record('experimental-'+mismatch+'-mismatch-rejected',toy_exit=target.returncode)
 report={'passed':True,'tests':results,'game_launched':False,'installed_files_changed':False,'production_hardware_compiled':False,'arbitrary_unknown_failures_proved_impossible':False}
 (out/'verification111.json').write_text(json.dumps(report,indent=2));print('ALL HARDENING CHECKS PASS',flush=True)
if __name__=='__main__':main()
