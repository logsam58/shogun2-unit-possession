"""Build the preserved x86 native mod and optional offline fixtures. Never install/launch the game."""
import argparse,hashlib,json,os,shutil,subprocess,sys,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];NATIVE=ROOT/'native';OUT=ROOT/'artifacts';BIN=OUT/'bin';OBJ=OUT/'obj';TEST=OUT/'tests';LOG=OUT/'logs'
ADAPTER=NATIVE/'outputs/shogun2-accuracy90-preview';QTE=NATIVE/'outputs/shogun2-authentic-hud80-preview';HUD=NATIVE/'outputs/shogun2-ammunition83-preview';AUTO=NATIVE/'work/autoload99-preview';INSTALL=NATIVE/'work/installer-siege99'
COMMON=['/nologo','/std:c++17','/EHsc','/O2','/MT','/W4'];BASE_LIBS=['bcrypt.lib','user32.lib','gdi32.lib'];ADAPTER_LIBS=BASE_LIBS+['d3d11.lib','d3d9.lib','dxgi.lib','d3dcompiler.lib'];QTE_LIBS=BASE_LIBS+['gdiplus.lib','ole32.lib'];SETUP_LIBS=['bcrypt.lib','shell32.lib','advapi32.lib','comdlg32.lib','user32.lib','gdi32.lib']
receipts=[];hooks=[];CANDIDATE=None;RELEASE=False
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def quote(command):
 for x in command:
  if any(c in str(x) for c in '\r\n%&|<>^!'):raise ValueError('Unsupported command path/argument')
 return subprocess.list2cmdline([str(x) for x in command])
def run(label,commands,cwd=ROOT):
 batch=LOG/(label+'.cmd');log=LOG/(label+'.log');lines=['@echo off','setlocal','call '+quote([VCVARS]),'if not "%errorlevel%"=="0" exit /b %errorlevel%']
 for command in commands:lines.extend([quote(command),'if not "%errorlevel%"=="0" exit /b %errorlevel%'])
 lines.append('exit /b 0');batch.write_text('\r\n'.join(lines)+'\r\n');started=time.time();print('Running '+label+' ...',flush=True)
 with log.open('w') as stream:r=subprocess.run([os.environ.get('COMSPEC','cmd.exe'),'/d','/c',str(batch)],cwd=cwd,stdout=stream,stderr=subprocess.STDOUT)
 fault='Fixture fault code=' in log.read_text(errors='replace')
 receipt={'name':label,'passed':r.returncode==0 and not fault,'seconds':round(time.time()-started,3),'log':str(log.relative_to(ROOT))};receipts.append(receipt)
 (OUT/'build-results.json').write_text(json.dumps({'results':receipts,'game_installed_or_launched':False},indent=2)+'\n')
 if r.returncode or fault:
  print(log.read_text(errors='replace')[-6500:]);raise SystemExit(r.returncode or 1)
 print('PASS '+label,flush=True)
def compile_cpp(source,target,extra=(),libs=(),options=()):
 return ['cl',*COMMON,*options,str(source),*map(str,extra),'/Fo'+str(OBJ/(target.stem+'.obj')),'/Fe'+str(target),'/link',*libs]
def minhook():
 global hooks
 if hooks:return
 vendor=ADAPTER/'vendor';commands=[]
 for file in ['buffer.c','hook.c','trampoline.c','hde/hde32.c']:
  source=vendor/'src'/file;obj=OBJ/(source.stem+'.obj');hooks.append(obj);commands.append(['cl','/nologo','/O2','/MT','/W3','/I'+str(vendor/'include'),'/c',str(source),'/Fo'+str(obj)])
 run('minhook',commands)
def rc(source,name):
 # RC resolves its resource paths relative to its working directory. Use a wrapper batch with an explicit directory.
 target=OBJ/(name+'.res');label=name+'-resources';batch=LOG/(label+'.cmd');log=LOG/(label+'.log')
 batch.write_text('@echo off\ncall '+quote([VCVARS])+'\nif errorlevel 1 exit /b %errorlevel%\ncd /d '+quote([source.parent])+'\nrc /nologo /fo '+quote([target,source.name])+'\nexit /b %errorlevel%\n')
 with log.open('w') as stream:r=subprocess.run([os.environ.get('COMSPEC','cmd.exe'),'/d','/c',str(batch)],cwd=ROOT,stdout=stream,stderr=subprocess.STDOUT)
 if r.returncode:print(log.read_text(errors='replace'));raise SystemExit(r.returncode)
 return target
