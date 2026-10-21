// Internal maintenance entry: same transactional installer as the wizard.
#include "install.h"
int wmain(int argc, wchar_t** argv) {
    try {
        require(argc==2 || (argc==3 && !wcscmp(argv[2],L"--check-only")),
                "Use install_cli <game-folder> [--check-only]");
        std::wstring game=argv[1];
        payload(201,SETUP_BOOTSTRAP_SHA);payload(202,EMBEDDED_PACK_SHA);
        if(argc==2)install_all(game,known_folder(CSIDL_APPDATA));
        else validate_game(game);
        std::cout<<(argc==2?"Installed and verified":"Payload and upgrade target verified")<<'\n';
        if(argc==2)std::cout<<"Cleanup: "<<retention_report.caches_removed<<" old caches removed; "<<retention_report.deferred<<" items deferred (locked, modified or unrecognized).\n";
        return 0;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
