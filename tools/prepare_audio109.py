"""Extract and crop the one local-game voice dependency. Never install or launch."""
import argparse,hashlib,json,struct,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--game',type=Path,required=True);p.add_argument('--ffmpeg',type=Path,required=True);a=p.parse_args()
    spec=json.loads((ROOT/'tools/audio109.json').read_text());pack=a.game/'data'/spec['pack']
    with pack.open('rb') as f:
        header=f.read(32);magic,flags,deps,dep_size,count,index_size,stamp=struct.unpack('<4s5IQ',header)
        if magic!=b'PFH3' or deps or dep_size or index_size>16*1024*1024 or count>100000:raise RuntimeError('Unsupported local pack')
        index=f.read(index_size);cursor=0;offset=32+index_size;source=None
        for _ in range(count):
            size=struct.unpack_from('<I',index,cursor)[0];cursor+=4;end=index.index(0,cursor);name=index[cursor:end].decode('utf-8').replace('\\','/');cursor=end+1
            if name==spec['entry']:
                if size!=spec['source_bytes']:raise RuntimeError('Voice source size differs')
                f.seek(offset);source=f.read(size)
            offset+=size
    if source is None or hashlib.sha256(source).hexdigest()!=spec['source_sha256']:raise RuntimeError('Original voice source differs')
    target=ROOT/spec['wave_path'];target.parent.mkdir(parents=True,exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='shogun-voice109-') as temp:
        original=Path(temp)/'voice.mp3';original.write_bytes(source);cropped=Path(temp)/'cropped.wav';start,end=spec['trim_seconds']
        subprocess.run([str(a.ffmpeg),'-hide_banner','-loglevel','error','-y','-i',str(original),'-af',f'atrim=start={start}:end={end},asetpts=PTS-STARTPTS','-ac','1','-ar','44100','-c:a','pcm_s16le','-fflags','+bitexact','-flags:a','+bitexact','-map_metadata','-1',str(cropped)],check=True)
        data=cropped.read_bytes()
        if hashlib.sha256(data).hexdigest()!=spec['wave_sha256']:raise RuntimeError('Cropped audio differs; existing dependency preserved')
        target.write_bytes(data)
    print('Verified local-game SHAMEFUL DISPLAY clip; routing lead-in excluded.')
if __name__=='__main__':main()
