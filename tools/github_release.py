"""Publish the verified VERSION release to this repository. Credentials stay in memory."""
import argparse,hashlib,json,os,re,subprocess,sys,urllib.error,urllib.parse,urllib.request,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];REPOSITORY='logsam58/shogun2-first-person';API='https://api.github.com/repos/'+REPOSITORY
VERSION=(ROOT/'VERSION').read_text().strip()
if not re.fullmatch(r'[0-9]+\.[0-9]+(?:\.[0-9]+)?',VERSION):raise RuntimeError('Invalid product version')
TAG='v'+VERSION
class NoRedirect(urllib.request.HTTPRedirectHandler):
 def redirect_request(self,*args,**kwargs):return None
opener=urllib.request.build_opener(NoRedirect)
def git(*args):return subprocess.check_output(['git',*args],cwd=ROOT,text=True).strip()
def credential():
 env=os.environ.copy();env['GIT_TERMINAL_PROMPT']='0';env['GCM_INTERACTIVE']='Never'
 result=subprocess.run(['git','-c','credential.interactive=never','credential','fill'],input=b'protocol=https\nhost=github.com\nusername=logsam58\n\n',stdout=subprocess.PIPE,stderr=subprocess.PIPE,env=env,cwd=ROOT)
 if result.returncode:raise RuntimeError('GitHub credentials are unavailable through the existing Git credential manager; no credential output was logged.')
 fields=dict(line.split('=',1) for line in result.stdout.decode().splitlines() if '=' in line)
 token=fields.get('password')
 if not token:raise RuntimeError('Git credential manager did not provide GitHub authentication')
 return token
class GitHub:
 def __init__(self,token):self.token=token
 def request(self,method,url,payload=None,binary=None):
  parsed=urllib.parse.urlsplit(url)
  if parsed.scheme!='https' or parsed.hostname not in {'api.github.com','uploads.github.com'}:raise RuntimeError('Refused an unexpected authenticated API host')
  if parsed.hostname=='uploads.github.com' and not parsed.path.startswith('/repos/'+REPOSITORY+'/releases/'):raise RuntimeError('Refused upload outside the intended repository')
  headers={'Authorization':'Bearer '+self.token,'Accept':'application/vnd.github+json','X-GitHub-Api-Version':'2026-03-10','User-Agent':'Shogun2FP-release-tool'}
  body=None
  if payload is not None:body=json.dumps(payload).encode();headers['Content-Type']='application/json'
  if binary is not None:body=binary;headers['Content-Type']='application/octet-stream'
  request=urllib.request.Request(url,data=body,headers=headers,method=method)
  try:
   with opener.open(request,timeout=120) as response:return json.load(response)
  except urllib.error.HTTPError as error:raise RuntimeError('GitHub '+method+' failed with HTTP '+str(error.code)) from None
 def get_optional(self,url):
  try:return self.request('GET',url)
  except RuntimeError as error:
   if str(error).endswith('HTTP 404'):return None
   raise
 def download_hash(self,asset_id):
  url=API+'/releases/assets/'+str(asset_id);headers={'Authorization':'Bearer '+self.token,'Accept':'application/octet-stream','X-GitHub-Api-Version':'2026-03-10','User-Agent':'Shogun2FP-release-verifier'}
  try:response=opener.open(urllib.request.Request(url,headers=headers),timeout=120)
  except urllib.error.HTTPError as error:
   if error.code not in {301,302,303,307}:raise RuntimeError('Asset verification download failed with HTTP '+str(error.code)) from None
   location=error.headers.get('Location','');parsed=urllib.parse.urlsplit(location)
   if parsed.scheme!='https' or parsed.hostname not in {'release-assets.githubusercontent.com','objects.githubusercontent.com'}:raise RuntimeError('Refused an unexpected asset-download host')
   # Signed asset redirects are fetched without GitHub authentication headers.
   try:response=opener.open(urllib.request.Request(location,headers={'User-Agent':'Shogun2FP-release-verifier'}),timeout=120)
   except urllib.error.HTTPError as error:raise RuntimeError('Asset verification download failed with HTTP '+str(error.code)) from None
  h=hashlib.sha256();size=0
  with response:
   while chunk:=response.read(1024*1024):h.update(chunk);size+=len(chunk)
  return h.hexdigest(),size

