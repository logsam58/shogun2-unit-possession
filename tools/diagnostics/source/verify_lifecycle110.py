"""Synthetic read-model tests. No game, watcher, registry or installed recorder use."""
import json,struct,tempfile,unittest,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from lifecycle110 import stable_effects,native_context,History,decode_command

class Memory:
    def __init__(self):self.data={}
    def put(self,a,b):
        for i,v in enumerate(b):self.data[a+i]=v
    def words(self,a,*v):self.put(a,struct.pack('<'+'I'*len(v),*v))
    def read(self,a,n):
        try:return bytes(self.data[a+i] for i in range(n))
        except KeyError:raise OSError('Unmapped synthetic address')

class Cases(unittest.TestCase):
    def effects(self):
        m=Memory();base=0x10000000;m.words(base+0x1ca3464,0x20000,1)
        m.words(0x20000,0,0,0,0,0,0,1,1,0x30000);m.put(0x30000,bytes(48))
        m.words(0x30000,base+0x1764384);m.words(0x30028,0x40000,2)
        return m,base
    def test_active_owner(self):
        m,b=self.effects();r=stable_effects(m.read,b);self.assertTrue(r['stable']);self.assertTrue(r['owners'][0]['known_effects_vtable']);self.assertFalse(r['owners'][0]['empty_table'])
    def test_destroyed_and_reused(self):
        m,b=self.effects();m.words(0x30000,b+0x176433c);m.words(0x30028,0,0);r=stable_effects(m.read,b)
        self.assertTrue(r['owners'][0]['base_destructor_vtable']);m.words(0x30000,3);self.assertFalse(stable_effects(m.read,b)['owners'][0]['known_effects_vtable'])
    def test_global_bound(self):
        m,b=self.effects();m.words(b+0x1ca3464,0x20000,257);self.assertEqual(stable_effects(m.read,b)['reason'],'global_bound')
    def test_churn_rejected(self):
        m,b=self.effects();calls=0
        def read(a,n):
            nonlocal calls
            if a==b+0x1ca3464:
                calls+=1
                if calls==2:return struct.pack('<II',0x20000,0)
            return m.read(a,n)
        self.assertFalse(stable_effects(read,b)['stable'])
    def test_unreadable_owner(self):
        m,b=self.effects();m.words(0x20020,0x50000);self.assertFalse(stable_effects(m.read,b)['owners'][0]['stable'])
    def test_menu_without_mod(self):
        m=Memory();m.words(0x10000000+0x1bc8180,0);r=native_context(m.read,0x10000000);self.assertEqual(r['phase_class'],'no_battle_environment_observed')
    def test_phase_not_falsely_named(self):
        m=Memory();b=0x10000000;m.words(b+0x1bc8180,0x20000);m.words(0x20110,0x30000);m.words(0x30008,0x40000);m.words(0x400b4,0x50000);m.words(0x50014,3)
        r=native_context(m.read,b);self.assertEqual(r['phase_id'],3);self.assertNotIn('actor',r)
    def test_battle_selected_unit_without_possession(self):
        m=Memory();b=0x10000000
        for address,values in [(b+0x1bc8180,[0x20000]),(0x20110,[0x30000]),(0x30008,[0x40000]),(0x400b4,[0x50000]),(0x50014,[5]),(0x40050,[123]),(0x30000+0x281fc,[0x60000]),(0x30000+0x2826c,[0x70000]),(0x704a8,[0x80000]),(0x608e0,[0]),(0x90000,[0xa0000]),(0xa0004,[99]),(0xa0ce8,[4,2])]:m.words(address,*values)
        m.put(0x800b0,bytes(28));m.words(0x800b0,b+0x15c4da0);m.words(0x800c0,2,1,0x90000);m.put(0xa0eb0,b'\x01\x01')
        r=native_context(m.read,b);self.assertEqual(r['simulation_tick'],123);self.assertTrue(r['selection_snapshot_stable']);self.assertEqual(r['selected_units'][0]['serial'],99);self.assertEqual(r['selected_units'][0]['guard'],1);self.assertNotIn('actor',r)
    def test_history_budget_preserves_earlier_bytes(self):
        with tempfile.TemporaryDirectory() as d:
            h=History(d,budget=90,ring_size=2);h.emit({'event':'first','tick_ms':1});first=(Path(d)/'lifecycle110.jsonl').read_bytes()
            for i in range(5):h.emit({'event':'later','tick_ms':i+2,'padding':'x'*80})
            self.assertTrue(h.limited);self.assertTrue((Path(d)/'lifecycle110.jsonl').read_bytes().startswith(first));self.assertEqual(len(h.ring),2)
            self.assertTrue(h.trigger(40000,'synthetic'));self.assertFalse(h.trigger(40100,'rate'));h.close()
            self.assertEqual(len(list(Path(d).glob('*pretrigger*'))),1)
    def test_command_identity_stale_and_bounds(self):
        raw=bytearray(80);struct.pack_into('<4I',raw,0,0x30304355,100,80,42);struct.pack_into('<7I',raw,20,7,8,9,2,1,0,0);struct.pack_into('<Q',raw,48,5000)
        self.assertTrue(decode_command(raw,42,5500)['fresh']);self.assertFalse(decode_command(raw,42,7000)['fresh'])
        with self.assertRaises(ValueError):decode_command(raw,43,5500)
        struct.pack_into('<I',raw,32,9)
        with self.assertRaises(ValueError):decode_command(raw,42,5500)
    def test_non_finite_evidence_is_explicit(self):
        with tempfile.TemporaryDirectory() as d:
            h=History(d);h.emit({'event':'bad_pose','tick_ms':1,'position':[float('nan')]});h.close()
            row=json.loads((Path(d)/'lifecycle110.jsonl').read_text());self.assertEqual(row['position'][0],{'non_finite':'nan'})

if __name__=='__main__':unittest.main(verbosity=2)
