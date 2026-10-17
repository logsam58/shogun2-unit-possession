"""Synthetic minidump format/context regression cases; no private recordings required."""
import json,sys,struct,tempfile,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/diagnostics/source'))
from inspect_minidump import analyze
def fixture(kind,exception=True):
 d=bytearray(256);streams=[]
 def alloc(raw):
  p=len(d);d.extend(raw);return p
 def stream(k,raw):
  p=alloc(raw);streams.append((k,len(raw),p));return p
 def context(eip):
  c=bytearray(204)
  for off,value in [(184,eip),(196,0x5000),(180,0x5010)]:struct.pack_into('<I',c,off,value)
  return alloc(c)
 ctx=context(0x11110010);name='test.dll'.encode('utf-16le');nr=alloc(struct.pack('<I',len(name))+name);mod=bytearray(112);struct.pack_into('<I',mod,0,1);struct.pack_into('<QII',mod,4,0x11110000,0x1000,0);struct.pack_into('<I',mod,24,nr);stream(4,mod)
 stack=bytearray(64);struct.pack_into('<I',stack,0,0x11110030);struct.pack_into('<II',stack,16,0x5020,0x11110040);struct.pack_into('<II',stack,32,0,0x11110050)
 stack_rva=0
 if kind=='embedded':stack_rva=alloc(stack)
 elif kind=='MemoryList':
  sr=alloc(stack);stream(5,struct.pack('<IQII',1,0x5000,64,sr))
 elif kind in ('Memory64','split64','truncated'):
  ranges=[(0x5000,64)] if kind!='split64' else [(0x5000,18),(0x5012,46)]
  sr=stream(9,struct.pack('<QQ',len(ranges),0)+b''.join(struct.pack('<QQ',*r) for r in ranges));off=alloc(stack);struct.pack_into('<Q',d,sr+8,off if kind!='truncated' else 999999)
 tr=bytearray(52);struct.pack_into('<I',tr,0,1);struct.pack_into('<I',tr,4,42);struct.pack_into('<QII',tr,28,0x5000,64,stack_rva);struct.pack_into('<II',tr,44,204,ctx);stream(3,tr)
 if exception:
  er=bytearray(168);struct.pack_into('<I',er,0,42);struct.pack_into('<I',er,8,0xc0000005);struct.pack_into('<Q',er,24,0x11110020);struct.pack_into('<II',er,160,204,context(0x11110020));stream(6,er)
 directory=alloc(b''.join(struct.pack('<III',*s) for s in streams));struct.pack_into('<4sIII',d,0,b'MDMP',0xa793,len(streams),directory);return d

class DumpReaderTests(unittest.TestCase):
 def check_kind(self,kind):
  with tempfile.TemporaryDirectory() as td:
   p=Path(td)/(kind+'.dmp');p.write_bytes(fixture(kind));r=analyze(p);t=r['threads'][0]
   self.assertEqual(t['context_source'],'ExceptionStream');self.assertEqual(t['eip'],'0x11110020');self.assertEqual(t['thread_list_registers']['eip'],'0x11110010')
   if kind=='missing':
    self.assertEqual(t['bounded_frame_pointer_chain'],[]);self.assertEqual(t['stack_word_candidates_not_unwound'],[]);self.assertTrue(t['stack_read_errors'])
   else:
    self.assertEqual(t['bounded_frame_pointer_chain'],['test.dll+0x40','test.dll+0x50']);self.assertEqual(t['stack_word_candidates_not_unwound'][0],{'stack_offset':'0x0','possible_module_address':'test.dll+0x30'})
 def test_embedded_thread_stack(self):self.check_kind('embedded')
 def test_memory_list(self):self.check_kind('MemoryList')
 def test_full_memory_zero_thread_stack_rva(self):self.check_kind('Memory64')
 def test_adjacent_memory64_regions(self):self.check_kind('split64')
 def test_missing_stack_is_explicit(self):self.check_kind('missing')
 def test_thread_context_without_exception(self):
  with tempfile.TemporaryDirectory() as td:
   p=Path(td)/'thread.dmp';p.write_bytes(fixture('Memory64',False));self.assertEqual(analyze(p)['threads'][0]['context_source'],'ThreadList')
 def test_truncated_memory64_rejected(self):
  with tempfile.TemporaryDirectory() as td:
   p=Path(td)/'truncated.dmp';p.write_bytes(fixture('truncated'))
   with self.assertRaisesRegex(ValueError,'Truncated Memory64'):analyze(p)
if __name__=='__main__':unittest.main()
