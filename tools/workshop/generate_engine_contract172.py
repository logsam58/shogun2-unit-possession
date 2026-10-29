"""Generate a conservative same-layout engine contract from owned, indexed input.

No engine code is executed or emitted. Hashes/ranges and our source references
are the distributable output. The preserved Ghidra export is private input.
"""
import argparse
import bisect
import csv
import hashlib
import json
from pathlib import Path
import re
import struct

ROOT = Path(__file__).resolve().parents[2]
STOCK = '022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5'
BUGFIX = 'c4d91d74ce9ff8338cf61d5b021b03a2776830177a590fcac205509a42239545'
NATIVE_ENTRIES = [
    'tools/workshop/entry165.cpp',
    'native/outputs/shogun2-accuracy90-preview/src/adapter.cpp',
    'native/outputs/shogun2-authentic-hud80-preview/qte-src/playable_runtime.cpp',
    'native/outputs/shogun2-ammunition83-preview/hud-src/hud.cpp',
]
ENTRIES=NATIVE_ENTRIES+['tools/installer/prepare_local_assets159.cpp']


def sha(data):
    return hashlib.sha256(data).hexdigest()


def image(data):
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    assert data[:2] == b'MZ' and data[pe:pe+4] == b'PE\0\0'
    assert struct.unpack_from('<H', data, pe+4)[0] == 0x14c
    count, opt = struct.unpack_from('<H', data, pe+6)[0], pe+24
    assert struct.unpack_from('<H', data, opt)[0] == 0x10b
    table = opt + struct.unpack_from('<H', data, pe+20)[0]
    sections = []
    for i in range(count):
        at = table + i*40
        virtual, rva, size, raw = struct.unpack_from('<4I', data, at+8)
        flags = struct.unpack_from('<I', data, at+36)[0]
        sections.append(dict(name=data[at:at+8].rstrip(b'\0').decode('ascii'),
                             rva=rva, virtual=virtual, size=size, raw=raw, flags=flags))
    return pe, opt, sections


def source_closure(entries=ENTRIES):
    pending = [ROOT / p for p in entries]
    found = {}
    while pending:
        p = pending.pop().resolve()
        if p in found:
            continue
        assert p.is_relative_to(ROOT)
        text = p.read_text(encoding='utf-8-sig')
        found[p] = text
        for inc in re.findall(r'^\s*#\s*include\s*"([^"]+)"', text, re.M):
            q = (p.parent / inc).resolve()
            # The generated manifest is data, not a source dependency on itself.
            if q.name == 'engine_contract172_generated.h':
                continue
            if q.is_file():
                pending.append(q)
    return found


