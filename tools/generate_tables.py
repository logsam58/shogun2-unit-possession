"""Render retained factual records; this does not decode or resample a game."""
import argparse
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def render(record):
    text = record['template']
    for name, rows in record['tables'].items():
        text = text.replace('@' + name + '@', '\n'.join(rows))
    if hashlib.sha256(text.encode()).hexdigest() != record['expected_sha256']:
        raise ValueError('Canonical records/template changed: review the new expected result')
    return text

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--check', action='store_true')
    o = p.parse_args()
    records = json.loads((ROOT / 'tools/data/derived-records.json').read_text(encoding='utf-8'))
    for record in records['headers']:
        path = ROOT / record['path']
        content = render(record)
        if o.check:
            if path.read_text(encoding='utf-8') != content:
                raise ValueError('Derived header differs: ' + record['path'])
        else:
            path.write_text(content, encoding='utf-8', newline='\n')
    print('PASS canonical derived headers: ' + str(len(records['headers'])))

if __name__ == '__main__': main()
