"""Bounded read-only session/effects/selected-unit history, independent of mod readiness.

Native phases remain numeric until live correlation establishes their meaning.
Address reuse observations are not allocation-generation proofs.
"""
import collections, hashlib, json, math, struct, time
from pathlib import Path

ENGINE_SHA = '022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5'

def decode_command(raw, pid, tick):
    if len(raw) != 80 or struct.unpack_from('<4I',raw) != (0x30304355,100,80,pid):
        raise ValueError('Command100 identity differs')
    if struct.unpack_from('<I',raw,16)[0] & 1:
        return {'unstable':True}
    names=['lease','actor','unit','move_mode','fire_mode','feedback','placing']
    result=dict(zip(names,struct.unpack_from('<7I',raw,20)))
    published=struct.unpack_from('<Q',raw,48)[0]
    result.update(tick_ms=published,fresh=0 <= tick-published <= 1000)
    result.update(zip(['volley','releases','blocked','error'],struct.unpack_from('<4I',raw,64)))
    if result['move_mode']>4 or result['fire_mode']>2 or result['placing']>1 or result['feedback']>12:
        raise ValueError('Command100 bounds differ')
    return result

class CommandChannel:
    def __init__(self,pid):
        import ctypes as C
        from ctypes import wintypes as W
        self.C=C;self.k=C.WinDLL('kernel32',use_last_error=True);self.pid=pid
        self.k.OpenFileMappingW.argtypes=[W.DWORD,W.BOOL,W.LPCWSTR];self.k.OpenFileMappingW.restype=W.HANDLE
        self.k.MapViewOfFile.argtypes=[W.HANDLE,W.DWORD,W.DWORD,W.DWORD,C.c_size_t];self.k.MapViewOfFile.restype=C.c_void_p
        self.k.UnmapViewOfFile.argtypes=[C.c_void_p];self.k.CloseHandle.argtypes=[W.HANDLE]
        self.handle=self.view=None;self.next_try=0
    def sample(self,tick):
        C=self.C
        if not self.view and tick>=self.next_try:
            self.next_try=tick+1000
            h=self.k.OpenFileMappingW(4,False,f'Local\\Shogun2PossessionUnitCommands100-{self.pid}')
            if h:
                view=self.k.MapViewOfFile(h,4,0,0,80)
                if view:self.handle,self.view=h,view
                else:self.k.CloseHandle(h)
        if not self.view:return {'available':False}
        for _ in range(3):
            before=struct.unpack('<I',C.string_at(self.view+16,4))[0]
            if before&1:continue
            raw=C.string_at(self.view,80)
            if before==struct.unpack('<I',C.string_at(self.view+16,4))[0]:return {'available':True,**decode_command(raw,self.pid,tick)}
        return {'available':True,'unstable':True}
    def close(self):
        if self.view:self.k.UnmapViewOfFile(self.view);self.view=None
        if self.handle:self.k.CloseHandle(self.handle);self.handle=None

def stable_effects(read, base):
    address = base + 0x1ca3464
    header = read(address, 8)
    array, count = struct.unpack('<II', header)
    if count > 256 or (count and array < 65536):
        return {'stable': False, 'reason': 'global_bound', 'array': array, 'count': count}
    raw = read(array, count * 36) if count else b''
    owners = []
    for index in range(count):
        active, element, owner = struct.unpack_from('<III', raw, index * 36 + 24)
        if not active & 255:
            continue
        item = {'index': index, 'owner': owner, 'element': element}
        try:
            a, b = read(owner, 48), read(owner, 48)
            values = lambda data: tuple(struct.unpack_from('<I', data, off)[0] for off in (0, 40, 44))
            vtable, table, size = values(a)
            item.update(vtable=vtable, table=table, table_count=size, stable=values(a) == values(b))
            item['empty_table'] = not table or not size
            item['known_effects_vtable'] = vtable == base + 0x1764384
            item['base_destructor_vtable'] = vtable == base + 0x176433c
        except OSError as error:
            item.update(stable=False, read_error=str(error))
        owners.append(item)
    after = read(array, count * 36) if count else b''
    return {'array': array, 'count': count, 'active': len(owners), 'owners': owners,
            'stable': raw == after and header == read(address, 8)}

