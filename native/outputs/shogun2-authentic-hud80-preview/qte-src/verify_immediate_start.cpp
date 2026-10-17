#define QTE_SESSION_FIXTURE_ONLY
#include "verify_session.cpp"
#undef QTE_SESSION_FIXTURE_ONLY
int main(){try{using namespace qte;unsigned cases=0;
    // Both actions accept first-cycle input even when neither actor has
    // finished turning. Exactly-once native replay and button guards remain.
    for(bool attack:{false,true})for(bool success:{false,true})for(bool initially_held:{false,true}){
        SessionFixture f;f.world.setup(attack,5);f.facing_ready_now=false;
        auto ports=f.calls();ports.facing_ready=SessionFixture::facing;
        Session s(f.world.reader(),f.world.selection(),ports,World::engine,{0,5,true},true);f.session=&s;auto owner=f.world.owner();
        s.levels(initially_held&&attack,initially_held&&!attack);
        s.prepare(World::witness,1,owner,{1,0,200,.1,1},true);
        auto start=s.publication(99,200);check(start.prompt.active&&start.prompt.aligning&&!start.prompt.paused,"Startup was not immediately actionable");
        Snapshot shown;check(transport::presentation(start,200.02,shown)&&shown.elapsed>.019f,"Startup display stayed still");
        if(initially_held){
            s.prepare(World::witness,1,owner,{1,1,200.1,.1,1},true,false);
            check(s.publication(99,200.1).prompt.result==Result::Pending,"Initially held button answered startup prompt");
            s.levels(false,false);s.prepare(World::witness,1,owner,{1,2,200.2,.1,1},true,false);
        }
        bool clicked=false;
        for(unsigned frame=1;frame<600&&s.occupied();++frame){
            double simulation=(initially_held?.2:0)+double(frame)/120.,wall=200+simulation;
            ClockFrame clock{1,uint64_t(std::floor(simulation/.1+1e-8)),wall,.1,1};s.poll_clock(clock);
            auto data=s.publication(99,wall);check(transport::presentation(data,wall,shown),"Immediate meter publication vanished");
            if(!clicked&&in_success_zone(shown)==success){
                Click click;check(transport::click(data,wall,attack,!attack,true,click)&&!click.paused,"Pre-facing input was gated");
                s.enqueue(click);s.levels(attack,!attack);clicked=true;
            }
            s.prepare(World::witness,1,owner,clock,true,false);
        }
        check(clicked&&!s.occupied()&&f.world.prepares==1&&f.world.rolls==1,"Pre-facing answer did not replay exactly once");
        check(f.world.committed.outcome==((attack==success)?5u:0u),"Pre-facing answer changed native consequence");
        check(s.publication(99,205).error==0,"Immediate click and displayed phase diverged");++cases;
    }
    // Real pause still freezes both directions; a paused click is discarded
    // and releasing it permits a fresh click after resume, before facing.
    for(bool attack:{false,true}){
        SessionFixture f;f.world.setup(attack,5);f.facing_ready_now=false;auto ports=f.calls();ports.facing_ready=SessionFixture::facing;
        Session s(f.world.reader(),f.world.selection(),ports,World::engine,{0,5,true},true);f.session=&s;auto owner=f.world.owner();
        s.prepare(World::witness,1,owner,{1,0,200,.1,1},true);
        s.prepare(World::witness,1,owner,{1,1,200.1,.1,1},true,false);
        s.poll_clock({1,1,200.11,.1,0});auto data=s.publication(99,200.11);Snapshot a,b;
        check(transport::presentation(data,200.11,a)&&transport::presentation(data,200.31,b)&&a.paused&&a.elapsed==b.elapsed,"True pause did not freeze the immediate meter");
        Click click;check(transport::click(data,200.11,attack,!attack,true,click)&&click.paused,"True pause click lost pause flag");s.enqueue(click);s.levels(attack,!attack);
        s.prepare(World::witness,1,owner,{1,1,200.31,.1,0},true,false);
        check(s.publication(99,200.31).prompt.result==Result::Pending&&f.world.prepares==0,"Paused click triggered native preparation");
        s.levels(false,false);s.prepare(World::witness,1,owner,{1,2,200.41,.1,1},true,false);
        auto resumed=s.publication(99,200.41);check(!resumed.prompt.paused&&resumed.prompt.aligning,"Facing prevented resume");++cases;
    }
    std::cout<<"PASS: "<<cases<<" immediate-start session cases; first-publication motion, LMB/RMB first-cycle green/red judgement before facing, initially-held rejection, genuine pause/resume and exactly-once unchanged native replay. Fixture callbacks only.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