def adapter(tests):
 minhook();run('adapter',[compile_cpp(ADAPTER/'src/adapter.cpp',BIN/'possession-v46.dll',hooks,ADAPTER_LIBS,['/LD']),compile_cpp(ADAPTER/'src/launcher.cpp',BIN/'possession.exe',(),BASE_LIBS)])
 if tests:
  commands=[];sources=[ADAPTER/'src/verify.cpp',*sorted((ADAPTER/'src').glob('verify_*.cpp'))]
  for source in sources:
    target=TEST/(source.stem+'.exe');commands.extend([compile_cpp(source,target,hooks,ADAPTER_LIBS),[target]])
  run('adapter-tests',commands);receipts[-1]['executables']=len(sources)
def qte(tests):
 minhook()
 required=json.loads((ROOT/'tools/local-assets.json').read_text())['files'];missing=[i['path'] for i in required if not (ROOT/i['path']).exists()]
 if missing:raise SystemExit('Missing local game UI inputs. Run tools/configure.py --asset-source <preserved root>. First: '+missing[0])
 resources=rc(QTE/'qte-src/qte-assets.rc','qte');run('qte',[compile_cpp(QTE/'qte-src/playable_runtime.cpp',BIN/'qte-playable-v79.dll',[resources,*hooks],QTE_LIBS,['/LD'])])
 if tests:
  commands=[];sources=sorted((QTE/'qte-src').glob('verify_*.cpp'))
  for source in sources:
   target=TEST/('qte-'+source.stem+'.exe');commands.extend([compile_cpp(source,target,[resources,*hooks],QTE_LIBS),[target]])
  run('qte-tests',commands);receipts[-1]['executables']=len(sources)
def hud(tests):
 spec=json.loads((ROOT/'tools/audio109.json').read_text());voice=ROOT/spec['wave_path']
 if not voice.is_file() or sha(voice)!=spec['wave_sha256']:raise SystemExit('Missing/corrupt local voice dependency: run tools/prepare_audio109.py --game <game> --ffmpeg <ffmpeg.exe>')
 resources=rc(HUD/'hud-src/hud-assets.rc','hud');run('hud',[compile_cpp(HUD/'hud-src/hud.cpp',BIN/'possession-hud.exe',[resources],QTE_LIBS+['winmm.lib'])])
 if tests:
  commands=[];sources=sorted((HUD/'hud-src').glob('verify_hud_*.cpp'))
  for source in sources:
   target=TEST/(source.stem+'.exe');audio=source.stem=='verify_hud_audio109';commands.extend([compile_cpp(source,target,[resources] if audio else (),BASE_LIBS),[target]])
  run('hud-tests',commands);receipts[-1]['executables']=len(sources)
def loader(tests):
 payload=NATIVE/'outputs/shogun2-siege99-preview';payload.mkdir(parents=True,exist_ok=True)
 for name in ['possession-v46.dll','possession-hud.exe','qte-playable-v79.dll']:
  source=BIN/name
  if not source.is_file():raise SystemExit('Build adapter, qte and hud first: '+name)
  shutil.copy2(source,payload/name)
 subprocess.run([sys.executable,str(AUTO/'prepare_embedded.py'),*(['--release'] if RELEASE else ['--candidate',str(CANDIDATE)])],check=True)
 resources=rc(AUTO/'payload.rc','loader');run('loader',[compile_cpp(AUTO/'bootstrap.cpp',BIN/'dinput8.dll',[resources],['/DEF:'+str(AUTO/'dinput8.def'),'bcrypt.lib','shell32.lib'],['/LD'])]);shutil.copy2(BIN/'dinput8.dll',AUTO/'dinput8.dll');shutil.copy2(AUTO/'marker.pack',BIN/'marker.pack')
 (AUTO/'setup_hashes.h').write_text('#pragma once\nstatic const char* SETUP_BOOTSTRAP_SHA="'+sha(BIN/'dinput8.dll')+'";\n')
 if tests:
  a=BIN/'verify_bootstrap.exe';b=BIN/'verify_integrated.exe';run('loader-tests',[compile_cpp(AUTO/'verify_bootstrap.cpp',a,(),['dinput8.lib']),[a],compile_cpp(AUTO/'verify_integrated.cpp',b,[resources],['bcrypt.lib','shell32.lib']),[b]])
