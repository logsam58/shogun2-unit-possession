import ast
import importlib.util
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'tools/diagnostics/source'
sys.path.insert(0, str(SOURCE))
sys.path.insert(0, str(ROOT / 'tools'))
from actor_roles import HUMAN_TABLES, HORSE_TABLE, human, combat_actor
from deployment import FILES, ENTRY_POINTS, dependency_closure, stage
from generate_roles import render

class RoleTests(unittest.TestCase):
    def test_generated_policy(self):
        self.assertEqual((SOURCE / 'actor_roles.py').read_text(encoding='utf-8'), render())
        for base in (0, 0x10000000, 0x55000000):
            for rva in HUMAN_TABLES:
                self.assertTrue(human(base, base + rva))
            self.assertFalse(human(base, base + HORSE_TABLE))
            self.assertTrue(combat_actor(base, base + HORSE_TABLE))
            for delta in (0, 1, 0x15df000, 0x15ffffff):
                self.assertFalse(combat_actor(base, base + delta))
            self.assertFalse(human(base, base - 1))

    def test_production_native_snapshot(self):
        # Compile just the unmodified production function. Do not import/run the
        # collector's module-level entry point or open a Windows process.
        tree = ast.parse((SOURCE / 'native_input.py').read_text(encoding='utf-8'))
        function = next(n for n in tree.body if isinstance(n, ast.FunctionDef) and n.name == 'snapshot')
        base = 0x10000000
        memory = {}
        def put(address, value):
            memory[address] = struct.pack('<I', value)
        def blob(address, size):
            result = bytearray(size)
            for pos, data in memory.items():
                if address <= pos and pos + len(data) <= address + size:
                    result[pos-address:pos-address+len(data)] = data
            return bytes(result)
        class Reader:
            def u32(self, address): return struct.unpack('<I', blob(address, 4))[0]
            def read(self, address, size): return blob(address, size)
        root, env, manager, camera, actor = [base + n for n in (0x1000, 0x2000, 0x3000, 0x4000, 0x5000)]
        put(base + 0x1bc8180, root); put(root + 0x110, env)
        put(env + 0x281fc, manager); put(manager + 0x8e0, camera)
        put(camera, base + 0x15d22e8); put(camera + 0x188, actor)
        put(actor + 0x1c8, 19); put(actor + 0xf0, 1); put(actor + 0x214, base + 0x6000)
        raw = bytearray(84); struct.pack_into('<I', raw, 72, 19)
        namespace = dict(p=Reader(), base=base, struct=struct, human=human, combat_actor=combat_actor)
        exec(compile(ast.Module(body=[function], type_ignores=[]), 'production-snapshot', 'exec'), namespace)
        for role in HUMAN_TABLES:
            put(actor, base + role)
            self.assertEqual(namespace['snapshot'](raw)['actor'], actor)
            put(actor + 0x1c8, 20)
            self.assertFalse(namespace['snapshot'](raw)['actor_valid'])
            put(actor + 0x1c8, 19)
        for role in (0, HORSE_TABLE, 0x15df000):
            put(actor, base + role)
            self.assertFalse(namespace['snapshot'](raw)['actor_valid'])

class DeploymentTests(unittest.TestCase):
    def test_dependency_closure(self):
        self.assertEqual(dependency_closure(SOURCE)['missing'], [])
        self.assertIn('accuracy_read90.py', FILES)
        self.assertIn('actor_roles.py', FILES)
        # A true omitted dependency, including ordinary import syntax, fails.
        import shutil
        with tempfile.TemporaryDirectory() as folder:
            target = Path(folder)
            for name in FILES: shutil.copy2(SOURCE / name, target / name)
            (target / 'new_dependency.py').write_text('value=1\n')
            with (target / 'native_input.py').open('a') as stream: stream.write('\nimport new_dependency\n')
            with self.assertRaises(ValueError): dependency_closure(target)

    @unittest.skipUnless(os.name == 'nt', 'Windows-only entry points')
    def test_fresh_staged_entry_points(self):
        # Source-only deployment to a new isolated directory; no debugger run.
        import shutil
        with tempfile.TemporaryDirectory(prefix='s2fp staged ') as folder:
            target = Path(folder)
            for name in FILES: shutil.copy2(SOURCE / name, target / name)
            for entry in ENTRY_POINTS:
                code = ("import sys,runpy;sys.path.insert(0,sys.argv[1]);"
                        "import process_probe;"
                        "exec('def forbidden(*a,**k): raise RuntimeError(\"process access forbidden in smoke test\")');"
                        "process_probe.Process=forbidden;process_probe.processes=forbidden;process_probe.modules=forbidden;"
                        "name=sys.argv[2];sys.argv=[name,'--help'];runpy.run_module(name,run_name='__main__')")
                result = subprocess.run([sys.executable, '-I', '-B', '-c', code, str(target), entry],
                                        cwd=target, text=True, capture_output=True, timeout=15)
                self.assertEqual(result.returncode, 0, entry + ': ' + result.stderr)
                self.assertIn('usage:', result.stdout.lower())
            self.assertFalse(list(target.glob('*.jsonl')))
            self.assertFalse((target / 'watcher-health.json').exists())

if __name__ == '__main__':
    unittest.main()
