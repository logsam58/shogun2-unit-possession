"""Verify and package the pinned Windows product build without installing or launching the game."""
import argparse
import ctypes
from ctypes import wintypes
import hashlib
import json
import re
import shutil
import struct
import subprocess
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / 'artifacts/bin'
VERSION = (ROOT / 'VERSION').read_text().strip()
if not re.fullmatch(r'[0-9]+\.[0-9]+(?:\.[0-9]+)?', VERSION):
    raise RuntimeError('Invalid product version')
TAG = 'v' + VERSION
SETUP = 'Shogun2-First-Person-' + TAG + '-Setup.exe'
ARCHIVE = 'Shogun2-First-Person-' + TAG + '-Windows.zip'
ENGINE_SHA = '022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5'
COMPONENTS = ['possession-v46.dll', 'possession-hud.exe', 'qte-playable-v79.dll']
RECEIPT = ROOT / 'docs/releases' / (TAG + '-validation.json')


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def sha(path):
    digest = hashlib.sha256()
    with path.open('rb') as stream:
        while chunk := stream.read(1024 * 1024):
            digest.update(chunk)
    return digest.hexdigest()


def record(path):
    return {'name': path.name, 'bytes': path.stat().st_size, 'sha256': sha(path)}


def git(*args):
    return subprocess.check_output(['git', *args], cwd=ROOT, text=True).strip()


def resource(path, identity):
    # Load as data only: no DllMain, hooks or executable code is run.
    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel.LoadLibraryExW.argtypes = [wintypes.LPCWSTR, wintypes.HANDLE, wintypes.DWORD]
    kernel.LoadLibraryExW.restype = wintypes.HMODULE
    kernel.FindResourceW.argtypes = [wintypes.HMODULE, ctypes.c_void_p, ctypes.c_void_p]
    kernel.FindResourceW.restype = ctypes.c_void_p
    kernel.SizeofResource.argtypes = [wintypes.HMODULE, ctypes.c_void_p]
    kernel.SizeofResource.restype = wintypes.DWORD
    kernel.LoadResource.argtypes = [wintypes.HMODULE, ctypes.c_void_p]
    kernel.LoadResource.restype = ctypes.c_void_p
    kernel.LockResource.argtypes = [ctypes.c_void_p]
    kernel.LockResource.restype = ctypes.c_void_p
    kernel.FreeLibrary.argtypes = [wintypes.HMODULE]
    kernel.FreeLibrary.restype = wintypes.BOOL
    module = kernel.LoadLibraryExW(str(path), None, 2)
    require(module, 'Cannot open PE resources: ' + path.name)
    try:
        found = kernel.FindResourceW(module, identity, 10)
        require(found, 'Missing embedded resource: ' + str(identity))
        size = kernel.SizeofResource(module, found)
        loaded = kernel.LoadResource(module, found)
        address = kernel.LockResource(loaded)
        require(address and size, 'Cannot read embedded resource')
        return ctypes.string_at(address, size)
    finally:
        kernel.FreeLibrary(module)


def pe_version(path):
    api = ctypes.WinDLL('version', use_last_error=True)
    api.GetFileVersionInfoSizeW.argtypes = [wintypes.LPCWSTR, ctypes.POINTER(wintypes.DWORD)]
    api.GetFileVersionInfoSizeW.restype = wintypes.DWORD
    api.GetFileVersionInfoW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD, ctypes.c_void_p]
    api.GetFileVersionInfoW.restype = wintypes.BOOL
    api.VerQueryValueW.argtypes = [ctypes.c_void_p, wintypes.LPCWSTR, ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(wintypes.UINT)]
    api.VerQueryValueW.restype = wintypes.BOOL
    unused = wintypes.DWORD()
    size = api.GetFileVersionInfoSizeW(str(path), ctypes.byref(unused))
    require(size, 'Missing installer version resource')
    buffer = ctypes.create_string_buffer(size)
    require(api.GetFileVersionInfoW(str(path), 0, size, buffer), 'Cannot read installer version')
    value = ctypes.c_void_p()
    length = wintypes.UINT()
    require(api.VerQueryValueW(buffer, '\\', ctypes.byref(value), ctypes.byref(length)), 'Missing fixed PE version')
    require(length.value >= 52, 'Truncated fixed PE version')
    fixed = ctypes.cast(value, ctypes.POINTER(wintypes.DWORD))
    require(fixed[0] == 0xFEEF04BD, 'Invalid fixed PE version')
    def version(ms, ls):
        return [ms >> 16, ms & 65535, ls >> 16, ls & 65535]
    result = {'file': version(fixed[2], fixed[3]), 'product': version(fixed[4], fixed[5])}
    expected = [int(x) for x in VERSION.split('.')]
    expected += [0] * (4 - len(expected))
    require(result['file'] == expected and result['product'] == expected, 'Installer version differs from VERSION')
    return result


