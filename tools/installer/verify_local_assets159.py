"""Compare native extraction against independently prepared artwork and audio."""
import argparse, array, hashlib, json, math, re, struct, subprocess
from pathlib import Path

def bundle(path):
    b=path.read_bytes();assert b[:8]==b'S2FPAS1\0';p=84;out={}
    for _ in range(struct.unpack_from('<I',b,16)[0]):
        i,w,h,n,t=struct.unpack_from('<IIIII',b,p);data=b[p+84:p+84+n]
        assert hashlib.sha256(data).hexdigest().encode()==b[p+20:p+84]
        out[i]=(w,h,data);p+=84+n
    assert p==len(b);return out

def main():
    p=argparse.ArgumentParser(description=__doc__)
    for name in ('assets','reference','magick','report'):p.add_argument('--'+name,type=Path,required=True)
    a=p.parse_args();hud=bundle(a.assets/'hud-assets159.s2fp');qte=bundle(a.assets/'qte-assets159.s2fp')
    native=a.reference/'native/outputs';base=native/'shogun2-authentic-hud80-preview';results=[]
    def compare(record,png):
        w,h,data=record
        # Compare visible RGBA; fully transparent RGB is immaterial to rendering.
        info=subprocess.check_output([str(a.magick),'identify','-format','%w %h',str(png)],text=True)
        rw,rh=map(int,info.split());assert rw>=w and rh>=h
        raw=subprocess.check_output([str(a.magick),str(png),'-crop',f'{w}x{h}+0+0','+repage','-depth','8','BGRA:-'])
        assert len(raw)==len(data)
        mismatches=sum(1 for n in range(0,len(raw),4) if raw[n+3]!=data[n+3] or (raw[n+3] and raw[n:n+3]!=data[n:n+3]))
        assert mismatches==0,(png.name,mismatches)
        results.append({'image':png.name,'visible_pixels_equal':True,'dimensions':[w,h]})
    for i in range(3):compare(hud[101+i],base/f'assets/processed/atlases/game-atlas-{i}.png')
    compare(qte[101],base/'assets/processed/qte-atlas/qte-atlas.png')
    for i in (30,31):compare(hud[400+i],native/f'shogun2-ammunition83-preview/hud-src/private/ability141-{i}.png')
    font_rc=(native/'shogun2-ammunition83-preview/hud-src/native-fonts.rc').read_text()
    for i,name in re.findall(r'(\d+) RCDATA "([^"]+)"',font_rc):
        raw=(native/'shogun2-ammunition83-preview/hud-src'/name).resolve().read_bytes()
        assert hud[int(i)][2]==qte[int(i)][2]==raw
    results.append({'fonts_byte_equal':20})
    ref=(native/'shogun2-ammunition83-preview/hud-src/private/shameful-display109.wav').read_bytes()
    voice=hud[301][2];assert len(voice)==len(ref)==136754
    x=array.array('h',voice[44:]);y=array.array('h',ref[44:])
    # Independent Windows/FFmpeg decoders may round or delay MP3 samples differently.
    best=(-1,0)
    for lag in range(-1600,1601,8):
        pairs=list(zip(x[max(0,lag)::8],y[max(0,-lag)::8]))
        xy=sum(i*j for i,j in pairs);xx=sum(i*i for i,j in pairs);yy=sum(j*j for i,j in pairs)
        corr=xy/math.sqrt(xx*yy) if xx and yy else 0
        if corr>best[0]:best=(corr,lag)
    assert best[0]>.98,best
    results.append({'voice_seconds':1.55,'reference_correlation':best[0],'decoder_offset_samples':best[1],'not_claimed_byte_identical':True})
    receipt={'passed':True,'comparisons':results,'game_launched':False}
    a.report.write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
