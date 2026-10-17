"""Control the local recorder. Stopping/uninstalling retains all diagnostic evidence."""
import argparse,json,os,subprocess,sys,time,winreg
from pathlib import Path
from session import hidden,write_json
ROOT=Path(__file__).resolve().parent
RUN=r'Software\Microsoft\Windows\CurrentVersion\Run';NAME='Shogun2CrashRecorder'
def stop():
    (ROOT/'watcher.stop').touch()
    for _ in range(80):
        try:
            data=json.loads((ROOT/'watcher-health.json').read_text())
            if data.get('state')=='stopped':return data
        except Exception:pass
        time.sleep(.1)
    return {'state':'stop_requested'}
def start():
    (ROOT/'watcher.stop').unlink(missing_ok=True)
    with (ROOT/'bootstrap.log').open('a') as output:
        child=hidden([ROOT/'runtime'/'pythonw.exe',ROOT/'watcher.py'],output)
    for _ in range(80):
        try:
            data=json.loads((ROOT/'watcher-health.json').read_text())
            if data.get('state')=='armed' and time.time()-data.get('unix',0)<10:return data
        except Exception:pass
        time.sleep(.1)
    raise RuntimeError('Watcher did not confirm readiness; inspect bootstrap.log')
if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('action',choices=['start','stop','status','uninstall']);o=a.parse_args()
    if o.action=='start':print(json.dumps(start(),indent=2))
    elif o.action=='stop':print(json.dumps(stop(),indent=2))
    elif o.action=='status':print((ROOT/'watcher-health.json').read_text())
    else:
        state=stop();receipt=json.loads((ROOT/'installation.json').read_text())
        with winreg.CreateKey(winreg.HKEY_CURRENT_USER,RUN) as key:
            try:current,_=winreg.QueryValueEx(key,NAME)
            except FileNotFoundError:current=None
            if current==receipt['autorun_command']:
                old=receipt.get('prior_autorun')
                if old:winreg.SetValueEx(key,NAME,0,old['type'],old['value'])
                else:winreg.DeleteValue(key,NAME)
        print(json.dumps({'autorun_removed':True,'watcher':state,'evidence_retained':True},indent=2))
