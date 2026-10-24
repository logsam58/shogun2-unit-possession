"""Build a PRIVATE inert Workshop-entry probe. Never install, launch or upload.

The output contains a locally copied original menu script. It is a diagnostic
input only, NOT the asset-free public Workshop package or a gameplay build.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys

HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from game_packs import Pack, digest, require_game

STOCK_SHA='8839d40e12e0141f6a5d967f278cc584c488e8a8527324b20c248412f5a3ffd1'
ROOT_SHA='255b305cefb24a68cf6d2fc79d07efad7c0252acfd98ecd3ad10fa55c959cae8'

def encode(data):
    return ',\n'.join('"'+''.join('\\%03d'%b for b in data[i:i+1024])+'"' for i in range(0,len(data),1024))

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--game',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--vcvars',type=Path,required=True)
    p.add_argument('--lua',type=Path,required=True,help='Preserved matching float Lua 5.1 interpreter')
    p.add_argument('--luac',type=Path,required=True,help='Preserved matching float Lua 5.1 compiler')
    p.add_argument('--entry',choices=['legacy-frontend','ui-root'],default='legacy-frontend')
    a=p.parse_args();game=a.game.resolve();out=a.output.resolve()
    require_game(game)
    if out.exists() or out==game or out.is_relative_to(game) or HERE.is_relative_to(out):
        p.error('Use a NEW separate output directory outside the game and source ancestors')
    for tool in [a.vcvars,a.lua,a.luac]:
        if not tool.is_file():p.error('Missing required tool: '+str(tool))
    out.mkdir(parents=True)
    source_names=['probe164.cpp','probe164.lua','build_probe164.py','ui_wrapper164.lua','verify_ui_wrapper164.lua']
    for name in source_names:
        (out/name).write_bytes((HERE/name).read_bytes())
    def command(args):
        for item in args:
            if any(x in str(item) for x in '\r\n%&|<>^!'):raise ValueError('Unsafe batch argument')
        return subprocess.list2cmdline(list(map(str,args)))
    dll=out/'s2up-workshop-probe164.dll'
    batch=out/'compile.cmd'
    batch.write_text('@echo off\ncall '+command([a.vcvars.resolve()])+'\nif errorlevel 1 exit /b %errorlevel%\n'+
        command(['cl','/nologo','/std:c++17','/EHsc','/O2','/MT','/W4','/LD',out/'probe164.cpp','/Fo'+str(out/'probe164.obj'),'/Fe'+str(dll),'/link','bcrypt.lib'])+
        '\nexit /b %errorlevel%\n')
    with (out/'compile.log').open('w') as log:
        subprocess.run([os.environ.get('COMSPEC','cmd.exe'),'/d','/c',batch],cwd=out,stdout=log,stderr=subprocess.STDOUT,check=True)
    payload=dll.read_bytes();sha=hashlib.sha256(payload).hexdigest()
    appendix=(HERE/'probe164.lua').read_text().replace('-- __NATIVE_PAYLOAD164__',encode(payload)).replace('__NATIVE_SHA164__',sha)
    (out/'appendix.lua').write_text(appendix,encoding='utf-8')
    pack=Pack(game/'data/data.pack')
    ui=a.entry=='ui-root'
    entry='ui/frontend ui/layout_scripts/root.luac' if ui else 'frontend_scripted.lua'
    expected_sha=ROOT_SHA if ui else STOCK_SHA
    stock=pack.read(entry,13739 if ui else 99281,expected_sha)
    if ui:
        source=(HERE/'ui_wrapper164.lua').read_text().replace('-- __ORIGINAL_CHUNK164__',encode(stock)).replace('-- __PROBE_INITIALIZER164__',appendix).encode()
    else:source=stock+b'\n'+appendix.encode('utf-8')
    script=out/'probe-script.lua';script.write_bytes(source)
    for path in [script,out/'appendix.lua']:
        subprocess.run([a.luac.resolve(),'-p',path],cwd=out,check=True)
    # Run actual package.loadlib and the worker under matching standalone Lua.
    # This is a non-game host; the expected receipt MUST reject game eligibility.
    cache=out/'isolated-localappdata';cache.mkdir()
    env=dict(os.environ,LOCALAPPDATA=str(cache))
    delivered=source
    if ui:
        compiled=out/'root-wrapper.luac'
        subprocess.run([a.luac.resolve(),'-s','-o',compiled,script],cwd=out,check=True)
        delivered=compiled.read_bytes()
        if delivered[:12]!=bytes.fromhex('1b4c75615100010404040400'):raise ValueError('Wrapper bytecode is not matching float Lua 5.1')
        reference=out/'owned-root.luac';reference.write_bytes(stock)
        ui_check=subprocess.run([a.lua.resolve(),out/'verify_ui_wrapper164.lua',reference,compiled],cwd=out,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        (out/'ui-wrapper-tests.log').write_bytes(ui_check.stdout)
        if ui_check.returncode:raise ValueError(ui_check.stdout.decode(errors='replace'))
    harness=out/'verify.lua'
    escaped=str(out/'appendix.lua').replace('\\','/')
    harness.write_text('dofile('+json.dumps(escaped)+')\ndofile('+json.dumps(escaped)+')\nos.execute("ping -n 2 127.0.0.1 >nul")\n')
    subprocess.run([a.lua.resolve(),harness],cwd=out,env=env,check=True)
    receipts=list(cache.glob('*.json'))
    if len(receipts)!=1:raise ValueError('Native probe did not produce exactly one receipt')
    native=json.loads(receipts[0].read_text())
    if not native['native_entry'] or native['supported_game_host'] or native['hooks_installed'] or native['gameplay_loaded']:
        raise ValueError('Inert native probe fixture violated its boundary')
    # Exercise protected failure without an available native loader or writable cache.
    fault=out/'verify-refusal.lua'
    fault.write_text('local saved=io.open\nio.open=function() return nil,"fixture denied" end\n'+
        'assert(pcall(dofile,'+json.dumps(escaped)+'))\nio.open=saved\npackage.loadlib=nil\n'+
        'assert(pcall(dofile,'+json.dumps(escaped)+'))\n')
    subprocess.run([a.lua.resolve(),fault],cwd=out,env=env,check=True)
    # Unknown pre-existing cache stays byte-exact and is never loaded.
    collision=out/'collision-localappdata';collision.mkdir()
    unknown=collision/('S2UP-Workshop-Probe164-'+sha+'.dll');unknown.write_bytes(b'foreign fixture file')
    collision_env=dict(os.environ,LOCALAPPDATA=str(collision))
    subprocess.run([a.lua.resolve(),harness],cwd=out,env=collision_env,check=True)
    if unknown.read_bytes()!=b'foreign fixture file' or list(collision.glob('*.json')):
        raise ValueError('Foreign cache file was changed or loaded')
    entries={entry:delivered,'possession/workshop164.json':json.dumps({
        'test':164,'private_diagnostic_only':True,'gameplay':False,
        'stock_script_from_owned_game':True,'not_for_upload':True}).encode()}
    # The stock engine uses backslashes in the raw PFH index. Our independent
    # reader normalizes separators, so its roundtrip alone cannot validate this.
    index=b''.join(struct.pack('<I',len(value))+key.replace('/','\\').encode()+b'\0' for key,value in entries.items())
    # Zero timestamps compare equal to a missing loose file in the native VFS;
    # that path can return an empty stream instead of the registered archive.
    with (game/'data/data.pack').open('rb') as stock_stream:
        stock_header=stock_stream.read(32)
    stamp=struct.unpack_from('<Q',stock_header,24)[0]
    if not stamp:raise ValueError('Owned-stock archive timestamp must be nonzero')
    target=out/'shogun2_unit_possession_entry_probe164.pack'
    target.write_bytes(struct.pack('<4s5IQ',b'PFH3',3,0,0,len(entries),len(index),stamp)+index+b''.join(entries.values()))
    reread=Pack(target)
    for key,value in entries.items():
        if reread.read(key,len(value),hashlib.sha256(value).hexdigest())!=value:raise ValueError('Pack roundtrip failed')
    receipt={'test':164,'status':'OFFLINE PROBE CHECKS PASS; NO LIVE ENTRY PROOF',
        'pack':target.name,'pack_sha256':digest(target),'native_sha256':sha,
        'native_bytes':len(payload),'entry':entry,'original_script_bytes':len(stock),'original_script_sha256':expected_sha,
        'pack_timestamp':stamp,
        'stock_prefix_unchanged':not ui and source[:len(stock)]==stock,'coreutils_overridden':False,
        'ui_root_callbacks_byte_exact':ui,'ui_root_environment_and_ordering_checks_passed':ui,
        'sources':{name:digest(out/name) for name in source_names},
        'lua_sha256':digest(a.lua),'luac_sha256':digest(a.luac),
        'fresh_checks':['x86 native compile','matching float Lua syntax','actual loadlib plus worker receipt',
            'repeated entry starts once','non-game host rejected','denied writes and missing loader contained',
            'foreign cache preserved','all37 root callbacks/environment/order/failure' if ui else 'stock prefix exact','PFH3 roundtrip'],
        'fixture_native_receipt':native,'game_changed':False,'steam_changed':False,'game_launched':False,
        'uploaded':False,'contains_local_copyrighted_script':True,
        'public_package_ready':False,'warning':'PRIVATE diagnostic pack. Do not upload or distribute. Not a gameplay build.'}
    (out/'verification.json').write_text(json.dumps(receipt,indent=2)+'\n')
    print(json.dumps(receipt,indent=2))

if __name__=='__main__':main()
