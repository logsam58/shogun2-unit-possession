"""Build isolated capture and optional toy-only trace tools; never deploy/attach."""
import argparse,hashlib,json,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SOURCE=Path(__file__).resolve().parent/'source/native'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 p=argparse.ArgumentParser();p.add_argument('--vcvars',type=Path,required=True);p.add_argument('--output-dir',type=Path,required=True);p.add_argument('--experimental',action='store_true');o=p.parse_args()
 out=o.output_dir.resolve();assert out.is_relative_to(ROOT/'artifacts'),'Build into this project artifacts directory'
 assert o.vcvars.is_file();out.mkdir(parents=True,exist_ok=False)
 def build(name,source,extra=(),libs=()):
  args=['cl','/nologo','/std:c++17','/EHsc','/O2','/MT','/W4','/I'+str(SOURCE),*extra,str(SOURCE/source),'/Fo'+str(out/(name+'.obj')),'/Fe'+str(out/(name+'.exe'))]
  if libs:args+=['/link',*libs]
  batch=out/(name+'.cmd');batch.write_bytes(('@echo off\r\ncall '+subprocess.list2cmdline([str(o.vcvars)])+'\r\nif errorlevel 1 exit /b %errorlevel%\r\n'+subprocess.list2cmdline(args)+'\r\n').encode())
  with (out/(name+'.build.log')).open('w') as log:
   result=subprocess.run(['cmd.exe','/d','/c',str(batch)],cwd=out,stdout=log,stderr=subprocess.STDOUT,creationflags=0x08000000)
  if result.returncode:raise RuntimeError((out/(name+'.build.log')).read_text())
 build('crash-recorder','crash_recorder.cpp',libs=['bcrypt.lib','dbghelp.lib','user32.lib'])
 build('diagnostics-fixture','fixture.cpp')
 build('verify-ownership111','verify_ownership111.cpp')
 if o.experimental:
  build('trace-fixture110','trace_fixture110.cpp')
  pin=out/'fixture-pin111.h';pin.write_text('#define SHOGUN2_EXPERIMENTAL_TRACE_FIXTURES 1\n#define SHOGUN2_TRACE_FIXTURE_SHA256 "'+sha(out/'trace-fixture110.exe')+'"\n')
  build('crash-recorder-fixture111','crash_recorder.cpp',extra=['/FI'+str(pin)],libs=['bcrypt.lib','dbghelp.lib','user32.lib'])
 report={'hardware_compiled_in_default':False,'experimental_requested':o.experimental,'deployed':False,'game_launched':False,
  'binaries':[{'file':x.name,'sha256':sha(x)} for x in out.glob('*.exe')],
  'sources':[{'file':str(x.relative_to(SOURCE)),'sha256':sha(x)} for x in SOURCE.iterdir() if x.is_file() and x.suffix in ['.cpp','.h']]}
 (out/'build111.json').write_text(json.dumps(report,indent=2));print(json.dumps({'built':len(report['binaries']),'output':str(out),'deployed':False}))
if __name__=='__main__':main()
