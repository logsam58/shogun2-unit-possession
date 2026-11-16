// Production owned-pack reader boundaries only; no engine entry or game hooks.
#include "pack174_generated.h"
#include <cassert>
#include <iostream>

namespace case_fixture {
std::filesystem::path directory;
unsigned file_probes{};
std::filesystem::path backing(const std::filesystem::path& path){
 if(!directory.empty()&&path.parent_path()==directory){
  if(path.filename()==L"City.pack")return directory/L"campaign-addon.pack";
  if(path.filename()==L"city.pack")return directory/L"campaign-other.pack";
 }
 return path;
}
}
// Model two case-distinct files using separately named fixture packs.
namespace workshop165 {
static std::vector<unsigned char> fixture_root(const std::filesystem::path& path,bool stock,
    std::string* metadata,const char* target,unsigned size,const char* sha){
 return ui_root168(case_fixture::backing(path),stock,metadata,target,size,sha);
}
}
#define is_regular_file(path) is_regular_file((++case_fixture::file_probes,case_fixture::backing(path)))
#define ui_root168 fixture_root
#include "ui_compat174_generated.h"
#undef ui_root168
#undef is_regular_file
int wmain(int argc,wchar_t** argv){
 assert(argc==3);std::filesystem::path game=argv[1],fixtures=argv[2];unsigned passed=0;
 constexpr char root[]="ui/campaign ui/layout.root.luac";
 constexpr char sha[]="d14bea292474e62bb6e1e7a5f4938cdfa121b5bcca3003d693c9a31d5d703b50";
 const std::string identity(64,'a');
 auto select=[&](const std::wstring& mods){return workshop168::selected_root(game,L"add_working_directory \""+fixtures.wstring()+L"\";\n"+mods,identity,root,60729,sha);};
 auto select_frontend_mods=[&](const std::wstring& mods){
  return workshop168::selected_root(game,L"add_working_directory \""+fixtures.wstring()+L"\";\n"+mods,identity);
 };
 auto select_frontend=[&](const std::wstring& mod){
  return workshop168::selected_root(game,L"add_working_directory \""+fixtures.wstring()+L"\";\nmod \""+mod+L"\";",identity);
 };
 auto original=select(L"");assert(original.size()==60729);++passed;
 auto frontend=workshop168::selected_root(game,L"");assert(frontend.size()==13739&&assets159::sha(frontend.data(),frontend.size())==workshop165::root_sha);++passed;
 assert(select(L"mod \"database.pack\";")==original);++passed;
 auto alternate=select(L"mod \"campaign-addon.pack\";");assert(std::string(alternate.begin(),alternate.end())=="owned test addon A");++passed;
 assert(select(L"mod \"self.pack\";")==original);++passed;
 bool oversized=false;try{select(L"mod \"oversized.pack\";");}catch(const std::exception&){oversized=true;}assert(oversized);++passed;
 assert(select(L"mod \"aliases.pack\";")==original);++passed;
 assert(select_frontend(L"aliases.pack")==frontend);++passed;
 auto campaign_aliases=select(L"mod \"campaign-aliases.pack\";");
 assert(std::string(campaign_aliases.begin(),campaign_aliases.end())=="campaign bytes");++passed;
 auto frontend_aliases=select_frontend(L"frontend-aliases.pack");
 assert(std::string(frontend_aliases.begin(),frontend_aliases.end())=="frontend bytes");++passed;
 auto rejects=[&](auto read,const char* diagnostic){
  bool rejected=false;
  try{read();}catch(const std::exception& error){rejected=std::string(error.what()).find(diagnostic)!=std::string::npos;}
  assert(rejected);++passed;
 };
 rejects([&]{select(L"mod \"campaign-addon.pack\";mod \"campaign-other.pack\";");},
         "Enabled packs campaign-addon.pack and campaign-other.pack replace the same UI root with different scripts");
 rejects([&]{select_frontend_mods(L"mod \"frontend-aliases.pack\";mod \"frontend-other.pack\";");},
         "Enabled packs frontend-aliases.pack and frontend-other.pack replace the same UI root with different scripts");
 rejects([&]{select(L"mod \"foreign-root.pack\";");},
         "Enabled pack foreign-root.pack: Another, older or incomplete Unit Possession build is enabled");
 rejects([&]{select(L"mod \"empty-campaign.pack\";");},"UI root script is missing or too large");
 rejects([&]{select_frontend(L"empty-frontend.pack");},"UI root script is missing or too large");
 rejects([&]{select(L"mod \"duplicate-root.pack\";");},"Duplicate UI root script");
 rejects([&]{select_frontend(L"duplicate-frontend.pack");},"Duplicate UI root script");
 rejects([&]{select(L"mod \"duplicate-metadata.pack\";");},"Duplicate Unit Possession metadata");
 rejects([&]{select(L"mod \"unsafe-aliases.pack\";");},"Unsafe/out-of-range pack entry");
 rejects([&]{select(L"mod \"truncated-aliases.pack\";");},"Unsafe/out-of-range pack entry");
 case_fixture::directory=fixtures.lexically_normal();
 assert(select(L"mod \"City.pack\"; mod \"City.pack\";")==alternate);++passed;
 auto other=select(L"mod \"city.pack\";");
 assert(std::string(other.begin(),other.end())=="owned test addon B");++passed;
 rejects([&]{select(L"mod \"City.pack\"; mod \"city.pack\";");},"replace the same UI root with different scripts");
 rejects([&]{select(L"mod \"city.pack\"; mod \"City.pack\";");},"replace the same UI root with different scripts");
 case_fixture::directory.clear();
 std::wstring repeated;
 const auto directory=L"add_working_directory \""+fixtures.wstring()+L"\"; ";
 for(unsigned i=0;i<128;++i)repeated+=directory;
 for(unsigned i=0;i<256;++i)repeated+=L"mod \"campaign-addon.pack\"; ";
 case_fixture::file_probes=0;
 assert(workshop168::selected_root(game,repeated,identity,root,60729,sha)==alternate);
 if(case_fixture::file_probes!=2){
  std::cerr<<"Repeated mod directives caused "<<case_fixture::file_probes<<" pack path probes instead of 2\n";
  return 1;
 }
 ++passed;
 auto path=fixtures/L"owned-transient.luac";HANDLE file=workshop165::transient_root168(path,original);assert(file!=INVALID_HANDLE_VALUE);
 bool occupied=false;try{auto other=workshop165::transient_root168(path,original);CloseHandle(other);}catch(const std::exception&){occupied=true;}assert(occupied);CloseHandle(file);assert(!std::filesystem::exists(path));++passed;
#ifdef S2UP_ROOT_PACK_HANDLE
 const auto addon=fixtures/L"campaign-addon.pack";
 {
  // A normal read-only participant can hold the pack throughout selection.
  HANDLE other_reader=CreateFileW(addon.c_str(),GENERIC_READ,FILE_SHARE_READ,
                                 nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
  assert(other_reader!=INVALID_HANDLE_VALUE);
  assert(select(L"mod \"campaign-addon.pack\";")==alternate);
  CloseHandle(other_reader);++passed;
 }
 {
  // A writer that allows concurrent readers hid the old reader's split view.
  HANDLE writer=CreateFileW(addon.c_str(),GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
                            nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
  assert(writer!=INVALID_HANDLE_VALUE);
  rejects([&]{select(L"mod \"campaign-addon.pack\";");},"Cannot open UI pack for a stable read");
  CloseHandle(writer);
  assert(select(L"mod \"campaign-addon.pack\";")==alternate);++passed;
 }
 {
  workshop165::RootPackFile held(addon);
  HANDLE writer=CreateFileW(addon.c_str(),GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
                            nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
  assert(writer==INVALID_HANDLE_VALUE&&GetLastError()==ERROR_SHARING_VIOLATION);
  assert(!MoveFileExW(addon.c_str(),(fixtures/L"renamed-while-reading.pack").c_str(),0)&&
         GetLastError()==ERROR_SHARING_VIOLATION);
  assert(held.size()==std::filesystem::file_size(addon));++passed;
 }
#endif
 std::cout<<"PASS "<<passed<<" campaign181 owned-reader/coexistence/lifetime checks; no game or Steam execution\n";
}