def installer(tests):
 if not (AUTO/'setup_hashes.h').exists():raise SystemExit('Build loader first')
 baseline=INSTALL/'previous_cache_catalog.h';text=baseline.read_text();assert text.rstrip().endswith('};');hashes=(AUTO/'embedded_hashes.h').read_text();import re
 cache=re.search(r'EMBEDDED_BUILD=L"([a-f0-9]+)"',hashes).group(1);entry='{L"'+cache+'",{'+','.join('{L"'+n+'","'+sha(BIN/n)+'"}' for n in ['possession-v46.dll','possession-hud.exe','qte-playable-v79.dll'])+'}},\n';(INSTALL/'cache_catalog.h').write_text(text.rstrip()[:-2]+entry+'};\n')
 resources=rc(INSTALL/'setup.rc','installer');wizard=BIN/'Shogun2-Possession-Setup.exe';cli=BIN/'install_cli.exe'
 run('installer',[compile_cpp(INSTALL/'wizard.cpp',wizard,[resources],['/MANIFEST:EMBED','/SUBSYSTEM:WINDOWS',"/MANIFESTUAC:level='requireAdministrator' uiAccess='false'",*SETUP_LIBS]),compile_cpp(INSTALL/'install_cli.cpp',cli,[resources],['/SUBSYSTEM:CONSOLE','/ENTRY:wmainCRTStartup','/MANIFEST:EMBED',"/MANIFESTUAC:level='asInvoker' uiAccess='false'",*SETUP_LIBS])])
 if tests:
  required=['work/autoload-test99-baseline/dinput8.dll','work/autoload-test99-baseline/marker.pack','work/autoload87-preview/dinput8.dll','outputs/shogun2-melee-meter78-preview/possession-v46.dll','outputs/shogun2-melee-meter78-preview/qte-playable-v78.dll','outputs/shogun2-possession-hud-v12/possession-hud.exe']
  if any(not (NATIVE/p).exists() for p in required):raise SystemExit('Installer upgrade tests require local historical fixtures: tools/configure.py --legacy-fixtures <preserved root>')
  target=INSTALL/'verify.exe';run('installer-tests',[compile_cpp(INSTALL/'verify.cpp',target,[resources],['/MANIFEST:EMBED',"/MANIFESTUAC:level='asInvoker' uiAccess='false'",*SETUP_LIBS]),[target]],cwd=INSTALL)
def find_vcvars():
 config=ROOT/'tools/local.json';settings=json.loads(config.read_text()) if config.exists() else {};p=os.environ.get('SHOGUN2_VCVARS',settings.get('vcvars',''))
 if p and Path(p).is_file():return Path(p)
 vswhere=Path(os.environ.get('ProgramFiles(x86)','C:/Program Files (x86)'))/'Microsoft Visual Studio/Installer/vswhere.exe'
 if vswhere.exists():
  result=subprocess.check_output([str(vswhere),'-latest','-products','*','-requires','Microsoft.VisualStudio.Component.VC.Tools.x86.x64','-property','installationPath'],text=True).strip();p=Path(result)/'VC/Auxiliary/Build/vcvars32.bat'
  if p.is_file():return p
 raise SystemExit('Set SHOGUN2_VCVARS to vcvars32.bat or run tools/configure.py --vcvars <path>.')
def main():
 global VCVARS,CANDIDATE,RELEASE
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('target',choices=['adapter','qte','hud','loader','installer','all']);parser.add_argument('--tests',action='store_true');mode=parser.add_mutually_exclusive_group();mode.add_argument('--candidate',type=int,default=149,help='Distinct unpublished Test number (100 or later)');mode.add_argument('--release',action='store_true',help='Package v0.27/Test149 gameplay with release metadata; does not publish or install');args=parser.parse_args();CANDIDATE=args.candidate;RELEASE=args.release
 if CANDIDATE is not None and CANDIDATE<100:parser.error('--candidate must be 100 or later')
 VCVARS=find_vcvars()
 for p in [BIN,OBJ,TEST,LOG]:p.mkdir(parents=True,exist_ok=True)
 targets=['adapter','qte','hud','loader','installer'] if args.target=='all' else [args.target]
 for target in targets:globals()[target](args.tests)
 binaries=[{'file':str(p.relative_to(ROOT)),'bytes':p.stat().st_size,'sha256':sha(p)} for p in BIN.iterdir() if p.suffix in {'.dll','.exe','.pack'}]
 (OUT/'build-results.json').write_text(json.dumps({'candidate':CANDIDATE,'development_candidate':not RELEASE,'results':receipts,'binaries':binaries,'game_installed_or_launched':False,'rebuilt_payload_is_not_the_installed_hash_identity':True},indent=2)+'\n');print('Build complete; outputs are in artifacts/bin. No installer or game was launched.')
if __name__=='__main__':main()
