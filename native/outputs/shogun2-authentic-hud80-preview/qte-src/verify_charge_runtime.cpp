#include "playable_runtime.cpp"
static volatile LONG calculations,fixture_errors;
static int __fastcall fixture_score(void*,void*,void* a,void* b,void*,uint32_t debug){
    if(debug)InterlockedIncrement(&fixture_errors);InterlockedIncrement(&calculations);
    return int(static_cast<effective::ActorContext*>(a)->flags)-int(static_cast<effective::ActorContext*>(b)->flags);
}
static int __fastcall fixture_defence(void*,void*,void*,void* b){return int(static_cast<effective::ActorContext*>(b)->flags);}
static int __fastcall fixture_armour(void*,void*,void*,void*,int){return 0;}
static int __fastcall fixture_facing(void*,void*){return 0;}
static DWORD WINAPI migrate(void*){
    for(unsigned i=0;i<1000;++i){
        while(InterlockedCompareExchange(&coordinator,1,0))Sleep(0);
        owns_coordinator=true;candidate_scope(0x1000,4);float own{},other{};
        if(!effective_pair(nullptr,100,200,own,other)||own!=60||other!=20||combat_candidate_count!=2)InterlockedIncrement(&fixture_errors);
        owns_coordinator=false;InterlockedExchange(&coordinator,0);
    }return 0;
}
int main(){try{
    native_score=reinterpret_cast<effective::Score>(fixture_score);
    native_mitigation={reinterpret_cast<effective::Defence>(fixture_defence),reinterpret_cast<effective::Armour>(fixture_armour),reinterpret_cast<effective::Facing>(fixture_facing)};
    candidate_scope(0x1000,4);
    effective::Capture chosen;chosen.valid=true;chosen.witness=0x1000;chosen.generation=4;
    chosen.a.actor=100;chosen.a.flags=30;chosen.b.actor=200;chosen.b.flags=10;chosen.pair.attacker=100;chosen.pair.defender=200;
    combat_candidates[0]=chosen;combat_candidates[1]=chosen;combat_candidates[1].b.actor=201;combat_candidates[1].b.flags=100;
    combat_candidate_count=2;combat_capture=combat_candidates[1];
    HANDLE a=CreateThread(nullptr,0,migrate,nullptr,0,nullptr),b=CreateThread(nullptr,0,migrate,nullptr,0,nullptr);
    require(a&&b&&WaitForSingleObject(a,10000)==WAIT_OBJECT_0&&WaitForSingleObject(b,10000)==WAIT_OBJECT_0,"Score migration fixture did not finish");
    CloseHandle(a);CloseHandle(b);require(!fixture_errors&&calculations==4000,"Chosen native pair score lost across workers or wrong group pair used");
    candidate_scope(0x1000,5);owns_coordinator=true;float own{},other{};
    require(!effective_pair(nullptr,100,200,own,other)&&!combat_candidate_count&&!combat_capture.valid,"Recycled witness reused old score contexts");
    std::cout<<"PASS: production generation-scoped score contexts preserve the chosen pair across 2000 callbacks on two workers, ignore a different last-scored group enemy, retain both directional margins, and discard reused witness contexts. Fixture calculator only; no game attached.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
