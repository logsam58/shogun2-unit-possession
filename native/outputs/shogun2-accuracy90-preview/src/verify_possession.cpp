#include "identity.h"
#include "possession_lease.h"
#include "steering.h"
int main(){
    try {
        PossessionIdentity first{1,2,3,4,5,6,7,8,9,10,0,12};
        PossessionLease lease;
        require(!lease.owns(first),"Unacquired soldier rejected");
        require(lease.acquire(first)&&lease.owns(first)&&lease.generation==1,"Native serial zero is valid");
        // Same addresses can belong to different units/soldiers in later frames.
        for(unsigned n=0;n<12;++n){
            auto stale=first;
            switch(n){
                case 0:++stale.environment;break;case 1:++stale.common;break;
                case 2:++stale.phase;break;case 3:++stale.manager;break;
                case 4:++stale.camera;break;case 5:++stale.view;break;
                case 6:++stale.holder;break;case 7:++stale.actor;break;
                case 8:++stale.unit;break;case 9:++stale.config;break;
                case 10:++stale.actor_id;break;case 11:++stale.unit_id;break;
            }
            float native_displacement=23;
            if(lease.owns(stale))native_displacement=steering(FORWARD,0,.1f).z;
            require(native_displacement==23,"Identity mismatch cannot write motion intent");
        }
        lease.release();require(!lease.owns(first),"Release invalidates all old identity snapshots");
        auto next=first;next.actor_id=37;next.unit_id=22;
        require(lease.acquire(next)&&lease.generation==2&&!lease.owns(first),"New selected unit gets a new lease");
        lease.release();next.config=0;
        require(!lease.acquire(next)&&!lease.active,"Missing native configuration rejects acquisition");
        std::cout<<"PASS: possession identity across recycled actor/unit IDs, camera/view/holder/config changes, battle phases, release and reacquisition; invalid identity preserves native intent.\n";
        return 0;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
