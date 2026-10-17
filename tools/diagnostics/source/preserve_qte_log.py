"""Supplement one already-running session with its private QTE log; no debugger/input sampling."""
import argparse,ctypes as C,json,os,time
from pathlib import Path
from ctypes import wintypes as W
from process_probe import Process,k
from session import creation,write_json,stamp
args=argparse.ArgumentParser(description=__doc__);args.add_argument('--pid',type=int,required=True);args.add_argument('--source',type=Path,required=True);args.add_argument('--folder',type=Path,required=True);args.add_argument('--creation',type=int,required=True);opts=args.parse_args()
allowed=Path(r'C:\Users\logsa\Documents\Codex\2026-10-16\continue-my-native-total-war-shogun\outputs\automatic-recordings').resolve()
assert opts.folder.resolve().is_relative_to(allowed) and opts.source.name==f'qte-playable79-{opts.pid}.jsonl'
assert opts.source.parent.name=='private' and opts.source.parent.parent.parent.name=='Shogun2PossessionCheckpoint45'
k.CreateMutexW.argtypes=[C.c_void_p,W.BOOL,W.LPCWSTR];k.CreateMutexW.restype=W.HANDLE
mutex=k.CreateMutexW(None,False,f'Local\\Shogun2QteLogMirror-{opts.pid}');assert mutex
if C.get_last_error()==183:k.CloseHandle(mutex);raise SystemExit(0)
k.GetExitCodeProcess.argtypes=[W.HANDLE,C.POINTER(W.DWORD)];k.GetExitCodeProcess.restype=W.BOOL
assert creation(opts.pid)==opts.creation
process=Process(opts.pid);target=opts.folder/'native-logs/private'/opts.source.name;target.parent.mkdir(parents=True,exist_ok=True)
position=0
if target.exists():
 saved=target.read_bytes()
 with opts.source.open('rb') as source:assert source.read(len(saved))==saved,'Existing QTE evidence is not a source prefix'
 position=len(saved)
health=opts.folder/'qte-log-mirror-health.json';next_health=0;reason='running'
try:
 while True:
  size=opts.source.stat().st_size
  assert size>=position,'QTE source was truncated; preserve existing evidence'
  if size>position:
   with opts.source.open('rb') as source,target.open('ab') as out:
    source.seek(position);data=source.read(size-position);out.write(data);position+=len(data)
  code=W.DWORD();assert k.GetExitCodeProcess(process.handle,C.byref(code))
  alive=code.value==259
  if time.monotonic()>=next_health or not alive:
   write_json(health,{'pid':opts.pid,'worker_pid':os.getpid(),**stamp(),'state':'active' if alive else 'finalized','source':str(opts.source),'preserved_log':str(target),'bytes_copied':position,'core_recorder_untouched':True,'kind':'Supplemental append-only QTE log mirror, not a duplicate crash/input/memory recorder.'});next_health=time.monotonic()+1
  if not alive:reason='game_exited';break
  time.sleep(.25)
finally:
 process.close();k.CloseHandle(mutex)
