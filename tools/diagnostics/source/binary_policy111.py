"""Fail closed on malformed PE files or a recorder containing invasive imports."""
import struct
from pathlib import Path

def recorder_imports111(path):
 data=Path(path).read_bytes()
 def need(value):
  if not value:raise ValueError('Malformed x86 recorder PE')
 def u16(p):need(0<=p<=len(data)-2);return struct.unpack_from('<H',data,p)[0]
 def u32(p):need(0<=p<=len(data)-4);return struct.unpack_from('<I',data,p)[0]
 def string(p):
  need(0<=p<len(data));end=data.find(b'\0',p,min(len(data),p+4096));need(end>=p)
  return data[p:end].decode('ascii')
 need(data[:2]==b'MZ');pe=u32(0x3c);need(data[pe:pe+4]==b'PE\0\0' and u16(pe+4)==0x14c)
 sections=u16(pe+6);opt=pe+24;size=u16(pe+20);need(size>=112 and u16(opt)==0x10b and 0<sections<=96)
 table=opt+size
 def offset(rva,n=1):
  for i in range(sections):
   s=table+i*40;start=u32(s+12);rawsize=u32(s+16);raw=u32(s+20)
   if start<=rva and rva+n<=start+rawsize:
    p=raw+rva-start;need(p+n<=len(data));return p
  raise ValueError('PE RVA outside file-backed sections')
 directory=u32(opt+104);length=u32(opt+108);need(directory and 20<=length<=1024*1024)
 imports=[]
 for i in range(min(length//20,4096)):
  p=offset(directory+i*20,20);original,_,_,name,thunk=struct.unpack_from('<IIIII',data,p)
  if not any((original,name,thunk)):break
  dll=string(offset(name));lookup=original or thunk
  for j in range(65536):
   item=u32(offset(lookup+j*4,4))
   if not item:break
   imports.append({'dll':dll,'name':'ordinal:'+str(item&0xffff) if item&0x80000000 else string(offset(item)+2)})
  else:raise ValueError('Unterminated PE import thunk')
 else:raise ValueError('Unterminated PE import directory')
 return imports

def require_capture_only111(path):
 imports=recorder_imports111(path)
 forbidden={'SetThreadContext','Wow64SetThreadContext','WriteProcessMemory','VirtualAllocEx','VirtualProtectEx','SuspendThread','ResumeThread'}
 found=sorted({x['name'] for x in imports}&forbidden)
 if found:raise ValueError('Recorder rejected: invasive imports '+', '.join(found))
 # Capability is compiled into CaptureOnly111, not supplied in a runtime config.
 if b'capture-only binary; hardware tracer not compiled' not in Path(path).read_bytes():
  raise ValueError('Recorder rejected: capture-only build capability absent')
 return {'capture_only':True,'forbidden_imports':found,'imports':imports}
