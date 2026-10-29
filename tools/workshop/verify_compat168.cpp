// Isolated synthetic packs/status mappings; never loads gameplay or calls the engine.
#include "../../native/work/autoload99-preview/integrated.h"
#include "ui_compat168.h"
#include "../../native/work/autoload99-preview/workshop_assets165.h"
#include <cstdio>
static void pack168(const std::filesystem::path& p,const char* name,const std::vector<unsigned char>& body,unsigned flags=3,const std::string& metadata="",bool pfh2=false,const std::string& dependencies=""){
    auto append=[&](std::vector<unsigned char>& index,unsigned size){auto q=(unsigned char*)&size;index.insert(index.end(),q,q+4);
        if(flags&0x40){uint64_t stamp=1;auto t=(unsigned char*)&stamp;index.insert(index.end(),t,t+8);}};
    std::string path=name;std::vector<unsigned char> index;append(index,unsigned(body.size()));
    index.insert(index.end(),path.begin(),path.end());index.push_back(0);
    if(!metadata.empty()){
        append(index,unsigned(metadata.size()));
        std::string key="possession\\workshop165.json";index.insert(index.end(),key.begin(),key.end());index.push_back(0);
    }
    unsigned header[8]={pfh2?0x32484650u:0x33484650u,flags,unsigned(std::count(dependencies.begin(),dependencies.end(),0)),unsigned(dependencies.size()),metadata.empty()?1u:2u,unsigned(index.size()),1,0};
    std::ofstream f(p,std::ios::binary);f.write((char*)header,sizeof header);f.write(dependencies.data(),dependencies.size());f.write((char*)index.data(),index.size());f.write((char*)body.data(),body.size());
    f.write(metadata.data(),metadata.size());
    require(bool(f),"Cannot write owned synthetic pack");
}
template<class F>static void refuses168(F f,const char* why){bool refused=false;try{f();}catch(...){refused=true;}require(refused,why);}
int wmain(int argc,wchar_t** argv){try{
    require(argc==3,"Use owned-game and new fixture directory");std::filesystem::path game=argv[1],out=argv[2];
    require(!std::filesystem::exists(out)&&std::filesystem::create_directories(out),"Fixture output exists");
    auto original=workshop165::original_root(game/L"data/data.pack");
    require(workshop168::selected_root(game,L"")==original,"No-mod startup changed stock UI");
    auto mods=out/L"mods with spaces";std::filesystem::create_directory(mods);
    std::vector<unsigned char> custom={'r','e','t','u','r','n',' ','1'};
    pack168(mods/L"ui-a.pack","ui\\frontend ui\\layout_scripts\\root.luac",custom);
    pack168(mods/L"ui-b.pack","ui\\frontend ui\\layout_scripts\\root.luac",{'r','e','t','u','r','n',' ','2'});
    pack168(mods/L"units.pack","db\\unit_stats_land_tables\\mod",{'u','n','i','t','s'});
    auto prefix=L"/* disabled mod \"ui-b.pack\"; */\nadd_working_directory \""+mods.wstring()+L"\";\n";
    require(workshop168::selected_root(game,prefix+L"mod \"units.pack\";")==original,"Unit mod replaced stock UI");
    for(bool older:{false,true})for(unsigned flags:{0u,1u,2u,3u,4u,0x40u,0x41u,0x42u,0x43u,0x44u}){
        auto tag=std::string(older?"pfh2-":"pfh3-")+std::to_string(flags)+".pack";
        auto path=mods/tag;
        pack168(path,"db\\unit_stats_land_tables\\fixture",{'u'},flags,"",older);
        require(workshop165::ui_root168(path,false).empty(),"Valid legacy/timestamp database pack refused");
        pack168(path,"ui\\frontend ui\\layout_scripts\\root.luac",custom,flags,"",older,std::string("base.pack\0",10));
        require(workshop165::ui_root168(path,false)==custom,"Legacy/timestamp UI bytes changed");
        require(workshop168::selected_root(game,prefix+L"mod \""+std::filesystem::path(tag).wstring()+L"\";")==custom,"Legacy UI selection failed");
    }
    pack168(mods/L"bad-flags.pack","db\\fixture",{'x'},0x83);
    bool named=false;try{workshop168::selected_root(game,prefix+L"mod \"bad-flags.pack\";");}
    catch(const std::exception& error){named=std::string(error.what()).find("bad-flags.pack")!=std::string::npos;}
    require(named,"Unsupported layout did not identify its selected filename");
    pack168(mods/L"bad-dependency.pack","db\\fixture",{'x'});
    {std::fstream f(mods/L"bad-dependency.pack",std::ios::binary|std::ios::in|std::ios::out);unsigned count=1;f.seekp(8);f.write((char*)&count,4);}
    refuses168([&](){workshop165::ui_root168(mods/L"bad-dependency.pack",false);},"Invalid dependency index accepted");
    pack168(mods/L"truncated-timestamp.pack","db\\fixture",{'x'},0x43,"",true);
    std::filesystem::resize_file(mods/L"truncated-timestamp.pack",36);
    refuses168([&](){workshop165::ui_root168(mods/L"truncated-timestamp.pack",false);},"Truncated timestamp layout accepted");
    pack168(mods/L"unsafe-legacy.pack","../outside",{'x'},0x43,"",true);
    refuses168([&](){workshop165::ui_root168(mods/L"unsafe-legacy.pack",false);},"Legacy timestamp path traversal accepted");
    pack168(mods/L"duplicate-legacy.pack","possession\\workshop165.json",{'x'},0x43,"{}",true);
    refuses168([&](){workshop165::ui_root168(mods/L"duplicate-legacy.pack",false);},"Duplicate legacy timestamp names accepted");
    require(workshop168::selected_root(game,prefix+L"mod \"ui-a.pack\"; mod \"units.pack\";")==custom,"Enabled UI mod was discarded");
    require(workshop168::selected_root(game,prefix+L"mod \"ui-a.pack\"; mod \"ui-a.pack\";")==custom,"Repeated pack directive changed UI");
    refuses168([&](){workshop168::selected_root(game,prefix+L"mod \"ui-a.pack\"; mod \"ui-b.pack\";");},"Ambiguous UI roots were merged silently");
    pack168(mods/L"ui-same.pack","ui\\frontend ui\\layout_scripts\\root.luac",custom,4);
    require(workshop168::selected_root(game,prefix+L"mod \"ui-a.pack\"; mod \"ui-same.pack\";")==custom,"Identical UI roots rejected");
    refuses168([&](){workshop168::selection(L"mod \"unterminated.pack;");},"Unterminated directive accepted");
    refuses168([&](){workshop168::selected_root(game,prefix+L"mod \"../ui-a.pack\";");},"Traversal mod name accepted");
    require(workshop168::selection(L"// mod \"off.pack\";\nmod \"on.pack\"; # ignored\n").size()==1,"Comment was treated as an enabled mod");
    require(workshop168::selection(L"add_working_directory \"C://example/path\";")[0].value==L"C://example/path","Quoted path was stripped as a comment");
    auto second=out/L"second mod directory";std::filesystem::create_directory(second);
    auto two=prefix+L"add_working_directory \""+second.wstring()+L"\";";
    pack168(second/L"units.pack","db\\unit_stats_land_tables\\other",{'d','i','f','f','e','r','e','n','t'});
    require(workshop168::selected_root(game,two+L"mod \"units.pack\";")==original,"Duplicate database-only names disabled menu startup");
    std::string sha169(64,'a');std::string marker="local path='/S2UP-Workshop165-"+sha169+".dll'";
    std::vector<unsigned char> self(marker.begin(),marker.end());self.resize(270000,' ');
    auto meta="{\"first_party_bootstrap\":true,\"native_sha256\":\""+sha169+"\"}";
    pack168(mods/L"renamed-possession.pack","ui\\frontend ui\\layout_scripts\\root.luac",self,3,meta);
    pack168(second/L"renamed-possession.pack","ui\\frontend ui\\layout_scripts\\root.luac",self,3,meta);
    require(workshop168::selected_root(game,two+L"mod \"renamed-possession.pack\";",sha169)==original,"Renamed/duplicate current bootstrap delegated back into itself");
    pack168(mods/L"legacy-possession.pack","ui\\frontend ui\\layout_scripts\\root.luac",self,3,"{\"first_party_bootstrap\":true}");
    refuses168([&](){workshop168::selected_root(game,prefix+L"mod \"legacy-possession.pack\";",sha169);},"Legacy/mixed possession copy accepted as a menu addon");
    pack168(mods/L"incomplete-copy.pack","ui\\frontend ui\\layout_scripts\\root.luac",self);
    refuses168([&](){workshop168::selected_root(game,prefix+L"mod \"incomplete-copy.pack\";",sha169);},"Self bootstrap without metadata delegated recursively");
    pack168(second/L"ui-a.pack","db\\unit_stats_land_tables\\not_ui",{'u','n','i','t'});
    refuses168([&](){workshop168::selected_root(game,two+L"mod \"ui-a.pack\";");},"Ambiguous UI/non-UI duplicate filename accepted");
    pack168(second/L"ui-a.pack","ui\\frontend ui\\layout_scripts\\root.luac",custom);
    require(workshop168::selected_root(game,two+L"mod \"ui-a.pack\";")==custom,"Identical menu roots at duplicate paths rejected");
    pack168(second/L"ui-a.pack","ui\\frontend ui\\layout_scripts\\root.luac",{'r','e','t','u','r','n',' ','3'});
    refuses168([&](){workshop168::selected_root(game,two+L"mod \"ui-a.pack\";");},"Different UI roots at duplicate paths accepted");
    for(auto encoding:{0,1,2}){
        auto p=out/(L"selection-"+std::to_wstring(encoding)+L".txt");
        auto value=prefix+L"mod \"ui-a.pack\";";std::ofstream f(p,std::ios::binary);
        if(encoding==0){f.write("\xef\xbb\xbf",3);auto s=std::filesystem::path(value).u8string();f.write(s.data(),s.size());}
        else{if(encoding==1)f.write("\xff\xfe",2);f.write((char*)value.data(),value.size()*2);}f.close();
        require(read_text(p.wstring())==value,"Selection file encoding changed directives");
    }
    auto oversized=out/L"oversized selection.txt";
    {std::ofstream f(oversized,std::ios::binary);std::string large(1048577,'x');f.write(large.data(),large.size());}
    refuses168([&](){read_text(oversized.wstring());},"Oversized selection was read without its bound");
    auto truncated=out/L"truncated UTF16.txt";
    {std::ofstream f(truncated,std::ios::binary);f.write("\xff\xfem",3);}
    refuses168([&](){read_text(truncated.wstring());},"Odd-byte UTF16 selection was silently truncated");
    auto transient=out/L"temporary root.luac";
    for(unsigned i=0;i<100;++i){
        HANDLE guard=workshop165::transient_root168(transient,custom);
        FILE* file=_wfopen(transient.c_str(),L"rbD");require(file!=nullptr,"CRT cannot share temporary UI root");
        char bytes[20]{};auto n=fread(bytes,1,sizeof bytes,file);fclose(file);
        require(n==custom.size()&&!memcmp(bytes,custom.data(),n),"Temporary UI bytes changed");
        CloseHandle(guard);require(!std::filesystem::exists(transient),"Temporary UI root accumulated");
    }
    {std::ofstream foreign(transient);foreign<<"foreign";}
    refuses168([&](){workshop165::transient_root168(transient,custom);},"Foreign temporary root overwritten");
    require(std::filesystem::file_size(transient)==7,"Foreign temporary root removed");
    // The coordinator stops only owned, correctly identified component mappings.
    wchar_t name[80]{};mapping_name(name,GetCurrentProcessId());
    Handle map(CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Status),name));
    require(map.h&&GetLastError()!=ERROR_ALREADY_EXISTS,"Fixture status name is occupied");
    auto state=(Status*)MapViewOfFile(map.h,FILE_MAP_ALL_ACCESS,0,0,sizeof(Status));require(state!=nullptr,"Fixture mapping failed");
    memset(state,0,sizeof *state);state->magic=POC_MAGIC;state->version=46;state->pid=GetCurrentProcessId();state->size=sizeof *state;
    stop_component168(false,false);require(!state->stop,"Foreign/unowned component stopped");
    state->magic=0;stop_component168(true,false);require(!state->stop,"Invalid component mapping stopped");
    state->magic=POC_MAGIC;state->pid=0;stop_component168(true,false);require(!state->stop,"Wrong process mapping stopped");
    state->pid=GetCurrentProcessId();state->size=20;stop_component168(true,false);require(!state->stop,"Wrong layout mapping stopped");
    state->size=sizeof *state;
    state->magic=POC_MAGIC;stop_component168(true,false);require(state->stop==1,"Owned partial startup not stopped");
    UnmapViewOfFile(state);
    swprintf_s(name,L"Local\\Shogun2QteObserver79-%lu",GetCurrentProcessId());
    Handle qmap(CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,32,name));
    require(qmap.h&&GetLastError()!=ERROR_ALREADY_EXISTS,"Fixture QTE mapping name is occupied");
    auto q=(unsigned*)MapViewOfFile(qmap.h,FILE_MAP_ALL_ACCESS,0,0,32);require(q!=nullptr,"Fixture QTE mapping failed");
    memset(q,0,32);q[0]=0x31545153;q[1]=79;q[2]=GetCurrentProcessId();q[3]=32;
    stop_component168(false,true);require(!q[4],"Unowned QTE stopped");
    q[3]=4096;stop_component168(true,true);require(!q[4],"Invalid QTE layout stopped");
    q[3]=32;stop_component168(true,true);require(q[4]==1,"Owned partial QTE startup not stopped");
    UnmapViewOfFile(q);
    std::cout<<"PASS native mod selection/stock+unit+UI packs, exact UTF8/UTF16, ambiguity/disabled/traversal rules,100 temporary UI lifetimes/foreign preservation, owned-only startup cleanup.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