def main():
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--check',action='store_true');parser.add_argument('--publish',action='store_true');args=parser.parse_args()
 if not (args.check ^ args.publish):parser.error('Choose exactly one of --check or --publish')
 if git('remote','get-url','origin')!='https://github.com/'+REPOSITORY+'.git':raise RuntimeError('Origin differs from the authorized repository')
 api=GitHub(credential());profile=api.request('GET','https://api.github.com/user');repo=api.request('GET',API)
 if profile['login']!='logsam58' or not repo.get('permissions',{}).get('push'):raise RuntimeError('GitHub account/repository permission differs from the authorized owner')
 existing=api.request('GET',API+'/releases?per_page=100');release=next((x for x in existing if x['tag_name']==TAG),None)
 if args.check:
  print(json.dumps({'authenticated_user':profile['login'],'repository':repo['full_name'],'visibility':repo['visibility'],'push_permission':True,'tag':TAG,'existing_release':None if release is None else {'id':release['id'],'draft':release['draft'],'url':release['html_url']}},indent=2));return
 if git('status','--porcelain'):raise RuntimeError('Commit and review the source tree before publishing')
 head=git('rev-parse','HEAD')
 if git('rev-parse',TAG+'^{}')!=head:raise RuntimeError('Release tag does not point to the reviewed current commit')
 remote=git('ls-remote','--tags','origin','refs/tags/'+TAG+'^{}').split()
 if not remote or remote[0]!=head:raise RuntimeError('The annotated release tag must be pushed and verified first')
 folder=ROOT/'artifacts/releases'/TAG;manifest=json.loads((folder/'upload-manifest.json').read_text());assert manifest['source_commit']==head and manifest['version']==VERSION
 files=manifest['assets'];allowed={'Shogun2-First-Person-'+TAG+'-Setup.exe','Shogun2-First-Person-'+TAG+'-Windows.zip','SHA256SUMS.txt','release-manifest.json'}
 if VERSION!='0.15':allowed.update({'Shogun2-First-Person-'+TAG+'-Source.zip','SOURCE_SHA256SUMS.txt'})
 if {x['name'] for x in files}!=allowed:raise RuntimeError('Release asset list differs from the reviewed package')
 for item in files:
  p=folder/item['name']
  if not p.is_file() or p.stat().st_size!=item['bytes'] or hashlib.sha256(p.read_bytes()).hexdigest()!=item['sha256']:raise RuntimeError('Local release asset differs: '+item['name'])
 if VERSION!='0.15':
  source='Shogun2-First-Person-'+TAG+'-Source.zip';blobs={}
  for line in subprocess.check_output(['git','ls-tree','-r','-z','HEAD'],cwd=ROOT).split(b'\0'):
   if not line:continue
   meta,name=line.split(b'\t',1);mode,kind,blob=meta.split()
   if kind!=b'blob':raise RuntimeError('Source archive contains an unsupported Git entry')
   blobs[name.decode()]=blob.decode()
  with zipfile.ZipFile(folder/source) as bundle:
   if bundle.testzip() is not None or {n for n in bundle.namelist() if not n.endswith('/')}!=set(blobs):raise RuntimeError('Source archive inventory differs from the release commit')
   for name,expected in blobs.items():
    data=bundle.read(name)
    if hashlib.sha1(b'blob '+str(len(data)).encode()+b'\0'+data).hexdigest()!=expected:raise RuntimeError('Source archive blob differs: '+name)
  expected_sum=hashlib.sha256((folder/source).read_bytes()).hexdigest()+'  '+source+'\n'
  if (folder/'SOURCE_SHA256SUMS.txt').read_text()!=expected_sum:raise RuntimeError('Source archive checksum differs')
 from package_release import web_links
 notes=web_links((ROOT/'docs/releases'/(TAG+'.md')).read_text(encoding='utf-8'),Path('docs/releases')/(TAG+'.md'))
 if release and not release['draft']:raise RuntimeError(TAG+' is already published; refusing to replace or overwrite it')
 if release:
  if release['target_commitish']!=head or release['body']!=notes:raise RuntimeError('Existing '+TAG+' draft differs; refusing to overwrite it')
 else:
  release=api.request('POST',API+'/releases',{'tag_name':TAG,'target_commitish':head,'name':TAG+' - Shogun 2 First-Person Possession','body':notes,'draft':True,'prerelease':False,'generate_release_notes':False})
 receipt={'tag':TAG,'source_commit':head,'release_id':release['id'],'draft':True,'url':release['html_url'],'assets':[]};receipt_path=folder/'publication-receipt.json';receipt_path.write_text(json.dumps(receipt,indent=2)+'\n')
 assets=api.request('GET',release['assets_url']);known={x['name']:x for x in assets}
 if set(known)-allowed:raise RuntimeError('Existing draft contains unexpected assets; no deletion attempted')
 for item in files:
  asset=known.get(item['name'])
  if asset is None:
   upload=release['upload_url'].split('{',1)[0]+'?'+urllib.parse.urlencode({'name':item['name']});asset=api.request('POST',upload,binary=(folder/item['name']).read_bytes())
  digest=asset.get('digest')
  if asset['size']!=item['bytes'] or (digest and digest!='sha256:'+item['sha256']):raise RuntimeError('Uploaded asset metadata differs: '+item['name'])
  actual,size=api.download_hash(asset['id'])
  if actual!=item['sha256'] or size!=item['bytes']:raise RuntimeError('Downloaded asset bytes differ: '+item['name'])
  receipt['assets'].append({'name':item['name'],'id':asset['id'],'bytes':size,'sha256':actual,'download_verified':True,'url':asset['browser_download_url']});receipt_path.write_text(json.dumps(receipt,indent=2)+'\n');print('Verified uploaded asset: '+item['name'],flush=True)
 api.request('PATCH',API+'/releases/'+str(release['id']),{'draft':False,'prerelease':False,'make_latest':'true'})
 published=api.request('GET',API+'/releases/tags/'+TAG)
 if published['draft'] or published['prerelease'] or {x['name'] for x in published['assets']}!=allowed or published['body']!=notes:raise RuntimeError('Published release state differs')
 receipt.update(draft=False,prerelease=False,published_at=published['published_at'],url=published['html_url'],visibility=repo['visibility'],body_sha256=hashlib.sha256(notes.encode()).hexdigest());receipt_path.write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps({'published':True,'url':receipt['url'],'tag':TAG,'commit':head,'assets_verified':len(receipt['assets']),'visibility':repo['visibility']},indent=2))
if __name__=='__main__':
 try:main()
 except (RuntimeError,ValueError,KeyError,OSError,AssertionError) as error:raise SystemExit(str(error)) from None