def verify_inputs():
    require((ROOT / 'VERSION').read_text().strip() == VERSION, 'Unexpected product version')
    receipt = json.loads(RECEIPT.read_text())
    require(receipt['version'] == VERSION and receipt['passed'], 'Missing passing release receipt')
    require(all(item['passed'] for item in receipt['fresh_checks']), 'A release check failed')
    for item in receipt['build_inputs']:
        require(sha(ROOT / item['file']) == item['sha256'], 'Build source changed: ' + item['file'])
    for item in receipt['release_binaries']:
        require(record(BIN / item['name']) == item, 'Build output changed: ' + item['name'])
    if VERSION == '0.15':
        # Historical v0.15 used the exact imported gameplay source closure.
        provenance = json.loads((ROOT / 'docs/source-import.json').read_text())
        gameplay = [item for item in provenance['files'] if item['path'].startswith('native/outputs/')]
        require(len(gameplay) == receipt['gameplay']['source_files'], 'Gameplay source inventory differs')
        for item in gameplay:
            require(sha(ROOT / item['path']) == item['sha256'], 'Retained gameplay source changed: ' + item['path'])
    else:
        # Later builds intentionally modify gameplay. Pin the complete tracked
        # native/tool closure instead of rewriting the immutable import manifest.
        tracked = set(git('ls-files', 'native', 'tools').splitlines()) - {'tools/local.json'}
        pinned = {item['file'] for item in receipt['build_inputs']}
        require(tracked == pinned, 'Build source closure differs from the receipt')
    for item in receipt['gameplay']['components']:
        require(sha(BIN / item['name']) == item['sha256'], 'Retained gameplay binary changed')
    setup = BIN / 'Shogun2-Possession-Setup.exe'
    versions = pe_version(setup)
    for identity, name in [(201, 'dinput8.dll'), (202, 'marker.pack')]:
        require(hashlib.sha256(resource(setup, identity)).hexdigest() == sha(BIN / name), 'Installer payload differs: ' + name)
    for identity, name in zip([101, 102, 103], COMPONENTS):
        require(hashlib.sha256(resource(BIN / 'dinput8.dll', identity)).hexdigest() == sha(BIN / name), 'Loader payload differs: ' + name)
    pack = (BIN / 'marker.pack').read_bytes()
    header = struct.unpack_from('<4s5IQ', pack)
    require(header[0] == b'PFH3' and header[4] == 1, 'Invalid marker pack')
    marker = json.loads(pack[32 + header[5]:])
    require(marker['version'] == VERSION, 'Marker version differs')
    require(marker.get('revision') == receipt.get('candidate', 99) and bool(marker.get('development_candidate')) == bool(receipt.get('development_candidate', False)), 'Marker candidate differs')
    require(marker['adapter_sha256'] == sha(BIN / COMPONENTS[0]), 'Marker adapter hash differs')
    cache = hashlib.sha256(''.join(sha(BIN / n) for n in [*COMPONENTS, 'marker.pack']).encode()).hexdigest()[:16]
    require(cache == receipt['cache_id'], 'Cache identity differs')
    settings = ROOT / 'tools/local.json'
    local = json.loads(settings.read_text()) if settings.exists() else {}
    game = Path(local.get('game', 'C:/Program Files (x86)/Steam/steamapps/common/Total War SHOGUN 2'))
    require(sha(game / 'Empire.Retail.dll') == ENGINE_SHA, 'Unsupported local target engine')
    managed = [game / 'DINPUT8.dll', game / 'data/shogun2_possession_checkpoint45.pack', game / 'Empire.Retail.dll']
    before = {p: sha(p) if p.exists() else None for p in managed}
    result = subprocess.run([str(BIN / 'install_cli.exe'), str(game), '--check-only'], cwd=ROOT, capture_output=True, text=True, timeout=60)
    require(result.returncode == 0, 'Read-only installer target check failed: ' + result.stderr.strip())
    require(before == {p: sha(p) if p.exists() else None for p in managed}, 'Read-only target check changed files')
    return receipt, versions, {'passed': True, 'mode': '--check-only', 'managed_files_unchanged': True, 'output': result.stdout.strip()}


def web_links(text, relative):
    base = 'https://github.com/logsam58/shogun2-first-person/blob/' + TAG + '/'
    def link(match):
        label, target = match.groups()
        if '://' in target or target.startswith('#'):
            return match.group(0)
        # Resolve documentation paths without accessing external files.
        parts = list(relative.parent.parts)
        for part in target.split('/'):
            if part == '..':
                require(bool(parts), 'Documentation link escapes repository')
                parts.pop()
            elif part != '.':
                parts.append(part)
        return '[' + label + '](' + base + '/'.join(parts) + ')'
    return re.sub(r'\[([^\]]+)\]\(([^)]+)\)', link, text)


