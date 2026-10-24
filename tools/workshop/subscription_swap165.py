"""Reversible owner-only exclusion for the real subscribed Test165. No launch."""
import argparse
import ctypes
import hashlib
import json
from pathlib import Path
import sys
import time

HERE = Path(__file__).resolve().parent
sys.path[:0] = [str(HERE), str(HERE.parent), str(HERE.parent / 'diagnostics/source')]
from game_packs import require_game
from stage_probe164 import plain, atomic
from process_probe import k, PROCESSENTRY32W

EXPECTED = {
    'dinput8.dll': '19f7c9b197cb8128a160aa8bd7b4e930b2612f420a5468413690bea55e791ae7',
    'data/shogun2_unit_possession_workshop165.pack': '8f26c9b14a4c732e90a207f2384afdc56958dfc3f23215d21a829b64c028a7f4',
    'data/shogun2_unit_possession_workshop165.png': '3ff5054d174957c3e5b41cfed8d53bca0e3fa4480560f7c5587b8603288728ae',
}


def digest(path):
    with Path(path).open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def active():
    handle = k.CreateToolhelp32Snapshot(2, 0)
    if handle == ctypes.c_void_p(-1).value:
        raise ctypes.WinError(ctypes.get_last_error())
    row = PROCESSENTRY32W()
    row.dwSize = ctypes.sizeof(row)
    found = []
    try:
        ok = k.Process32FirstW(handle, ctypes.byref(row))
        while ok:
            if row.szExeFile.lower() in ('shogun2.exe', 'shogun2.retail.exe', 'launcher.exe'):
                found.append(row.th32ProcessID)
            ok = k.Process32NextW(handle, ctypes.byref(row))
    finally:
        k.CloseHandle(handle)
    return found


def closed():
    if active():
        raise ValueError('Game or launcher running; targets preserved')


def save(folder, journal):
    atomic(folder / 'journal.json', (json.dumps(journal, indent=2) + '\n').encode())


def copy_new(source, destination):
    with destination.open('xb') as stream:
        stream.write(source.read_bytes())
        stream.flush()
        import os
        os.fsync(stream.fileno())


def paths(folder, journal, row):
    game = Path(journal['game']).absolute()
    relative = row['relative']
    if relative not in EXPECTED or row['sha256'] != EXPECTED[relative]:
        raise ValueError('Unrecognized target in journal')
    target = game / relative
    saved = folder / ('excluded-' + target.name)
    backup = folder / ('previous-' + target.name)
    for path in (game, target, folder, saved, backup):
        plain(path)
    if not target.resolve().is_relative_to(game.resolve()):
        raise ValueError('Target escaped game')
    if not backup.is_file() or digest(backup) != row['sha256']:
        raise ValueError('Rollback backup differs; preserve all targets')
    for path in (target, saved):
        if path.exists() and (not path.is_file() or digest(path) != row['sha256']):
            raise ValueError('Foreign target/exclusion preserved: ' + str(path))
    if not target.exists() and not saved.exists():
        raise ValueError('Missing both original and excluded file; preserve all targets')
    return target, saved, backup


def rollback(folder, journal):
    closed()
    rows = [paths(folder, journal, row) for row in journal['files']]
    for target, saved, backup in rows:
        retired = folder / ('steam-deployed-' + target.name)
        plain(retired)
        if saved.exists() and target.exists() and retired.exists():
            raise ValueError('Existing retirement destination; preserve all targets')
    for target, saved, backup in rows:
        if saved.exists():
            if target.exists():
                # A matching file deployed by Steam is preserved separately.
                target.rename(folder / ('steam-deployed-' + target.name))
            saved.rename(target)
    journal['restored'] = True
    save(folder, journal)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('action', choices=['prepare', 'stage', 'rollback'])
    parser.add_argument('--journal', type=Path, required=True)
    parser.add_argument('--game', type=Path)
    parser.add_argument('--closed', action='store_true')
    args = parser.parse_args()
    folder = args.journal.absolute()
    if args.action == 'prepare':
        if not args.game or folder.exists():
            parser.error('Supply --game and a new journal directory')
        game = args.game.absolute()
        plain(game)
        plain(folder)
        if folder == game or folder.is_relative_to(game) or game.is_relative_to(folder):
            raise ValueError('Journal must be separate from game')
        require_game(game)
        closed()
        rows = []
        for relative, expected in EXPECTED.items():
            target = game / relative
            plain(target)
            if not target.is_file() or digest(target) != expected:
                raise ValueError('Installed original differs; preserved: ' + relative)
            rows.append({'relative': relative, 'sha256': expected, 'bytes': target.stat().st_size})
        download = game.parents[1] / 'workshop/content/34330/3825625050'
        for row in rows[1:]:
            if digest(download / Path(row['relative']).name) != row['sha256']:
                raise ValueError('Steam subscribed download differs')
        folder.mkdir(parents=True)
        for row in rows:
            target = game / row['relative']
            copy_new(target, folder / ('previous-' + target.name))
        selection = Path.home() / 'AppData/Roaming/The Creative Assembly/Shogun2/scripts/user.script.txt'
        plain(selection)
        copy_new(selection, folder / 'previous-selection.bin')
        journal = {'test': 165, 'item': '3825625050', 'game': str(game),
                   'created_utc': time.strftime('%Y-%m-%dT%H:%M:%SZ', time.gmtime()),
                   'files': rows, 'selection_path': str(selection), 'selection_sha256': digest(selection),
                   'staged': False, 'restored': False, 'automatic_rollback': False,
                   'game_launched': False, 'recording_started': False}
        save(folder, journal)
        print('Prepared verified rollback only; game unchanged.')
        return
    if not args.closed:
        parser.error('A fresh human CLOSED is required: pass --closed only after that confirmation')
    closed()
    journal = json.loads((folder / 'journal.json').read_text())
    if journal['test'] != 165 or journal['item'] != '3825625050':
        raise ValueError('Unrecognized subscription journal')
    if args.action == 'rollback':
        rollback(folder, journal)
        print('Previous files restored. Workshop must be OFF before launching the previous build.')
        return
    if journal['staged'] or journal['restored']:
        raise ValueError('Journal already used')
    rows = [paths(folder, journal, row) for row in journal['files']]
    if any(not target.exists() or saved.exists() for target, saved, backup in rows):
        raise ValueError('Original/exclusion state differs; preserved')
    selection = Path(journal['selection_path'])
    if digest(selection) != journal['selection_sha256']:
        raise ValueError('Selection changed after preparation; preserved')
    try:
        for target, saved, backup in rows:
            target.rename(saved)  # Windows rename never replaces a destination.
        assert all(not target.exists() and digest(saved) == digest(backup)
                   for target, saved, backup in rows)
        assert digest(selection) == journal['selection_sha256']
        journal['staged'] = True
        save(folder, journal)
    except Exception:
        rollback(folder, journal)
        raise
    print('Legacy loader and local upload copies excluded. Steam subscription untouched. No game launch.')


if __name__ == '__main__':
    main()
