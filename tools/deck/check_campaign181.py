"""Validate campaign entry against owned scripts and an inert native fixture."""
import argparse,hashlib,json,os,struct,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path[:0]=[str(ROOT/'tools'),str(ROOT/'tools/deck')]
from game_packs import Pack,digest
from build174 import batch_script
import adapt_frontend181

def main():
 p=argparse.ArgumentParser(description=__doc__)
 for n in ('build','game','output','vcvars','lua','luac'):p.add_argument('--'+n,type=Path,required=True)
 p.add_argument('--companion-selection', type=Path, action='append', default=[],
                help='Check an existing selection expected to preserve stock frontend and campaign roots; repeatable')
 p.add_argument('--production-pack', type=Path)
 a=p.parse_args();build=a.build.resolve();out=a.output.resolve();game=a.game.resolve()
 if out.exists()or out.is_relative_to(game):p.error('Use a new isolated output outside game')
 result=json.loads((build/'verification.json').read_text())
 combined=result.get('test')==181 and result.get('combined_pc_deck') is True
 direct=combined or a.production_pack is not None
 if not combined and (result.get('candidate')!=181 or result.get('quick_only')):
  raise ValueError('Campaign tests require a complete combined candidate')
 workshop=build if combined else build/'workshop'
 if direct:
  pack=a.production_pack.resolve() if a.production_pack else workshop/'package'/result['pack']
  if pack.name!=result['pack']:
   raise ValueError('Unexpected pack filename')
  if digest(pack)!=result['pack_sha256']:
   raise ValueError('Combined pack differs from the build receipt')
  packaged=Pack(pack)
  wrapper_path=(build/('campaign%d.luac'%result['revision']) if a.production_pack else workshop/'campaign-production.luac')
  wrapper=wrapper_path.read_bytes()
  if packaged.read('ui/campaign ui/layout.root.luac',len(wrapper),digest(wrapper_path))!=wrapper:
   raise ValueError('Combined pack campaign wrapper differs from build output')
  if result['native_sha256'].encode() not in wrapper:
   raise ValueError('Combined pack campaign wrapper points to a different bridge')
 out.mkdir(parents=True);fixture=workshop/'bin/s2up-workshop165-fixture.dll'
 if not fixture.is_file():raise ValueError('Missing inert Workshop fixture')
 source=out/'campaign-fixture.lua';compiled=out/'campaign-fixture.luac'
 source.write_text(adapt_frontend181.campaign_wrapper(workshop/'source',digest(fixture)),encoding='utf-8')
 subprocess.run([str(a.luac.resolve()),'-s','-o',str(compiled),str(source)],check=True)
 pack=Pack(game/'data/data.pack');off,size=pack.entries['ui/campaign ui/layout.root.luac']
 with (game/'data/data.pack').open('rb')as f:f.seek(off);owned=f.read(size)
 assert size==60729 and hashlib.sha256(owned).hexdigest()=='d14bea292474e62bb6e1e7a5f4938cdfa121b5bcca3003d693c9a31d5d703b50'
 reference=out/'private-owned-campaign.luac';reference.write_bytes(owned)
 cache=out/'cache';cache.mkdir()
 empty_selection=out/'stock-selection.txt';empty_selection.write_bytes(b'')
 env=dict(os.environ,LOCALAPPDATA=str(cache),S2UP165_FIXTURE_GAME=str(game),
          S2UP168_FIXTURE_SELECTION=str(empty_selection))
 def check_entry(log_name,scenario='stock',expected=reference,selection=empty_selection):
  fixture_env=dict(env,S2UP168_FIXTURE_SELECTION=str(selection))
  with (out/log_name).open('w') as log:
   subprocess.run([str(a.lua.resolve()),str(ROOT/'tools/deck/verify_campaign_entry181.lua'),
                   str(workshop/'bootstrap-fixture.luac'),str(compiled),str(expected),
                   scenario,str(game/'data/data.pack')],
                  check=True,cwd=out,env=fixture_env,stdout=log,stderr=subprocess.STDOUT)
 # Reproduce the previous collision with both an exact orphan and a foreign
 # file. The campaign export must return the bytes through Lua and leave it intact.
 legacy=cache/('S2UP-Workshop165-'+digest(fixture)+'.dll.campaign.luac')
 for marker in (owned,b'foreign campaign handoff'):
  legacy.write_bytes(marker)
  check_entry('compiled-entry-exact.log' if marker==owned else 'compiled-entry-foreign.log')
  assert legacy.read_bytes()==marker
  assert list(cache.glob('*.campaign.luac'))==[legacy]
 legacy.unlink()
 check_entry('compiled-entry.log')
 for log in ('compiled-entry.log','compiled-entry-exact.log','compiled-entry-foreign.log'):
  assert 'PASS 8 compiled181 menu/campaign' in (out/log).read_text()
 companion_checks=[]
 for index, selection in enumerate(a.companion_selection, 1):
  selection=selection.resolve(strict=True)
  fingerprint=digest(selection)
  log=f'compiled-companion-{index}.log'
  check_entry(log, 'companions', reference, selection)
  if digest(selection)!=fingerprint or 'PASS 2 compiled181 companion stock-root' not in (out/log).read_text():
   raise ValueError(f'Companion selection changed or did not complete: {selection.name}')
  companion_checks.append({'selection_sha256':fingerprint,'log':log,'checks':2})
 assert not list(cache.glob('*.campaign.luac'))
 cases=out/'reader-fixtures';cases.mkdir()
 def write(name,entries,magic=b'PFH3',flags=3):
  rows=list(entries.items()) if isinstance(entries,dict) else entries
  timestamp=b'\0'*8 if flags&0x40 else b''
  index=b''.join(struct.pack('<I',len(value))+timestamp+key.replace('/','\\').encode()+b'\0' for key,value in rows)
  (cases/name).write_bytes(struct.pack('<4s5IQ',magic,flags,0,0,len(rows),len(index),1)+index+b''.join(value for _,value in rows))
 target='ui/campaign ui/layout.root.luac'
 front='ui/frontend ui/layout_scripts/root.luac'
 metadata='possession/workshop165.json'
 aliases=[('ui/battle ui/textures/Banner.dds',b'image'),('ui/battle ui/textures/banner.dds',b'other image')]
 write('aliases.pack',aliases)
 write('campaign-aliases.pack',[aliases[0],(target,b'campaign bytes'),aliases[1]],b'PFH2',0x43)
 write('frontend-aliases.pack',[aliases[0],(front,b'frontend bytes'),aliases[1]])
 write('frontend-other.pack',[(front,b'other frontend bytes')])
 write('empty-campaign.pack',[(target,b'')])
 write('empty-frontend.pack',[(front,b'')])
 write('duplicate-root.pack',[(target,b''),(target.upper(),b'campaign bytes')])
 write('duplicate-frontend.pack',[(front,b'frontend bytes'),(front.upper(),b'frontend bytes')])
 write('duplicate-metadata.pack',[(metadata,b''),(target,b'campaign bytes'),(metadata.upper(),b'{}')])
 write('unsafe-aliases.pack',aliases+[('../outside',b'outside')])
 write('truncated-aliases.pack',aliases)
 truncated=cases/'truncated-aliases.pack'
 truncated.write_bytes(truncated.read_bytes()[:-1])
 write('database.pack',{'db/unit_tables/fixture':b'not a UI root'},b'PFH2')
 write('campaign-addon.pack',{target:b'owned test addon A'})
 write('campaign-other.pack',{target:b'owned test addon B'})
 write('oversized.pack',{target:b'x'*262145})
 identity='a'*64
 write('self.pack',{target:('/S2UP-Workshop165-'+identity+'.dll').encode(),'possession/workshop165.json':json.dumps(dict(first_party_bootstrap=True,native_sha256=identity)).encode()})
 write('foreign-root.pack',{target:b"return '/S2UP-Workshop165-"+b'b'*64+b".dll'"})
 # Only the native reader sees the synthetic pack contents. The Lua fixture
 # verifies the bytes handed to the compiler and supplies the mock callbacks.
 selection_cases=0
 if direct:
  selected_reference=out/'selected-campaign-root.bin'
  selected_reference.write_bytes(b'campaign bytes')
  for scenario,mods,expected in (
      ('selected',('campaign-aliases.pack','database.pack'),selected_reference),
      ('conflict',('campaign-aliases.pack','campaign-other.pack'),reference),
      ('foreign',('self.pack',),reference),
      ('empty',('empty-campaign.pack',),reference),
      ('foreign-root',('foreign-root.pack',),reference)):
   selection=out/(scenario+'-selection.txt')
   selection.write_text(f'add_working_directory "{cases.as_posix()}";\n' +
                        ''.join(f'mod "{name}";\n' for name in mods),encoding='utf-16-le')
   log='compiled-selection-'+scenario+'.log'
   check_entry(log,scenario,expected,selection)
   assert 'PASS compiled181 campaign selection '+scenario in (out/log).read_text()
   assert not list(cache.glob('*.campaign.luac'))
   selection_cases+=1
 fixture_source=workshop/'source/tools/deck' if direct else ROOT/'tools/deck'
 exe=out/'verify-campaign181.exe';batch=out/'native.cmd'
 commands=[['cl','/nologo','/std:c++17','/EHsc','/O2','/MT','/W4','/I'+str(workshop/'source/tools/deck'),fixture_source/'verify_campaign181.cpp','/Fo'+str(out/'native.obj'),'/Fe'+str(exe),'/link','bcrypt.lib','user32.lib'],[exe,game,cases]]
 backend_exe=out/'verify-context181.exe'
 commands.extend([['cl','/nologo','/std:c++17','/EHsc','/O2','/MT',fixture_source/'verify_context_backend181.cpp','/Fo'+str(out/'context.obj'),'/Fe'+str(backend_exe),'/link','user32.lib'],[backend_exe]])
 batch.write_text(batch_script(a.vcvars.resolve(),commands))
 with (out/'native-reader.log').open('w')as log:subprocess.run([os.environ.get('COMSPEC','cmd.exe'),'/d','/c',str(batch)],check=True,cwd=out,stdout=log,stderr=subprocess.STDOUT)
 assert 'PASS 5 context181' in (out/'native-reader.log').read_text()
 reader_cases=30 if 'S2UP_ROOT_PACK_HANDLE' in (workshop/'source/tools/deck/pack174_generated.h').read_text() else 26
 assert f'PASS {reader_cases} campaign181' in (out/'native-reader.log').read_text()
 assert owned not in compiled.read_bytes()
 receipt=dict(candidate=181,compiled_menu_campaign_cases=8,compiled_campaign_selection_cases=selection_cases,owned_reader_cases=reader_cases,context_backend_cases=5,stale_file_suites=2,production_pack_readback=direct,no_campaign_handoff_created=True,actual_stock_campaign_handoff_sha256=hashlib.sha256(owned).hexdigest(),stock_execution='mocked only; exact handed-off bytes compared',native_entry='inert fixture; real export lookup and campaign preparation',game_or_Steam_started=False,Deck_validation=False)
 if companion_checks:
  receipt['companion_stock_root_checks']=companion_checks
 (out/'verification.json').write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
