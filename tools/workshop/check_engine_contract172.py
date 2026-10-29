"""Check maintained engine bindings against every compiled source dependency."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import sys
sys.path.insert(0,str(Path(__file__).resolve().parent))
from generate_engine_contract172 import ROOT, ENTRIES, NATIVE_ENTRIES, source_closure


def check(root=ROOT):
    manifest=json.loads((root/'tools/data/engine-contract172.json').read_text(encoding='utf-8'))
    if manifest['schema']!=172 or manifest['family']!='original-steam-x86-2023':
        raise ValueError('Unsupported engine dependency registry')
    # source_closure uses the current tree; packaged builds run their own copy.
    if root!=ROOT:
        raise ValueError('Registry check must use its own source tree')
    source=source_closure()
    actual={p.relative_to(root).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in source}
    if actual!=manifest['source_files']:
        difference=sorted(k for k in set(actual)|set(manifest['source_files']) if actual.get(k)!=manifest['source_files'].get(k))
        raise ValueError('Engine dependency sources need review/regeneration: '+', '.join(difference[:8]))
    if not set(ENTRIES)<=set(actual):
        raise ValueError('Missing compiled runtime entry point')
    generated=(root/'native/common/engine_contract172_generated.h').read_bytes()
    if hashlib.sha256(generated).hexdigest()!=manifest['generated_header_sha256']:
        raise ValueError('Generated native contract differs from its registry')
    contract=re.search(rb'contract_id\[\]="([a-f0-9]{64})"',generated)
    if not contract or contract[1].decode()!=manifest['contract_id']:
        raise ValueError('Engine contract identity differs')
    # Audit source candidates, not only hand-listed hook sites. The conservative
    # registry records false-positive numeric seeds explicitly rather than
    # claiming every literal is a native function or an ABI proof.
    old={(v['source'],int(v['value'],16)) for v in manifest['bindings']}
    regions=manifest['regions']
    for path,text in source_closure(NATIVE_ENTRIES).items():
        if path.name=='engine_contract172.h':
            continue
        name=path.relative_to(root).as_posix()
        for token in re.findall(r'\b0x[0-9a-fA-F]+\b',text):
            rv=int(token,16)
            backed=any(s['rva']<=rv<s['rva']+s['size'] for s in manifest['sections'])
            if backed and ((name,rv) not in old or not any(r['rva']<=rv<r['rva']+r['size'] for r in regions)):
                raise ValueError('Unregistered or unprotected engine candidate: '+name+' '+hex(rv))
    return dict(passed=True,schema=172,contract_id=manifest['contract_id'],source_files=len(actual),bindings=len(manifest['bindings']),protected_regions=len(regions))


if __name__=='__main__':
    argparse.ArgumentParser(description=__doc__).parse_args()
    print(json.dumps(check(),indent=2))
