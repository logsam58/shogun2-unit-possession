"""Update four diagnostic files for future launches; retain the existing watcher.

Requires an idle, healthy installation and explicit existing baseline/config.
No game launch, watcher restart, autorun edit or baseline rewrite.
"""
import argparse,hashlib,json,os,shutil,subprocess,sys,time
from pathlib import Path
SOURCE=Path(__file__).resolve().parent
sys.path.insert(0,str(SOURCE))
from process_probe import processes,modules
from session import creation
from binary_policy111 import require_capture_only111

def sha(p):
    with p.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()

def main():
    p=argparse.ArgumentParser();p.add_argument('--installed-root',type=Path,required=True);p.add_argument('--built-recorder',type=Path,required=True);p.add_argument('--evidence',type=Path,required=True);o=p.parse_args()
    # Run before any evidence-directory or installation mutation. Runtime flags
    # cannot make an experimental executable suitable for automatic deployment.
    require_capture_only111(o.built_recorder)
    root=o.installed_root.resolve();o.evidence.mkdir(parents=True,exist_ok=True)
    health=json.loads((root/'watcher-health.json').read_text())
    assert health['state']=='armed' and not health['active_sessions'] and time.time()-health['unix']<10
    assert not processes(),'A real game is active; deployment waits for an idle recorder.'
    watcher=modules(health['pid'])[0]
    assert os.path.normcase(watcher['path'])==os.path.normcase(str(root/'runtime/pythonw.exe'))
    born=creation(health['pid'])/1e7-11644473600
    assert abs(born-health['started_unix'])<3
    baseline=(root/'baseline.json').read_bytes();config=(root/'config.json').read_bytes()
    targets=[(SOURCE/'session.py',root/'session.py'),(SOURCE/'lifecycle110.py',root/'lifecycle110.py'),(SOURCE/'events.ps1',root/'events.ps1'),(o.built_recorder,root/'native/crash-recorder.exe')]
    before=[];replaced=[];backup=o.evidence/'rollback';backup.mkdir(exist_ok=False)
    try:
        for src,dest in targets:
            assert src.is_file()
            relative=dest.relative_to(root);old=backup/relative
            if dest.exists():old.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(dest,old)
            before.append({'file':str(relative),'existed':dest.exists(),'sha256':sha(dest) if dest.exists() else None})
            temp=dest.with_name(dest.name+'.diagnostics110.tmp');shutil.copy2(src,temp)
            assert sha(temp)==sha(src);os.replace(temp,dest);replaced.append((dest,old))
        check=subprocess.run([str(root/'runtime/python.exe'),'-c','import session,lifecycle110; print("Diagnostic imports PASS")'],cwd=root,capture_output=True,text=True,creationflags=0x08000000,timeout=10)
        assert check.returncode==0,check.stderr
        assert (root/'baseline.json').read_bytes()==baseline and (root/'config.json').read_bytes()==config
        assert not processes(),'Game launched during deployment; stop and verify installed diagnostic generation before play.'
    except Exception:
        for dest,old in reversed(replaced):
            if old.exists():shutil.copy2(old,dest)
            elif dest.exists():dest.unlink()
        raise
    receipt={'version':110,'unix':time.time(),'installed_root':str(root),'files':[{'file':str(dest.relative_to(root)),'sha256':sha(dest),'bytes':dest.stat().st_size} for src,dest in targets],
             'prior':before,'watcher_pid':health['pid'],'watcher_creation_unix':born,'watcher_restarted':False,'duplicate_recorder_started':False,
             'baseline_unchanged':True,'config_unchanged':True,'game_launched':False,'applies_to':'Next normally detected game launch; no live acceptance yet','import_check':check.stdout.strip()}
    path=o.evidence/'installation110.json';path.write_bytes((json.dumps(receipt,indent=2)+'\n').encode());print(json.dumps(receipt,indent=2))

if __name__=='__main__':main()
