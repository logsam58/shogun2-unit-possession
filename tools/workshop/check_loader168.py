"""Optional offline source-patch compatibility check; never install or launch Shogun.

Obtain Dacadey/shogun2-loader separately at the recorded commit. Only its patch
install routines run, inside our synthetic child with an uninitialized mapped
engine. DllMain, Install, engine functions and hook bodies do not run. This is
not a combined gameplay test, nor a test of its optional crash handling.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from build_frontend import source_files
from game_packs import digest, require_game

COMMIT = '371a351bb960ac11d6ab732dcfddcf5e1ce49a51'
SOURCE_SHA = 'dbcb9592a2073b0cd5e02dd25583e0357e0f712d7e1bcc270bd6efd725bd4936'
SOURCES172 = {
    SOURCE_SHA: COMMIT,
    '441f749952a662d25f93fdc61abdcdf41c35ce7ec015cba586cfe1e2d37e5682':
        '583a2eb02ede15ea77e1e6fdbb9133f537af73d4',
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for key in ('source', 'game', 'components', 'output', 'vcvars'):
        parser.add_argument('--' + key, type=Path, required=True)
    parser.add_argument('--compat-engine',type=Path,help='Optional separately obtained exact reviewed ModDB image')
    args = parser.parse_args()
    game, out = args.game.resolve(), args.output.resolve()
    require_game(game)
    if out.exists() or out.is_relative_to(game) or ROOT.is_relative_to(out):
        parser.error('Use a new isolated output, outside the game/source ancestors')
    source_sha=digest(args.source)
    if source_sha not in SOURCES172:
        parser.error('Author source differs from both reviewed source revisions')
    author_commit=SOURCES172[source_sha]
    if args.compat_engine and digest(args.compat_engine)!='c4d91d74ce9ff8338cf61d5b021b03a2776830177a590fcac205509a42239545':
        parser.error('Compatibility fixture differs from reviewed ModDB image')
    out.mkdir(parents=True)
    snapshot = out / 'source'
    inputs = []
    for p in source_files(ROOT):
        rel = p.relative_to(ROOT)
        target = snapshot / rel
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(p, target)
        inputs.append({'path': rel.as_posix(), 'sha256': digest(p)})
    author = out / 'private-author-source.cpp'
    shutil.copyfile(args.source, author)
    catalog_paths = [
        p for p in (snapshot / 'native').rglob('*')
        if p.suffix in {'.h', '.cpp'} and not p.name.startswith('verify')
        and 'vendor' not in p.parts
    ] + [snapshot / 'tools/workshop/entry165.cpp']
    rvas = sorted({int(x, 16) for p in catalog_paths
                   for x in re.findall(r'\b0x([0-9a-fA-F]+)\b', p.read_text())
                   if 0x10000 <= int(x, 16) < 0x1d88000 - 16})
    catalog = out / 'catalog.h'
    catalog.write_text('static constexpr unsigned source_rvas168[]={'
                       + ','.join(hex(r) for r in rvas) + '};\n')
    objects = [args.components.resolve() / 'obj' / (n + '.obj')
               for n in ('buffer', 'hook', 'trampoline', 'hde32')]
    for obj in objects:
        if not obj.is_file():
            parser.error('Build the native components first; missing ' + str(obj))
    command = [
        'cl', '/nologo', '/std:c++17', '/EHsc', '/O2', '/MT', '/DS2FP_LOCAL_ASSETS',
        '/DNOMINMAX', '/D_CRT_SECURE_NO_WARNINGS',
        '/DS2UP_EXTERNAL_LOADER_SOURCE168="' + author.as_posix() + '"',
        '/DS2UP_RVA_CATALOG168="' + catalog.as_posix() + '"',
        snapshot / 'tools/workshop/verify_loader168.cpp', *objects,
        '/Fo' + str(out / 'verify.obj'), '/Fe' + str(out / 'verify.exe'), '/link',
        'bcrypt.lib', 'user32.lib', 'gdi32.lib', 'd3d11.lib', 'd3d9.lib',
        'dxgi.lib', 'd3dcompiler.lib', 'shell32.lib', 'dbghelp.lib',
    ]
    commands = [[args.vcvars.resolve()], command,
                [out / 'verify.exe', game / 'Empire.Retail.dll']]
    if args.compat_engine:
        commands.append([out / 'verify.exe',args.compat_engine.resolve()])
    for values in commands:
        if any(any(c in str(v) for c in '\r\n%&|<>^!') for v in values):
            parser.error('Unsupported command path')
    batch = out / 'run.cmd'
    batch.write_text('@echo off\ncall ' + subprocess.list2cmdline(list(map(str, commands[0])))
                     + '\nif errorlevel 1 exit /b %errorlevel%\n'
                     + ''.join(subprocess.list2cmdline(list(map(str, c)))
                               + '\nif errorlevel 1 exit /b %errorlevel%\n' for c in commands[1:]))
    images = ['Shogun2.exe', 'Empire.Retail.dll', 'mss32.dll', 'installscript.vdf']
    protected = {n: digest(game / n) for n in images}
    with (out / 'run.log').open('w') as log:
        result = subprocess.run([os.environ.get('COMSPEC', 'cmd.exe'), '/d', '/c', batch],
                                cwd=out, stdout=log, stderr=subprocess.STDOUT, timeout=180)
    unchanged = protected == {n: digest(game / n) for n in images}
    receipt = {
        'passed': result.returncode == 0 and unchanged,
        'author_commit': author_commit, 'author_source_sha256': source_sha,
        'reviewed_bugfix_combination_checked':bool(args.compat_engine),
        'patch_sites': 8, 'source_rva_windows': len(rvas),
        'source_files': inputs, 'minhook_objects': {p.name: digest(p) for p in objects},
        'game_files_unchanged': unchanged, 'protected': protected,
        'game_launched': False, 'engine_entry_or_gameplay_executed': False,
        'third_party_DllMain_or_Install_executed': False,
        'third_party_crash_handlers_or_statistics_threads_started': False,
        'combined_gameplay_compatibility_proven': False,
    }
    (out / 'verification.json').write_text(json.dumps(receipt, indent=2) + '\n')
    print((out / 'run.log').read_text(errors='replace')[-2000:])
    if not receipt['passed']:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
