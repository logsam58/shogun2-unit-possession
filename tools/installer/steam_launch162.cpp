#include "steam_session163.h"
int wmain(int argc,wchar_t** argv){
    try{
#ifdef STEAM162_FIXTURE
        require(argc==6,"Fixture needs command, app, transaction, report and synthetic Steam root");
        auto root=steam162::fs::absolute(argv[5]).lexically_normal();
        require(root.filename()==L"synthetic-steam162"&&!steam162::fs::exists(root/L"steam.exe")&&
            steam162::read(root/L".unit-possession-fixture162")=="synthetic only\n","Refusing nonfixture Steam directory");
#else
        require(argc==5,"Setup needs command, app, transaction and report");
        auto root=steam162::registry_root();
#endif
        auto app=steam162::fs::absolute(argv[2]).lexically_normal();steam162::plain(app);
        auto transaction=steam162::fs::absolute(argv[3]).lexically_normal();auto state=app/L"steam-launch162.state";
        const std::wstring command=argv[1];
        if(command==L"status"){
            std::ofstream out(argv[4],std::ios::binary);out<<(steam163::running(root)?"RUNNING":"CLOSED");return 0;
        }
        if(command==L"request-shutdown"||command==L"restart"){
            if(command==L"request-shutdown"){if(steam163::running(root))steam163::launch_client(root,true);}
            else if(!steam163::running(root))steam163::launch_client(root,false);
        }
        else if(command==L"wait-closed"){steam163::wait_closed(root);}
        else {
#ifdef STEAM162_FIXTURE
            require(!steam163::running(root),"Synthetic Steam still running; fixture config preserved");
#else
            steam162::closed();
#endif
        if(command==L"check"){steam162::prepare(root,app);}
        else if(command==L"prepare"){auto plan=steam162::prepare(root,app);steam162::fresh(transaction,steam162::pack(plan));}
        else if(command==L"install"||command==L"rollback"){
            auto plan=steam162::unpack(steam162::read(transaction));steam162::verify(plan,root,app);
            if(command==L"install")steam162::apply(plan,app);else steam162::restore(plan,plan.before);
        }else if(command==L"uninstall-check"||command==L"uninstall"){
            auto plan=steam162::unpack(steam162::read(state));steam162::verify(plan,root,app);
            auto current=steam162::value(steam162::read(steam162::target(plan)));
            require(steam162::equivalent(current,plan.original)||current==std::optional<std::string>(plan.installed),"Launch Options changed outside setup; preserved. Restore them in Steam before uninstalling.");
            if(command==L"uninstall")steam162::restore(plan,plan.original);
        }else throw std::runtime_error("Unknown Steam integration command");
        }
        std::ofstream out(argv[4],std::ios::binary);out<<"OK: native Steam launch configuration "<<steam162::utf8(command)<<"\n";return 0;
    }catch(const std::exception& e){if(argc>=5){std::ofstream out(argv[4],std::ios::binary);out<<e.what()<<'\n';}std::cerr<<e.what()<<'\n';return 1;}
}