def main():
    global RECEIPT
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--verify-only', action='store_true', help='Check build inputs without requiring a committed tree or creating player downloads')
    parser.add_argument('--development-package', action='store_true', help='Keep an unpublished Test candidate in its own version/Test directory')
    parser.add_argument('--candidate', type=int, help='Use docs/development/TestN-validation.json without replacing a published release receipt')
    args = parser.parse_args()
    if args.candidate is not None:
        require(args.development_package and args.candidate >= 100, '--candidate requires --development-package and a Test number >=100')
        RECEIPT = ROOT / 'docs/development' / ('Test' + str(args.candidate) + '-validation.json')
    receipt, versions, target = verify_inputs()
    if args.verify_only:
        print(json.dumps({'passed': True, 'embedded_payloads_verified': 5, 'pe_version': versions, 'target_check': target}, indent=2))
        return
    require(not git('status', '--porcelain'), 'Commit and review the source before creating a release package')
    head = git('rev-parse', 'HEAD')
    if args.development_package:
        require(receipt.get('development_candidate') and isinstance(receipt.get('candidate'), int), 'Development package requires a numbered candidate receipt')
    folder = ROOT / 'artifacts/releases' / (TAG + ('-Test' + str(receipt['candidate']) if args.development_package else ''))
    folder.mkdir(parents=True, exist_ok=True)
    publication = folder / 'publication-receipt.json'
    require(not publication.exists(), 'Publication receipt already exists; refusing to replace release assets')
    shutil.copy2(BIN / 'Shogun2-Possession-Setup.exe', folder / SETUP)
    manifest = {
        'product': 'Shogun 2 First-Person Possession', 'version': VERSION, 'tag': TAG,
        'source_commit': head, 'cache_id': receipt['cache_id'], 'engine_sha256': ENGINE_SHA,
        'pe_version': versions, 'components': receipt['release_binaries'],
        'installer': record(folder / SETUP), 'validation': RECEIPT.relative_to(ROOT).as_posix(), 'candidate': receipt.get('candidate'), 'development_candidate': receipt.get('development_candidate', False),
        'validation_sha256': sha(RECEIPT), 'target_check': target,
        'game_installed_or_launched': False,
        'known_issues': ['Unresolved native effects and audio-thread crashes, including a blood-disabled capture; mod involvement undetermined', 'Mounted-bow presentation', 'Ultra unit size with heavy army presence can still cause severe slowdown; Large 1v1 max-DX11 play was reported smooth', 'Incomplete specialty-unit, roster and siege-map validation']
    }
    manifest_path = folder / 'release-manifest.json'
    manifest_path.write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8', newline='\n')
    members = {SETUP: (folder / SETUP).read_bytes(), 'release-manifest.json': manifest_path.read_bytes()}
    for source, name in [('README.md', 'README.md'), ('docs/KNOWN_ISSUES.md', 'KNOWN_ISSUES.md'), ('THIRD_PARTY_NOTICES.md', 'THIRD_PARTY_NOTICES.md'), ('docs/releases/' + TAG + '.md', 'RELEASE_NOTES.md')]:
        members[name] = web_links((ROOT / source).read_text(encoding='utf-8'), Path(source)).encode('utf-8')
    if args.candidate is not None:
        members['CANDIDATE_NOTES.md'] = (ROOT / 'docs/development' / ('Test' + str(args.candidate) + '.md')).read_bytes()
    members['LICENSE-MINHOOK.txt'] = (ROOT / 'native/outputs/shogun2-accuracy90-preview/vendor/LICENSE.txt').read_bytes()
    members['PACKAGE_SHA256SUMS.txt'] = ''.join(hashlib.sha256(data).hexdigest() + '  ' + name + '\n' for name, data in sorted(members.items())).encode()
    archive = folder / ARCHIVE
    with zipfile.ZipFile(archive, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as bundle:
        for name, data in sorted(members.items()):
            require('/' not in name and '\\' not in name and name not in {'.', '..'}, 'Unsafe package member')
            bundle.writestr(name, data)
    with zipfile.ZipFile(archive) as bundle:
        require(set(bundle.namelist()) == set(members), 'ZIP inventory differs')
        require(bundle.testzip() is None, 'ZIP CRC verification failed')
        for name, data in members.items():
            require(bundle.read(name) == data, 'ZIP contents differ: ' + name)
    sums = folder / 'SHA256SUMS.txt'
    sums.write_text(''.join(sha(p) + '  ' + p.name + '\n' for p in [folder / SETUP, archive, manifest_path]), encoding='ascii', newline='\n')
    assets = [record(p) for p in [folder / SETUP, archive, sums, manifest_path]]
    (folder / 'upload-manifest.json').write_text(json.dumps({'version': VERSION, 'source_commit': head, 'assets': assets, 'zip_members_verified': sorted(members)}, indent=2) + '\n')
    print(json.dumps({'passed': True, 'source_commit': head, 'assets': assets, 'zip_members_verified': len(members), 'game_installed_or_launched': False}, indent=2))


if __name__ == '__main__':
    try:
        main()
    except (RuntimeError, OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        raise SystemExit(str(error)) from None
