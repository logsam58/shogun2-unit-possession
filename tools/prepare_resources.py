"""Recreate local UI resources from a supported owned game. No install/launch."""
import argparse
import concurrent.futures
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from game_packs import Pack, digest, require_game

def run(command):
    result = subprocess.run([str(x) for x in command], capture_output=True, text=True,
                            creationflags=0x08000000 if sys.platform == 'win32' else 0)
    if result.returncode:
        raise RuntimeError('Resource tool failed: ' + str(command[0]) + '\n' + result.stderr[-3000:] + result.stdout[-1000:])
    return result.stdout

def save(path, data):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)

def document(path, value):
    save(path, (json.dumps(value, indent=2, ensure_ascii=False) + '\n').encode('utf-8'))

def sprite_header(rows, count):
    lines = ['#pragma once', 'namespace hud80 {',
             'struct Sprite { const wchar_t* key; int atlas,x,y,w,h,logical_w,logical_h,offset_x,offset_y; };',
             'inline const Sprite sprites[]={']
    lines += ['{L"' + key + '",' + ','.join(str(v) for v in values) + '},' for key, values in rows]
    lines += ['};', f'inline constexpr int atlas_count={count};',
              'inline const Sprite* sprite(const wchar_t* key){for(const auto& s:sprites)if(wcscmp(s.key,key)==0)return &s;return nullptr;}', '}', '']
    return '\n'.join(lines)

def pack_atlas(args, output, label, entries, width, height):
    folder = output / label
    folder.mkdir()
    options = dict(textureName=label, width=width, height=height, fixedSize=False,
                   powerOfTwo=False, padding=2, extrude=0, allowRotation=False,
                   detectIdentical=False, allowTrim=True, trimMode='trim', alphaThreshold=0,
                   removeFileExtension=False, prependFolderName=True, textureFormat='png',
                   base64Export=False, scale=1, scaleMethod='NEAREST_NEIGHBOR',
                   exporter='JsonHash', filter='none', packer='MaxRectsBin', packerMethod='BestShortSideFit')
    project = folder / (label + '.ftpp')
    document(project, {'savePath': str(folder), 'images': [{'name': e['key'], 'path': str(e['trimmed'])} for e in entries],
                       'folders': [], 'packOptions': options})
    run([args.node, args.packer, '--project', project, '--output', folder])
    metadata = sorted(folder.glob(label + '*.json'))
    if not metadata:
        raise ValueError('Texture packer produced no atlas metadata')
    expected = {e['key']: e for e in entries}
    seen, rows, images = set(), [], []
    for index, path in enumerate(metadata):
        data = json.loads(path.read_text(encoding='utf-8'))
        image = folder / data['meta']['image']
        if not image.is_file() or not image.stat().st_size:
            raise ValueError('Missing atlas PNG: ' + image.name)
        images.append(image)
        size = data['meta']['size']
        for key, value in data['frames'].items():
            if key not in expected or key in seen or value['rotated'] or not isinstance(value['trimmed'], bool):
                raise ValueError('Unknown, duplicate or rotated atlas sprite: ' + key)
            seen.add(key)
            frame, source = value['frame'], value['spriteSourceSize']
            if not value.get('sourceSize') or any(not isinstance(frame[n], int) for n in ('x', 'y', 'w', 'h')):
                raise ValueError('Missing atlas alignment metadata: ' + key)
            if frame['x'] < 0 or frame['y'] < 0 or frame['w'] <= 0 or frame['h'] <= 0 or frame['x'] + frame['w'] > size['w'] or frame['y'] + frame['h'] > size['h']:
                raise ValueError('Out-of-bounds atlas frame: ' + key)
            original = expected[key]
            trim = original['trim']
            # The packer deliberately retains the padded 5x5 image for a fully
            # transparent 1x1 sprite. The accepted importer also uses -2 offsets
            # for this case. Do not invent a visible 1x1 placeholder.
            dimensions = (frame['w'], frame['h'])
            if value['trimmed']:
                valid_dimensions = dimensions == (trim['w'], trim['h'])
            else:
                valid_dimensions = dimensions == (trim['w'] + 4, trim['h'] + 4) and source['x'] == source['y'] == 0
            if not valid_dimensions:
                raise ValueError('Unexpected packer trim geometry: ' + key)
            rows.append((key, [index, frame['x'], frame['y'], frame['w'], frame['h'],
                               original['sourceSize']['w'], original['sourceSize']['h'],
                               trim['x'] + source['x'] - 2, trim['y'] + source['y'] - 2]))
    if seen != set(expected):
        raise ValueError('Some raw PNGs are missing from the packed atlas')
    return images, rows

