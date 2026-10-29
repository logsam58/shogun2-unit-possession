"""Offline localized/missing voice checks using private owned-game fixtures."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from game_packs import Pack, digest, require_game


def entries(path):
    raw = path.read_bytes()
    count = struct.unpack_from('<I', raw, 16)[0]
    cursor = 84
    result = {}
    for _ in range(count):
        item, width, height, size, kind = struct.unpack_from('<5I', raw, cursor)
        data = raw[cursor + 84:cursor + 84 + size]
        assert hashlib.sha256(data).hexdigest().encode() == raw[cursor + 20:cursor + 84]
        result[item] = (width, height, data)
        cursor += 84 + size
    assert cursor == len(raw)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for key in ('game', 'components', 'output', 'reference'):
        parser.add_argument('--' + key, type=Path, required=True)
    args = parser.parse_args()
    game = args.game.resolve()
    require_game(game)
    out = args.output.resolve()
    assert not out.exists() and not out.is_relative_to(game) and not ROOT.is_relative_to(out)
    out.mkdir(parents=True)
    recipe = (ROOT / 'tools/installer/asset_recipe159.generated.h').read_text()
    font_specs = re.findall(r'\{L"local_en.pack","(font/[^"\n]+)",(\d+),"([a-f0-9]{64})"\}', recipe)
    assert len(font_specs) == 20
    english = Pack(game / 'data/local_en.pack')
    fonts = {name: english.read(name, int(size), sha) for name, size, sha in font_specs}
    packs = sorted(set(re.findall(r'\{L"([^"\n]+\.pack)"', recipe)) - {'local_en.pack'})
    helper = args.components / 'prepare-local-assets159.exe'
    sources = {name: digest(game / 'data' / name) for name in packs}
    sources['Empire.Retail.dll'] = digest(game / 'Empire.Retail.dll')

    def fixture(name):
        target = out / name
        (target / 'data').mkdir(parents=True)
        (target / 'Empire.Retail.dll').hardlink_to(game / 'Empire.Retail.dll')
        for pack in packs:
            (target / 'data' / pack).hardlink_to(game / 'data' / pack)
        return target

    def font_pack(target, localized=False, corrupt=False):
        data = dict(fonts)
        if localized or corrupt:
            key = next(iter(data))
            raw = bytearray(data[key])
            if corrupt:
                struct.pack_into('<I', raw, 28, 0xffffffff)
            else:
                count = struct.unpack_from('<H', raw, 26)[0]
                raw[0x20020 + count * 8] ^= 1
            data[key] = bytes(raw)
        index = b''.join(struct.pack('<I', len(value)) + name.replace('/', '\\').encode() + b'\0'
                         for name, value in data.items())
        target.write_bytes(struct.pack('<4s5IQ', b'PFH3', 3, 0, 0, len(data), len(index), 1)
                           + index + b''.join(data.values()))

    def run_case(name, installed, success):
        output = out / (name + '-resources')
        result = subprocess.run([helper, installed, output], capture_output=True, text=True,
                                encoding='utf-8', errors='replace', timeout=120)
        (out / (name + '.txt')).write_text(result.stdout + result.stderr, encoding='utf-8')
        assert (result.returncode == 0) == success, (name, result.returncode, result.stderr)
        if not success:
            assert not output.exists(), 'Failed extraction left resource clutter'
            assert 'compatible installed HUD fonts' in result.stderr
            return {'name': name, 'passed': True, 'refused_safely_without_staging': True}
        hud, qte = entries(output / 'hud-assets159.s2fp'), entries(output / 'qte-assets159.s2fp')
        assert set(hud) == set(range(101, 104)) | set(range(201, 221)) | {430, 431}
        assert set(qte) == {101} | set(range(201, 221))
        assert 301 not in hud
        for label in ('hud', 'qte'):
            name_exe = 'verify-asset-renderer159-' + label + '.exe'
            renderer = output / name_exe
            shutil.copy2(args.components.parent / 'tests' / name_exe, renderer)
            check = subprocess.run([renderer], capture_output=True, text=True, timeout=30)
            assert check.returncode == 0, check.stderr
        return {'name': name, 'passed': True, 'voice_absent_without_disabling_resources': True,
                'actual_production_HUD_QTE_renderers_passed': True}

    localized = fixture('owned-localized-game')
    font_pack(localized / 'data/local_fr.pack', localized=True)
    results = [run_case('localized-without-English', localized, True)]
    no_voice = fixture('owned-English-fonts-without-voice')
    font_pack(no_voice / 'data/local_en.pack')
    results.append(run_case('English-fonts-missing-voice', no_voice, True))
    missing = fixture('owned-game-without-localization')
    results.append(run_case('missing-localization', missing, False))
    malformed = fixture('owned-game-with-malformed-localization')
    font_pack(malformed / 'data/local_zh.pack', corrupt=True)
    results.append(run_case('malformed-localization', malformed, False))
    # English assets must remain byte-exact to accepted Test166 output.
    for name in ('hud-assets159.s2fp', 'qte-assets159.s2fp'):
        assert digest(args.components / name) == digest(args.reference / name), name
    for name in packs:
        assert digest(game / 'data' / name) == sources[name]
    assert digest(game / 'Empire.Retail.dll') == sources['Empire.Retail.dll']
    receipt = {'passed': True, 'cases': results, 'English_resources_exact_to_Test166': True,
               'real_nonEnglish_game_installation_tested': False,
               'localized_fonts_are_private_derived_fixture_inputs': True,
               'owned_game_files_unchanged': True, 'game_launched': False}
    (out / 'verification.json').write_text(json.dumps(receipt, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(receipt, indent=2))


if __name__ == '__main__':
    main()
