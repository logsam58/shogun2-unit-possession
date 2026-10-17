"""Read-only PDH sensors. Missing/invalid counters are explicit, never fake zeros."""
import ctypes as C
from ctypes import wintypes as W
class ValueUnion(C.Union):
    _fields_=[('doubleValue',C.c_double),('longValue',W.LONG),('largeValue',C.c_longlong),('text',C.c_void_p)]
class Value(C.Structure):
    _anonymous_=('value',)
    _fields_=[('status',W.DWORD),('value',ValueUnion)]
class Item(C.Structure):
    _fields_=[('name',W.LPWSTR),('value',Value)]
class Sensors:
    def __init__(self):
        self.p=C.WinDLL('pdh');self.query=W.HANDLE();self.handles={};self.errors={}
        self.p.PdhOpenQueryW.argtypes=[W.LPCWSTR,C.c_size_t,C.POINTER(W.HANDLE)]
        self.p.PdhAddEnglishCounterW.argtypes=[W.HANDLE,W.LPCWSTR,C.c_size_t,C.POINTER(W.HANDLE)]
        self.p.PdhCollectQueryData.argtypes=[W.HANDLE]
        self.p.PdhGetFormattedCounterArrayW.argtypes=[W.HANDLE,W.DWORD,C.POINTER(W.DWORD),C.POINTER(W.DWORD),C.c_void_p]
        self.p.PdhCloseQuery.argtypes=[W.HANDLE]
        result=self.p.PdhOpenQueryW(None,0,C.byref(self.query))
        if result:raise OSError('PdhOpenQuery '+hex(result&0xffffffff))
        paths={'cpu_core_percent':r'\Processor Information(*)\% Processor Time',
          'cpu_frequency_MHz':r'\Processor Information(*)\Processor Frequency',
          'cpu_percent_max_frequency':r'\Processor Information(*)\% of Maximum Frequency',
          'gpu_dedicated_bytes':r'\GPU Process Memory(*)\Dedicated Usage',
          'gpu_shared_bytes':r'\GPU Process Memory(*)\Shared Usage',
          'gpu_engine_percent':r'\GPU Engine(*)\Utilization Percentage',
          'memory_pages_input_per_sec':r'\Memory\Pages Input/sec',
          'disk_queue':r'\PhysicalDisk(_Total)\Current Disk Queue Length'}
        for key,path in paths.items():
            h=W.HANDLE();code=self.p.PdhAddEnglishCounterW(self.query,path,0,C.byref(h))
            if not code:self.handles[key]=h
            else:self.errors[key]=hex(code&0xffffffff)
        self.p.PdhCollectQueryData(self.query)
    def sample(self,pid):
        code=self.p.PdhCollectQueryData(self.query);out={'counter_errors':dict(self.errors)}
        if code:out['counter_errors']['collect']=hex(code&0xffffffff)
        for key,h in self.handles.items():
            size=W.DWORD();count=W.DWORD();code=self.p.PdhGetFormattedCounterArrayW(h,0x200,C.byref(size),C.byref(count),None)
            if not size.value:out['counter_errors'][key]=hex(code&0xffffffff);continue
            buf=C.create_string_buffer(size.value);code=self.p.PdhGetFormattedCounterArrayW(h,0x200,C.byref(size),C.byref(count),buf)
            if code:out['counter_errors'][key]=hex(code&0xffffffff);continue
            values={};invalid={}
            for item in C.cast(buf,C.POINTER(Item))[:count.value]:
                name=item.name
                if key.startswith('gpu_') and not name.startswith(f'pid_{pid}_'):continue
                if item.value.status in (0,1):values[name]=item.value.doubleValue
                else:invalid[name]=hex(item.value.status)
            out[key]=values
            if invalid:out['counter_errors'][key]=invalid
        return out
    def close(self):
        if self.query:self.p.PdhCloseQuery(self.query);self.query=None