def prepare(args):
    game, output = args.game.resolve(), args.output_dir.resolve()
    require_game(game)
    if output == game or output.is_relative_to(game) or output == ROOT or ROOT.is_relative_to(output):
        raise ValueError('Use a separate build directory outside the game and repository ancestors')
    if output.exists():
        raise ValueError('Use a new resource output directory; existing data is preserved')
    recipe = json.loads((ROOT / 'tools/resource-recipe.json').read_text(encoding='utf-8'))
    versions = {'python': sys.version.split()[0], 'magick': run([args.magick, '-version']).splitlines()[0],
                'node': run([args.node, '-v']).strip(), 'ffmpeg': run([args.ffmpeg, '-version']).splitlines()[0]}
    packer_package = json.loads((args.packer.parent / 'package.json').read_text(encoding='utf-8'))
    if packer_package['name'] != 'free-tex-packer-cli' or packer_package['version'] != '0.3.0':
        raise ValueError('Use the lockfile-pinned Free Texture Packer CLI 0.3.0')
    versions['packer'] = packer_package['version']
    if 'ImageMagick 7.1.2-24 ' not in versions['magick']:
        raise ValueError('The verified image recipe uses ImageMagick 7.1.2-24; review/pin output changes before using another version')
    output.mkdir(parents=True)
    prepared = output / 'prepared'
    packs = {name: Pack(game / 'data' / name) for name in sorted({r['pack'] for r in recipe['images'] + recipe['fonts'] + recipe['ability_images']})}
    def extract(row):
        data = packs[row['pack']].read(row['entry'], row['bytes'], row['sha256'])
        original = output / 'raw-game' / row['pack'] / row['entry']
        save(original, data)
        return original
    def decode(row):
        original = extract(row)
        png = output / 'raw' / row['key']
        png.parent.mkdir(parents=True, exist_ok=True)
        run([args.magick, original, '-strip', '-define', 'png:exclude-chunks=date,time', png])
        return {'key': row['key'], 'raw': png, 'source_sha256': digest(png)}
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as executor:
        decoded = list(executor.map(decode, recipe['images']))
    print('Decoded ' + str(len(decoded)) + ' exact local game UI entries', flush=True)
    by_key = {e['key']: e for e in decoded}
    for alias in recipe['aliases']:
        original = by_key[alias['source']]
        png = output / 'raw' / alias['key']
        save(png, original['raw'].read_bytes())
        decoded.append({'key': alias['key'], 'raw': png, 'source_sha256': digest(png)})
    def trim(entry):
        target = output / 'trimmed' / entry['key']
        target.parent.mkdir(parents=True, exist_ok=True)
        text = run([args.magick, entry['raw'], '-format', '%w %h\\n', '-write', 'info:',
                    '-trim', '-format', '%w %h %X %Y\\n', '-write', 'info:', '+repage',
                    '-bordercolor', 'none', '-border', '2', '-strip', target])
        lines = text.replace('\\n', '\n').splitlines()
        w, h = map(int, lines[0].split())
        tw, th, x, y = map(int, lines[1].split())
        if digest(entry['raw']) != entry['source_sha256'] or not target.stat().st_size:
            raise ValueError('Raw source changed or trim is empty: ' + entry['key'])
        return {**entry, 'trimmed': target, 'sourceSize': {'w': w, 'h': h}, 'trim': {'x': x, 'y': y, 'w': tw, 'h': th}}
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as executor:
        trimmed = list(executor.map(trim, sorted(decoded, key=lambda e: e['key'])))
    document(output / 'trim-report.json', [{k: (str(v.relative_to(output)) if isinstance(v, Path) else v) for k, v in row.items()} for row in trimmed])
    atlas_images, sprites = pack_atlas(args, output, 'game-atlas', trimmed, 2048, 2048)
    geometry = {key: values[3:] for key, values in sprites}
    for row in recipe['images']:
        if geometry[row['key']] != row['expected_geometry']:
            raise ValueError('Native sprite geometry differs from accepted import: ' + row['key'])
    qte_set = set(recipe['qte_keys'])
    qte_images, qte_sprites = pack_atlas(args, output, 'qte-atlas', [e for e in trimmed if e['key'] in qte_set], 512, 512)
    authentic = Path('native/outputs/shogun2-authentic-hud80-preview')
    hud = Path('native/outputs/shogun2-ammunition83-preview/hud-src')
    for index, image in enumerate(atlas_images):
        save(prepared / authentic / f'assets/processed/atlases/game-atlas-{index}.png', image.read_bytes())
    if len(qte_images) != 1:
        raise ValueError('Expected one bounded QTE atlas')
    save(prepared / authentic / 'assets/processed/qte-atlas/qte-atlas.png', qte_images[0].read_bytes())
    for folder in (authentic / 'src', hud):
        save(prepared / folder / 'sprites.generated.h', sprite_header(sprites, len(atlas_images)).encode('utf-8'))
        save(prepared / folder / 'qte-sprites.generated.h', sprite_header(qte_sprites, 1).encode('utf-8'))
    for row in recipe['fonts']:
        save(prepared / row['output'], extract(row).read_bytes())
    for row in recipe['ability_images']:
        original = extract(row)
        png = prepared / row['output']
        png.parent.mkdir(parents=True, exist_ok=True)
        run([args.magick, original, '-strip', png])
        rgba = subprocess.check_output([str(args.magick), str(png), '-depth', '8', 'RGBA:-'])
        if hashlib.sha256(rgba).hexdigest() != row['rgba_sha256']:
            raise ValueError('Ability pixels differ from accepted native icon: ' + str(row['id']))
    rc = ''.join(f'{101+i} RCDATA "../../shogun2-authentic-hud80-preview/assets/processed/atlases/game-atlas-{i}.png"\n' for i in range(len(atlas_images)))
    rc += '#include "native-fonts.rc"\n301 WAVE "private/shameful-display109.wav"\n430 RCDATA "private/ability141-30.png"\n431 RCDATA "private/ability141-31.png"\n'
    save(prepared / hud / 'hud-assets.rc', rc.encode('utf-8'))
    run([sys.executable, ROOT / 'tools/prepare_audio109.py', '--game', game, '--ffmpeg', args.ffmpeg, '--output-root', prepared])
    files = [{'path': str(p.relative_to(prepared)).replace('\\', '/'), 'bytes': p.stat().st_size, 'sha256': digest(p)}
             for p in sorted(prepared.rglob('*')) if p.is_file()]
    report = {'schema': 1, 'profile': 'local-game', 'recipe_sha256': digest(ROOT / 'tools/resource-recipe.json'),
              'engine_sha256': recipe['engine_sha256'], 'tools': versions, 'files': files,
              'raw_png_count': len(decoded), 'trimmed_png_count': len(trimmed),
              'atlas_png_files': [str(p.relative_to(output)) for p in atlas_images + qte_images],
              'atlas_json_files': [str(p.relative_to(output)) for p in output.glob('*-atlas/*.json')],
              'skipped_files': [], 'errors': [], 'old_release_resources_used': False,
              'native_status_icon_profile': True, 'game_changed': False, 'public_binary_redistribution_approved': False}
    document(output / 'resource-manifest.json', report)
    print(json.dumps({k: report[k] for k in ('raw_png_count', 'trimmed_png_count', 'atlas_png_files', 'atlas_json_files', 'skipped_files', 'errors')}, indent=2))

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('game', 'output-dir', 'magick', 'node', 'packer', 'ffmpeg'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--jobs', type=int, default=2)
    args = parser.parse_args()
    if not 1 <= args.jobs <= 4:
        parser.error('--jobs must be between 1 and 4')
    prepare(args)

if __name__ == '__main__':
    main()