def native_context(read, base):
    word = lambda p: struct.unpack('<I', read(p, 4))[0]
    root = word(base + 0x1bc8180)
    result = {'root': root, 'phase_class': 'no_battle_environment_observed'}
    if not root:
        return result
    env = word(root + 0x110)
    result['environment'] = env
    if not env:
        return result
    common = word(env + 8)
    result['common'] = common
    if not common:
        return result
    phase = word(common + 0xb4)
    result['phase_object'] = phase
    if not phase:
        return result
    phase_id = word(phase + 0x14)
    result.update(phase_id=phase_id, phase_class='native_phase_observed')
    if not 5 <= phase_id <= 8:
        return result
    result.update(phase_class='battle_phase_candidate', simulation_tick=word(common + 0x50))
    manager = word(env + 0x281fc)
    selected = word(env + 0x2826c)
    result.update(manager=manager, selected=selected)
    result['selected_units'] = []
    if selected:
        owner = word(selected + 0x4a8)
        if owner:
            group = owner + 0xb0
            raw = read(group, 28)
            vt, capacity, count, members = (struct.unpack_from('<I', raw, p)[0] for p in (0, 16, 20, 24))
            result.update(selection_count=count, selection_capacity=capacity,
                          selection_valid=vt == base + 0x15c4da0 and count <= capacity <= 4096)
            if result['selection_valid'] and members and count:
                pointers = struct.unpack('<' + 'I' * min(count, 64), read(members, min(count, 64) * 4))
                for unit in pointers:
                    try:
                        serial=word(unit+4)
                        item={'pointer': unit, 'serial': serial,
                            'morale_raw': word(unit + 0xce8), 'routing_raw': word(unit + 0xcec),
                            'fire_at_will': read(unit + 0xeb0, 1)[0], 'guard': read(unit + 0xeb1, 1)[0]}
                        item['serial_stable']=word(unit+4)==serial
                        result['selected_units'].append(item)
                    except OSError:
                        result['selected_units'].append({'pointer': unit, 'readable': False})
                result['selection_truncated'] = count > 64
                result['selection_snapshot_stable']=read(group,28)==raw and read(members,min(count,64)*4)==struct.pack('<'+'I'*len(pointers),*pointers)
    if manager:
        camera = word(manager + 0x8e0)
        result['camera'] = camera
        if camera:
            result['camera_vtable'] = word(camera)
            result['camera_eye'] = list(struct.unpack('<3f', read(camera + 8, 12)))
            result['camera_target'] = list(struct.unpack('<3f', read(camera + 20, 12)))
            if result['camera_vtable'] == base + 0x15d22e8 and word(camera + 0x180) == manager:
                actor = word(camera + 0x188)
                result['followed_actor'] = actor
                if actor:
                    a = read(actor, 0x220)
                    w = lambda off: struct.unpack_from('<I', a, off)[0]
                    if w(0) == base + 0x15dfec8 and w(0x28) == env:
                        result['actor'] = {'pointer': actor, 'serial': w(0x1c8), 'unit': w(0x214),
                            'alive': a[0xf0], 'state': w(0x1d0), 'clip': w(0x1d4),
                            'animation_frame': w(0x1d8), 'mode': w(0x1f0),
                            'position': list(struct.unpack_from('<3f', a, 0x48)),
                            'velocity': list(struct.unpack_from('<3f', a, 0x8c))}
    if word(root + 0x110) != env or word(common + 0xb4) != phase:
        result['unstable'] = True
    return result

