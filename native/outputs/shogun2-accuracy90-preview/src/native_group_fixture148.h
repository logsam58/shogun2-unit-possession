#pragma once
#include <set>
static std::set<void*> blocks147;
static void* __cdecl words147(unsigned n,unsigned){void* p=calloc(n,4);require(p!=nullptr,"Fixture allocation");blocks147.insert(p);return p;}
static void __cdecl free_words147(void* p){require(blocks147.erase(p)==1,"Foreign native allocation free");free(p);}
static unsigned __fastcall definition147(void*,void*){return 7;}
static bool __fastcall ability_probe147(void*,void*,unsigned){return false;}
static uintptr_t __fastcall no_terrain_cell147(void*,void*,const float*,unsigned){return 0;}
static LONG WINAPI fault147(EXCEPTION_POINTERS* e){auto c=e->ContextRecord;std::cerr<<"EIP="<<std::hex<<c->Eip<<" read="<<e->ExceptionRecord->ExceptionInformation[1]<<" ecx="<<c->Ecx<<" eax="<<c->Eax<<" stack=";auto s=reinterpret_cast<unsigned*>(c->Esp);for(unsigned i=0;i<20;++i)std::cerr<<s[i]<<',';std::cerr<<'\n';return fault100(e);}
