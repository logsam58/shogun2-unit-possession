#include "launch161.h"
#include <iostream>
static void check(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
static unsigned starts=0;
static DWORD count_start(const launch161::Plan&,bool){++starts;return 42;}
struct Action {int button;std::filesystem::path report;bool saw_child{};};
static void select(HWND window,void* value){
    auto a=static_cast<Action*>(value);a->saw_child=std::filesystem::exists(a->report);
    check(!(GetWindowLongW(window,GWL_EXSTYLE)&WS_EX_TOPMOST),"Chooser should not be globally topmost");
    check(GetWindow(window,GW_OWNER)==nullptr,"Prelaunch chooser unexpectedly owns a game window");
    if(a->button==IDCANCEL)PostMessageW(window,WM_CLOSE,0,0);else PostMessageW(window,WM_COMMAND,a->button,0);
}
static std::wstring quote(const std::wstring& value){
    std::wstring out=L"\"";unsigned slashes=0;
    for(auto c:value){if(c==L'\\'){++slashes;continue;}if(c==L'\"'){out.append(slashes*2+1,L'\\');out+=c;}else{out.append(slashes,L'\\');out+=c;}slashes=0;}
    out.append(slashes*2,L'\\');return out+L'\"';
}
int wmain(int argc,wchar_t** argv){try{
    check(argc==4,"Supply toy, new output directory and owned game directory");
    auto root=std::filesystem::absolute(argv[2]);check(std::filesystem::create_directory(root),"Fixture output must be new");
    auto toy=root/L"toy path with spaces.exe";std::filesystem::copy_file(argv[1],toy);
    wchar_t cwd[32768]{};GetCurrentDirectoryW(32768,cwd);
    SetEnvironmentVariableW(L"SteamAppId",L"34330");
    auto original_game=std::filesystem::path(argv[3]);
    launch161::validate({(original_game/L"shogun2.exe").wstring(),L"not executed"});
    launch161::validate({(original_game/L"launcher/launcher.exe").wstring(),L"not executed"});
    check(!launch162::signed_ca(toy.wstring()),"Unsigned toy was accepted as the CA launcher");
    for(bool relay:{false,true})for(int button:{startupdialog160::third_button,startupdialog160::first_button,IDCANCEL}){
        auto report=root/(std::to_wstring(button)+(relay?L"-relay.txt":L".txt"));
        auto command=quote(toy.wstring())+(relay?L" --relay ":L" ")+quote(report.wstring())+L" "+quote(L"unicode \u65e5\u672c")+L" "+quote(L"literal \"quote\" and \\tail\\")+L" \"\"";
        auto plan=launch161::parse(command);check(plan.command==command&&plan.application==toy,"Original Steam command was changed");
        Action action{button,report};startupdialog160::Observer observer{select,&action};
        auto status=launch161::choose_then_start(GetModuleHandleW(nullptr),plan,&observer);
        check(!action.saw_child,"Child was created before a choice");
        if(button==IDCANCEL){check(status==0&&!std::filesystem::exists(report),"Closing chooser launched a child");continue;}
        check(status==37,"Original child status not preserved");
        std::ifstream bytes(report,std::ios::binary);std::vector<char> data((std::istreambuf_iterator<char>(bytes)),{});
        check(data.size()%sizeof(wchar_t)==0,"Toy report is truncated");
        std::wstring text(data.size()/sizeof(wchar_t),0);memcpy(text.data(),data.data(),data.size());
        std::wistringstream input(text);std::wstring line;
        std::getline(input,line);check(line==(button==startupdialog160::third_button?L"third":L"first"),"Child view environment differs");
        std::getline(input,line);check(line==cwd,"Steam working directory changed");
        std::getline(input,line);check(line==L"unicode \u65e5\u672c","Unicode argument changed");
        std::getline(input,line);check(line==L"literal \"quote\" and \\tail\\","Quoted argument changed");
        std::getline(input,line);check(line.empty(),"Empty argument dropped");
        std::getline(input,line);check(line==L"34330","Steam environment was lost");
    }
    check(launch161::game_name(L"C:\\Steam Library\\Shogun2.exe")&&launch161::game_name(L"D:\\Game\\SHOGUN2.RETAIL.EXE"),"Stock executable names rejected");
    for(auto name:{L"C:\\cmd.exe",L"C:\\shogun2-unit-possession-launcher.exe",L"C:\\Shogun2.exe.bat"})check(!launch161::game_name(name),"Foreign launch target accepted");
    for(auto raw:{L"",L"relative.exe",L"%command%"}){bool rejected=false;try{launch161::parse(raw);}catch(...){rejected=true;}check(rejected,"Invalid Steam command accepted");}
    Action close{IDCANCEL,root/L"never.txt"};startupdialog160::Observer observer{select,&close};
    check(launch161::choose_then_start(GetModuleHandleW(nullptr),{L"unused",L"unused"},&observer,count_start)==0&&starts==0,"Cancellation dispatched launch");
    std::cout<<"PASS actual prelaunch dialogs and toy child: no child before selection; third/first inheritance; cancel; exact Unicode/quoted/empty arguments; Steam context/CWD; child exit; target guards. No game launched.\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
