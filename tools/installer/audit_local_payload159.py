"""Read PE resource directories as data; never load executable code."""
import hashlib
import struct
from pathlib import Path

def resources(path):
    b=Path(path).read_bytes(); pe=struct.unpack_from('<I',b,60)[0]
    assert b[:2]==b'MZ' and b[pe:pe+4]==b'PE\0\0'
    count=struct.unpack_from('<H',b,pe+6)[0];opt=pe+24
    assert struct.unpack_from('<H',b,opt)[0]==0x10b, 'Expected x86 PE'
    size=struct.unpack_from('<H',b,pe+20)[0]; sections=[]
    for i in range(count):
        p=opt+size+i*40; virtual,va,raw,offset=struct.unpack_from('<IIII',b,p+8)
        sections.append((va,max(virtual,raw),offset))
    def filepos(rva):
        for va,n,p in sections:
            if va<=rva<va+n:return p+rva-va
        raise ValueError('Invalid PE resource RVA')
    rva,n=struct.unpack_from('<II',b,opt+112)
    if not rva:return {}
    base=filepos(rva);out={}
    def walk(offset,keys):
        p=base+offset; named,ids=struct.unpack_from('<HH',b,p+12)
        for i in range(named+ids):
            name,child=struct.unpack_from('<II',b,p+16+8*i)
            if name&0x80000000:
                q=base+(name&0x7fffffff);length=struct.unpack_from('<H',b,q)[0]
                name=b[q+2:q+2+length*2].decode('utf-16-le')
            key=keys+(name,)
            if child&0x80000000:walk(child&0x7fffffff,key)
            else:
                data,length=struct.unpack_from('<II',b,base+child);q=filepos(data)
                assert q+length<=len(b);out[key]=b[q:q+length]
    walk(0,());return out

def audit(folder):
    folder=Path(folder);rows={}
    for name in ('possession-v46.dll','possession-hud.exe','qte-playable-v79.dll','prepare-local-assets159.exe'):
        leaves=resources(folder/name)
        assert all(key[0] in (16,24) for key in leaves), 'Unexpected embedded data/artwork: '+name
        rows[name]={'sha256':hashlib.sha256((folder/name).read_bytes()).hexdigest(),'resource_keys':[str(k) for k in leaves]}
    leaves=resources(folder/'dinput8.dll')
    payloads={key[1]:value for key,value in leaves.items() if key[0]==10}
    assert set(payloads)=={101,102,103}
    for i,name in enumerate(('possession-v46.dll','possession-hud.exe','qte-playable-v79.dll'),101):
        assert payloads[i]==(folder/name).read_bytes(), 'Loader closure mismatch'
    assert all(k[0] in (10,16,24) for k in leaves)
    rows['dinput8.dll']={'embedded_first_party_components_only':True,'sha256':hashlib.sha256((folder/'dinput8.dll').read_bytes()).hexdigest()}
    return {'passed':True,'game_images_fonts_audio_embedded':False,'files':rows}
