#include "adapter.cpp"
int main(){try{
    std::vector<unsigned char> engine_bytes(0x15b2500),horse_bytes(0x710),rider_bytes(0x710),unit_bytes(0x1000);
    base=reinterpret_cast<uintptr_t>(engine_bytes.data());auto horse=reinterpret_cast<uintptr_t>(horse_bytes.data()),rider=reinterpret_cast<uintptr_t>(rider_bytes.data()),unit=reinterpret_cast<uintptr_t>(unit_bytes.data());
    Status fixture{};state=&fixture;state->mode=POSSESSED;state->lease_generation=42;state->actor_id=923;state->unit_id=9;state->heartbeat=GetTickCount64();
    enabled=1;possession_lease.active=true;possession_lease.generation=42;possession_lease.identity.mount=horse;possession_lease.identity.mount_id=983;
    field<uintptr_t>(horse,0)=base+0x15e8524;field<uintptr_t>(rider,0)=base+0x15dfec8;field<unsigned>(unit,4)=9;
    for(auto actor:{horse,rider}){field<uintptr_t>(actor,0x214)=unit;field<unsigned char>(actor,0xf0)=1;}
    field<uintptr_t>(rider,0x210)=horse;field<unsigned>(rider,0x1c8)=923;field<unsigned>(horse,0x1c8)=983;
    uintptr_t passenger=rider;field<unsigned>(horse,0x6cc)=1;field<uintptr_t>(horse,0x6d0)=reinterpret_cast<uintptr_t>(&passenger);
    field<float>(horse,0x48)=5;field<float>(horse,0x50)=8;field<float>(rider,0x48)=5.8f;field<float>(rider,0x50)=8.1f;
    melee60::Request request;request.pid=GetCurrentProcessId();request.phase=melee60::Facing;request.lease=42;request.player=983;request.player_unit=9;
    request.opponent=1492;request.opponent_unit=77;request.serial=1;request.heartbeat=GetTickCount64();request.player_kind=1;
    request.player_anchor[0]=5;request.player_anchor[2]=8;
    require(PossessionMeleeCommitment63(&request)==63,"Test63 commitment API rejected");melee60::Request current;
    require(melee_request(current),"Horse principal serial was confused with camera rider");
    bool player{},enemy{};uintptr_t linked{};
    require(melee_motion_identity(horse,current,player,enemy,linked)&&player&&!enemy&&linked==rider,"Horse motion identity not validated");
    require(!melee_motion_identity(rider,current,player,enemy,linked),"Rider received a second physical-root override");
    require(hold_melee_motion(horse)&&field<float>(horse,0x48)==5&&field<float>(horse,0x50)==8,"Native horse anchor shifted by rider offset");
    field<unsigned>(horse,0x1c8)=984;require(!melee_motion_identity(horse,current,player,enemy,linked),"Recycled horse serial admitted");field<unsigned>(horse,0x1c8)=983;
    ++state->lease_generation;require(!melee_request(current),"Stale possession lease admitted");--state->lease_generation;
    request.version=60;require(!PossessionMeleeCommitment63(&request),"Old commitment protocol admitted");
    std::cout<<"PASS: production mounted commitment admission, distinct rider/horse serials, root ownership, native anchor coordinates, recycled horse/stale lease/old protocol rejection. Local synthetic objects only.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
