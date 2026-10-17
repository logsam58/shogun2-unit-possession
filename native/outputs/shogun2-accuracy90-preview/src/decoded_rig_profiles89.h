#pragma once
#include <cstdint>
#include <cwchar>
namespace rig89 {
enum class Layout {Unknown,Human,HumanAlternate, Horse};
struct Node {const wchar_t* name;unsigned length;int32_t parent;};
struct Definition {const wchar_t* name;int32_t parent;};
inline constexpr Definition human[]={
{L"Hips",-1},
{L"Weapon1",-1},
{L"Weapon2",-1},
{L"Weapon3",-1},
{L"Armor_Front",0},
{L"LeftUpLeg",0},
{L"RightUpLeg",0},
{L"Spine",0},
{L"Tail",0},
{L"LeftLeg",5},
{L"RightLeg",6},
{L"Spine1",7},
{L"LeftFoot",9},
{L"RightFoot",10},
{L"Spine2",11},
{L"LeftShoulder",14},
{L"LeftToeBase",12},
{L"Neck",14},
{L"RightShoulder",14},
{L"RightToeBase",13},
{L"Sashimono",14},
{L"Head",17},
{L"LeftArm",15},
{L"RightArm",18},
{L"Eyes",21},
{L"Jaw",21},
{L"LeftForeArm",22},
{L"Left_Brow",21},
{L"Left_Mouth",21},
{L"RightForeArm",23},
{L"Right_Brow",21},
{L"Right_Mouth",21},
{L"LeftForeArmRoll",26},
{L"RightForeArmRoll",29},
{L"LeftHand",32},
{L"RightHand",33},
{L"LeftHandIndex1",34},
{L"LeftHandMiddle1",34},
{L"LeftHandThumb1",34},
{L"RightHandIndex1",35},
{L"RightHandMiddle1",35},
{L"RightHandThumb1",35},
{L"LeftHandIndex2",36},
{L"LeftHandMiddle2",37},
{L"LeftHandThumb2",38},
{L"RightHandIndex2",39},
{L"RightHandMiddle2",40},
{L"RightHandThumb2",41},
};
inline constexpr Definition horse[]={
{L"Hips",-1},
{L"LeftUpLeg",0},
{L"RightUpLeg",0},
{L"Spine",0},
{L"Tail",0},
{L"LeftLeg",1},
{L"RightLeg",2},
{L"Spine1",3},
{L"Tail1",4},
{L"LeftFoot",5},
{L"LeftShoulder",7},
{L"Lstirrup1",7},
{L"Neck",7},
{L"RightFoot",6},
{L"RightShoulder",7},
{L"Rstirrup1",7},
{L"Tail2",8},
{L"LeftShoulder2",10},
{L"LeftToeBase",9},
{L"Lstirrup2",11},
{L"Neck1",12},
{L"RightShoulder2",14},
{L"RightToeBase",13},
{L"Rstirrup2",15},
{L"Tail3",16},
{L"LeftArm",17},
{L"LeftToe_End",18},
{L"Neck2",20},
{L"RightArm",21},
{L"RightToe_End",22},
{L"Head",27},
{L"LeftForearm",25},
{L"RightForearm",28},
{L"Jaw",30},
{L"LeftEar",30},
{L"LeftHand",31},
{L"RightEar",30},
{L"RightHand",32},
{L"LeftHandIndex",35},
{L"RightHandIndex",37},
};
inline constexpr Definition human_alternate[]={
{L"Hips",-1},
{L"Weapon1",-1},
{L"Weapon2",-1},
{L"Weapon3",-1},
{L"Armor_Front",0},
{L"LeftUpLeg",0},
{L"RightUpLeg",0},
{L"Spine",0},
{L"Tail",0},
{L"LeftLeg",5},
{L"RightLeg",6},
{L"Spine1",7},
{L"LeftFoot",9},
{L"RightFoot",10},
{L"Spine2",11},
{L"LeftShoulder",14},
{L"LeftToeBase",12},
{L"Neck",14},
{L"RightShoulder",14},
{L"RightToeBase",13},
{L"Sashimono",14},
{L"Head",17},
{L"LeftArm",15},
{L"RightArm",18},
{L"Eyes",21},
{L"Jaw",21},
{L"LeftForeArm",22},
{L"Left_Brow",21},
{L"Left_Mouth",21},
{L"RightForeArm",23},
{L"Right_Brow",21},
{L"Right_Mouth",21},
{L"LeftForearmRoll",26},
{L"RightForearmRoll",29},
{L"LeftHand",32},
{L"RightHand",33},
{L"LeftHandIndex1",34},
{L"LeftHandMiddle1",34},
{L"LeftHandThumb1",34},
{L"RightHandIndex1",35},
{L"RightHandMiddle1",35},
{L"RightHandThumb1",35},
{L"LeftHandIndex2",36},
{L"LeftHandMiddle2",37},
{L"LeftHandThumb2",38},
{L"RightHandIndex2",39},
{L"RightHandMiddle2",40},
{L"RightHandThumb2",41},
};
template<unsigned N> inline bool matches(const Node* nodes,unsigned count,const Definition (&definition)[N]){
 if(!nodes||count!=N)return false;
 for(unsigned i=0;i<count;++i){auto length=std::wcslen(definition[i].name);
  if(!nodes[i].name||nodes[i].length!=length||nodes[i].parent!=definition[i].parent||std::wmemcmp(nodes[i].name,definition[i].name,length))return false;}
 return true;
}
inline Layout classify(const Node* nodes,unsigned count){
 if(matches(nodes,count,human))return Layout::Human;
 if(matches(nodes,count,human_alternate))return Layout::HumanAlternate;
 if(matches(nodes,count,horse))return Layout::Horse;
 return Layout::Unknown;
}
inline bool personal(Layout layout){return layout==Layout::Human||layout==Layout::HumanAlternate;}
// Exact shoulders, head, neck and independent weapon roots observed in Test87.
inline bool sockets(Layout layout,const unsigned* indices){
 static constexpr unsigned expected[]={15,18,21,17,1,2,3};
 if(!personal(layout)||!indices)return false;
 for(unsigned i=0;i<7;++i)if(indices[i]!=expected[i])return false;
 return true;
}
}
