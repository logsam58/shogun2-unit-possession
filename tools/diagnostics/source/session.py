"""One diagnostic session per game PID. Read-only measurements; external debugger dumps faults."""
import ctypes as C
from ctypes import wintypes as W
import argparse,csv,hashlib,json,os,shutil,subprocess,sys,threading,time,traceback
from pathlib import Path
ROOT=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT))
from process_probe import Process,processes,modules,k,PROCESSENTRY32W
from process_memory import sample as memory_sample,Region
from sensors import Sensors
ROOT=Path(__file__).resolve().parent
CREATE_NO_WINDOW=0x08000000
k.GetExitCodeProcess.argtypes=[W.HANDLE,C.POINTER(W.DWORD)];k.GetExitCodeProcess.restype=W.BOOL
k.GetProcessTimes.argtypes=[W.HANDLE,*([C.POINTER(W.FILETIME)]*4)];k.GetProcessTimes.restype=W.BOOL
k.GetTickCount64.restype=C.c_uint64
k.VirtualQueryEx.argtypes=[W.HANDLE,C.c_void_p,C.POINTER(Region),C.c_size_t];k.VirtualQueryEx.restype=C.c_size_t
class IO(C.Structure):_fields_=[(n,C.c_uint64) for n in ('ReadOperationCount','WriteOperationCount','OtherOperationCount','ReadTransferCount','WriteTransferCount','OtherTransferCount')]
k.GetProcessIoCounters.argtypes=[W.HANDLE,C.POINTER(IO)];k.GetProcessIoCounters.restype=W.BOOL
u=C.WinDLL('user32',use_last_error=True);u.GetForegroundWindow.restype=W.HWND
u.GetWindowThreadProcessId.argtypes=[W.HWND,C.POINTER(W.DWORD)];u.GetAsyncKeyState.argtypes=[C.c_int];u.GetAsyncKeyState.restype=C.c_short

def stamp():return {'utc':time.strftime('%Y-%m-%dT%H:%M:%SZ',time.gmtime()),'unix':time.time(),'tick_ms':k.GetTickCount64()}
def write_json(path,value):
    tmp=path.with_suffix('.tmp');tmp.write_text(json.dumps(value,indent=2),encoding='utf-8');tmp.replace(path)
