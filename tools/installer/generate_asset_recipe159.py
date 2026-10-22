"""Generate metadata-only extraction instructions; never copy pixel/audio bytes."""
import argparse,hashlib,json,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]

def main():
    p=argparse.ArgumentParser();p.add_argument('--prepared-source',type=Path,required=True);a=p.parse_args()
    spec=json.loads((ROOT/'tools/resource-recipe.json').read_text());audio=json.loads((ROOT/'tools/audio109.json').read_text())
    base=a.prepared_source/'native/outputs/shogun2-ammunition83-preview/hud-src'
    layouts={}
    for name in ('sprites.generated.h','qte-sprites.generated.h'):
        text=(base/name).read_text()
        rows={key:list(map(int,nums.split(','))) for key,nums in re.findall(r'\{L"([^"]+)",([\d, -]+)\}',text)}
        assert rows and all(len(x)==9 for x in rows.values())
        layouts[name]=rows
        (ROOT/'native/common'/('hud_sprites159.h' if name.startswith('sprites') else 'qte_sprites159.h')).write_text(text,encoding='utf-8',newline='\n')
    rows={x['key']:x for x in spec['images']}
    for alias in spec['aliases']:rows[alias['key']]={**rows[alias['source']],'key':alias['key']}
    assert set(rows)==set(layouts['sprites.generated.h'])
    recipe=hashlib.sha256(json.dumps({'inputs':spec,'audio':audio,'layouts':layouts},sort_keys=True).encode()).hexdigest()
    header=['#pragma once','namespace assets159 {',f'inline constexpr char recipe[]="{recipe}";','}']
    (ROOT/'native/common/local_assets159_recipe.h').write_text('\n'.join(header)+'\n',encoding='utf-8',newline='\n')
    lines=['#pragma once','// Entry names, hashes and placement only. No game pixels/fonts/audio.','struct Input159 {const wchar_t* pack;const char* entry;unsigned bytes;const char* sha;};','struct Image159 {unsigned input;int atlas,x,y,w,h,lw,lh,ox,oy;};']
    inputs=[]
    def index(row):
        value=(row['pack'],row['entry'],row.get('bytes',row.get('source_bytes')),row.get('sha256',row.get('source_sha256')))
        if value not in inputs:inputs.append(value)
        return inputs.index(value)
    tables=[]
    for name,layout in layouts.items():
        table=[]
        for key,values in layout.items():table.append((index(rows[key]),values))
        tables.append(table)
    fonts=[index(x) for x in spec['fonts']];abilities=[index(x) for x in spec['ability_images']];voice=index(audio)
    lines+=['inline const Input159 inputs159[]={']
    for pack,entry,size,digest in inputs:lines.append('{L'+json.dumps(pack)+','+json.dumps(entry)+','+str(size)+','+json.dumps(digest)+'},')
    lines+=['};']
    for label,table in zip(('hud159','qte159'),tables):
        lines+=['inline const Image159 '+label+'[]={']
        lines+=['{'+str(i)+','+','.join(map(str,v))+'},' for i,v in table];lines+=['};']
    lines+=['inline const unsigned fonts159[]={'+','.join(map(str,fonts))+'};','inline const unsigned abilities159[]={'+','.join(map(str,abilities))+'};',f'inline const unsigned voice159={voice};']
    (ROOT/'tools/installer/asset_recipe159.generated.h').write_text('\n'.join(lines)+'\n',encoding='utf-8',newline='\n')
    print(json.dumps({'recipe':recipe,'original_entries':len(inputs),'hud_images':len(tables[0]),'qte_images':len(tables[1])}))

if __name__=='__main__':main()
