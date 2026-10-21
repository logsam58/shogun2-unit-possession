"""Index source-native integration sites, without claiming semantic proof."""
import argparse
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
PATTERNS = {'abi': r'__(?:thiscall|fastcall|cdecl|stdcall)|WINAPI',
            'hook': r'MH_(?:CreateHook|EnableHook|DisableHook)|hook_target',
            'synchronization': r'(?:Acquire|Release)SRWLock|thread_local|active_callbacks',
            'native_offset': r'(?:base|engine)\s*\+\s*0x[0-9a-fA-F]+'}

def inventory():
    rows = []
    for path in sorted((ROOT / 'native').rglob('*')):
        if path.suffix not in ('.h', '.cpp') or 'vendor' in path.parts: continue
        for n, line in enumerate(path.read_text(encoding='utf-8', errors='strict').splitlines(), 1):
            tags = [name for name, pattern in PATTERNS.items() if re.search(pattern, line)]
            if tags:
                rows.append({'path': path.relative_to(ROOT).as_posix(), 'line': n, 'tags': tags,
                             'rvas': sorted(set(re.findall(r'(?:base|engine)\s*\+\s*(0x[0-9a-fA-F]+)', line)))})
    return {'schema': 1, 'meaning': 'Source index only; typedefs, guards and tests must be read in context.', 'sites': rows}

def main():
    p = argparse.ArgumentParser(description=__doc__); p.add_argument('--check', action='store_true'); o = p.parse_args()
    path = ROOT / 'docs/public-release/native-inventory.json'
    text = json.dumps(inventory(), indent=2) + '\n'
    if o.check:
        if path.read_text(encoding='utf-8') != text: raise ValueError('Native inventory is stale')
    else: path.write_text(text, encoding='utf-8')
    print('PASS native integration index')

if __name__ == '__main__': main()
