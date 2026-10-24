"""Correct only the private probe's zero PFH timestamp. Never stage or launch."""
import argparse
import ctypes
from ctypes import wintypes
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
sys.path.insert(0, str(Path(__file__).resolve().parent))
from game_packs import Pack, digest, require_game
from repack_probe164 import raw_names


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--game', type=Path, required=True)
    args = parser.parse_args()
    old, out, game = args.build.resolve(), args.output.resolve(), args.game.resolve()
    require_game(game)
    if out.exists() or out == game or out.is_relative_to(game) or old.is_relative_to(out):
        parser.error('Use a new output directory separate from the game and inputs')
    prior = json.loads((old / 'verification.json').read_text())
    source = old / prior['pack']
    if prior['test'] != 164 or prior['public_package_ready'] or prior['game_changed']:
        raise ValueError('Not the verified private probe')
    if digest(source) != prior['pack_sha256']:
        raise ValueError('Input differs from verified probe')
    before = source.read_bytes()
    names, payload_start = raw_names(before)
    if struct.unpack_from('<Q', before, 24)[0] != 0:
        raise ValueError('This correction requires the observed zero timestamp')
    with (game / 'data/data.pack').open('rb') as stream:
        stock_header = stream.read(32)
    if len(stock_header) != 32 or stock_header[:4] != b'PFH3':
        raise ValueError('Unexpected owned-stock header')
    stamp = stock_header[24:32]
    if not struct.unpack('<Q', stamp)[0]:
        raise ValueError('Owned-stock timestamp is also zero')
    after = before[:24] + stamp + before[32:]
    changes = [i for i, pair in enumerate(zip(before, after)) if pair[0] != pair[1]]
    if not changes or any(i < 24 or i >= 32 for i in changes):
        raise ValueError('Change exceeded timestamp field')
    if before[32:] != after[32:] or raw_names(after) != (names, payload_start):
        raise ValueError('Index or payload changed')

    class FileTime(ctypes.Structure):
        _fields_ = [('low', wintypes.DWORD), ('high', wintypes.DWORD)]

    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel.CompareFileTime.argtypes = [ctypes.POINTER(FileTime), ctypes.POINTER(FileTime)]
    kernel.CompareFileTime.restype = wintypes.LONG
    missing = FileTime(0, 0)
    stamped = FileTime(*struct.unpack('<II', stamp))
    zero_result = kernel.CompareFileTime(ctypes.byref(missing), ctypes.byref(missing))
    corrected_result = kernel.CompareFileTime(ctypes.byref(missing), ctypes.byref(stamped))
    if zero_result != 0 or corrected_result != -1:
        raise ValueError('Native timestamp branch comparison differs')

    out.mkdir(parents=True)
    target = out / prior['pack']
    target.write_bytes(after)
    old_pack, new_pack = Pack(source), Pack(target)
    if old_pack.entries != new_pack.entries:
        raise ValueError('Entry offsets or sizes changed')
    for name, (offset, size) in old_pack.entries.items():
        if before[offset:offset + size] != after[offset:offset + size]:
            raise ValueError('Entry payload changed')
    result = dict(prior)
    result.update({
        'status': 'TIMESTAMP-ONLY CORRECTION PASS; exact native/Lua evidence retained, not freshly rerun',
        'pack_sha256': digest(target), 'repacked_from_sha256': prior['pack_sha256'],
        'stock_timestamp_hex': stamp.hex(), 'changed_byte_offsets': changes,
        'native_windows_comparisons': {'zero': zero_result, 'corrected': corrected_result},
        'fresh_checks': ['only timestamp bytes changed', 'raw index byte-exact',
                         'complete payload byte-exact', 'all entry offsets/sizes retained',
                         'actual Windows CompareFileTime branch comparison'],
        'retained_verification_path': str(old / 'verification.json'),
        'retimestamp_tool_sha256': digest(Path(__file__)),
        'live_causality_verified': False, 'game_changed': False,
        'game_launched': False, 'uploaded': False,
    })
    (out / 'verification.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: result[key] for key in
                     ['status', 'pack_sha256', 'native_sha256', 'changed_byte_offsets',
                      'stock_timestamp_hex', 'native_windows_comparisons']}, indent=2))


if __name__ == '__main__':
    main()
