#include "integrated.h"
int wmain(){try{
    require(selected_text(L"mod shogun2_possession_checkpoint45.pack;"),"Unquoted selection failed");
    require(selected_text(L"mod another.pack;\r\nmod \"SHOGUN2_POSSESSION_CHECKPOINT45.PACK\";"),"Quoted selection failed");
    for(const wchar_t* text:{L"// mod shogun2_possession_checkpoint45.pack;",L"/* mod shogun2_possession_checkpoint45.pack; */",L"# mod shogun2_possession_checkpoint45.pack;",L"mod shogun2_possession_checkpoint450.pack;",L"mod other.pack;"})require(!selected_text(text),"Wrong activation accepted");
    wchar_t exe[MAX_PATH];GetModuleFileNameW(nullptr,exe,MAX_PATH);std::wstring folder=exe;folder=folder.substr(0,folder.find_last_of(L'\\'));
    runtime_folder=folder+L"\\embedded-fixture-"+std::to_wstring(GetTickCount64());require(CreateDirectoryW(runtime_folder.c_str(),nullptr)!=0,"Fixture directory failed");
    auto adapter=extract(GetModuleHandleW(nullptr),101,L"possession-v46.dll",EMBEDDED_ADAPTER_SHA);
    auto hud=extract(GetModuleHandleW(nullptr),102,L"possession-hud.exe",EMBEDDED_HUD_SHA);
    auto qte=extract(GetModuleHandleW(nullptr),103,L"qte-playable-v79.dll",EMBEDDED_QTE_SHA);
    require(hash_file(qte)==EMBEDDED_QTE_SHA,"Embedded QTE integrity differs");
    require(extract(GetModuleHandleW(nullptr),103,L"qte-playable-v79.dll",EMBEDDED_QTE_SHA)==qte,"Cached QTE resource failed");
    require(hash_file(adapter)==EMBEDDED_ADAPTER_SHA&&hash_file(hud)==EMBEDDED_HUD_SHA,"Resource integrity differs");
    require(extract(GetModuleHandleW(nullptr),101,L"possession-v46.dll",EMBEDDED_ADAPTER_SHA)==adapter,"Cached resource failed");
    {std::ofstream changed(hud,std::ios::binary);changed<<"changed fixture";}
    bool rejected=false;try{extract(GetModuleHandleW(nullptr),102,L"possession-hud.exe",EMBEDDED_HUD_SHA);}catch(...){rejected=true;}require(rejected,"Changed cache accepted");
    require(!battle_ready(0),"Invalid battle accepted");
    std::cout<<"PASS: native exact/comment-aware mod activation; embedded accepted DLL/HUD byte integrity; repeat extraction; modified cache preserved/rejected; invalid battle guarded.\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