def sha(path):
    with open(path,'rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def expected_module_hash(baseline,module):
    # The integrated loader forwards DInput to the real Windows DLL. Both may
    # share a Toolhelp name, so known paths have their own verified fingerprints.
    path=os.path.normcase(os.path.abspath(module['path']))
    return baseline.get('module_path_hashes',{}).get(path,baseline.get('module_names',{}).get(module['name'].lower()))

def native_log_paths(cache,pid):
    # Runtime QTE logs live in private/, while adapter/autoload logs use root.
    # Inspect only these two known locations, with no recursive traversal.
    for source in (cache,cache/'private'):
        for path in source.glob(f'*{pid}*'):
            if path.is_file() and path.suffix.lower() in ('.log','.jsonl'):yield path

def all_processes():
    h=k.CreateToolhelp32Snapshot(2,0)
    if h==C.c_void_p(-1).value:raise C.WinError(C.get_last_error())
    row=PROCESSENTRY32W();row.dwSize=C.sizeof(row);out=[]
    try:
        ok=k.Process32FirstW(h,C.byref(row))
        while ok:
            if row.szExeFile.lower() in ('shogun2.exe','shogun2.retail.exe','possession-hud.exe'):out.append({'pid':row.th32ProcessID,'name':row.szExeFile,'threads':row.cntThreads})
            ok=k.Process32NextW(h,C.byref(row))
    finally:k.CloseHandle(h)
    return out

def creation(pid):
    p=Process(pid)
    try:
        values=[W.FILETIME() for _ in range(4)]
        if not k.GetProcessTimes(p.handle,*[C.byref(v) for v in values]):raise C.WinError(C.get_last_error())
        return values[0].dwHighDateTime<<32|values[0].dwLowDateTime
    finally:p.close()
def cpu(handle):
    v=[W.FILETIME() for _ in range(4)]
    if not k.GetProcessTimes(handle,*[C.byref(x) for x in v]):raise C.WinError(C.get_last_error())
    nums=[x.dwHighDateTime<<32|x.dwLowDateTime for x in v]
    return {'creation_filetime':nums[0],'kernel_seconds':nums[2]/1e7,'user_seconds':nums[3]/1e7}
def alive(handle):
    code=W.DWORD()
    if not k.GetExitCodeProcess(handle,C.byref(code)):return False,None
    return code.value==259,code.value

def region_map(handle):
    out=[];address=0;error=None
    while address<1<<32:
        r=Region();n=k.VirtualQueryEx(handle,address,C.byref(r),C.sizeof(r))
        if n!=C.sizeof(r):error={'address':address,'winerror':C.get_last_error()};break
        end=min((r.BaseAddress or 0)+r.RegionSize,1<<32)
        if end<=address:error={'invalid_region_at':address};break
        out.append({'base':r.BaseAddress or 0,'allocation_base':r.AllocationBase or 0,'size':r.RegionSize,'state':r.State,'protect':r.Protect,'type':r.Type});address=end
    return {'regions':out,'complete':address==1<<32,'error':error}

def hidden(cmd,output):
    return subprocess.Popen([str(x) for x in cmd],stdout=output,stderr=subprocess.STDOUT,creationflags=CREATE_NO_WINDOW,stdin=subprocess.DEVNULL)

def event_export(folder,start,inventory=False):
    dest=folder/('hardware-inventory.json' if inventory else 'windows-events.json')
    with (folder/('inventory-export.log' if inventory else 'event-export.log')).open('a') as log:
        cmd=[r'C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe','-NoProfile','-ExecutionPolicy','Bypass','-File',ROOT/'events.ps1','-Output',dest,'-StartUnix',str(int(start))]
        if inventory:cmd+=['-Inventory']
        try:hidden(cmd,log).wait(timeout=90)
        except Exception as e:log.write(str(e)+'\n')

def gpu_loop(folder,done,emit):
    fields=['timestamp','index','name','driver_version','memory.total','memory.used','memory.free','utilization.gpu','utilization.memory','temperature.gpu','power.draw','clocks.current.graphics','clocks.current.memory','pstate']
    exe=Path(r'C:\Windows\System32\nvidia-smi.exe')
    if not exe.exists():emit('gpu_unavailable',reason='nvidia-smi absent');return
    err=(folder/'nvidia-smi-stderr.log').open('a');child=None
    try:
        child=subprocess.Popen([str(exe),'--query-gpu='+','.join(fields),'--format=csv,noheader,nounits','--loop-ms=1000'],stdout=subprocess.PIPE,stderr=err,text=True,creationflags=CREATE_NO_WINDOW)
        with (folder/'gpu-device.jsonl').open('a',buffering=1) as out:
            while not done.is_set():
                line=child.stdout.readline()
                if not line:break
                values=next(csv.reader([line]));row={'event':'gpu_device',**stamp()}
                if len(values)==len(fields):row['nvidia']=dict(zip(fields,[v.strip() for v in values]));out.write(json.dumps(row)+'\n')
                else:out.write(json.dumps({'event':'gpu_parse_error','raw':line,**stamp()})+'\n')
        emit('gpu_reader_ended',returncode=child.poll())
    except Exception as e:emit('gpu_error',error=str(e))
    finally:
        if child and child.poll() is None:child.terminate();child.wait(timeout=5)
        err.close()

def summarize(folder):
    result={'created':stamp(),'unhandled_exceptions':[],'handled_first_chance_exceptions':[],'mismatches':[],'dump_errors':[],'resource_ranges':{},'limitations':['An exception inside Empire.Retail.dll does not establish who corrupted its inputs.','Loaded module paths/file hashes identify backing files; dumps preserve runtime memory.','GPU process counters can be inaccurate; device VRAM is machine-wide.','No continuous full execution trace, CPU temperature provider, video, or unrelated-app keystrokes.']}
    for path in folder.glob('exceptions-*.jsonl'):
        for line in path.open(encoding='utf-8'):
            try:row=json.loads(line)
            except ValueError:continue
            if row.get('event')=='exception':result['handled_first_chance_exceptions' if row.get('first_chance') else 'unhandled_exceptions'].append(row)
            if row.get('event')=='dump_error':result['dump_errors'].append(row)
            if row.get('event')=='process_exited':result['exit_code']=row['exit_code']
    # A first-chance event may later become unhandled; this collection is evidence, not a claim it was recovered.
    result['first_chance_exceptions']=result.pop('handled_first_chance_exceptions')
    resources=folder/'resources.jsonl'
    if resources.exists():
        for line in resources.open():
            try:row=json.loads(line)
            except ValueError:continue
            for name in ('private_bytes','working_set_bytes','system_commit_available_bytes','system_physical_available_bytes','cpu_percent_one_core'):
                value=row.get(name)
                if isinstance(value,(int,float)):
                    pair=result['resource_ranges'].setdefault(name,{'min':value,'max':value,'last':value});pair.update(min=min(pair['min'],value),max=max(pair['max'],value),last=value)
            space=row.get('address_space',{})
            if 'largest_free_region_bytes' in space:
                value=space['largest_free_region_bytes'];result['smallest_largest_free_region_bytes']=min(result.get('smallest_largest_free_region_bytes',value),value)
    inventory=folder/'module-inventory.jsonl'
    if inventory.exists():
        for line in inventory.open():
            try:row=json.loads(line)
            except ValueError:continue
            for module in row.get('modules',[]):
                if module.get('expected_sha256') and module.get('match') is False:result['mismatches'].append(module)
    gpu=folder/'gpu-device.jsonl'
    if gpu.exists():
        for line in gpu.open():
            try:r=json.loads(line)['nvidia'];value=float(r['memory.used']);result['gpu_device_max_used_MiB']=max(value,result.get('gpu_device_max_used_MiB',value))
            except (ValueError,KeyError):pass
    result['dumps']=[{'file':str(p.relative_to(folder)),'bytes':p.stat().st_size} for p in folder.rglob('*.dmp')]
    write_json(folder/'session-summary.json',result)
    return result

def game_files(folder,baseline):
    game=Path(baseline['game_dir']);saved=folder/'game-configuration';saved.mkdir(exist_ok=True)
    packs=[{'name':p.name,'bytes':p.stat().st_size,'mtime_ns':p.stat().st_mtime_ns} for p in sorted((game/'data').glob('*.pack'))]
    write_json(saved/'pack-inventory.json',{'packs':packs,'note':'Installed pack inventory; actual order may differ. user.script and game logs are copied separately; large packs are not rehashed during gameplay.'})
    app=Path(os.environ['APPDATA'])/'The Creative Assembly'/'Shogun2'
    for sub in ('scripts','logs'):
        src=app/sub
        if src.exists():
            for path in src.glob('*'):
                if path.is_file() and path.stat().st_size<32*1024*1024:shutil.copy2(path,saved/(sub+'-'+path.name))
    for path in game.glob('*.log'):
        if path.stat().st_size<32*1024*1024:shutil.copy2(path,saved/('root-'+path.name))

def session(pid,folder,fixture=False):
    folder=Path(folder);folder.mkdir(parents=True,exist_ok=True);start=time.time();done=threading.Event();lock=threading.Lock();events=(folder/'session-events.jsonl').open('a',buffering=1)
    def emit(event,**items):
        with lock:events.write(json.dumps({'event':event,**stamp(),**items})+'\n')
    lifecycle=None;native=None;native_out=None;debug=None;debug_out=(folder/'debugger.log').open('a');gpu=None;sensors=None;process=None
    status={'pid':pid,'started_unix':start,'fixture':fixture,'folder':str(folder),'debugger_attached':False,'native_input_active':False}
    try:
        # Attach before hardware discovery or battle/mod checks. No pause/READY gate.
        debug=hidden([ROOT/'native'/'crash-recorder.exe','--fixture' if fixture else '--watch-auto',pid,0,folder],debug_out)
        process=Process(pid);status['creation_filetime']=cpu(process.handle)['creation_filetime'];write_json(folder/'session.json',status);emit('recording_started',**status)
        threading.Thread(target=event_export,args=(folder,start,True),daemon=True).start()
        gpu=threading.Thread(target=gpu_loop,args=(folder,done,emit),daemon=True);gpu.start()
        try:sensors=Sensors()
        except Exception as e:emit('pdh_unavailable',error=str(e))
        baseline=json.loads((ROOT/'baseline.json').read_text());
        try:
            from lifecycle110 import Lifecycle
            lifecycle=Lifecycle(process,pid,folder,emit,modules)
        except Exception as e:emit('lifecycle110_start_error',error=str(e))
        if not fixture:
            try:game_files(folder,baseline)
            except Exception as e:emit('game_config_error',error=str(e))
        hashes={};seen_modules={};log_offsets={};last_cpu=None;next_resource=next_inventory=next_map=next_native=0;last_input=None;debug_retries=0
        with (folder/'resources.jsonl').open('a',buffering=1) as resources,(folder/'module-inventory.jsonl').open('a',buffering=1) as inventory,(folder/'address-maps.jsonl').open('a',buffering=1) as maps,(folder/'input.jsonl').open('a',buffering=1) as inputs:
            while True:
                live,exit_code=alive(process.handle);now=time.monotonic()
                if not live:emit('game_exited',exit_code=exit_code);status['exit_code']=exit_code;break
                if (folder/'session.stop').exists() or (ROOT/'watcher.stop').exists():emit('stop_requested');break
                if lifecycle:
                    try:lifecycle.poll(now)
                    except Exception as e:emit('lifecycle110_poll_error',error=str(e));lifecycle.close();lifecycle=None
                foreground=W.DWORD();u.GetWindowThreadProcessId(u.GetForegroundWindow(),C.byref(foreground));focused=foreground.value==pid
                controls={name:bool(u.GetAsyncKeyState(key)&0x8000) if focused else False for name,key in [('left',1),('right',2),('w',0x57),('a',0x41),('s',0x53),('d',0x44),('escape',0x1b),('space',0x20),('insert',0x2d),('f9',0x78),('f10',0x79),('f12',0x7b),('tab',9),('ctrl',0x11),('shift',0x10),('q',0x51),('e',0x45),('z',0x5a),('c',0x43),('r',0x52),('f',0x46),('weapon1',0x31),('weapon2',0x32)]}
                state={'focused':focused,**controls}
                if state!=last_input:inputs.write(json.dumps({'event':'input_state',**stamp(),**state})+'\n');last_input=state
                if now>=next_resource:
                    try:
                        row={'event':'resource_sample',**stamp(),**memory_sample(process.handle,include_address_space=now>=next_map)};times=cpu(process.handle);total=times['kernel_seconds']+times['user_seconds'];row['process_times']=times
                        if last_cpu:row['cpu_percent_one_core']=100*(total-last_cpu[1])/(now-last_cpu[0]);row['cpu_percent_machine']=row['cpu_percent_one_core']/(os.cpu_count() or 1)
                        last_cpu=(now,total);io=IO()
                        if k.GetProcessIoCounters(process.handle,C.byref(io)):row['io']={n:getattr(io,n) for n,_ in io._fields_}
                        else:row['io_error']=C.get_last_error()
                        if sensors:row['pdh']=sensors.sample(pid)
                        row['recording_disk_free_bytes']=shutil.disk_usage(folder).free;resources.write(json.dumps(row)+'\n')
                        if now>=next_map:maps.write(json.dumps({**stamp(),**region_map(process.handle)})+'\n');next_map=now+10
                        status['resource_sample_tick_ms']=row['tick_ms'];status['native_input_active']=native is not None and native.poll() is None
                        exception_files=list(folder.glob('exceptions-*.jsonl'));status['debugger_attached']=bool(exception_files) and debug.poll() is None
                        write_json(folder/'health.json',{**status,**stamp()});next_resource=now+1
                    except Exception as e:emit('resource_error',error=str(e));next_resource=now+1
                if debug.poll() is not None and debug_retries<3:
                    emit('debugger_ended',returncode=debug.returncode,retry=debug_retries);debug_retries+=1
                    debug=hidden([ROOT/'native'/'crash-recorder.exe','--fixture' if fixture else '--watch-auto',pid,0,folder],debug_out)
                if now>=next_inventory:
                    try:
                        current=modules(pid);fingerprint=tuple((m['base'],m['path']) for m in current)
                        # Hash newly observed backing files once; repeat if size or mtime changes.
                        recorded=[]
                        for m in current:
                            record=dict(m);record['sha256']=None;path=Path(m['path'])
                            try:
                                st=path.stat();key=(str(path).lower(),st.st_size,st.st_mtime_ns)
                                if key not in hashes:hashes[key]=sha(path)
                                record['sha256']=hashes[key]
                            except OSError as e:record['hash_error']=str(e)
                            expected=expected_module_hash(baseline,m);record['expected_sha256']=expected
                            if expected:record['match']=(expected==record['sha256']) if record['sha256'] else None
                            recorded.append(record)
                        signature=tuple((r['base'],r['path'],r['sha256']) for r in recorded)
                        if seen_modules.get('signature')!=signature:inventory.write(json.dumps({'event':'module_inventory',**stamp(),'modules':recorded,'note':'Toolhelp loaded module addresses plus backing-file SHA256; no claim disk bytes equal patched runtime pages'})+'\n');seen_modules['signature']=signature
                        cache_dirs={Path(m['path']).parent for m in current if m['name'].lower() in ('possession-v46.dll','qte-playable-v79.dll')}
                        cache_dirs.add(Path(baseline['cache_dir']))
                        for cache in cache_dirs:
                            for path in native_log_paths(cache,pid):
                                if path.suffix.lower() not in ('.log','.jsonl'):continue
                                previous=log_offsets.get(str(path),0);size=path.stat().st_size
                                if size<previous:previous=0
                                if size>previous:
                                    target=folder/'native-logs'/path.relative_to(cache);target.parent.mkdir(parents=True,exist_ok=True)
                                    with path.open('rb') as inp,target.open('ab') as out:inp.seek(previous);out.write(inp.read(size-previous))
                                    log_offsets[str(path)]=size
                        huds=[p for p in all_processes() if p['name'].lower()=='possession-hud.exe']
                        for hud in huds:
                            hm=modules(hud['pid'])[0];path=Path(hm['path']);st=path.stat();key=(str(path).lower(),st.st_size,st.st_mtime_ns)
                            if key not in hashes:hashes[key]=sha(path)
                            record={'pid':hud['pid'],'path':str(path),'sha256':hashes[key],'expected_sha256':baseline['hud_sha256']};record['match']=record['sha256']==record['expected_sha256']
                            if seen_modules.get('hud')!=record:inventory.write(json.dumps({'event':'hud_process',**stamp(),'modules':[record]})+'\n');seen_modules['hud']=record
                        known_engine=any(r['name'].lower()=='empire.retail.dll' and r.get('match') for r in recorded)
                        if not fixture and known_engine and now>=next_native and (native is None or native.poll() is not None):
                            if native:emit('native_input_ended',returncode=native.returncode)
                            if native_out:native_out.close()
                            native_out=(folder/'native-input-collector.log').open('a');native=hidden([sys.executable,ROOT/'native_input.py','--pid',pid,'--qte-version',79,'--output-dir',folder,'--native-log',Path(baseline['cache_dir'])/'private'/f'qte-playable79-{pid}.jsonl'],native_out);next_native=now+15
                    except Exception as e:emit('inventory_error',error=str(e))
                    next_inventory=now+5
                time.sleep(.02)
    except Exception as e:emit('session_error',error=str(e),traceback=traceback.format_exc())
    finally:
        done.set();(folder/'crash.stop').touch()
        if native:
            for active in folder.glob('active-live-input*.json'):
                try:Path(json.loads(active.read_text())['stop_file']).touch()
                except Exception:pass
            try:native.wait(timeout=5)
            except subprocess.TimeoutExpired:native.terminate()
        if debug:
            try:debug.wait(timeout=90)
            except subprocess.TimeoutExpired:emit('debugger_still_finishing',pid=debug.pid) # Do not kill it mid-dump.
        if gpu:gpu.join(timeout=3)
        if lifecycle:lifecycle.close()
        if sensors:sensors.close()
        if process:process.close()
        if native_out:native_out.close()
        debug_out.close()
        # Final log copy includes tail after the last 5-second inventory pass.
        try:
            baseline=json.loads((ROOT/'baseline.json').read_text())
            if not fixture:game_files(folder,baseline)
            cache=Path(baseline['cache_dir'])
            for path in native_log_paths(cache,pid):
                if path.suffix.lower() in ('.log','.jsonl'):
                    dest=folder/'native-logs'/path.relative_to(cache);dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(path,dest)
        except Exception as e:emit('native_tail_copy_error',error=str(e))
        event_export(folder,start)
        # Preserve new WER reports and Steam/LocalDumps files left by this session.
        try:
            roots=[Path(os.environ['LOCALAPPDATA'])/'CrashDumps',Path(r'C:\Program Files (x86)\Steam\dumps'),Path(r'C:\ProgramData\Microsoft\Windows\WER\ReportArchive'),Path(r'C:\ProgramData\Microsoft\Windows\WER\ReportQueue')]
            for root in roots:
                if not root.exists():continue
                for path in root.glob('**/*'):
                    if path.is_file() and path.stat().st_mtime>=start-2 and (path.suffix.lower() in ('.dmp','.mdmp','.wer')) and ('shogun' in str(path).lower() or 'empire' in str(path).lower()):
                        dest=folder/'windows-crash-files'/path.parent.name/path.name;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(path,dest)
        except Exception as e:emit('crash_file_copy_error',error=str(e))
        for dump in folder.glob('*.dmp'):
            try:
                with (folder/'dump-analysis.log').open('a') as output:hidden([sys.executable,ROOT/'inspect_minidump.py',dump],output).wait(timeout=60)
            except Exception as e:emit('dump_analysis_error',error=str(e))
        summarize(folder);status.update(ended_unix=time.time(),recording_active=False);write_json(folder/'session.json',status);emit('recording_ended');events.close()

if __name__=='__main__':
    a=argparse.ArgumentParser();a.add_argument('--pid',type=int,required=True);a.add_argument('--folder',type=Path,required=True);a.add_argument('--fixture',action='store_true');o=a.parse_args();session(o.pid,o.folder,o.fixture)
