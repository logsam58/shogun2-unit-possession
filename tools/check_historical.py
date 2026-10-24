"""Optional archive preflight. Missing history is a skip unless explicitly required."""

import argparse
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def inspect(folder):
    manifest = json.loads(
        (ROOT / 'tools/data/historical-fixtures.json').read_text(encoding='utf-8')
    )
    missing, changed = [], []
    for row in manifest['files']:
        path = folder / row['path'] if folder else None
        if not path or not path.is_file():
            missing.append(row['path'])
            continue
        with path.open('rb') as stream:
            actual = hashlib.file_digest(stream, 'sha256').hexdigest()
        if actual != row['sha256'] or path.stat().st_size != row['bytes']:
            changed.append(row['path'])
    return {
        'status': (
            'failed' if changed else 'skipped' if missing else 'prerequisites-verified'
        ),
        'optional': True,
        'suite_executed': False,
        'files_expected': len(manifest['files']),
        'missing': missing,
        'changed': changed,
        'reason': 'Historical executable suite was not run. Archive verification is not compatibility acceptance.',
    }


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--fixtures', type=Path)
    p.add_argument('--require', action='store_true')
    p.add_argument('--output', type=Path)
    o = p.parse_args()
    result = inspect(o.fixtures)
    text = json.dumps(result, indent=2) + '\n'
    if o.output:
        with o.output.open('x', encoding='utf-8') as stream:
            stream.write(text)
    print(text, end='')
    if result['changed'] or (o.require and result['missing']):
        raise SystemExit(1)


if __name__ == '__main__':
    main()
