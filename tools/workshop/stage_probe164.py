"""Temporarily stage/rollback the PRIVATE entry probe. Never launch/stop the game.

No action is implicit. Preparation writes a new journal outside the game; stage
requires --closed after the operator obtains a fresh human CLOSED confirmation.
"""
import argparse
import base64
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import time

sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from game_packs import require_game

NAME='shogun2_unit_possession_entry_probe164.pack'
LINE='mod '+NAME+';'
TEST=164
LEGACY_LOADER_SHA='19f7c9b197cb8128a160aa8bd7b4e930b2612f420a5468413690bea55e791ae7'

def sha(raw):return hashlib.sha256(raw).hexdigest()

def game_pids():
    result=subprocess.check_output(['powershell.exe','-NoProfile','-NonInteractive','-Command',
        "@(Get-Process -Name shogun2,shogun2.retail -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Id) | ConvertTo-Json -Compress"],text=True,creationflags=subprocess.CREATE_NO_WINDOW).strip()
    value=json.loads(result) if result else []
    return [value] if isinstance(value,int) else value

def closed():
    if game_pids():raise ValueError('Game is running; no file change allowed')

def plain(path):
    # Reject junction/symlink redirection in every existing ancestor.
    current=path.absolute()
    for part in [current,*current.parents]:
        if part.exists() and getattr(part.stat(follow_symlinks=False),'st_file_attributes',0)&0x400:
            raise ValueError('Reparse path refused: '+str(part))

def decode(raw):
    if raw.startswith(b'\xff\xfe'):return raw[2:].decode('utf-16-le'),'utf-16-le',b'\xff\xfe'
    if raw.startswith(b'\xfe\xff'):return raw[2:].decode('utf-16-be'),'utf-16-be',b'\xfe\xff'
    if raw.startswith(b'\xef\xbb\xbf'):return raw[3:].decode('utf-8'),'utf-8',b'\xef\xbb\xbf'
    if b'\0' in raw:raise ValueError('Unknown selection-file encoding; original preserved')
    try:return raw.decode('utf-8'),'utf-8',b''
    except UnicodeDecodeError:return raw.decode('mbcs'),'mbcs',b''

def atomic(path,raw):
    temp=path.with_name(path.name+'.probe164-'+str(os.getpid())+'.new')
    with temp.open('xb') as f:f.write(raw);f.flush();os.fsync(f.fileno())
    os.replace(temp,path)

def journal(path):return json.loads((path/'journal.json').read_text())
def save(path,value):atomic(path/'journal.json',(json.dumps(value,indent=2)+'\n').encode())

def legacy_paths(folder,j):
    game=Path(j['game']).absolute()
    target=game/'dinput8.dll';excluded=folder/'excluded-dinput8.dll'
    backup=folder/'previous-dinput8.dll'
    for path in [target,excluded,backup]:plain(path)
    if target.resolve().parent!=game.resolve() or excluded.resolve().parent!=folder.resolve():
        raise ValueError('Legacy-loader paths leave their exact intended directories')
    return target,excluded,backup

def legacy_state(folder,j):
    if not j.get('legacy_loader'):return None
    target,excluded,backup=legacy_paths(folder,j);expected=j['legacy_loader']['sha256']
    if expected!=LEGACY_LOADER_SHA or not backup.is_file() or sha(backup.read_bytes())!=expected:
        raise ValueError('Legacy-loader backup differs; all targets preserved')
    if target.exists() and (not target.is_file() or sha(target.read_bytes())!=expected):
        raise ValueError('Legacy-loader target has foreign edits; all targets preserved')
    if excluded.exists() and (not excluded.is_file() or sha(excluded.read_bytes())!=expected):
        raise ValueError('Excluded legacy loader has foreign edits; all targets preserved')
    if target.exists() and not excluded.exists():return 'original'
    if excluded.exists() and not target.exists():return 'excluded'
    raise ValueError('Ambiguous legacy-loader paths; all targets preserved')

