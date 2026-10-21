"""Stage diagnostics explicitly; no autorun, watcher, collection or game changes."""
import argparse
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from deployment import dependency_closure, stage

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--game', type=Path, required=True)
    parser.add_argument('--recordings', type=Path, required=True)
    parser.add_argument('--debugger', type=Path, required=True)
    parser.add_argument('--python', type=Path, default=Path(sys.executable))
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args()
    if args.dry_run:
        print(json.dumps({'closure': dependency_closure(Path(__file__).parent),
                          'game_changed': False, 'watcher_started': False, 'staged': False}))
        return
    baseline = json.loads(args.baseline.read_text(encoding='utf-8')) if args.baseline else None
    receipt = stage(Path(__file__).parent, args.output_dir, args.debugger,
                    args.game, args.recordings, args.python, baseline)
    print(json.dumps(receipt, indent=2))

if __name__ == '__main__':
    main()