class History:
    """Never delete earlier evidence. Stop dense output at its explicit budget."""
    def __init__(self, folder, budget=128*1024*1024, ring_size=600):
        self.folder = Path(folder)
        self.stream = (self.folder/'lifecycle110.jsonl').open('x', encoding='utf-8', buffering=1)
        self.ring = collections.deque(maxlen=ring_size)
        self.budget = budget
        self.bytes = 0
        self.limited = False
        self.triggers = 0
        self.last_trigger = -float('inf')

    def emit(self, row):
        def clean(value):
            if isinstance(value,float) and not math.isfinite(value):return {'non_finite':repr(value)}
            if isinstance(value,dict):return {k:clean(v) for k,v in value.items()}
            if isinstance(value,(list,tuple)):return [clean(v) for v in value]
            return value
        row=clean(row)
        self.ring.append(row)
        if self.limited:return
        line = json.dumps(row, allow_nan=False, separators=(',', ':')) + '\n'
        size = len(line.encode('utf-8'))
        if self.bytes + size > self.budget:
            if not self.limited:
                self.stream.write(json.dumps({'event':'dense_trace_budget_reached', 'tick_ms':row.get('tick_ms'),
                    'budget':self.budget, 'earlier_evidence_preserved':True})+'\n')
                self.limited = True
            return
        self.stream.write(line)
        self.bytes += size

    def trigger(self, tick, reason):
        if self.triggers >= 16 or tick - self.last_trigger < 30000:
            return False
        self.triggers += 1
        self.last_trigger = tick
        dest = self.folder/f'lifecycle110-pretrigger-{self.triggers:02d}.jsonl'
        with dest.open('x', encoding='utf-8') as f:
            f.write(json.dumps({'event':'pretrigger_history','tick_ms':tick,'reason':reason})+'\n')
            for item in self.ring:
                f.write(json.dumps(item, allow_nan=False)+'\n')
        return True

    def close(self):
        self.stream.close()

