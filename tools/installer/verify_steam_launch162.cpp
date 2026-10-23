#include "steam_launch162.h"
static void check(bool b,const char* text){if(!b)throw std::runtime_error(text);}
int wmain(int argc,wchar_t** argv){try{
    check(argc==2,"Need new fixture output directory");using namespace steam162;auto base=fs::absolute(argv[1]);check(!fs::exists(base),"Fixture already exists");
    auto root=base/L"synthetic-steam162",app=base/L"app with spaces";
    fs::create_directories(root/L"config");fs::create_directories(root/L"userdata/123/config");fs::create_directories(app);
    fresh(root/L"config/loginusers.vdf","\"users\" { \"76561197960265851\" { \"MostRecent\" \"1\" } }");
    check(user(root)=="123","Recent Steam account resolution failed");
    auto file=root/L"userdata/123/config/localconfig.vdf";
    std::string original="\xef\xbb\xbf// comment\r\n\"UserLocalConfigStore\"\r\n{\r\n\"Software\" { \"Valve\" { \"Steam\" { \"apps\" { \"34330\" { \"LaunchOptions\" \"-dx11 -foo \\\"value\\\"\" \"Other\" \"keep\" } \"9\" {\"LaunchOptions\" \"untouched\"} } } } }\r\n}\r\n";
    fresh(file,original);auto p=prepare(root,app);verify(p,root,app);check(p.original==value(original),"Existing game arguments lost");
    check(unpack(pack(p)).installed==p.installed,"Transaction did not round trip");apply(p,app);
    auto configured=read(file);check(value(configured)==std::optional<std::string>(p.installed),"Launch options not installed");
    check(configured.find("\"Other\" \"keep\"")!=configured.npos&&configured.find("\"untouched\"")!=configured.npos,"Other config changed");
    auto repeat=prepare(root,app);apply(repeat,app);check(read(file)==configured,"Repeated setup changed Steam bytes");
    restore(repeat,repeat.before);check(read(file)==configured,"Failed repair disabled an existing wrapper");
    auto changed=configured+"// unrelated later setting\r\n";replace(file,configured,changed);restore(p,p.original);
    check(read(file)==original+"// unrelated later setting\r\n","Uninstall lost unrelated changes or original arguments");
    restore(p,p.original);check(value(read(file))==p.original,"Repeat uninstall failed");
    auto clean=std::string("\"UserLocalConfigStore\" { \"Unrelated\" \"value\" }\n");
    auto added=edit(clean,"wrapper");check(value(added)==std::optional<std::string>("wrapper"),"Missing nested keys not inserted");check(!value(edit(added,{})),"Absent original launch key not removed");
    check(equivalent({},std::string(""))&&!equivalent({},std::string("-dx11")),"Steam UI Clear action differs from an absent option");
    for(const char* bad:{"\"a\" {", "\"a\" \"b\" }", "\"a\" \"unterminated", "\"UserLocalConfigStore\" {} \"userlocalconfigstore\" {}"}){
        bool rejected=false;try{value(bad);}catch(...){rejected=true;}check(rejected,"Malformed/ambiguous VDF accepted");}
    auto escaping=std::string("-opt \\\"\t\n\r");check(Parser{vdf_quote(escaping)}.token()==escaping,"VDF escaping changed arguments");
    bool refused=false;try{installed(p.launcher,"another.exe %command%");}catch(...){refused=true;}check(refused,"Foreign wrapper overwritten");
    auto current=read(file);replace(file,current,edit(current,"-changed-by-player"));refused=false;try{prepare(root,app);}catch(...){refused=true;}check(refused&&value(read(file))==std::optional<std::string>("-changed-by-player"),"Edited user options overwritten");
    auto manualApp=base/L"legacy app";fs::create_directories(manualApp);auto launch=utf8((manualApp/L"shogun2-unit-possession-launcher.exe").wstring());
    current=read(file);replace(file,current,edit(current,'"'+launch+"\" %command% -dx9"));auto migrated=prepare(root,manualApp);
    check(migrated.original==std::optional<std::string>("-dx9"),"Exact v0.31 manual wrapper not migrated");
    std::cout<<"PASS: native VDF preservation, BOM/CRLF/quotes, recent-account selection, nested insert, arguments, repeat repair, uninstall, interrupted repair rollback, foreign wrappers, unknown edits and manual161 migration. No actual Steam setting changed.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
