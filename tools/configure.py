"""Configure explicit local inputs; never install, launch or copy private archives."""
import argparse
import json
from pathlib import Path
import sys
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from game_packs import require_game
from build_frontend import resource_inputs

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game',type=Path)
    parser.add_argument('--resources',type=Path)
    parser.add_argument('--vcvars',type=Path)
    args=parser.parse_args()
    target=ROOT/'tools/local.json'
    settings=json.loads(target.read_text(encoding='utf-8')) if target.is_file() else {}
    if args.game:
        require_game(args.game)
        settings['game']=str(args.game.resolve())
    if args.resources:
        resource_inputs(args.resources.resolve())
        settings['resources']=str(args.resources.resolve())
    if args.vcvars:
        if not args.vcvars.is_file() or args.vcvars.name.lower()!='vcvars32.bat':
            parser.error('--vcvars must name an existing vcvars32.bat')
        settings['vcvars']=str(args.vcvars.resolve())
    # Purge obsolete import settings without touching old input files.
    settings={key:value for key,value in settings.items() if key in ('game','resources','vcvars')}
    target.write_text(json.dumps(settings,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({'configuration':'tools/local.json','game_changed':False,'installed':False}))

if __name__=='__main__':main()
