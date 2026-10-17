// Exercise production subject admission and independent steering in this process.
// No game process, remote writes, hooks or injected DLLs are used by this fixture.
#include "adapter.cpp"
int main(){try{
    base=0x10000000;
    std::vector<unsigned char> human(0x700),horse(0x708),other(0x700),land_unit(0x1000),
        man_config(0x100),horse_config(0x100),record(0x100),view_bytes(0x40),h1(0x20),h2(0x20),h3(0x20);
    auto ptr=[](auto& v){return reinterpret_cast<uintptr_t>(v.data());};
    auto man=ptr(human),mount=ptr(horse),u=ptr(land_unit),v=ptr(view_bytes),extra=ptr(other);
    std::array<uintptr_t,3> holders{ptr(h1),ptr(h2),ptr(h3)};std::array<uintptr_t,1> passengers{man};
    field<uintptr_t>(man,0)=field<uintptr_t>(extra,0)=base+0x15dfec8;field<uintptr_t>(mount,0)=base+0x15e8524;
    field<uintptr_t>(man,0x20c)=ptr(man_config);field<uintptr_t>(mount,0x20c)=ptr(horse_config);
    for(auto a:{man,mount,extra}){field<uintptr_t>(a,0x214)=u;field<unsigned char>(a,0xf0)=1;}
    field<uintptr_t>(man,0x210)=mount;field<unsigned>(man,0x1f0)=29;
    field<unsigned char>(mount,0xa1)=1;field<uint32_t>(mount,0x1c8)=32;
    field<uintptr_t>(u,0xf9c)=ptr(horse_config);field<uintptr_t>(u,0x2c)=ptr(record);field<unsigned>(ptr(record),0x1c)=1;
    field<uintptr_t>(v,0x1c)=reinterpret_cast<uintptr_t>(holders.data());field<unsigned>(v,0x18)=2;
    field<uintptr_t>(ptr(h1),8)=man;field<uintptr_t>(ptr(h2),8)=mount;field<uintptr_t>(ptr(h3),8)=extra;
    field<unsigned>(mount,0x6cc)=1;field<uintptr_t>(mount,0x6d0)=reinterpret_cast<uintptr_t>(passengers.data());
    NativeSubject subject;require(native_subject(man,u,v,false,subject)&&subject.actor==mount&&subject.holder==ptr(h2)&&subject.config==ptr(horse_config)&&subject.serial==32,"Exact native horse/passenger identity");
    // Replay the failed live Gen_Taisho category, which the recovered regular
    // cavalry predicate excludes. Reciprocal native mount ownership is valid.
    field<unsigned>(ptr(record),0x1c)=0;
    require(native_subject(man,u,v,false,subject)&&subject.actor==mount,"Live general category 0 must permit its exact native mount");
    field<unsigned>(ptr(record),0x1c)=1;
    auto reject=[&](){require(!native_subject(man,u,v,false,subject),"Unproven horse link accepted");};
    field<uintptr_t>(man,0x210)=0;reject();field<uintptr_t>(man,0x210)=mount;
    passengers[0]=extra;reject();passengers[0]=man;
    field<unsigned>(mount,0x6cc)=0;reject();field<unsigned>(mount,0x6cc)=1;
    field<uintptr_t>(mount,0x214)=0;reject();field<uintptr_t>(mount,0x214)=u;
    field<uintptr_t>(mount,0x20c)=ptr(man_config);reject();field<uintptr_t>(mount,0x20c)=ptr(horse_config);
    field<unsigned char>(mount,0xf0)=0;reject();field<unsigned char>(mount,0xf0)=1;
    field<unsigned>(v,0x18)=1;reject();field<unsigned>(v,0x18)=2;
    field<unsigned>(v,0x18)=3;field<uintptr_t>(extra,0x210)=mount;reject();field<uintptr_t>(extra,0x210)=0;field<unsigned>(v,0x18)=2;
    field<unsigned char>(mount,0xa1)=0;reject();require(native_subject(man,u,v,true,subject),"Native combat flags retain exact admitted mount");field<unsigned char>(mount,0xa1)=1;
    PossessionIdentity id{1,2,3,4,5,v,ptr(h1),man,u,ptr(man_config),2,1,mount,ptr(h2),ptr(horse_config),32};
    PossessionLease lease;require(lease.acquire(id)&&lease.owns(id),"Mounted lease acquisition");
    for(unsigned i=0;i<4;++i){auto stale=id;if(i==0)++stale.mount;if(i==1)++stale.mount_holder;if(i==2)++stale.mount_config;if(i==3)++stale.mount_id;require(!lease.owns(stale),"Recycled mount ownership");}
    field<uintptr_t>(man,0x210)=0;field<uintptr_t>(u,0xf9c)=ptr(man_config);field<unsigned char>(man,0xa1)=1;
    require(native_subject(man,u,v,false,subject)&&!subject.actor,"Dismounted subject uses human locomotion");
    for(unsigned bearing=0;bearing<65536;bearing+=251)for(unsigned input=0;input<16;++input){
        auto intent=steering_mount(input,bearing,.02f,4,1.4f);
        // Mouse look has no argument or state in mounted steering. The native
        // turn bound and movement direction depend only on horse bearing/keys.
        float n=std::hypot(intent.x,intent.z);bool forward=bool(input&FORWARD)!=bool(input&BACKWARD);
        require(std::abs(n-(forward?.08f:0.f))<.00001f,"Horse A/D must never strafe");
        require(std::abs(int(intent.turn))<=292,"Horse native turn rate bound");
        auto same=steering_mount(input,bearing,.02f,4,1.4f);require(intent.x==same.x&&intent.z==same.z&&intent.turn==same.turn,"Independent horse heading");
    }
    require(steering_mount(FORWARD,0,0,4,1).z==0&&steering_mount(FORWARD,0,.3f,4,1).z==0,"Invalid delta cannot move mount");
    int32_t parents[48]={-1,-1,-1,-1,0,0,0,0,0,5,6,7,9,10,11,14,12,14,14,13,14,17,15,18,21,21,22,21,21,23,21,21,26,29,32,33,34,34,34,35,35,35,36,37,38,39,40,41};
    float matrices[48*12]{};for(unsigned i=0;i<48;++i){auto m=matrices+i*12;m[0]=m[5]=m[10]=1;m[3]=float(i)/10;m[7]=float(i)/20;m[11]=float(i)/30;}
    float before[48*12];memcpy(before,matrices,sizeof matrices);unsigned weapons[]={1,2,3};
    require(pose_rider_matrices(matrices,48,parents,7,15,18,21,17,weapons,1.2f,.4f),"Live decoded samurai hierarchy upper-body pose");
    for(unsigned i=0;i<48;++i)if(!bone_descends(i,7,parents,48)&&i!=1&&i!=2&&i!=3)require(!memcmp(before+i*12,matrices+i*12,48),"Pelvis/legs/saddle must remain native");
    require(memcmp(before+15*12,matrices+15*12,48)!=0&&std::abs(matrices[21*12])<.001f,"Upper-body rotation and hidden head");
    memcpy(matrices,before,sizeof matrices);require(!pose_rider_matrices(matrices,48,parents,5,15,18,21,17,weapons,1,.4f)&&!memcmp(matrices,before,sizeof matrices),"Bad hierarchy cannot alter skin matrices");
    std::cout<<"PASS: exact native rider/horse/passenger links, unique roster ownership, dismount distinction, mount lifetime rejection, independent WASD steering, native turn limits and decoded-rig upper-body pose with unchanged pelvis/legs. No live game modified.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
