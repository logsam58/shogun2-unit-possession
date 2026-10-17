"""Read-only Windows process memory/resource counters, including the lower 4 GiB map.

This module never opens a writable process handle or changes the working set.
VirtualQueryEx describes address space, not RAM or video memory usage.
"""
import ctypes as C
from ctypes import wintypes as W
import time

k = C.WinDLL('kernel32', use_last_error=True)
ps = C.WinDLL('psapi', use_last_error=True)
u = C.WinDLL('user32', use_last_error=True)

class Counters(C.Structure):
    _fields_ = [('cb', W.DWORD), ('PageFaultCount', W.DWORD)] + [
        (name, C.c_size_t) for name in (
            'PeakWorkingSetSize', 'WorkingSetSize', 'QuotaPeakPagedPoolUsage',
            'QuotaPagedPoolUsage', 'QuotaPeakNonPagedPoolUsage',
            'QuotaNonPagedPoolUsage', 'PagefileUsage', 'PeakPagefileUsage', 'PrivateUsage')]

class Region(C.Structure):
    # VirtualQueryEx uses the caller's ABI, including for a WOW64 target.
    _fields_ = [('BaseAddress', C.c_void_p), ('AllocationBase', C.c_void_p),
                ('AllocationProtect', W.DWORD)] + (
                    [('PartitionId', W.WORD)] if C.sizeof(C.c_void_p) == 8 else []) + [
                ('RegionSize', C.c_size_t), ('State', W.DWORD),
                ('Protect', W.DWORD), ('Type', W.DWORD)]

class SystemMemory(C.Structure):
    _fields_ = [('dwLength', W.DWORD), ('dwMemoryLoad', W.DWORD)] + [
        (name, C.c_uint64) for name in ('ullTotalPhys', 'ullAvailPhys',
            'ullTotalPageFile', 'ullAvailPageFile', 'ullTotalVirtual',
            'ullAvailVirtual', 'ullAvailExtendedVirtual')]

ps.GetProcessMemoryInfo.argtypes = [W.HANDLE, C.POINTER(Counters), W.DWORD]
ps.GetProcessMemoryInfo.restype = W.BOOL
k.GetProcessHandleCount.argtypes = [W.HANDLE, C.POINTER(W.DWORD)]
k.GetProcessHandleCount.restype = W.BOOL
k.VirtualQueryEx.argtypes = [W.HANDLE, C.c_void_p, C.POINTER(Region), C.c_size_t]
k.VirtualQueryEx.restype = C.c_size_t
k.GlobalMemoryStatusEx.argtypes = [C.POINTER(SystemMemory)]
k.GlobalMemoryStatusEx.restype = W.BOOL
u.GetGuiResources.argtypes = [W.HANDLE, W.DWORD]
u.GetGuiResources.restype = W.DWORD

def address_space(handle, limit=1 << 32):
    """Walk a bounded range, retaining an explicit failure/incomplete-map marker."""
    started = time.perf_counter()
    totals = dict(committed_bytes=0, reserved_bytes=0, free_bytes=0,
                  largest_free_region_bytes=0, private_committed_bytes=0,
                  mapped_committed_bytes=0, image_committed_bytes=0)
    address = 0
    count = 0
    error = None
    while address < limit:
        region = Region()
        C.set_last_error(0)
        size = k.VirtualQueryEx(handle, address, C.byref(region), C.sizeof(region))
        if size != C.sizeof(region):
            error = {'address': address, 'winerror': C.get_last_error(), 'returned_bytes': size}
            break
        base = region.BaseAddress or 0
        end = min(base + region.RegionSize, limit)
        if base > address or end <= address:
            error = {'address': address, 'invalid_region': [base, region.RegionSize]}
            break
        extent = end - address
        if region.State == 0x1000:
            totals['committed_bytes'] += extent
            key = {0x20000: 'private_committed_bytes', 0x40000: 'mapped_committed_bytes',
                   0x1000000: 'image_committed_bytes'}.get(region.Type)
            if key:
                totals[key] += extent
        elif region.State == 0x2000:
            totals['reserved_bytes'] += extent
        elif region.State == 0x10000:
            totals['free_bytes'] += extent
            totals['largest_free_region_bytes'] = max(totals['largest_free_region_bytes'], extent)
        else:
            error = {'address': address, 'unexpected_state': region.State}
            break
        address = end
        count += 1
    return dict(**totals, range_limit_bytes=limit, queried_through_address=address,
                complete=address == limit, regions=count, query_error=error,
                scan_duration_ms=round((time.perf_counter() - started) * 1000, 3))

