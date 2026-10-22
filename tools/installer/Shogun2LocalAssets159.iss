; Compile only with a completed, verified payload directory. Never builds DLLs.
#ifndef PayloadDir
  #error Supply /DPayloadDir=<verified packaging stage>
#endif
#ifndef OutputDir
  #error Supply /DOutputDir=<isolated output folder>
#endif
#ifdef Fixture
  #define Identity "Shogun2FirstPerson-LocalAssetsFixture159"
#else
  #define Identity "{{E5F27095-5890-47A2-BA9F-159A233BB196}"
#endif

#ifndef ProductVersion
  #error Supply /DProductVersion
#endif
#ifndef ProductFileVersion
  #error Supply /DProductFileVersion
#endif

[Setup]
AppId={#Identity}
AppName=Shogun 2 Unit Possession
AppVersion={#ProductVersion}
AppVerName=Shogun 2 Unit Possession v{#ProductVersion}
AppPublisher=logsam58
AppPublisherURL=https://github.com/logsam58/shogun2-unit-possession
AppSupportURL=https://github.com/logsam58/shogun2-unit-possession/blob/main/docs/KNOWN_ISSUES.md
DefaultDirName={autopf}\Shogun 2 Unit Possession
DisableDirPage=yes
DisableProgramGroupPage=yes
UsePreviousAppDir=yes
WizardStyle=modern
WizardSizePercent=110
LicenseFile={#PayloadDir}\LICENSE
InfoBeforeFile={#PayloadDir}\BeforeInstall.txt
InfoAfterFile={#PayloadDir}\AfterInstall.txt
OutputDir={#OutputDir}
OutputBaseFilename=Shogun2-Unit-Possession-v{#ProductVersion}-Setup
Compression=lzma2
SolidCompression=yes
SetupLogging=yes
UninstallDisplayName=Shogun 2 Unit Possession
UninstallFilesDir={app}
UninstallLogMode=overwrite
VersionInfoVersion={#ProductFileVersion}
VersionInfoDescription=Shogun 2 Unit Possession Setup
CloseApplications=no
RestartApplications=no
RestartIfNeededByRun=no
AlwaysRestart=no
AllowCancelDuringInstall=no
SetupMutex=Shogun2FirstPersonStandardSetup
MinVersion=10.0
#ifdef Fixture
PrivilegesRequired=lowest
#else
PrivilegesRequired=admin
#endif

[Files]
; Temporary copies allow all checks and snapshots before Inno deploys files.
Source: "{#PayloadDir}\prepare-local-assets159.exe"; Flags: dontcopy
Source: "{#PayloadDir}\game-integration.exe"; Flags: dontcopy
Source: "{#PayloadDir}\dinput8.dll"; Flags: dontcopy
Source: "{#PayloadDir}\marker.pack"; Flags: dontcopy
Source: "{#PayloadDir}\game-integration.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#PayloadDir}\shogun2-unit-possession-launcher.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#PayloadDir}\LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#PayloadDir}\THIRD_PARTY_NOTICES.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#PayloadDir}\MinHook-LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#PayloadDir}\PLAYING.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#PayloadDir}\payload-manifest.json"; DestDir: "{app}"; Flags: ignoreversion
; The custom uninstall action verifies these hashes before removing them.
; uninsneveruninstall prevents the generic uninstaller deleting replaced mods.
Source: "{#PayloadDir}\dinput8.dll"; DestDir: "{code:GameFolder}"; Flags: ignoreversion uninsneveruninstall
Source: "{#PayloadDir}\marker.pack"; DestDir: "{code:GameFolder}\data"; DestName: "shogun2_possession_checkpoint45.pack"; Flags: ignoreversion uninsneveruninstall

; These files do not exist at compile time and are not in the setup download.
Source: "{tmp}\local-assets\hud-assets159.s2fp"; DestDir: "{code:GameFolder}\data\shogun2_possession_local_assets159"; Flags: external ignoreversion uninsneveruninstall
Source: "{tmp}\local-assets\qte-assets159.s2fp"; DestDir: "{code:GameFolder}\data\shogun2_possession_local_assets159"; Flags: external ignoreversion uninsneveruninstall

[UninstallDelete]
Type: files; Name: "{app}\Steam-Launch-Options.txt"

[INI]
Filename: "{app}\installation.ini"; Section: "Installation"; Key: "Game"; String: "{code:GameFolder}"
Filename: "{app}\installation.ini"; Section: "Installation"; Key: "Profile"; String: "{code:ProfileFolder}"
Filename: "{app}\installation.ini"; Section: "Installation"; Key: "Rollback"; String: "rollback-v{#ProductVersion}"

[Code]
var
  GamePage: TInputDirWizardPage;
  LaunchPage: TInputQueryWizardPage;
  Prepared, Completed: Boolean;
  RemovalGame, RemovalProfile: String;

function GameFolder(Param: String): String;
begin
  Result := RemoveBackslashUnlessRoot(GamePage.Values[0]);
end;

function ProfileFolder(Param: String): String;
begin
#ifdef Fixture
  Result := ExpandConstant('{param:PROFILE}');
#else
  Result := ExpandConstant('{userappdata}');
#endif
end;

function Quoted(Value: String): String;
begin
  Result := '"' + Value + '"';
end;

function Integration(Executable, Command, Game, Profile: String; var ErrorText: String): Boolean;
var
  ExitCode: Integer;
  Report, RollbackFolder: String;
  Bytes: AnsiString;
begin
  Report := ExpandConstant('{tmp}\integration-result.txt');
  if IsUninstaller then RollbackFolder := AddBackslash(ExpandConstant('{app}')) + GetIniString('Installation', 'Rollback', 'rollback-v{#ProductVersion}', ExpandConstant('{app}\installation.ini'))
  else RollbackFolder := AddBackslash(WizardDirValue) + 'rollback-v{#ProductVersion}';
  DeleteFile(Report);
  Result := Exec(Executable, Command + ' ' + Quoted(Game) + ' ' + Quoted(Profile) + ' ' +
    Quoted(RollbackFolder) + ' ' + Quoted(Report), '', SW_HIDE, ewWaitUntilTerminated, ExitCode);
  Result := Result and (ExitCode = 0);
  ErrorText := 'The game integration could not complete. No unknown files will be removed. Close Shogun 2 and run setup again.';
  if LoadStringFromFile(Report, Bytes) then ErrorText := String(Bytes);
  Log(Command + ': ' + ErrorText);
end;

procedure InitializeWizard;
var
  Steam, DefaultGame: String;
begin
  GamePage := CreateInputDirPage(wpInfoBefore, 'Choose your Shogun 2 folder',
    'Where is the original game installed?',
    'Choose the folder containing Shogun2.exe and Empire.Retail.dll. Close the game before continuing.', False, '');
  GamePage.Add('Total War: SHOGUN 2 folder:');
  DefaultGame := GetIniString('Installation', 'Game', '', AddBackslash(WizardDirValue) + 'installation.ini');
  if DefaultGame = '' then begin
    if not RegQueryStringValue(HKCU, 'Software\Valve\Steam', 'SteamPath', Steam) then
      Steam := ExpandConstant('{pf32}\Steam');
    DefaultGame := Steam + '\steamapps\common\Total War SHOGUN 2';
  end;
  GamePage.Values[0] := ExpandConstant('{param:GAME|' + DefaultGame + '}');
  LaunchPage := CreateInputQueryPage(GamePage.ID, 'Show the camera chooser before launch',
    'One-time Steam setup',
    'After installation, right-click Shogun 2 in Steam, choose Properties, then paste this line into Launch Options. Keep existing game arguments after %command%. If you already use another launcher, review compatibility first.');
  LaunchPage.Add('Steam Launch Options (select and press Ctrl+C):', False);
  LaunchPage.Values[0] := Quoted(AddBackslash(WizardDirValue) + 'shogun2-unit-possession-launcher.exe') + ' %command%';
  LaunchPage.Edits[0].ReadOnly := True;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var ErrorText: String;
begin
  Result := True;
  if CurPageID = GamePage.ID then begin
    ExtractTemporaryFile('game-integration.exe');
    Result := Integration(ExpandConstant('{tmp}\game-integration.exe'), 'check', GameFolder(''), ProfileFolder(''), ErrorText);
    if not Result then MsgBox(ErrorText, mbError, MB_OK);
  end;
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
var ErrorText: String; ExitCode: Integer;
begin
  Result := '';
  ExtractTemporaryFile('game-integration.exe');
  ExtractTemporaryFile('dinput8.dll');
  ExtractTemporaryFile('marker.pack');
  if not Integration(ExpandConstant('{tmp}\game-integration.exe'), 'check', GameFolder(''), ProfileFolder(''), ErrorText) then begin
    Result := ErrorText; exit;
  end;
  ExtractTemporaryFile('prepare-local-assets159.exe');
  WizardForm.StatusLabel.Caption := 'Preparing artwork and audio from your installed Shogun 2...';
  if not Exec(ExpandConstant('{tmp}\prepare-local-assets159.exe'), Quoted(GameFolder('')) + ' ' +
    Quoted(ExpandConstant('{tmp}\local-assets')), '', SW_HIDE, ewWaitUntilTerminated, ExitCode) or (ExitCode <> 0) then begin
    Result := 'Could not prepare local game resources. Your game has not been changed. Check that the original Steam game files are complete. Windows N requires the Media Feature Pack.'; exit;
  end;
  if not ForceDirectories(ExpandConstant('{app}')) then begin
    Result := 'Cannot create the setup records folder.'; exit;
  end;
  if Integration(ExpandConstant('{tmp}\game-integration.exe'), 'prepare', GameFolder(''), ProfileFolder(''), ErrorText) then
    Prepared := True
  else Result := ErrorText;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var ErrorText: String;
begin
  if CurStep = ssPostInstall then begin
    SaveStringToFile(ExpandConstant('{app}\Steam-Launch-Options.txt'), Utf8Encode(LaunchPage.Values[0] + #13#10), False);
    if not Integration(ExpandConstant('{tmp}\game-integration.exe'), 'finish', GameFolder(''), ProfileFolder(''), ErrorText) then
      RaiseException(ErrorText);
    Completed := True;
  end;
end;

procedure DeinitializeSetup;
var ErrorText: String;
begin
  if Prepared and not Completed then
    if not Integration(ExpandConstant('{tmp}\game-integration.exe'), 'rollback', GameFolder(''), ProfileFolder(''), ErrorText) then
      MsgBox('Setup stopped. Previous files are backed up in the rollback folder, but automatic restoration stopped: ' + ErrorText, mbError, MB_OK);
end;

function InitializeUninstall: Boolean;
var ErrorText: String;
begin
  RemovalGame := GetIniString('Installation', 'Game', '', ExpandConstant('{app}\installation.ini'));
  RemovalProfile := GetIniString('Installation', 'Profile', '', ExpandConstant('{app}\installation.ini'));
  Result := (RemovalGame <> '') and (RemovalProfile <> '');
  if Result then Result := Integration(ExpandConstant('{app}\game-integration.exe'), 'uninstall-check', RemovalGame, RemovalProfile, ErrorText)
  else ErrorText := 'The installation record is missing. Files have been preserved.';
  if not Result then MsgBox(ErrorText, mbError, MB_OK);
  if Result and not UninstallSilent then
    Result := MsgBox('Remove this mod''s camera-chooser line from Shogun 2 > Properties > Launch Options in Steam before continuing. This lets Steam launch the game normally after uninstall.', mbConfirmation, MB_OKCANCEL) = IDOK;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var ErrorText: String;
begin
  if CurUninstallStep = usUninstall then
    if not Integration(ExpandConstant('{app}\game-integration.exe'), 'uninstall', RemovalGame, RemovalProfile, ErrorText) then
      RaiseException(ErrorText);
end;