class Lifecycle:
    def __init__(self, process, pid, folder, emit, modules):
        self.process, self.pid, self.folder, self.emit_core = process, pid, Path(folder), emit
        self.modules = modules
        self.base = 0
        self.next_find = self.next_sample = self.next_health = 0
        self.history = History(folder)
        self.commands = CommandChannel(pid)
        self.samples = self.errors = self.unstable = 0
        self.max_us = 0
        self.previous = None
        self.empty = {}
        self.previous_owners = {}
        self.battle_generation = 0
        self.previous_battle = None
        self.last_actor = None
        self.history.emit({'event':'lifecycle110_started','pid':pid,'tick_ms':self.tick(),
            'interval_ms':200,'starts_before_mod_or_battle':True,'access':'QUERY_INFORMATION|VM_READ',
            'limits':'Selected units and followed actor, not all AI intentions. Native phase IDs need correlation. Samples are not complete write history.'})

    @staticmethod
    def tick():
        from process_probe import k
        k.GetTickCount64.restype = __import__('ctypes').c_uint64
        return k.GetTickCount64()

    def poll(self, now):
        if now < self.next_sample:
            return
        self.next_sample = now + .2
        started = time.perf_counter()
        tick = self.tick()
        try:
            if not self.base and now >= self.next_find:
                self.next_find = now + 1
                engine = next((m for m in self.modules(self.pid) if m['name'].lower() == 'empire.retail.dll'), None)
                if engine:
                    with Path(engine['path']).open('rb') as f:
                        digest = hashlib.file_digest(f,'sha256').hexdigest()
                    if digest != ENGINE_SHA:
                        self.emit_core('lifecycle110_engine_rejected', sha256=digest)
                        self.next_find = now + 60
                    else:
                        self.base = engine['base']
                        self.history.emit({'event':'engine_verified','tick_ms':tick, **engine, 'sha256':digest})
            if not self.base:
                return
            context = native_context(self.process.read, self.base)
            try:
                effects = stable_effects(self.process.read, self.base)
            except OSError as error:
                effects = {'stable':False, 'read_error':str(error)}
            identity = (context.get('environment'), context.get('phase_object'), context.get('phase_id'))
            if identity != self.previous_battle:
                self.battle_generation += 1
                self.history.emit({'event':'native_phase_transition','tick_ms':tick,
                    'previous':self.previous_battle,'current':identity,'context_epoch':self.battle_generation,
                    'phase_labels_unverified':True})
                self.previous_battle = identity
            row = {'event':'lifecycle_sample','tick_ms':tick,'context_epoch':self.battle_generation,
                   'context':context,'effects':effects,'commands100':self.commands.sample(tick)}
            anomalies = []
            if effects.get('stable'):
                current = {}
                for owner in effects.get('owners', []):
                    key = (owner['index'], owner['owner'], owner['element'])
                    current[key] = owner
                    if owner.get('stable') and (owner.get('empty_table') or not owner.get('known_effects_vtable')):
                        self.empty[key] = self.empty.get(key, 0) + 1
                        if self.empty[key] == 3:
                            anomalies.append({'kind':'persistent_active_effect_owner_candidate','entry':owner,
                                              'samples':3,'not_a_proven_root_cause':True})
                    else:
                        self.empty.pop(key, None)
                    old = self.previous_owners.get(key)
                    if old and old.get('stable') and owner.get('stable') and old.get('vtable') != owner.get('vtable'):
                        anomalies.append({'kind':'active_owner_vtable_transition','before':old,'after':owner,
                                          'allocation_generation_unproven':True})
                self.empty = {k:v for k,v in self.empty.items() if k in current}
                self.previous_owners = current
            self.unstable += int(not effects.get('stable') or context.get('unstable',False))
            actor = context.get('actor')
            if actor and not all(math.isfinite(x) for x in actor['position']+actor['velocity']):
                anomalies.append({'kind':'non_finite_actor_pose','actor':actor['pointer']})
            if actor and self.last_actor:
                old_tick, epoch, old = self.last_actor
                if epoch == self.battle_generation and actor['serial'] == old['serial'] and actor['pointer'] == old['pointer']:
                    dt = (tick-old_tick)/1000
                    displacement = math.dist(actor['position'],old['position'])
                    if 0 < dt <= 1 and displacement > max(12, math.dist(actor['velocity'],[0,0,0])*dt*4+3):
                        anomalies.append({'kind':'pose_discontinuity_candidate','distance':displacement,'seconds':dt,
                                          'animation_or_native_reposition_may_be_valid':True})
            self.last_actor = (tick,self.battle_generation,actor) if actor else None
            if anomalies:
                row['anomaly_candidates'] = anomalies
            self.history.emit(row)
            self.samples += 1
            if anomalies:
                self.history.trigger(tick, anomalies)
        except (OSError, ValueError, OverflowError) as error:
            self.errors += 1
            if self.errors <= 10 or self.errors % 100 == 0:
                self.history.emit({'event':'lifecycle_read_unavailable','tick_ms':tick,'error':str(error),
                                   'transition_or_process_exit_may_be_normal':True})
        finally:
            self.max_us = max(self.max_us, round((time.perf_counter()-started)*1e6))
            if now >= self.next_health:
                self.next_health = now + 2
                value = {'version':110,'pid':self.pid,'tick_ms':tick,'unix':time.time(),
                    'state':'sampling' if self.base else 'waiting_for_supported_engine','samples':self.samples,
                    'read_errors':self.errors,'unstable':self.unstable,'maximum_sample_us':self.max_us,
                    'dense_bytes':self.history.bytes,'dense_budget_reached':self.history.limited,
                    'pretriggers':self.history.triggers,'context_epoch':self.battle_generation}
                p=self.folder/'lifecycle110-health.json';tmp=p.with_suffix('.tmp')
                tmp.write_text(json.dumps(value));tmp.replace(p)

    def close(self):
        self.commands.close()
        self.history.emit({'event':'lifecycle110_finished','tick_ms':self.tick(),
                           'samples':self.samples,'read_errors':self.errors})
        self.history.close()
        path=self.folder/'lifecycle110-health.json'
        try:
            value=json.loads(path.read_text());value.update(state='finished',unix=time.time(),tick_ms=self.tick())
            tmp=path.with_suffix('.tmp');tmp.write_text(json.dumps(value));tmp.replace(path)
        except (OSError,ValueError):pass
