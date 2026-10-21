// Self-contained production transaction tests. No game image/payload required.
#define VERIFY_FIXTURE
#include "../../outputs/shogun2-possession-cleanup-v46/src/identity.h"
static const wchar_t* PACK=L"shogun2_possession_checkpoint45.pack";
#include "installer_io.h"
#include "flash_startup86.h"
#include "durable_transaction.h"
#include <filesystem>

static void flags(unsigned a,unsigned b){flashstartup86::Key key;
    require(RegCreateKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),0,nullptr,0,KEY_ALL_ACCESS|KEY_WOW64_32KEY,nullptr,&key.value,nullptr)==ERROR_SUCCESS,"Fixture key creation");
    unsigned values[]={a,b};for(unsigned i=0;i<2;++i)require(RegSetValueExW(key.value,flashstartup86::names[i],0,REG_DWORD,reinterpret_cast<BYTE*>(&values[i]),4)==ERROR_SUCCESS,"Fixture flag write");
    DWORD unrelated=77;require(RegSetValueExW(key.value,L"Unrelated",0,REG_DWORD,reinterpret_cast<BYTE*>(&unrelated),4)==ERROR_SUCCESS,"Unrelated fixture value");}
static std::vector<SavedFile> saved(const std::wstring& game,const std::wstring& app){std::vector<SavedFile> out;
    for(auto& path:durable::Transaction::targets(game,app)){SavedFile file;file.path=path;file.existed=durable::exists(path);file.bytes=read_bytes(path);out.push_back(file);}return out;}
static std::vector<std::string> desired(){return {"new pack","new loader","new selection","new launcher","new registry restore"};}
static void initialize(const std::wstring& game,const std::wstring& app){require(SHCreateDirectoryExW(nullptr,(game+L"\\data").c_str(),nullptr)==ERROR_SUCCESS,"Fixture directories");
    auto paths=durable::Transaction::targets(game,app);for(unsigned i:{1u,2u,3u})atomic_bytes(paths[i],"original "+std::to_string(i));
    atomic_bytes(paths[1]+L".possession-setup-new","unknown legacy staging data");flags(0,9);}
static void check_before(const std::wstring& game,const std::wstring& app){auto paths=durable::Transaction::targets(game,app);
    for(unsigned i=0;i<5;++i)require((i==0||i==4)?!durable::exists(paths[i]):read_bytes(paths[i])=="original "+std::to_string(i),"Terminated install did not restore original file state");
    auto state=flashstartup86::Snapshot{};require(state.before[0].exists&&state.before[0].value==0&&state.before[1].exists&&state.before[1].value==9,"Registry rollback differs");}
static void check_after(const std::wstring& game,const std::wstring& app){auto paths=durable::Transaction::targets(game,app);auto after=desired();
    for(unsigned i=0;i<5;++i)require(read_bytes(paths[i])==after[i],"Committed transaction did not retain new file state");require(flashstartup86::Snapshot{}.skipped(),"Committed registry state differs");}
static void child(const std::wstring& game,const std::wstring& app,const std::wstring& event,int stop){
    durable::Lock lock;durable::Transaction::recover(game,app);auto files=saved(game,app);flashstartup86::Snapshot before;
    Handle ready(OpenEventW(EVENT_MODIFY_STATE,FALSE,event.c_str()));require(ready.h!=nullptr,"Fixture event open");
    durable::checkpoint=[&](int point){if(point==stop){require(SetEvent(ready.h)!=0,"Fixture checkpoint notification");Sleep(INFINITE);}};
    durable::Transaction transaction(game,app,files,desired(),before);transaction.apply();}
