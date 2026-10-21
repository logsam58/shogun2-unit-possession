"""Verify a staged deployment without starting collection or touching any process."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

def check(root):
    receipt = json.loads((root / 'installation.json').read_text(encoding='utf-8'))
    for name, expected in receipt['files'].items():
        relative = Path(name)
        if relative.is_absolute() or '..' in relative.parts: raise ValueError('Unsafe receipt path')
        if hashlib.sha256((root / relative).read_bytes()).hexdigest() != expected:
            raise ValueError('Staged file differs: ' + name)
    entries = ('watcher', 'session', 'native_input', 'control', 'inspect_minidump')
    for entry in entries:
        code = ('import sys,runpy;sys.path.insert(0,sys.argv[1]);import process_probe;'
                'exec(\'def forbidden(*a,**k): raise RuntimeError("process access forbidden")\');'
                'process_probe.Process=forbidden;process_probe.processes=forbidden;process_probe.modules=forbidden;'
                'name=sys.argv[2];sys.argv=[name,"--help"];runpy.run_module(name,run_name="__main__")')
        result = subprocess.run([sys.executable, '-I', '-B', '-c', code, str(root), entry], cwd=root,
                                capture_output=True, text=True, timeout=15)
        if result.returncode or 'usage:' not in result.stdout.lower():
            raise ValueError(entry + ': ' + result.stderr)
    if list(root.glob('*.jsonl')) or (root / 'watcher-health.json').exists():
        raise ValueError('Unexpected collection output in fresh stage')
    return {'passed': True, 'entry_points': list(entries), 'receipt_files': len(receipt['files']),
            'collectors_started': False, 'game_attached': False}

if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__); p.add_argument('--stage', type=Path, required=True)
    print(json.dumps(check(p.parse_args().stage.resolve()), indent=2))
