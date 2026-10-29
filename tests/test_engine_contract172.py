"""Portable regression checks for the shared native dependency boundary."""
import copy
import json
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools/workshop'))
import check_engine_contract172 as checker
from generate_engine_contract172 import ENTRIES


class EngineContractTests(unittest.TestCase):
    def manifest(self):
        return json.loads((ROOT/'tools/data/engine-contract172.json').read_text())

    def rejects(self,manifest):
        with patch.object(checker.json,'loads',return_value=manifest):
            with self.assertRaises(ValueError):
                checker.check()

    def test_current_compiled_dependencies_are_covered(self):
        self.assertTrue(checker.check()['passed'])
        for name in ENTRIES:
            text=(ROOT/name).read_text(encoding='utf-8-sig')
            self.assertNotIn('engine170::supported(',text)
            self.assertNotIn('022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5',text)
            self.assertIn('engine172::admit(',text)

    def test_stale_component_or_header_is_refused(self):
        for field in ('source','header','schema'):
            m=self.manifest()
            if field=='source':
                m['source_files'][ENTRIES[2]]='0'*64
            elif field=='header':
                m['generated_header_sha256']='0'*64
            else:
                m['schema']=0
            self.rejects(m)

    def test_missing_binding_or_protected_region_is_refused(self):
        m=self.manifest();victim=m['bindings'][0]
        m['bindings']=[v for v in m['bindings'] if (v['source'],v['value'])!=(victim['source'],victim['value'])]
        self.rejects(m)
        m=self.manifest();rv=int(m['bindings'][0]['value'],16)
        m['regions']=[r for r in m['regions'] if not r['rva']<=rv<r['rva']+r['size']]
        self.rejects(m)


if __name__=='__main__':
    unittest.main()