def generate(stock, modified, export):
    a, b = stock.read_bytes(), modified.read_bytes()
    assert sha(a) == STOCK and sha(b) == BUGFIX
    pe, opt, sections = image(a)
    assert image(b)[2] == sections or all(
        all(x[k] == y[k] for k in ('name', 'rva', 'size', 'raw', 'flags'))
        for x, y in zip(sections, image(b)[2]))
    assert (export / 'image-provenance.tsv').read_text().split('\t')[1] == STOCK
    funcs = export / 'functions'
    starts = sorted(int(p.stem, 16) for p in funcs.glob('*.asm'))
    start_set = set(starts)
    # Every byte starts protected. Only complete, verified instruction bodies
    # outside the conservative dependency closure may become extension regions.
    protected = bytearray(b'\1') * len(a)
    spans, bodies = {}, {}

    def raw(rv):
        for sec in sections:
            if sec['rva'] <= rv < sec['rva']+sec['size']:
                return sec['raw']+rv-sec['rva']
        return None

    def body(rv):
        if rv in bodies:
            return bodies[rv]
        p = funcs / f'{rv:x}.asm'
        rows = []
        if p.is_file():
            for line in p.read_text(encoding='utf-8').splitlines():
                if not line.startswith('0x'):
                    continue
                addr, encoded, _ = line.split('\t', 2)
                pos, data = int(addr, 16), bytes.fromhex(encoded)
                off = raw(pos)
                assert off is not None and a[off:off+len(data)] == data, str(p)
                rows.append((pos, len(data), off))
        bodies[rv] = rows
        if rows:
            spans[rv] = (min(x[0] for x in rows), max(x[0]+x[1] for x in rows))
        return rows

    def containing(rv):
        n = bisect.bisect_right(starts, rv)-1
        if n < 0:
            return None
        candidate = starts[n]
        # A preceding function name is never sufficient evidence of containment.
        for pos, length, _ in body(candidate):
            if pos <= rv < pos+length:
                return candidate
        return None

    sources = source_closure(NATIVE_ENTRIES)
    references, seeds = [], set()
    for path, text in sorted(sources.items()):
        if path.name == 'engine_contract172.h':
            continue  # PE/crypto validator has no engine calls or actor layouts.
        for number, line in enumerate(text.splitlines(), 1):
            for token in re.findall(r'\b0x[0-9a-fA-F]+\b', line):
                rv = int(token, 16)
                off = raw(rv)
                if off is None:
                    continue
                owner = containing(rv)
                if owner is not None:
                    seeds.add(owner)
                references.append(dict(source=path.relative_to(ROOT).as_posix(), line=number,
                    value=hex(rv), function=hex(owner) if owner is not None else None,
                    classification='conservative numeric seed; not a claimed ABI proof'))

    # Include every address-taken function found in the relocation table. This
    # conservatively covers virtual tables, callback/factory pointers, and many
    # construction/layout producers, not only the few methods explicitly called.
    reloc_rva, reloc_size = struct.unpack_from('<II', a, opt+96+5*8)
    reloc = raw(reloc_rva)
    cursor = reloc
    while cursor < reloc+reloc_size:
        page, size = struct.unpack_from('<II', a, cursor)
        assert size >= 8 and cursor+size <= reloc+reloc_size
        for at in range(cursor+8, cursor+size, 2):
            word = struct.unpack_from('<H', a, at)[0]
            if word >> 12 != 3:
                continue
            ref = page+(word & 0xfff)
            off = raw(ref)
            if off is None:
                continue
            target = struct.unpack_from('<I', a, off)[0]-0x10000000
            if target in start_set:
                seeds.add(target)
            # Pin routines constructing/accessing non-code engine data. Their
            # operands are useful layout evidence; unresolved code stays pinned.
            for sec in sections:
                if sec['rva'] <= target < sec['rva']+max(sec['size'], sec['virtual']) and not sec['flags'] & 0x20000000:
                    owner = containing(ref)
                    if owner is not None:
                        seeds.add(owner)
                    break
        cursor += size
    edges = {}
    with (export / 'call-edges.tsv').open(encoding='utf-8', newline='') as f:
        for row in csv.DictReader(f, delimiter='\t'):
            caller = int(row['caller_rva'], 16)
            try:
                callee = int(row['to_address'], 16)-0x10000000
            except ValueError:
                continue
            if callee in start_set:
                edges.setdefault(caller, set()).add(callee)
    reached, queue = set(), list(seeds)
    while queue:
        rv = queue.pop()
        if rv in reached:
            continue
        reached.add(rv)
        queue.extend(edges.get(rv, set())-reached)
    extension_functions = []
    for rv in starts:
        if rv in reached:
            continue
        rows = body(rv)
        if not rows:
            continue
        # A routine can share chunks with a required function. Those bytes must
        # remain protected even when another function also claims them.
        extension_functions.append(rv)
        for _, length, off in rows:
            protected[off:off+length] = b'\0'*length
    for rv in reached:
        for _, length, off in body(rv):
            protected[off:off+length] = b'\1'*length
    # No source-derived address may fall into an extension region, including
    # a shared function chunk the nearest lexical function did not own.
    for ref in references:
        off=raw(int(ref['value'],16))
        if off is not None and not protected[off]:
            raise ValueError('Source binding overlaps an extension region: '+str(ref))
    # Header identity is parsed independently; executable/data section content
    # and relocation/import data are contracts. Resource artwork/version records
    # are not native ABI. No raw game data is emitted.
    records = []
    for sec in sections:
        if sec['name'] == '.rsrc':
            continue
        begin, end = sec['raw'], sec['raw']+sec['size']
        p = begin
        while p < end:
            if not protected[p]:
                p += 1
                continue
            q = p+1
            while q < end and protected[q] and q-p < 32768:
                q += 1
            records.append(dict(rva=sec['rva']+p-begin, raw=p, size=q-p,
                                stock=sha(a[p:q]), bugfix=sha(b[p:q]), section=sec['name']))
            p = q
    payload = dict(schema=172, family='original-steam-x86-2023', image_base=0x10000000,
        image_size=0x1d88000, entry_point=struct.unpack_from('<I', a, opt+16)[0],
        sections=sections, directories=list(struct.unpack_from('<32I', a, opt+96)),
        records=records)
    def normalized_header(data):
        h=bytearray(data[:struct.unpack_from('<I',data,opt+60)[0]])
        for pos,length in ((pe+8,4),(opt+64,4),(opt+96+4*8,8)):
            h[pos:pos+length]=b'\0'*length
        # The reviewed replacement extends .rdata's virtual size by eight.
        table=opt+struct.unpack_from('<H',a,pe+20)[0]
        for i,sec in enumerate(sections):
            if sec['name']=='.rdata':
                struct.pack_into('<I',h,table+i*40+8,sec['virtual'])
        return h
    assert normalized_header(a)==normalized_header(b)
    payload['header_sha256']=sha(normalized_header(a))
    payload['header_size']=struct.unpack_from('<I',a,opt+60)[0]
    payload['rdata_virtual_sizes']=[s['virtual'] for s in sections if s['name']=='.rdata']+[s['virtual'] for s in image(b)[2] if s['name']=='.rdata']
    hooks={'adapter':[0x367600,0x321310,0x319dd0,0x134fad0,0x2fdcf0,0x2ac0b0,0x31f660],
           'qte':[0x3e5d40,0x3eaac0,0x3cea10,0x433b80,0x3e1930,0x3e3de0,0xb92800,0x3e5aa0,0x3eaa10,0x3c4170],
           'lua':[0xea2d60]}
    payload['hooks']=[dict(kind=kind,rva=rv,sha256=sha(a[raw(rv):raw(rv)+16])) for kind,values in hooks.items() for rv in values]
    payload['contract_id'] = sha(json.dumps(payload, sort_keys=True, separators=(',', ':')).encode())
    source_digest = sha(json.dumps({p.relative_to(ROOT).as_posix():sha(t.encode())
        for p,t in sorted(sources.items())}, sort_keys=True).encode())
    evidence = dict(source_closure_sha256=source_digest, references=references,
        roots=len(seeds), protected_functions=len(reached),
        extension_functions=[hex(v) for v in extension_functions],
        export_summary=json.loads((export/'export-summary.json').read_text()),
        protected_bytes=sum(r['size'] for r in records),
        boundary='Conservative same-layout binary contract, not a proof of every indirect semantic interaction. Unknown/unrecognized code remains protected; address-taken/data-referencing routines and direct-call closure pinned. No relocated builds or foreign hook chaining.')
    return payload, evidence


