"""Data-only staging helpers. Importing this module never starts collection."""
import ast
import hashlib
import json
import shutil
from pathlib import Path

FILES = ('process_probe.py', 'process_memory.py', 'native_input.py',
         'accuracy_read90.py', 'actor_roles.py', 'inspect_minidump.py',
         'sensors.py', 'events.ps1', 'session.py', 'lifecycle110.py',
         'watcher.py', 'control.py', 'binary_policy111.py', 'deployment.py')
ENTRY_POINTS = ('watcher', 'session', 'native_input', 'control', 'inspect_minidump')

def dependency_closure(source):
    """Check both import syntaxes without importing an entry point."""
    source = Path(source)
    missing = []
    for name in FILES:
        if not name.endswith('.py'):
            continue
        tree = ast.parse((source / name).read_text(encoding='utf-8'), filename=name)
        for node in ast.walk(tree):
            modules = ([node.module.split('.')[0]] if isinstance(node, ast.ImportFrom) and node.module
                       else [n.name.split('.')[0] for n in node.names] if isinstance(node, ast.Import) else [])
            for module in modules:
                if (source / (module + '.py')).is_file() and module + '.py' not in FILES:
                    missing.append((name, module))
    if missing:
        raise ValueError('Missing deployed dependencies: ' + repr(missing))
    return {'files': len(FILES), 'missing': []}

def stage(source, output, debugger, game, recordings, python, baseline=None):
    source, output = Path(source).resolve(), Path(output).resolve()
    from binary_policy111 import require_capture_only111
    dependency_closure(source)
    require_capture_only111(debugger)
    game, recordings, python = Path(game).resolve(), Path(recordings).resolve(), Path(python).resolve()
    if not python.is_file() or not (game / 'Empire.Retail.dll').is_file():
        raise ValueError('Supply a Python executable and legitimate game directory')
    from lifecycle110 import ENGINE_SHA
    with (game / 'Empire.Retail.dll').open('rb') as stream:
        if hashlib.file_digest(stream, 'sha256').hexdigest() != ENGINE_SHA:
            raise ValueError('Unsupported original-game engine fingerprint')
    if output.exists():
        raise ValueError('Staging destination must be new; existing deployment preserved')
    if output == game or output.is_relative_to(game) or source.is_relative_to(output) or output.is_relative_to(source):
        raise ValueError('Stage outside the game and diagnostic source directories')
    output.mkdir(parents=True)
    for name in FILES:
        shutil.copy2(source / name, output / name)
    (output / 'native').mkdir()
    shutil.copy2(debugger, output / 'native/crash-recorder.exe')
    (output / 'config.json').write_text(json.dumps({'recordings': str(recordings)}, indent=2) + '\n', encoding='utf-8')
    data = dict(baseline or {})
    data['game_dir'] = str(game)
    data.setdefault('module_names', {}).setdefault('empire.retail.dll', ENGINE_SHA)
    (output / 'baseline.json').write_text(json.dumps(data, indent=2) + '\n', encoding='utf-8')
    receipt = {'schema': 1, 'python': str(python), 'root': str(output),
               'autorun_enabled': False, 'watcher_started': False,
               'native_input_roles': 'generated shared final-human policy',
               'files': {str(p.relative_to(output)): hashlib.sha256(p.read_bytes()).hexdigest()
                         for p in output.rglob('*') if p.is_file()}}
    (output / 'installation.json').write_text(json.dumps(receipt, indent=2) + '\n', encoding='utf-8')
    return receipt
