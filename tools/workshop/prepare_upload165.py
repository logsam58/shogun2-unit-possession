"""Package the accepted runtime with its notices. Never stage, launch or upload."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from game_packs import Pack, digest

ACCEPTED = '7d4986b4fb7e447a25cb0132a28507ecd1845a27c913871441be2e62af259740'
ORIGINAL = {'ui/frontend ui/layout_scripts/root.luac', 'possession/workshop165.json'}
NOTICES = {
    'LICENSE': 'possession/licenses/LICENSE.txt',
    'MinHook-LICENSE.txt': 'possession/licenses/MinHook-LICENSE.txt',
    'THIRD_PARTY_NOTICES.md': 'possession/licenses/THIRD_PARTY_NOTICES.md',
    'PLAYING.md': 'possession/PLAYING.md',
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--package', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    source = args.package.resolve()
    out = args.output.resolve()
    if out.exists() or out == source or source.is_relative_to(out) or out.is_relative_to(source):
        parser.error('Use a new separate output directory')
    pack = source / 'shogun2_unit_possession_workshop165.pack'
    if digest(pack) != ACCEPTED:
        raise ValueError('Not the exact accepted Test165 archive; inputs preserved')
    parsed = Pack(pack)
    if set(parsed.entries) != ORIGINAL:
        raise ValueError('Unexpected archive entries')
    raw = pack.read_bytes()
    entries = {name: raw[offset:offset + size] for name, (offset, size) in parsed.entries.items()}
    meta = json.loads(entries['possession/workshop165.json'])
    if meta['test'] != 165 or meta['chooser'] or meta['resources'] != 'prepared from owned game':
        raise ValueError('Unexpected bootstrap metadata')
    for filename, name in NOTICES.items():
        data = (source / filename).read_bytes()
        if not 0 < len(data) < 128 * 1024 or b'\0' in data:
            raise ValueError('Invalid text notice: ' + filename)
        data.decode('utf-8')
        entries[name] = data
    magic, flags, deps, dep_size, count, index_size, timestamp = struct.unpack('<4s5IQ', raw[:32])
    if not timestamp or deps or dep_size:
        raise ValueError('Unexpected original header')
    index = b''.join(struct.pack('<I', len(data)) + name.replace('/', '\\').encode() + b'\0'
                     for name, data in entries.items())
    payload = struct.pack('<4s5IQ', magic, flags, 0, 0, len(entries), len(index), timestamp)
    payload += index + b''.join(entries.values())
    out.mkdir(parents=True)
    target = out / pack.name
    with target.open('xb') as stream:
        stream.write(payload)
    checked = Pack(target)
    if set(checked.entries) != set(entries):
        raise ValueError('Pack entry roundtrip differs')
    for name, data in entries.items():
        checked.read(name, len(data), hashlib.sha256(data).hexdigest())
    original_unchanged = all(entries[name] == raw[offset:offset + size]
                             for name, (offset, size) in parsed.entries.items())
    assert original_unchanged and digest(pack) == ACCEPTED
    receipt = {
        'test': 165, 'accepted_pack_sha256': ACCEPTED, 'pack': target.name,
        'pack_sha256': digest(target), 'bytes': len(payload),
        'packaging_only': True, 'all_original_entries_byte_exact': original_unchanged,
        'compiled_bootstrap_and_native_payload_changed': False,
        'added_entries': list(NOTICES.values()), 'entries': {
            name: {'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
            for name, data in entries.items()},
        'timestamp_preserved': True, 'canonical_backslash_index': True,
        'steam_visibility_required': 'private/hidden', 'uploaded': False,
        'game_files_changed': False,
    }
    (out / 'packaging-verification.json').write_text(json.dumps(receipt, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(receipt, indent=2))


if __name__ == '__main__':
    main()