def header(p):
    lines=['#pragma once', '// Generated hashes/metadata only; no game code or assets.',
           'namespace engine172 {', 'inline constexpr unsigned schema=172;',
           f'inline constexpr char contract_id[]="{p["contract_id"]}";',
           f'inline constexpr unsigned entry_point={p["entry_point"]};',
           f'inline constexpr char header_sha[]="{p["header_sha256"]}";',
           f'inline constexpr unsigned header_size={p["header_size"]};',
           'inline constexpr unsigned rdata_virtual_sizes[]={'+','.join(map(str,p['rdata_virtual_sizes']))+'};',
           'struct Section {const char* name;unsigned rva,virtual_size,size,raw,flags;};',
           'inline constexpr Section sections[]={']
    for s in p['sections']:
        lines.append('{"%s",%s},'%(s['name'],','.join(str(s[k]) for k in ('rva','virtual','size','raw','flags'))))
    lines += ['};', 'inline constexpr unsigned directories[]={'+','.join(map(str,p['directories']))+'};',
              'struct Region {unsigned rva,raw,size;const char* stock;const char* bugfix;};',
              'inline constexpr Region regions[]={']
    for r in p['records']:
        lines.append('{%d,%d,%d,"%s","%s"},'%(r['rva'],r['raw'],r['size'],r['stock'],r['bugfix']))
    lines += ['};','struct Hook {unsigned kind,rva;const char* sha;};','inline constexpr Hook hooks[]={']
    for h in p['hooks']:
        lines.append('{%d,%d,"%s"},'%({'adapter':1,'qte':2,'lua':3}[h['kind']],h['rva'],h['sha256']))
    return '\n'.join(lines+['};','}'])+'\n'


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    for name in ('stock','modified','export','output'):
        ap.add_argument('--'+name,type=Path,required=True)
    ap.add_argument('--write',action='store_true',help='Write maintained generated contract')
    args=ap.parse_args()
    p,e=generate(args.stock,args.modified,args.export)
    args.output.mkdir(parents=True,exist_ok=False)
    (args.output/'contract.json').write_text(json.dumps(p,indent=2)+'\n')
    (args.output/'provenance.json').write_text(json.dumps(e,indent=2)+'\n')
    target=ROOT/'native/common/engine_contract172_generated.h'
    if args.write:
        target.write_text(header(p),encoding='utf-8')
        sources=source_closure()
        manifest=dict(schema=172,family=p['family'],contract_id=p['contract_id'],
            source_files={q.relative_to(ROOT).as_posix():sha(q.read_bytes()) for q in sorted(sources)},
            generated_header_sha256=sha(target.read_bytes()),sections=p['sections'],
            regions=p['records'],bindings=e['references'],protected_functions=e['protected_functions'],
            extension_functions=e['extension_functions'],scope=e['boundary'],
            provenance=dict(stock_sha256=STOCK,reviewed_bugfix_sha256=BUGFIX,
                preserved_export_recognized_functions=e['export_summary']['recognized_functions'],
                private_analysis_reused=True,engine_code_or_assets_included=False))
        (ROOT/'tools/data/engine-contract172.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
    elif target.read_text(encoding='utf-8') != header(p):
        raise ValueError('Generated contract differs')
    print(json.dumps({k:e[k] for k in ('roots','protected_functions','protected_bytes')},indent=2),flush=True)


if __name__=='__main__':
    main()