static std::wstring arg(const std::wstring& s){require(s.find(L'"')==s.npos,"Invalid fixture argument");return L"\""+s+L"\"";}
int wmain(int argc,wchar_t** argv){try{
    if(argc==7&&!wcscmp(argv[1],L"--child")){flashstartup86::key_path=argv[5];child(argv[2],argv[3],argv[4],_wtoi(argv[6]));return 0;}
    require(argc==1,"Invalid transaction fixture arguments");
    wchar_t exe[32768];require(GetModuleFileNameW(nullptr,exe,32768)>0,"Fixture executable path");
    wchar_t temporary[MAX_PATH];require(GetTempPathW(MAX_PATH,temporary)>0,"Fixture temporary directory");
    auto root=std::wstring(temporary)+L"s2fp fixture "+transaction_nonce();
    unsigned cases=0;
    for(int point=0;point<=14;++point){auto scenario=root+L"\\case-"+std::to_wstring(point),game=scenario+L"\\game",app=scenario+L"\\profile";
        flashstartup86::key_path=L"Software\\Shogun2PossessionTransactionFixture-"+transaction_nonce();initialize(game,app);
        auto event=L"Local\\Shogun2TransactionKill-"+transaction_nonce();Handle ready(CreateEventW(nullptr,TRUE,FALSE,event.c_str()));require(ready.h!=nullptr,"Fixture event creation");
        std::wstring command=arg(exe)+L" --child "+arg(game)+L" "+arg(app)+L" "+arg(event)+L" "+arg(flashstartup86::key_path)+L" "+std::to_wstring(point);
        STARTUPINFOW startup{};startup.cb=sizeof startup;PROCESS_INFORMATION pi{};
        require(CreateProcessW(exe,command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&pi)!=0,"Cannot start owned installer fixture");
        Handle process(pi.hProcess),thread(pi.hThread);auto wait=WaitForSingleObject(ready.h,15000);
        // This is our just-created toy child, never a game PID or discovered process.
        if(wait!=WAIT_OBJECT_0){TerminateProcess(process.h,78);WaitForSingleObject(process.h,5000);require(false,"Child did not reach requested transaction boundary");}
        require(TerminateProcess(process.h,77)!=0,"Cannot terminate owned fixture");require(WaitForSingleObject(process.h,5000)==WAIT_OBJECT_0,"Owned fixture did not terminate");
        durable::Transaction::recover(game,app);if(point==14)check_after(game,app);else check_before(game,app);
        durable::Transaction::recover(game,app); // Repeated recovery must be harmless.
        require(read_bytes(durable::Transaction::targets(game,app)[1]+L".possession-setup-new")=="unknown legacy staging data","Unknown staging file changed");
        {durable::Lock lock;auto files=saved(game,app);flashstartup86::Snapshot before;durable::Transaction repair(game,app,files,desired(),before);repair.apply();}
        check_after(game,app);durable::Transaction::recover(game,app);check_after(game,app);
        require(RegDeleteKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),KEY_WOW64_32KEY,0)==ERROR_SUCCESS,"Fixture registry cleanup");
        ++cases;std::cout<<"PASS forced termination at boundary "<<point<<"; recovery and repair\n";
    }
    // Unknown target edits block recovery without overwriting that edit or any
    // other target. This exercises real comparison logic, not a mock policy.
    auto game=root+L"\\conflict\\game",app=root+L"\\conflict\\profile";flashstartup86::key_path=L"Software\\Shogun2PossessionTransactionFixture-"+transaction_nonce();initialize(game,app);
    {auto files=saved(game,app);flashstartup86::Snapshot before;durable::Transaction transaction(game,app,files,desired(),before);
        atomic_bytes(files[1].path,"user changed this after preparation");bool rejected=false;try{transaction.restore();}catch(...){rejected=true;}
        require(rejected&&read_bytes(files[1].path)=="user changed this after preparation"&&!durable::exists(files[0].path),"Unknown edit was overwritten during recovery");}
    RegDeleteKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),KEY_WOW64_32KEY,0);
    // An originally absent key must be absent again after partial application.
    game=root+L"\\absent\\game";app=root+L"\\absent\\profile";flashstartup86::key_path=L"Software\\Shogun2PossessionTransactionFixture-"+transaction_nonce();initialize(game,app);
    require(RegDeleteKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),KEY_WOW64_32KEY,0)==ERROR_SUCCESS,"Remove absent-key fixture");
    {auto files=saved(game,app);flashstartup86::Snapshot before;require(!before.key_existed,"Fixture key unexpectedly exists");
     durable::Transaction transaction(game,app,files,desired(),before);bool failed=false;try{transaction.apply({},{},5);}catch(...){failed=true;}
     require(failed&&!flashstartup86::Snapshot{}.key_existed,"Absent registry key was not restored");}
    // Preserve an external registry change and unknown journal contents.
    game=root+L"\\registry-conflict\\game";app=root+L"\\registry-conflict\\profile";flashstartup86::key_path=L"Software\\Shogun2PossessionTransactionFixture-"+transaction_nonce();initialize(game,app);
    {auto files=saved(game,app);flashstartup86::Snapshot before;durable::Transaction transaction(game,app,files,desired(),before);
     atomic_bytes(transaction.folder+L"\\user-note.txt","unknown journal content");flags(42,9);bool rejected=false;try{transaction.restore();}catch(...){rejected=true;}
     require(rejected&&flashstartup86::Snapshot{}.before[0].value==42,"External registry change overwritten");
     require(read_bytes(files[1].path)=="original 1","File mutated before registry conflict preflight");flags(0,9);transaction.restore();
     require(read_bytes(transaction.folder+L"\\user-note.txt")=="unknown journal content","Unknown journal file removed");}
    RegDeleteKeyExW(flashstartup86::hive,flashstartup86::key_path.c_str(),KEY_WOW64_32KEY,0);
    std::cout<<"PASS "<<cases<<" real child kills, repair, absent registry key, external file/registry edits and unknown journal/stage preservation. HKCU fixtures only; no game.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
