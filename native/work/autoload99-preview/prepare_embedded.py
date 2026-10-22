from pathlib import Path
import argparse,hashlib,json,struct,re
here=Path(__file__).resolve().parent
root=here.parents[1]
parser=argparse.ArgumentParser();mode=parser;mode.add_argument('--candidate',type=int,default=161);mode.add_argument('--release',action='store_true',help='Use product release metadata');args=parser.parse_args()
if args.candidate is not None:assert args.candidate>=100, 'Candidate must be 100 or later'
version=(root.parent/'VERSION').read_text().strip()
development=not args.release
title='Shogun 2 Unit Possession v'+version+((' development Test'+str(args.candidate)) if development else '')
assert re.fullmatch(r'[0-9]+\.[0-9]+(?:\.[0-9]+)?',version), 'Invalid product version'
adapter=root/'outputs/shogun2-siege99-preview/possession-v46.dll'
hud=root/'outputs/shogun2-siege99-preview/possession-hud.exe'
qte=root/'outputs/shogun2-siege99-preview/qte-playable-v79.dll'
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
assert adapter.exists()
assert hud.exists()
payload=json.dumps({'title':title,'version':version,'revision':args.candidate,'development_candidate':development,'hud':83,'requires':'Integrated DINPUT8.dll included with this mod; possession engine and HUD are embedded and start automatically','controls':'F9 possession; F climb wall; W ascend; S descend; release W/S hold; F10 RTS mode; Q single walk/double run contextual move or attack; hold Q drag formation; E stop; Z single walk/double run hold here; C follow with native pace/catch-up; R hold fire/fire at will/fire on command; 1 native melee/ranged toggle; RMB ready; LMB fire; TAB native ammo; hold Ctrl kneeling fire where unlocked; WASD locked while kneeling; F12 disable','adapter_sha256':sha(adapter)},indent=2).encode('utf-8')
if args.candidate>=141:
    metadata=json.loads(payload)
    metadata['controls']=metadata['controls'].replace('TAB native ammo;','T cycle native whole-unit ability/ammunition; G activate/end selected ability;')
    if args.candidate>=142:metadata['controls']=metadata['controls'].replace('LMB fire;','LMB click fire guns/throw weapons; hold/release LMB for variable bow strength;')
    if args.candidate>=148:metadata['controls']+='; W+Shift personal native charge toward enemy; K hide UI except melee prompt, shared with RTS'
    if args.candidate>=152:metadata['controls']+='; P first/third person; mouse wheel third-person distance; QTE eye look follows animated face; broken/shattered native combat has no QTE'
    if args.candidate>=154:metadata['controls']+='; hold middle mouse to orbit third person independently of WASD; M native whole-unit mount/dismount'
    if args.candidate>=160:metadata['startup_view']='Choose Third-Person or First-Person at each launch; P switches during possession'
    if args.candidate>=161:metadata['startup_view']='Steam prelaunch helper: choose before the game process starts; P switches during possession'
    payload=json.dumps(metadata,indent=2).encode('utf-8')
index=struct.pack('<I',len(payload))+b'possession\\checkpoint45.json\0'
pack=here/'marker.pack'
pack.write_bytes(struct.pack('<4s5IQ',b'PFH3',3,0,0,1,len(index),0)+index+payload)
build=hashlib.sha256((sha(adapter)+sha(hud)+sha(qte)+sha(pack)).encode()).hexdigest()[:16]
(here/'embedded_hashes.h').write_text('#pragma once\n'+''.join(f'static const char* EMBEDDED_{name}_SHA="{sha(p)}";\n' for name,p in [('ADAPTER',adapter),('HUD',hud),('PACK',pack),('QTE',qte)])+f'static const wchar_t* EMBEDDED_BUILD=L"{build}";\nstatic const wchar_t* EMBEDDED_TITLE=L"{title}";\n',encoding='ascii')
(here/'payload.rc').write_text('101 RCDATA "../../outputs/shogun2-siege99-preview/possession-v46.dll"\n102 RCDATA "../../outputs/shogun2-siege99-preview/possession-hud.exe"\n103 RCDATA "../../outputs/shogun2-siege99-preview/qte-playable-v79.dll"\n',encoding='ascii')
print(json.dumps({'embedded_build':build,'adapter_sha256':sha(adapter),'hud_sha256':sha(hud),'pack_sha256':sha(pack)}))

