"""Read only native process/module inspection. No injection or writes."""
import ctypes as C,json,sys,hashlib,struct
from ctypes import wintypes as W
from pathlib import Path
k=C.WinDLL('kernel32',use_last_error=True)
ULONG_PTR=C.c_size_t
class PROCESSENTRY32W(C.Structure):
    _fields_=[('dwSize',W.DWORD),('cntUsage',W.DWORD),('th32ProcessID',W.DWORD),('th32DefaultHeapID',ULONG_PTR),('th32ModuleID',W.DWORD),('cntThreads',W.DWORD),('th32ParentProcessID',W.DWORD),('pcPriClassBase',W.LONG),('dwFlags',W.DWORD),('szExeFile',W.WCHAR*260)]
class MODULEENTRY32W(C.Structure):
    _fields_=[('dwSize',W.DWORD),('th32ModuleID',W.DWORD),('th32ProcessID',W.DWORD),('GlblcntUsage',W.DWORD),('ProccntUsage',W.DWORD),('modBaseAddr',C.c_void_p),('modBaseSize',W.DWORD),('hModule',W.HMODULE),('szModule',W.WCHAR*256),('szExePath',W.WCHAR*260)]
k.CreateToolhelp32Snapshot.argtypes=[W.DWORD,W.DWORD];k.CreateToolhelp32Snapshot.restype=W.HANDLE
k.Process32FirstW.argtypes=[W.HANDLE,C.POINTER(PROCESSENTRY32W)];k.Process32NextW.argtypes=k.Process32FirstW.argtypes
k.Module32FirstW.argtypes=[W.HANDLE,C.POINTER(MODULEENTRY32W)];k.Module32NextW.argtypes=k.Module32FirstW.argtypes
k.CloseHandle.argtypes=[W.HANDLE]
k.OpenProcess.argtypes=[W.DWORD,W.BOOL,W.DWORD];k.OpenProcess.restype=W.HANDLE
k.ReadProcessMemory.argtypes=[W.HANDLE,C.c_void_p,C.c_void_p,C.c_size_t,C.POINTER(C.c_size_t)];k.ReadProcessMemory.restype=W.BOOL

def processes():
    h=k.CreateToolhelp32Snapshot(2,0)
    if h==C.c_void_p(-1).value:raise C.WinError(C.get_last_error())
    entry=PROCESSENTRY32W();entry.dwSize=C.sizeof(entry);result=[]
    try:
        ok=k.Process32FirstW(h,C.byref(entry))
        while ok:
            if entry.szExeFile.lower() in ('shogun2.exe','shogun2.retail.exe'):result.append(dict(pid=entry.th32ProcessID,name=entry.szExeFile))
            ok=k.Process32NextW(h,C.byref(entry))
    finally:k.CloseHandle(h)
    return result

def modules(pid):
    h=k.CreateToolhelp32Snapshot(0x18,pid)
    if h==C.c_void_p(-1).value:raise C.WinError(C.get_last_error())
    entry=MODULEENTRY32W();entry.dwSize=C.sizeof(entry);result=[]
    try:
        ok=k.Module32FirstW(h,C.byref(entry))
        if not ok:raise C.WinError(C.get_last_error())
        while ok:
            result.append(dict(name=entry.szModule,path=entry.szExePath,base=entry.modBaseAddr,size=entry.modBaseSize))
            ok=k.Module32NextW(h,C.byref(entry))
    finally:k.CloseHandle(h)
    return result

class Process:
    def __init__(self,pid):
        self.handle=k.OpenProcess(0x410,False,pid)
        if not self.handle:raise C.WinError(C.get_last_error())
    def read(self,addr,size):
        buf=C.create_string_buffer(size);n=C.c_size_t()
        if not k.ReadProcessMemory(self.handle,addr,buf,size,C.byref(n)) or n.value!=size:raise C.WinError(C.get_last_error())
        return buf.raw
    def u32(self,addr):return struct.unpack('<I',self.read(addr,4))[0]
    def close(self):k.CloseHandle(self.handle)

if __name__=='__main__':
    report=[]
    for process in processes():
        try:
            mods=modules(process['pid']);process['modules']=mods
            engine=next((m for m in mods if m['name'].lower()=='empire.retail.dll'),None)
            if engine:
                p=Process(process['pid']);base=engine['base']
                process['flags']={name:dict(object_rva=hex(rva),object_hex=p.read(base+rva,0x60).hex(),value=p.read(base+rva+0x5c,1)[0]) for name,rva in [('enable_first_person',0x192a700),('first_person_enable_men',0x1a718f0),('first_person_real_aim',0x1a71720),('first_person_allow_all',0x1a717f8)]}
                root=p.u32(base+0x1bc8180)
                process['battle_root_candidate']=hex(root)
                if root:
                    env=p.u32(root+0x110);process['battle_environment_candidate']=hex(env)
                    if env:
                        manager=p.u32(env+0x2834c);process['first_person_manager_candidate']=hex(manager)
                        if manager:
                            process['manager_environment_backlink']=hex(p.u32(manager+0x38))
                            process['manager_hex']=p.read(manager,0x484).hex()
                            process['current_entity_candidate']=hex(p.u32(manager+0x204))
                p.close()
        except OSError as e:process['error']=str(e)
        report.append(process)
    path=Path(__file__).parent/'analysis'/'runtime-probe.json';path.write_text(json.dumps(report,indent=2),encoding='utf-8')
    print(json.dumps([{**{k:v for k,v in p.items() if k!='modules'},'modules':[m for m in p.get('modules',[]) if m['name'].lower() in ['shogun2.exe','empire.retail.dll','steam_api.dll']]} for p in report],indent=2))