def sample(handle, include_address_space=False):
    """Counter failures stay explicit; an unavailable value is never reported as zero."""
    row = {}
    errors = {}
    counters = Counters(cb=C.sizeof(Counters))
    if ps.GetProcessMemoryInfo(handle, C.byref(counters), counters.cb):
        row.update(private_bytes=counters.PrivateUsage, working_set_bytes=counters.WorkingSetSize,
                   peak_working_set_bytes=counters.PeakWorkingSetSize,
                   peak_private_bytes=counters.PeakPagefileUsage, page_fault_count=counters.PageFaultCount)
    else:
        errors['process_memory'] = C.get_last_error()
    handles = W.DWORD()
    if k.GetProcessHandleCount(handle, C.byref(handles)):
        row['handle_count'] = handles.value
    else:
        errors['handle_count'] = C.get_last_error()
    for flag, name in ((0, 'gdi_objects'), (1, 'user_objects')):
        C.set_last_error(0)
        count = u.GetGuiResources(handle, flag)
        error = C.get_last_error()
        if count or not error:
            row[name] = count
        else:
            errors[name] = error
    system = SystemMemory(dwLength=C.sizeof(SystemMemory))
    if k.GlobalMemoryStatusEx(C.byref(system)):
        row.update(system_physical_total_bytes=system.ullTotalPhys,
                   system_physical_available_bytes=system.ullAvailPhys,
                   system_commit_limit_bytes=system.ullTotalPageFile,
                   system_commit_available_bytes=system.ullAvailPageFile)
    else:
        errors['system_memory'] = C.get_last_error()
    if include_address_space:
        row['address_space'] = address_space(handle)
    if errors:
        row['counter_errors'] = errors
    return row

def summarize(rows):
    samples = [r for r in rows if r.get('event') == 'memory_sample']
    result = {'samples': len(samples), 'limitations':
        'Growth can reflect normal asset caching. These counters do not establish allocation ownership, '
        'a memory leak, or GPU exhaustion; correlate them with the crash dump and RTS/FPS phases.'}
    if not samples:
        return result
    result['duration_seconds'] = (samples[-1]['tick_ms'] - samples[0]['tick_ms']) / 1000
    result['counter_error_samples'] = sum(bool(r.get('counter_errors')) for r in samples)
    for key in ('private_bytes', 'working_set_bytes', 'handle_count', 'gdi_objects', 'user_objects',
                'system_commit_available_bytes'):
        values = [r[key] for r in samples if key in r]
        if values:
            result[key] = {'first': values[0], 'last': values[-1], 'min': min(values),
                           'max': max(values), 'change': values[-1] - values[0]}
    maps = [r['address_space'] for r in samples if 'address_space' in r]
    result['address_space_scans'] = len(maps)
    result['incomplete_address_space_scans'] = sum(not r['complete'] for r in maps)
    if maps:
        result['last_address_space'] = maps[-1]
        result['min_free_bytes'] = min(r['free_bytes'] for r in maps)
        result['min_largest_free_region_bytes'] = min(r['largest_free_region_bytes'] for r in maps)
        result['max_scan_duration_ms'] = max(r['scan_duration_ms'] for r in maps)
    return result
