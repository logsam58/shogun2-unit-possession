#include "install.h"
static HWND main_window,heading,body,path_box,browse_button,back_button,next_button,cancel_button,status_label;
static HFONT font,title_font;static int page;static bool preview;static std::wstring game_path;
static std::wstring detect_game(){std::vector<std::wstring> roots={registry(HKEY_CURRENT_USER,L"Software\\Valve\\Steam",L"SteamPath"),registry(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Valve\\Steam",L"InstallPath"),known_folder(CSIDL_PROGRAM_FILES)+L"\\Steam"};auto original=roots;
    for(auto& root:original)if(!root.empty()){auto libraries=read_text(root+L"\\steamapps\\libraryfolders.vdf");std::wregex re(L"\"path\"\\s*\"([^\"]+)\"");for(auto i=std::wsregex_iterator(libraries.begin(),libraries.end(),re);i!=std::wsregex_iterator();++i)roots.push_back(std::regex_replace((*i)[1].str(),std::wregex(L"\\\\\\\\"),L"\\"));}
    for(auto& root:roots){auto candidate=root+L"\\steamapps\\common\\Total War SHOGUN 2";if(GetFileAttributesW((candidate+L"\\Empire.Retail.dll").c_str())!=INVALID_FILE_ATTRIBUTES)return candidate;}return L"";}
static HWND control(const wchar_t* cls,const wchar_t* text,DWORD style,int x,int y,int w,int h,int id=0){auto window=CreateWindowExW(cls==std::wstring(L"EDIT")?WS_EX_CLIENTEDGE:0,cls,text,WS_CHILD|WS_VISIBLE|style,x,y,w,h,main_window,reinterpret_cast<HMENU>(id),GetModuleHandleW(nullptr),nullptr);SendMessageW(window,WM_SETFONT,reinterpret_cast<WPARAM>(font),TRUE);return window;}
static void show_page(){ShowWindow(path_box,page==1?SW_SHOW:SW_HIDE);ShowWindow(browse_button,page==1?SW_SHOW:SW_HIDE);EnableWindow(back_button,page>0&&page<3);EnableWindow(next_button,TRUE);ShowWindow(cancel_button,page==4?SW_HIDE:SW_SHOW);SetWindowTextW(status_label,L"");
    const wchar_t* titles[]={EMBEDDED_TITLE,L"Choose your game folder",L"Ready to install",L"Installing",L"Setup complete"};SetWindowTextW(heading,titles[page]);SetWindowTextW(next_button,page==2?L"Install":page==4?L"Finish":L"Next >");
    if(page==0)SetWindowTextW(body,L"Control a soldier inside a Shogun 2 battle.\r\n\r\nVersion 0.18 includes fullscreen HUD support, smoother melee QTE timing, personal combat, native unit commands, formation placement, follow pace, animation head-look and siege-wall climbing. The HUD starts automatically with the mod.\r\n\r\nOnce installed, launch Shogun 2 normally through Steam.\r\n\r\nClose the game and its mod launcher before installing.");
    if(page==1){SetWindowTextW(body,L"Setup found this Shogun 2 installation. You can change the folder or browse to Shogun2.exe.\r\n\r\nGame folder:");SetWindowTextW(path_box,game_path.c_str());}
    if(page==2){std::wstring text=L"Install into:\r\n"+game_path+L"\r\n\r\nSetup will install soldier control and the HUD, then select the possession mod in the launcher.\r\n\r\nSetup replaces the previous version, retains one rollback backup, and removes obsolete recognized runtime caches. Other mods and their selections are preserved.";SetWindowTextW(body,text.c_str());}
    if(page==3)SetWindowTextW(body,L"Installing and activating possession...");
    if(page==4)SetWindowTextW(body,preview?L"Preview finished. No game files were changed.":L"Possession is installed and enabled.\r\n\r\nLaunch Shogun 2 through Steam and enter a single-player land battle. Select infantry and press F9.\r\n\r\nWASD: move    Shift: run    Mouse: look\r\nRanged: hold RMB, then LMB to fire\r\nMelee: respond to LMB / RMB prompts\r\nQ: advance / drag    E: stop\r\nZ: hold here    C: follow me    R: fire mode\r\nF: climb wall    W / S: ascend / descend\r\nF10 or Escape: RTS mode\r\n\r\nThe HUD starts automatically with the mod.");
    if(page==4&&!preview&&retention_report.deferred)SetWindowTextW(status_label,L"Some old files were preserved. Setup will retry cleanup next time.");
}
static void error(const std::exception& e){std::string t=e.what();std::wstring message(t.begin(),t.end());MessageBoxW(main_window,message.c_str(),L"Shogun 2 Possession setup",MB_OK|MB_ICONERROR);}
static LRESULT CALLBACK procedure(HWND window,UINT message,WPARAM wp,LPARAM lp){try{
    if(message==WM_COMMAND){int id=LOWORD(wp);if(id==103||id==IDCANCEL){DestroyWindow(window);return 0;}
        if(id==101){if(page>0&&page<3)--page;show_page();return 0;}
        if(id==104){wchar_t file[32768]{};OPENFILENAMEW dlg{};dlg.lStructSize=sizeof dlg;dlg.hwndOwner=window;dlg.lpstrFilter=L"Shogun 2 executable\0Shogun2.exe;Shogun2.retail.exe\0\0";dlg.lpstrFile=file;dlg.nMaxFile=32768;dlg.Flags=OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR;if(GetOpenFileNameW(&dlg)){game_path=file;game_path=game_path.substr(0,game_path.find_last_of(L"\\/"));SetWindowTextW(path_box,game_path.c_str());}return 0;}
        if(id==102||id==IDOK){if(page==4){DestroyWindow(window);return 0;}if(page==1){std::vector<wchar_t> text(GetWindowTextLengthW(path_box)+1);GetWindowTextW(path_box,text.data(),int(text.size()));game_path=text.data();while(game_path.size()>3&&(game_path.back()==L'\\'||game_path.back()==L'/'))game_path.pop_back();validate_game(game_path);}
            if(page==2){require(preview||!running(),"Close Shogun 2 before installing, then click Install again.");page=3;show_page();EnableWindow(next_button,FALSE);EnableWindow(cancel_button,FALSE);UpdateWindow(window);
                try{if(!preview)install_all(game_path,known_folder(CSIDL_APPDATA),[](const wchar_t* text){SetWindowTextW(status_label,text);UpdateWindow(main_window);});page=4;}
                catch(...){page=2;EnableWindow(cancel_button,TRUE);show_page();throw;}EnableWindow(cancel_button,TRUE);show_page();return 0;}
            ++page;show_page();return 0;}}
    if(message==WM_CLOSE){if(page!=3)DestroyWindow(window);return 0;}if(message==WM_DESTROY){PostQuitMessage(0);return 0;}
}catch(const std::exception& e){error(e);}return DefWindowProcW(window,message,wp,lp);}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR command,int){try{
    preview=std::wstring(command)==L"--preview";
#ifdef PREVIEW_WIZARD
    preview=true;
#endif
require(std::wstring(command).empty()||preview,"Run setup normally to open the installation wizard.");SetProcessDPIAware();
    font=CreateFontW(-16,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,L"Segoe UI");title_font=CreateFontW(-25,0,0,0,FW_SEMIBOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,L"Segoe UI");
    WNDCLASSW cls{};cls.lpfnWndProc=procedure;cls.hInstance=instance;cls.lpszClassName=L"Shogun2PossessionSetupWizard";cls.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));cls.hbrBackground=reinterpret_cast<HBRUSH>(COLOR_WINDOW+1);require(RegisterClassW(&cls)!=0,"Cannot create setup window");
    RECT rect{0,0,670,450};AdjustWindowRect(&rect,WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,FALSE);main_window=CreateWindowW(cls.lpszClassName,(std::wstring(EMBEDDED_TITLE)+(preview?L" Wizard Preview":L" Setup")).c_str(),WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,(GetSystemMetrics(SM_CXSCREEN)-(rect.right-rect.left))/2,(GetSystemMetrics(SM_CYSCREEN)-(rect.bottom-rect.top))/2,rect.right-rect.left,rect.bottom-rect.top,nullptr,nullptr,instance,nullptr);require(main_window!=nullptr,"Cannot open setup window");
    heading=control(L"STATIC",L"",0,30,25,610,40);SendMessageW(heading,WM_SETFONT,reinterpret_cast<WPARAM>(title_font),TRUE);
    body=control(L"STATIC",L"",0,30,85,610,270);path_box=control(L"EDIT",L"",ES_AUTOHSCROLL|WS_TABSTOP,30,180,500,32);browse_button=control(L"BUTTON",L"Browse...",WS_TABSTOP,545,180,95,32,104);
    status_label=control(L"STATIC",L"",0,30,360,610,30);back_button=control(L"BUTTON",L"< Back",WS_TABSTOP,350,405,90,30,101);next_button=control(L"BUTTON",L"Next >",BS_DEFPUSHBUTTON|WS_TABSTOP,450,405,90,30,102);cancel_button=control(L"BUTTON",L"Cancel",WS_TABSTOP,550,405,90,30,103);
    game_path=detect_game();show_page();ShowWindow(main_window,SW_SHOW);UpdateWindow(main_window);MSG msg{};while(GetMessageW(&msg,nullptr,0,0)>0){if(!IsDialogMessageW(main_window,&msg)){TranslateMessage(&msg);DispatchMessageW(&msg);}}DeleteObject(font);DeleteObject(title_font);return 0;
}catch(const std::exception& e){error(e);return 1;}}



