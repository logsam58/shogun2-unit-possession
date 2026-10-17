#include "identity.h"
#include "decoded_rig_profiles89.h"
#include "mounted_settle_geometry88.h"
#include "native_palettes89.generated.h"
int main(){try{
 rig89::Node nodes[48];for(unsigned i=0;i<48;++i)nodes[i]={rig89::human[i].name,unsigned(wcslen(rig89::human[i].name)),rig89::human[i].parent};
 require(rig89::classify(nodes,48)==rig89::Layout::Human,"Human registry differs from decoded names/parents");
 const unsigned sockets[]={15,18,21,17,1,2,3};require(rig89::sockets(rig89::classify(nodes,48),sockets),"Captured sockets rejected");
 nodes[32].name=L"LeftForearmRoll";nodes[33].name=L"RightForearmRoll";
 require(rig89::classify(nodes,48)==rig89::Layout::HumanAlternate,"Archived alternate spelling not supported");
 nodes[3].name=L"Weapon1";require(rig89::classify(nodes,48)==rig89::Layout::Unknown,"Duplicate/foreign socket name accepted");
 unsigned wrong[]={15,18,21,17,1,3,2};require(!rig89::sockets(rig89::Layout::Human,wrong),"Swapped live binding accepted");
 rig89::Node mount[40];for(unsigned i=0;i<40;++i)mount[i]={rig89::horse[i].name,unsigned(wcslen(rig89::horse[i].name)),rig89::horse[i].parent};
 require(rig89::classify(mount,40)==rig89::Layout::Horse&&!rig89::sockets(rig89::Layout::Horse,sockets),"Horse was treated as rider");
 int32_t parents[48];for(unsigned i=0;i<48;++i)parents[i]=rig89::human[i].parent;
 unsigned weapons[]={1,2,3};unsigned checked=0;
 for(const auto& bits:NATIVE_PALETTES89){
  float m[576],expected[576];memcpy(expected,bits,sizeof expected);heldpose::Cache cache;heldpose::Acquisition88 acquisition;heldpose::Key key{1,2,3,4,++checked};
  for(unsigned tick=1;tick<=3;++tick){memcpy(m,expected,sizeof m);auto result=heldpose::apply_settled88(cache,acquisition,key,m,48,parents,7,weapons,true,tick,false);
   require(result==(tick==3?heldpose::SettleResult::Applied:heldpose::SettleResult::Waiting),"Saved original palette did not acquire on third distinct tick");
   for(unsigned j=0;j<576;++j)require(std::abs(m[j]-expected[j])<.0001f,"Acquisition distorted saved original geometry");}
  unsigned swapped[]={1,3,2};memcpy(m,expected,sizeof m);
  require(heldpose::apply_settled88(cache,acquisition,key,m,48,parents,7,swapped,true,4,false)==heldpose::SettleResult::Waiting&&!cache.valid,"Binding reuse retained a previous socket cache");
 }
 std::cout<<"PASS: exact human/alternate/horse profiles, mismatched socket refusal, "<<checked<<" saved original full palettes, stable acquisition and binding-reuse retirement. Finite offline evidence; no live acceptance.\n";
 return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
