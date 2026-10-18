from pathlib import Path
import argparse,hashlib,json,struct,re
here=Path(__file__).resolve().parent
root=here.parents[1]
parser=argparse.ArgumentParser();parser.add_argument('--candidate',type=int,default=104);args=parser.parse_args()
if args.candidate is not None:assert args.candidate>=100, 'Candidate must be 100 or later'
version=(root.parent/'VERSION').read_text().strip()
title='Shogun 2 Possession v'+version+((' development Test'+str(args.candidate)) if args.candidate else '')
assert re.fullmatch(r'[0-9]+\.[0-9]+(?:\.[0-9]+)?',version), 'Invalid product version'
adapter=root/'outputs/shogun2-siege99-preview/possession-v46.dll'
hud=root/'outputs/shogun2-siege99-preview/possession-hud.exe'
qte=root/'outputs/shogun2-siege99-preview/qte-playable-v79.dll'
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
assert adapter.exists()
assert hud.exists()
payload=json.dumps({'title':title,'version':version,'revision':args.candidate or 99,'development_candidate':bool(args.candidate),'hud':83,'requires':'Integrated DINPUT8.dll included with this mod; possession engine and HUD are embedded and start automatically','controls':'F9 FPS mode; F climb wall; W ascend; S descend; release W/S hold; F10 RTS mode; Q single walk/double run contextual move or attack; hold Q drag formation; E stop; Z single walk/double run hold here; C follow with native pace/catch-up; R hold fire/fire at will/fire on command; 1 melee; 2 ranged; RMB ready; LMB fire; TAB native ammo; hold Ctrl kneeling fire where unlocked; WASD locked while kneeling; F12 disable','adapter_sha256':sha(adapter)},indent=2).encode('utf-8')
index=struct.pack('<I',len(payload))+b'possession\\checkpoint45.json\0'
pack=here/'marker.pack'
pack.write_bytes(struct.pack('<4s5IQ',b'PFH3',3,0,0,1,len(index),0)+index+payload)
build=hashlib.sha256((sha(adapter)+sha(hud)+sha(qte)+sha(pack)).encode()).hexdigest()[:16]
(here/'embedded_hashes.h').write_text('#pragma once\n'+''.join(f'static const char* EMBEDDED_{name}_SHA="{sha(p)}";\n' for name,p in [('ADAPTER',adapter),('HUD',hud),('PACK',pack),('QTE',qte)])+f'static const wchar_t* EMBEDDED_BUILD=L"{build}";\nstatic const wchar_t* EMBEDDED_TITLE=L"{title}";\n',encoding='ascii')
(here/'payload.rc').write_text('101 RCDATA "../../outputs/shogun2-siege99-preview/possession-v46.dll"\n102 RCDATA "../../outputs/shogun2-siege99-preview/possession-hud.exe"\n103 RCDATA "../../outputs/shogun2-siege99-preview/qte-playable-v79.dll"\n',encoding='ascii')
print(json.dumps({'embedded_build':build,'adapter_sha256':sha(adapter),'hud_sha256':sha(hud),'pack_sha256':sha(pack)}))

