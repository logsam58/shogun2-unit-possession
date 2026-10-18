"""Exercise actual x86 processor breakpoints against a synthetic target only."""
import argparse,json,subprocess,time
from pathlib import Path

def wait_until(test, seconds=12):
    end=time.monotonic()+seconds
    while time.monotonic()<end:
        result=test()
        if result:return result
        time.sleep(.02)
    raise AssertionError('Synthetic trace readiness timed out')

def load(path):
    try:return json.loads(path.read_text())
    except (OSError,ValueError):return {}

def main():
    a=argparse.ArgumentParser();a.add_argument('--build-dir',type=Path,required=True);a.add_argument('--output-dir',type=Path,required=True);a.add_argument('--repeat-normal',type=int,default=0);o=a.parse_args()
    o.output_dir.mkdir(parents=True,exist_ok=True);results=[]
    for number,mode in enumerate(['normal','handled','crash','detach','foreign-step','budget']+['normal']*o.repeat_normal):
        folder=o.output_dir/(mode if number<6 else f'{mode}-{number}');folder.mkdir(exist_ok=False)
        with (folder/'console.log').open('w') as output:
            target=subprocess.Popen([str(o.build_dir/'trace-fixture110.exe'),str(folder),mode],stdout=output,stderr=subprocess.STDOUT,creationflags=0x08000000)
            wait_until(lambda:(folder/'targeted110-fixture.txt').exists())
            debug=subprocess.Popen([str(o.build_dir/'crash-recorder.exe'),'--fixture',str(target.pid),'0',str(folder)],stdout=output,stderr=subprocess.STDOUT,creationflags=0x08000000)
            wait_until(lambda:load(folder/'targeted110-health.json').get('armed_threads',0)>0)
            (folder/'trace-go').touch();wait_until(lambda:(folder/'actions.done').exists())
            if mode=='detach':
                (folder/'crash.stop').touch();debug.wait(timeout=8);assert target.poll() is None
                (folder/'after-detach').touch()
            target.wait(timeout=12);debug.wait(timeout=15)
        assert debug.returncode==0,(mode,(folder/'console.log').read_text())
        rows=[json.loads(x) for x in (folder/'targeted110.jsonl').read_text().splitlines()];events=[x['event'] for x in rows]
        for e in ['audio_interface_binding','effects_destructor_with_active_reference','effects_owner_word_write','audio_wrapper_pointer_write','audio_interface_word_write']:
            assert e in events,(mode,e,events)
        changes=[x for x in rows if x['event']=='audio_interface_word_write']
        assert any(x['vtable']=='0xd4b5ffe7' and not x['target_executable'] for x in changes),(mode,changes)
        assert len({x['thread'] for x in changes})>=2,(mode,'new-thread writes not captured')
        assert all(x.get('eip_is_after_write') and x.get('stack_words_not_unwound') for x in changes)
        exceptions=[json.loads(x) for x in (folder/f'exceptions-{target.pid}.jsonl').read_text().splitlines()]
        faults=[x for x in exceptions if x['event']=='exception']
        if mode=='crash':assert target.returncode==0xc0000005 and any(not x['first_chance'] and x['code']=='0xc0000005' for x in faults)
        else:assert target.returncode==0,(mode,target.returncode)
        if mode=='handled':assert len(faults)==1 and faults[0]['code']=='0xc0000005' and faults[0]['first_chance']
        if mode=='foreign-step':assert len(faults)==1 and faults[0]['code']=='0x80000004' and faults[0]['first_chance']
        if mode in ['normal','detach','budget']:assert not faults,(mode,faults)
        if mode=='detach':assert any(x['event']=='debug_registers_restored' and x['failures']==0 and x['threads']>0 for x in rows)
        if mode=='budget':assert 'targeted_trace_budget_reached' in events and load(folder/'targeted110-health.json')['budget_exhausted']
        results.append({'mode':mode,'passed':True,'target_pid':target.pid,'target_exit':target.returncode,'debugger_exit':debug.returncode,'hardware_events':len(rows),'observed_writer_threads':len({x['thread'] for x in changes}),'native_exceptions_forwarded':len(faults)})
        print(json.dumps(results[-1]),flush=True)
    (o.output_dir/'verification.json').write_text(json.dumps({'passed':True,'synthetic_only':True,'game_launched':False,'tests':results},indent=2))

if __name__=='__main__':main()
