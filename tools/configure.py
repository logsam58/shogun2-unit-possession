"""Configure local build inputs; never install or launch Shogun 2."""
import argparse,hashlib,json,shutil
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--asset-source',type=Path);p.add_argument('--legacy-fixtures',type=Path);p.add_argument('--vcvars',type=Path);a=p.parse_args()
 config=ROOT/'tools/local.json';settings=json.loads(config.read_text()) if config.exists() else {}
 if a.vcvars:
  if not a.vcvars.is_file():p.error('vcvars32.bat does not exist')
  settings['vcvars']=str(a.vcvars.resolve())
 source=a.asset_source or (Path(settings['asset_source']) if 'asset_source' in settings else None);copied=0
 if source:
  for item in json.loads((ROOT/'tools/local-assets.json').read_text())['files']:
   original=source/item['source_relative'];dest=ROOT/item['path']
   if not original.is_file() or sha(original)!=item['sha256']:raise SystemExit('Missing/different local UI input: '+str(original))
   if dest.exists() and sha(dest)!=item['sha256']:raise SystemExit('Existing local UI input differs; preserved: '+str(dest))
   dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(original,dest);copied+=1
  settings['asset_source']=str(source.resolve())
 legacy=a.legacy_fixtures;fixtures=0
 if legacy:
  for directory in (legacy/'work').glob('autoload*'):
   if not directory.is_dir() or directory.name=='autoload99-preview':continue
   for name in ['dinput8.dll','marker.pack']:
    original=directory/name
    if original.is_file():
     dest=ROOT/'native/work'/directory.name/name;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(original,dest);fixtures+=1
  for relative in ['outputs/shogun2-melee-meter78-preview/possession-v46.dll','outputs/shogun2-melee-meter78-preview/qte-playable-v78.dll','outputs/shogun2-possession-hud-v12/possession-hud.exe']:
   original=legacy/relative
   if not original.is_file():raise SystemExit('Missing historical runtime fixture: '+str(original))
   dest=ROOT/'native'/relative;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(original,dest);fixtures+=1
  settings['legacy_fixture_source']=str(legacy.resolve())
 config.write_text(json.dumps(settings,indent=2)+'\n');print(json.dumps({'local_UI_inputs':copied,'local_legacy_fixture_files':fixtures,'configuration':str(config),'game_changed':False}))
if __name__=='__main__':main()
