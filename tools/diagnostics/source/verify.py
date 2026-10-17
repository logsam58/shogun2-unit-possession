import json,subprocess,sys,time,ctypes,os
from pathlib import Path
root=Path(__file__).parent.resolve();sys.path.insert(0,str(root));from sensors import Sensors
from session import hidden
results=[]
s=Sensors();time.sleep(1.1);v=s.sample(os.getpid());s.close();(root/'sensor-test.json').write_text(json.dumps(v,indent=2));assert v.get('cpu_core_percent'),v
for mode in ('normal','handled','crash'):
    folder=root/'fixture-results'/'final'/mode;folder.mkdir(parents=True,exist_ok=True)
    with (folder/'fixture.log').open('w') as output:
        game=hidden([root/'native'/'diagnostics-fixture.exe',mode],output)
        worker=hidden([sys.executable,root/'session.py','--pid',game.pid,'--folder',folder,'--fixture'],output)
        game.wait(timeout=30);worker.wait(timeout=110)
    summary=json.loads((folder/'session-summary.json').read_text())
    rows=[json.loads(x) for x in (folder/f'exceptions-{game.pid}.jsonl').read_text().splitlines()]
    attached=any(r.get('event')=='attached' for r in rows)
    unhandled=summary['unhandled_exceptions'];first=summary['first_chance_exceptions']
    assert worker.returncode==0 and attached,(mode,worker.returncode,rows)
    assert (folder/'resources.jsonl').stat().st_size>0
    if mode=='crash':
        assert len(unhandled)==1 and unhandled[0]['code']=='0xc0000005',summary
        assert unhandled[0]['access']=='write' and unhandled[0]['fault_address']=='0x42',unhandled
        full=[d for d in folder.glob('unhandled-*.dmp')];assert len(full)==1
        analysis=json.loads(full[0].with_suffix('.analysis.json').read_text());assert 9 in analysis['streams'] and analysis['exception']['code']=='0xc0000005',analysis
    else:assert not unhandled and game.returncode==0,(mode,summary)
    if mode=='handled':assert len(first)==1,summary
    if mode=='normal':assert not first,summary
    results.append({'mode':mode,'pass':True,'target_exit':game.returncode,'worker_exit':worker.returncode,'attached':attached,'first_chance':len(first),'unhandled':len(unhandled),'dumps':summary['dumps']})
    print(json.dumps(results[-1]),flush=True)
# Explicit stop must detach without terminating the target.
folder=root/'fixture-results'/'final'/'detach';folder.mkdir(parents=True,exist_ok=True)
with (folder/'fixture.log').open('w') as output:
    game=hidden([root/'native'/'diagnostics-fixture.exe','normal'],output)
    debug=hidden([root/'native'/'crash-recorder.exe','--fixture',game.pid,0,folder],output)
    time.sleep(.5);(folder/'crash.stop').touch();debug.wait(timeout=5);assert game.poll() is None
    game.wait(timeout=10);assert game.returncode==0 and debug.returncode==0
results.append({'mode':'safe_detach','pass':True})
(root/'verification.json').write_text(json.dumps({'tests':results,'sensors':'PASS','game_launched':False},indent=2));print('ALL PASS',flush=True)
