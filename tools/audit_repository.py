"""Audit the exact staged file list, or untracked source files before the initial add."""
import hashlib,json,re,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
BANNED={'.exe','.dll','.obj','.lib','.exp','.res','.pdb','.pack','.png','.cuf','.dmp','.dump','.sqlite','.db','.jsonl','.log','.zip','.7z','.pyc','.pem','.key'}
PATTERNS=[rb'gh[pousr]_[A-Za-z0-9]{30,}',rb'github_pat_[A-Za-z0-9_]{30,}',rb'AKIA[0-9A-Z]{16}',rb'-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----']
def files():
 records=subprocess.check_output(['git','ls-files','--stage','-z'],cwd=ROOT).split(b'\0');records=[x for x in records if x]
 if records:
  entries=[]
  for record in records:
   meta,name=record.split(b'\t',1);mode,oid,stage=meta.split()
   if stage!=b'0':raise SystemExit('Resolve the merge before auditing publication')
   entries.append((name.decode(),oid))
  raw=subprocess.check_output(['git','cat-file','--batch'],input=b'\n'.join(oid for _,oid in entries)+b'\n',cwd=ROOT);cursor=0;result=[]
  for name,oid in entries:
   end=raw.index(b'\n',cursor);actual,kind,size=raw[cursor:end].split();assert actual==oid and kind==b'blob';cursor=end+1;length=int(size);data=raw[cursor:cursor+length];cursor+=length+1;result.append((name,data))
  return result
 names=[x.decode() for x in subprocess.check_output(['git','ls-files','--others','--exclude-standard','-z'],cwd=ROOT).split(b'\0') if x]
 return [(name,(ROOT/name).read_bytes()) for name in names]

def main():
 names=files();errors=[];total=0;largest=[]
 for name,data in names:
  p=ROOT/name;total+=len(data);largest.append((len(data),name))
  if p.suffix.lower() in BANNED or name=='tools/local.json' or p.name.startswith('.env') or any(part in {'__pycache__','private','artifacts','automatic-recordings','reference'} for part in p.relative_to(ROOT).parts):errors.append(name+': excluded local/binary/evidence path')
  if len(data)>5*1024*1024:errors.append(name+': exceeds5MiB source-file limit')
  if data[:2]==b'MZ' or data[:4] in [b'MDMP',b'PFH3',b'PFH4',b'PK\x03\x04']:errors.append(name+': binary signature')
  if any(re.search(pattern,data) for pattern in PATTERNS):errors.append(name+': recognized credential pattern')
 report={'pass':not errors,'files':len(names),'bytes':total,'largest_files':[{'bytes':n,'path':p} for n,p in sorted(largest,reverse=True)[:5]],'errors':errors}
 print(json.dumps(report,indent=2));return 1 if errors else 0
if __name__=='__main__':raise SystemExit(main())
