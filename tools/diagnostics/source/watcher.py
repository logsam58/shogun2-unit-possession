"""Persistent per-user Shogun watcher. No game launch, injection, or pause/READY requirement."""
import ctypes as C,json,os,subprocess,sys,time,traceback
from ctypes import wintypes as W
from pathlib import Path
from process_probe import processes
from session import creation,hidden,stamp,write_json
ROOT=Path(__file__).resolve().parent
k=C.WinDLL('kernel32',use_last_error=True);k.CreateMutexW.argtypes=[C.c_void_p,W.BOOL,W.LPCWSTR];k.CreateMutexW.restype=W.HANDLE;k.CloseHandle.argtypes=[W.HANDLE]

def run():
    mutex=k.CreateMutexW(None,False,'Local\\Shogun2AutomaticCrashRecorder-'+os.environ.get('USERNAME','user'))
    if not mutex:raise C.WinError(C.get_last_error())
    if C.get_last_error()==183:k.CloseHandle(mutex);return
    config=json.loads((ROOT/'config.json').read_text());records=Path(config['recordings']);records.mkdir(parents=True,exist_ok=True);children={};seen={};next_health=0;started=time.time()
    with (ROOT/'watcher.jsonl').open('a',buffering=1) as log:
        def emit(event,**items):log.write(json.dumps({'event':event,**stamp(),**items})+'\n')
        emit('watcher_started',pid=os.getpid(),recordings=str(records))
        try:
            while not (ROOT/'watcher.stop').exists():
                try:
                    current=processes();present=set()
                    for item in current:
                        pid=item['pid']
                        try:born=creation(pid)
                        except OSError as e:emit('process_probe_error',pid=pid,error=str(e));continue
                        key=f'{pid}-{born}';present.add(key)
                        if key in children and children[key]['child'].poll() is None:continue
                        prior=seen.get(key,0)
                        if prior>=3:continue
                        seen[key]=prior+1
                        folder=records/(time.strftime('%Y%m%d-%H%M%S',time.localtime())+f'-pid{pid}'+(f'-recovery{prior}' if prior else ''))
                        folder.mkdir(parents=True,exist_ok=True);output=(folder/'session-worker.log').open('a')
                        child=hidden([sys.executable,ROOT/'session.py','--pid',pid,'--folder',folder],output)
                        children[key]={'child':child,'folder':str(folder),'pid':pid,'output':output};emit('session_started',pid=pid,worker_pid=child.pid,folder=str(folder),retry=prior)
                    for key,item in list(children.items()):
                        if item['child'].poll() is not None:
                            emit('session_worker_ended',pid=item['pid'],returncode=item['child'].returncode,folder=item['folder']);item['output'].close();del children[key]
                        # Keep workers alive while finalizing exited games; the loop may start another game concurrently.
                    if time.monotonic()>=next_health:
                        write_json(ROOT/'watcher-health.json',{'pid':os.getpid(),'started_unix':started,**stamp(),'state':'armed','poll_interval_ms':250,'active_sessions':[{'pid':c['pid'],'worker_pid':c['child'].pid,'folder':c['folder']} for c in children.values()]});next_health=time.monotonic()+5
                except Exception as e:emit('watcher_loop_error',error=str(e),traceback=traceback.format_exc());time.sleep(1)
                time.sleep(.25)
        finally:
            emit('watcher_stopping')
            for item in children.values():Path(item['folder'],'session.stop').touch()
            write_json(ROOT/'watcher-health.json',{'pid':os.getpid(),**stamp(),'state':'stopped','active_sessions':[]});k.CloseHandle(mutex)
if __name__=='__main__':
    import argparse
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--run',action='store_true',help='Explicitly arm automatic recording')
    options=parser.parse_args()
    if not options.run:parser.error('Use --run to arm recording explicitly')
    run()
