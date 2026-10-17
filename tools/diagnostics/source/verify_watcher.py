from pathlib import Path
import json,subprocess,time,shutil,sys,os
root=Path('work/auto-recorder').resolve();install=Path(r'C:\Users\logsa\Documents\Codex\Shogun2Diagnostics')
sys.path.insert(0,str(root));from session import hidden
health=json.loads((install/'watcher-health.json').read_text());assert health['state']=='armed' and time.time()-health['unix']<10
# Exact-name detection uses a compiled fixture in the test folder, never the Steam game binary.
fixture=root/'native'/'shogun2.retail.exe';shutil.copy2(root/'native'/'diagnostics-fixture.exe',fixture)
records=Path(json.loads((install/'config.json').read_text())['recordings']);prior=set(records.iterdir())
with (root/'watcher-integration.log').open('w') as output:
    target=hidden([fixture,'crash'],output);target.wait(timeout=25)
for _ in range(120):
    folders=[p for p in records.iterdir() if p not in prior and p.is_dir()]
    finished=[p for p in folders if (p/'session-summary.json').exists()]
    if finished:break
    time.sleep(.25)
assert finished,'Automatic watcher did not finalize the controlled fixture'
folder=finished[0];summary=json.loads((folder/'session-summary.json').read_text())
assert summary['unhandled_exceptions'] and summary['unhandled_exceptions'][0]['code']=='0xc0000005',summary
assert list(folder.glob('unhandled-*.dmp'))
assert (folder/'gpu-device.jsonl').stat().st_size and (folder/'resources.jsonl').stat().st_size
resources=[json.loads(x) for x in (folder/'resources.jsonl').read_text().splitlines()]
assert any(r.get('pdh',{}).get('cpu_core_percent') for r in resources)
assert any(r.get('pdh',{}).get('cpu_frequency_MHz') for r in resources)
assert (folder/'game-configuration'/'pack-inventory.json').exists()
# Keep controlled fixture evidence separate from future genuine sessions.
dest=root/'fixture-results'/'automatic-watcher-repeat';shutil.copytree(folder,dest,dirs_exist_ok=True)
(folder/'CONTROLLED-FIXTURE.txt').write_text('Controlled recorder verification. This was not a game crash.')
receipt={'pass':True,'fixture_pid':target.pid,'full_dump':True,'cpu_ram_gpu':True,'configuration_capture':True,'game_launched':False,'startup_armed':True,'evidence':str(dest)}
(root/'watcher-integration.json').write_text(json.dumps(receipt,indent=2));print(json.dumps(receipt,indent=2))
