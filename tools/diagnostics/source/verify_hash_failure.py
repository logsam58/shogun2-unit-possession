import json,subprocess,sys,time,shutil
from pathlib import Path
root=Path(__file__).resolve().parent;sys.path.insert(0,str(root));import session
fixture=root/'native'/'diagnostics-fixture.exe';folder=root/'fixture-results'/'hash-read-error';folder.mkdir(parents=True,exist_ok=True)
real=session.modules
session.modules=lambda pid: real(pid)+[{'name':'unreadable-fixture.dll','path':str(root/'intentionally-absent.dll'),'base':0x70000000,'size':4096}]
with (folder/'fixture.log').open('w') as output:
    target=session.hidden([fixture,'normal'],output)
    session.session(target.pid,folder,fixture=True);target.wait(timeout=5)
rows=[json.loads(x) for x in (folder/'module-inventory.jsonl').read_text().splitlines()]
mods=next(r['modules'] for r in rows if r.get('event')=='module_inventory')
assert any(m.get('hash_error') for m in mods if m['name']=='unreadable-fixture.dll')
assert any(m.get('sha256') for m in mods if m['name']=='diagnostics-fixture.exe')
assert (folder/'resources.jsonl').stat().st_size and target.returncode==0
(root/'hash-failure-verification.json').write_text(json.dumps({'pass':True,'unreadable_module_explicit':True,'other_module_hashes_recorded':True,'core_recording_continued':True,'game_launched':False},indent=2))
print('Unreadable-module isolation PASS')
