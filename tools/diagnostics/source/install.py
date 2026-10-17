"""Install a self-contained, hidden per-user recorder and enable it at logon."""
import hashlib,json,os,shutil,subprocess,sys,time,winreg
from pathlib import Path
root=Path(__file__).resolve().parent
install=Path(r'C:\Users\logsa\Documents\Codex\Shogun2Diagnostics')
if install.exists() and not (install/'installation.json').exists():raise RuntimeError('Refusing to overwrite an unrelated installation directory')
install.mkdir(parents=True,exist_ok=True)
for name in ('process_probe.py','process_memory.py','native_input.py','inspect_minidump.py','sensors.py','events.ps1','session.py','watcher.py','control.py','config.json','baseline.json'):
    shutil.copy2(root/name,install/name)
(install/'native').mkdir(exist_ok=True);shutil.copy2(root/'native'/'crash-recorder.exe',install/'native'/'crash-recorder.exe')
# Private standard-library runtime survives updates/removal of the Codex dependency cache.
python=Path(sys.executable).parent;runtime=install/'runtime';runtime.mkdir(exist_ok=True)
for pattern in ('python*.exe','python*.dll','vcruntime*.dll','LICENSE.txt'):
    for p in python.glob(pattern):shutil.copy2(p,runtime/p.name)
ignore=shutil.ignore_patterns('site-packages','__pycache__','test','tests','idlelib','tkinter','turtledemo','ensurepip')
shutil.copytree(python/'Lib',runtime/'Lib',dirs_exist_ok=True,ignore=ignore)
shutil.copytree(python/'DLLs',runtime/'DLLs',dirs_exist_ok=True)
(runtime/'python312._pth').write_text('Lib\nDLLs\n..\n',encoding='ascii')
check=subprocess.run([str(runtime/'python.exe'),'-c','import ctypes,json,hashlib,subprocess,threading,session,sensors; print("Standalone runtime PASS")'],capture_output=True,text=True,creationflags=0x08000000)
if check.returncode:raise RuntimeError(check.stderr)
command='"'+str(runtime/'pythonw.exe')+'" "'+str(install/'watcher.py')+'"'
assert len(command)<260
run=r'Software\Microsoft\Windows\CurrentVersion\Run';name='Shogun2CrashRecorder';prior=None
with winreg.CreateKey(winreg.HKEY_CURRENT_USER,run) as key:
    try:old,kind=winreg.QueryValueEx(key,name);prior={'value':old,'type':kind}
    except FileNotFoundError:pass
    if prior and (install/'installation.json').exists():prior=json.loads((install/'installation.json').read_text()).get('prior_autorun')
    winreg.SetValueEx(key,name,0,winreg.REG_SZ,command)
    assert winreg.QueryValueEx(key,name)[0]==command
receipt={'installed_unix':time.time(),'root':str(install),'autorun_key':'HKCU\\'+run,'autorun_name':name,'autorun_command':command,'prior_autorun':prior,'self_contained_runtime':True,'game_files_changed':False,'game_launched':False,'crash_recorder_sha256':hashlib.sha256((install/'native'/'crash-recorder.exe').read_bytes()).hexdigest()}
(install/'installation.json').write_text(json.dumps(receipt,indent=2))
result=subprocess.run([str(runtime/'python.exe'),str(install/'control.py'),'start'],capture_output=True,text=True,creationflags=0x08000000,timeout=15)
if result.returncode:raise RuntimeError(result.stderr)
receipt['watcher_start']=json.loads(result.stdout);(install/'installation.json').write_text(json.dumps(receipt,indent=2));print(json.dumps(receipt,indent=2))
