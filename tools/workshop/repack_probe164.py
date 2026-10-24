"""Correct raw PFH path separators only, retaining exact tested private payloads."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from game_packs import Pack,digest,require_game

def raw_names(data):
    if len(data)<32:raise ValueError('Truncated pack')
    magic,flags,deps,dep_bytes,count,index_bytes,stamp=struct.unpack_from('<4s5IQ',data)
    if magic!=b'PFH3' or flags not in (0,1,3) or deps or dep_bytes or count>100000 or index_bytes>16*1024*1024:
        raise ValueError('Unsupported raw index')
    index=data[32:32+index_bytes];cursor=0;names=[]
    for _ in range(count):
        if cursor+4>len(index):raise ValueError('Truncated entry')
        size=struct.unpack_from('<I',index,cursor)[0];cursor+=4
        end=index.find(b'\0',cursor)
        if end<0:raise ValueError('Unterminated entry')
        name=index[cursor:end].decode('utf-8');names.append((name,size,cursor,end));cursor=end+1
    if cursor!=index_bytes:raise ValueError('Index not fully consumed')
    return names,32+index_bytes

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--build',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--game',type=Path,required=True)
    a=p.parse_args();old=a.build.resolve();out=a.output.resolve();game=a.game.resolve();require_game(game)
    if out.exists() or out==game or out.is_relative_to(game) or old.is_relative_to(out):p.error('Use a new output outside game/source inputs')
    receipt=json.loads((old/'verification.json').read_text());source=old/receipt['pack']
    if receipt['test']!=164 or receipt['entry']!='ui/frontend ui/layout_scripts/root.luac' or receipt['public_package_ready']:
        raise ValueError('Not the verified private UI-root probe')
    if digest(source)!=receipt['pack_sha256']:raise ValueError('Source pack changed after tests')
    data=source.read_bytes();names,end=raw_names(data);corrected=bytearray(data)
    for name,size,start,stop in names:
        corrected[32+start:32+stop]=name.replace('/','\\').encode()
    # No new native or Lua code: prove every changed byte is one index separator.
    changes=[i for i,(x,y) in enumerate(zip(data,corrected)) if x!=y]
    if not changes or len(data)!=len(corrected) or any(i>=end or data[i]!=47 or corrected[i]!=92 for i in changes):
        raise ValueError('Change exceeded raw path separators')
    if data[end:]!=corrected[end:]:raise ValueError('Payload region changed')
    canonical_names,_=raw_names(corrected)
    if any('/' in name for name,*_ in canonical_names):raise ValueError('Noncanonical index path')
    # Independently compare the target entry spelling against the actual stock index.
    with (game/'data/data.pack').open('rb') as f:
        header=f.read(32);stock=header+f.read(struct.unpack_from('<I',header,20)[0])
    stock_names,_=raw_names(stock)
    target=next(name for name,*_ in canonical_names if name.replace('\\','/')==receipt['entry'])
    if not any(name==target for name,*_ in stock_names):raise ValueError('Target spelling differs from owned stock index')
    out.mkdir(parents=True);destination=out/receipt['pack'];destination.write_bytes(corrected)
    before=Pack(source);after=Pack(destination);entries=[]
    for name,(offset,size) in before.entries.items():
        payload=data[offset:offset+size];sha=hashlib.sha256(payload).hexdigest()
        if after.read(name,size,sha)!=payload:raise ValueError('Entry payload changed')
        entries.append({'name':name,'bytes':size,'sha256':sha})
    result=dict(receipt)
    result.update({'status':'RAW INDEX CORRECTION PASS; exact native/Lua evidence retained, not freshly rerun',
        'pack_sha256':digest(destination),'repacked_from_sha256':receipt['pack_sha256'],
        'fresh_checks':['raw index target spelling matches stock','only slash-to-backslash bytes changed inside index',
            'complete payload region byte-exact','every entry hash retained'],
        'retained_native_lua_verification':receipt,'changed_byte_offsets':changes,'entries':entries,
        'raw_entry_names':[name for name,*_ in canonical_names],
        'repacker_sha256':digest(Path(__file__)),'game_changed':False,'game_launched':False,'uploaded':False})
    (out/'verification.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({key:result[key] for key in ['status','pack_sha256','native_sha256','changed_byte_offsets','raw_entry_names']},indent=2))

if __name__=='__main__':main()