def rollback(folder):
    closed();j=journal(folder)
    target=Path(j['game'])/'data'/NAME;selection=Path(j['selection'])
    plain(target);plain(selection)
    # Validate every owned object before changing either one.
    if target.exists() and sha(target.read_bytes())!=j['pack_sha256']:
        raise ValueError('Probe pack has external edits; preserved')
    now=selection.read_bytes() if selection.exists() else b''
    previous=base64.b64decode(j['previous_selection_base64'])
    if now==base64.b64decode(j['staged_selection_base64']):restored=previous
    elif now==previous:restored=now
    else:
        text,encoding,bom=decode(now);lines=text.splitlines(keepends=True)
        owned=[]
        for directive in j.get('owned_lines',[LINE]):
            matches=[i for i,line in enumerate(lines) if line.strip()==directive]
            if len(matches)!=1:raise ValueError('Selection changed ambiguously; preserved for manual review')
            owned.extend(matches)
        restored=bom+''.join(line for i,line in enumerate(lines) if i not in owned).encode(encoding)
    destination=folder/'retired-probe.pack'
    if target.exists() and destination.exists():raise ValueError('Retired probe destination exists; source preserved')
    loader_state=legacy_state(folder,j)
    if loader_state=='excluded':
        original,excluded,backup=legacy_paths(folder,j)
        # Windows rename refuses an existing destination; never replace a foreign DLL.
        excluded.rename(original)
    if now!=restored:atomic(selection,restored)
    if target.exists():
        shutil.move(str(target),str(destination))
    if loader_state:j['legacy_loader']['restored']=True
    j['rolled_back']=True;j['rollback_selection_sha256']=sha(selection.read_bytes() if selection.exists() else b'')
    save(folder,j);print('Probe rolled back. No game process was stopped.')

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('action',choices=['prepare','stage','rollback','watch'])
    p.add_argument('--journal',type=Path,required=True)
    p.add_argument('--probe',type=Path)
    p.add_argument('--verification',type=Path,help='Optional build receipt outside the distributable package folder')
    p.add_argument('--game',type=Path)
    p.add_argument('--selection',type=Path)
    p.add_argument('--closed',action='store_true')
    p.add_argument('--lua-logging',action='store_true',help='Temporary native Lua errors/output only; backed up and restored with owned directives')
    p.add_argument('--exclude-legacy-loader',action='store_true',help='Back up and temporarily exclude only the exact installed Test160 DInput loader')
    a=p.parse_args();folder=a.journal.absolute()
    if a.action=='prepare':
        if not all([a.game,a.probe,a.selection]):p.error('prepare requires --game --probe --selection')
        if folder.exists():p.error('Use a new journal directory')
        probe=a.probe.absolute();verification_path=a.verification or probe.parent/'verification.json'
        verification=json.loads(verification_path.read_text())
        if verification['test']!=TEST or verification['game_changed'] or verification['public_package_ready']:
            raise ValueError('Not the verified unpublished candidate')
        raw=probe.read_bytes()
        if sha(raw)!=verification['pack_sha256']:raise ValueError('Probe changed after offline checks')
        game=a.game.absolute();selection=a.selection.absolute();plain(game);plain(selection);plain(folder)
        if folder==game or folder.is_relative_to(game) or selection.is_relative_to(folder):
            raise ValueError('Journal must be separate from game and profile inputs')
        require_game(game)
        if (game/'data'/NAME).exists():raise ValueError('Target already exists; preserved')
        if not selection.is_file():raise ValueError('Existing selection file required for this private probe')
        previous=selection.read_bytes()
        if len(previous)>1024*1024:raise ValueError('Selection file exceeds bound')
        text,encoding,bom=decode(previous)
        if NAME.lower() in text.lower():raise ValueError('Existing probe selection preserved')
        newline='\r\n' if '\r\n' in text else '\n'
        if text and not text.endswith(('\r','\n')):text+=newline
        owned_lines=[LINE]
        if a.lua_logging:
            for directive in ['log_lua_errors_to_file;','log_lua_output_to_file;']:
                key=directive[:-1]
                matches=[line.strip() for line in text.splitlines() if line.strip().startswith(key)]
                if matches and matches!=[directive]:raise ValueError('Existing Lua logging directive differs; preserved')
                if not matches:owned_lines.append(directive)
        staged=bom+(text+''.join(line+newline for line in owned_lines)).encode(encoding)
        legacy=None;legacy_bytes=None
        if a.exclude_legacy_loader:
            legacy_target=game/'dinput8.dll';plain(legacy_target)
            if not legacy_target.is_file() or legacy_target.stat().st_size>4*1024*1024:
                raise ValueError('Expected bounded Test160 loader is absent; preserved')
            legacy_bytes=legacy_target.read_bytes()
            if sha(legacy_bytes)!=LEGACY_LOADER_SHA:raise ValueError('Unrecognized DInput loader; preserved')
            legacy={'sha256':LEGACY_LOADER_SHA,'bytes':len(legacy_bytes),'restored':False}
        folder.mkdir(parents=True)
        (folder/'verified-probe.pack').write_bytes(raw)
        (folder/'previous-selection.bin').write_bytes(previous)
        if legacy_bytes is not None:
            with (folder/'previous-dinput8.dll').open('xb') as stream:
                stream.write(legacy_bytes);stream.flush();os.fsync(stream.fileno())
        log_backups=[]
        if a.lua_logging:
            logs=selection.parent.parent/'logs'
            if logs.exists():
                plain(logs)
                for source in sorted(logs.glob('lua*')):
                    if not source.is_file():continue
                    plain(source)
                    if source.stat().st_size>16*1024*1024:raise ValueError('Prior Lua log exceeds backup bound; preserved')
                    target=folder/('previous-'+source.name);log_bytes=source.read_bytes();target.write_bytes(log_bytes)
                    log_backups.append({'name':source.name,'sha256':sha(log_bytes),'bytes':len(log_bytes)})
        save(folder,{'schema':1,'test':TEST,'game':str(game),'selection':str(selection),
            'pack_sha256':sha(raw),'previous_selection_sha256':sha(previous),
            'previous_selection_base64':base64.b64encode(previous).decode(),
            'staged_selection_base64':base64.b64encode(staged).decode(),
            'owned_lines':owned_lines,'temporary_native_lua_logging':a.lua_logging,'prior_lua_logs_backed_up':log_backups,
            'legacy_loader':legacy,
            'game_targets_changed':False,'staged':False,'rolled_back':False})
        print('Prepared only. Game and selection are unchanged. Fresh CLOSED required for stage.')
    elif a.action=='stage':
        if not a.closed:p.error('stage needs --closed after fresh human confirmation')
        closed();j=journal(folder);target=Path(j['game'])/'data'/NAME;selection=Path(j['selection'])
        plain(target);plain(selection)
        if j['staged'] or j['rolled_back']:raise ValueError('Journal already used; prepare a new one')
        if sha(selection.read_bytes())!=j['previous_selection_sha256'] or target.exists():raise ValueError('Prepared inputs changed; preserved')
        raw=(folder/'verified-probe.pack').read_bytes()
        if sha(raw)!=j['pack_sha256']:raise ValueError('Prepared pack changed')
        loader_state=legacy_state(folder,j)
        if loader_state not in (None,'original'):raise ValueError('Legacy loader is already excluded; preserved')
        # Durable journal exists before the first real game write.
        try:
            if loader_state:
                original,excluded,backup=legacy_paths(folder,j)
                original.rename(excluded)
            with target.open('xb') as f:f.write(raw);f.flush();os.fsync(f.fileno())
            atomic(selection,base64.b64decode(j['staged_selection_base64']))
            j['staged']=True;j['game_targets_changed']=True;save(folder,j)
        except BaseException:
            rollback(folder);raise
        print('Private inert probe enabled. No game/Steam launch or termination.')
    elif a.action=='rollback':rollback(folder)
    else:
        # Owner invokes this only after staging. It never terminates a process.
        j=journal(folder)
        if not j['staged'] or j['rolled_back']:raise ValueError('No active staged probe')
        watched=[];deadline=time.monotonic()+900
        while time.monotonic()<deadline:
            watched=game_pids()
            if watched:break
            time.sleep(3)
        if watched:
            while game_pids():time.sleep(3)
        rollback(folder)

if __name__=='__main__':main()
