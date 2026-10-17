"""Read x86 Windows minidumps, including FullMemory stacks stored in Memory64.
Stack-word candidates are explicitly not an unwound call stack.
"""
import argparse,bisect,json,mmap,struct
from pathlib import Path

def analyze(path):
 path=Path(path)
 with path.open('rb') as source, mmap.mmap(source.fileno(),0,access=mmap.ACCESS_READ) as data:
  def u32(offset):return struct.unpack_from('<I',data,offset)[0]
  def u64(offset):return struct.unpack_from('<Q',data,offset)[0]
  if data[:4]!=b'MDMP':raise ValueError('Not a Windows minidump')
  streams={}
  for i in range(u32(8)):
   kind,size,rva=struct.unpack_from('<III',data,u32(12)+i*12);streams[kind]=(size,rva)
  regions=[]
  if 9 in streams:
   rva=streams[9][1];cursor=u64(rva+8)
   for i in range(u64(rva)):
    start,size=struct.unpack_from('<QQ',data,rva+16+i*16)
    if cursor+size>len(data):raise ValueError('Truncated Memory64 data')
    regions.append((start,start+size,cursor,'Memory64'));cursor+=size
  if 5 in streams:
   rva=streams[5][1]
   for i in range(u32(rva)):
    start,size,offset=struct.unpack_from('<QII',data,rva+4+i*16)
    if offset and offset+size<=len(data):regions.append((start,start+size,offset,'MemoryList'))
  regions.sort();starts=[r[0] for r in regions]
  def virtual(address,size,stack):
   # A zero ThreadList Stack RVA is not file offset zero. FullMemory uses Memory64.
   begin,length,offset=stack
   if offset and begin<=address and address+size<=begin+length and offset+address-begin+size<=len(data):
    return data[offset+address-begin:offset+address-begin+size],'ThreadList'
   result=bytearray();sources=[]
   while size:
    index=bisect.bisect_right(starts,address)-1
    if index<0 or not regions[index][0]<=address<regions[index][1]:raise ValueError('Stack memory absent at '+hex(address))
    lo,hi,off,kind=regions[index];take=min(size,hi-address)
    result.extend(data[off+address-lo:off+address-lo+take]);sources.append(kind);address+=take;size-=take
   return bytes(result),'+'.join(dict.fromkeys(sources))
  modules=[]
  if 4 in streams:
   rva=streams[4][1]
   for i in range(u32(rva)):
    rec=rva+4+i*108;name_rva=u32(rec+20)
    modules.append({'base':u64(rec),'size':u32(rec+8),'name':data[name_rva+4:name_rva+4+u32(name_rva)].decode('utf-16le',errors='replace')})
  def locate(address):
   for m in modules:
    if m['base']<=address<m['base']+m['size']:return Path(m['name']).name+'+'+hex(address-m['base'])
   return hex(address)
  offsets=[('edi',156),('esi',160),('ebx',164),('edx',168),('ecx',172),('eax',176),('ebp',180),('eip',184),('eflags',192),('esp',196)]
  def context(size,rva):
   if not rva or size<204 or rva+204>len(data):return None
   return {name:hex(u32(rva+off)) for name,off in offsets}
  exception=None;fault_context=None
  if 6 in streams:
   rva=streams[6][1];exception={'tid':u32(rva),'code':hex(u32(rva+8)),'address':hex(u64(rva+24)),'module':locate(u64(rva+24)),
    'parameters':[hex(u64(rva+40+i*8)) for i in range(min(15,u32(rva+32)))]}
   fault_context=context(*struct.unpack_from('<II',data,rva+160))
   if fault_context:exception['registers']=fault_context
  threads=[]
  if 3 in streams:
   rva=streams[3][1]
   for i in range(u32(rva)):
    rec=rva+4+i*48;registers=context(*struct.unpack_from('<II',data,rec+40))
    if not registers:continue
    tid=u32(rec);ctx_source='ThreadList'
    saved_registers=registers
    if exception and tid==exception['tid'] and fault_context:registers=fault_context;ctx_source='ExceptionStream'
    esp=int(registers['esp'],16);ebp=int(registers['ebp'],16);start=u64(rec+24);stack_size,stack_rva=struct.unpack_from('<II',data,rec+32)
    stack=(start,stack_size,stack_rva)
    thread={'tid':tid,'eip':registers['eip'],'pc':locate(int(registers['eip'],16)),'esp':registers['esp'],'ebp':registers['ebp'],
     'registers':registers,'context_source':ctx_source,'stack_range':{'start':hex(start),'bytes':stack_size,'thread_stack_rva':stack_rva}}
    if ctx_source=='ExceptionStream':thread['thread_list_registers']=saved_registers
    errors=[];chain=[];candidates=[];read_sources=set()
    for _ in range(24):
     if not start<=ebp<=start+stack_size-8:break
     try:
      raw,kind=virtual(ebp,8,stack);read_sources.add(kind);next_ebp,ret=struct.unpack('<II',raw)
     except ValueError as error:errors.append(str(error));break
     chain.append(locate(ret))
     if next_ebp<=ebp:break
     ebp=next_ebp
    for addr in range(max(start,esp),min(start+stack_size,esp+384)-3,4):
     try:raw,kind=virtual(addr,4,stack);read_sources.add(kind);value=struct.unpack('<I',raw)[0]
     except ValueError as error:errors.append(str(error));break
     found=locate(value)
     if '+' in found:candidates.append({'stack_offset':hex(addr-esp),'possible_module_address':found})
    thread.update(bounded_frame_pointer_chain=chain,stack_word_candidates_not_unwound=candidates[:48],stack_memory_sources=sorted(read_sources))
    if errors:thread['stack_read_errors']=list(dict.fromkeys(errors))
    threads.append(thread)
  report={'analysis_version':2,'dump':path.name,'streams':list(streams),'modules':modules,'threads':threads}
  if exception:report['exception']=exception
  return report

def main():
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('dump',type=Path);parser.add_argument('--output',type=Path);args=parser.parse_args()
 report=analyze(args.dump);target=args.output or args.dump.with_suffix('.analysis.json');target.write_text(json.dumps(report,indent=2)+'\n')
 print(json.dumps({'dump':args.dump.name,'exception':report.get('exception'),'threads':[{'tid':t['tid'],'pc':t['pc'],'chain':t['bounded_frame_pointer_chain']} for t in report['threads']]},indent=2))
if __name__=='__main__':main()
