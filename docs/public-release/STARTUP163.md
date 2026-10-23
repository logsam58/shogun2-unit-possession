# Automatic startup installer repair — Test163

Unpublished v0.32 candidate; no real installation or live acceptance is implied by offline tests.

The player closes Shogun 2, installs the mod and selects the folder containing Shogun2.exe and Empire.Retail.dll. There is no script to run, pasted command, Steam-property edit or account picker. Steam Play opens the compiled how-to/perspective chooser; choosing a view starts the original Creative Assembly launcher/game. Closing the guide starts nothing. No popup is introduced into the game process, and no game executable is replaced.

Test162's installer incorrectly required MostRecent in loginusers.vdf. A real one-account file omitted it. The repaired resolver accepts an unambiguous sole account, prioritizes an explicit unique recent account, then a unique newest timestamp or (when timestamps are absent) a unique automatic-login account. Field names are case insensitive. Invalid IDs, duplicate fields, conflicting recent flags and unresolved ties are refused without changing configuration. No account names, credentials or entire private config files are copied into mod records.

When Steam is running, setup invokes its normal -shutdown request as the installer's original user. A native guard validates Valve's cached Authenticode signature and refuses an elevated token before starting the client. The installer waits up to 30 seconds for normal exit, then edits only app34330 LaunchOptions using the existing transaction/ownership system. It reopens only a client it previously asked to close, including after normal failure/rollback. A declined shutdown stops preparation without forced termination or config edits. Steam already closed at setup start stays closed. Run setup normally from the Windows account used to play; explicit elevation that loses the original unelevated user is refused rather than launching Steam as administrator.

Original game arguments, unrelated config bytes and later unrelated edits are retained. Another wrapper or unknown modification remains a compatibility error rather than an overwrite. A version-specific Test163 game rollback directory preserves earlier v0.32 candidate records. The existing Steam ownership record format is retained for repair/upgrade. Uninstall still requires the game and Steam closed and restores the prior owned launch option.

The guide, adapter/QTE/HUD and game-process callout design from [Test162](STARTUP162.md) are retained. The clip is prepared from each owner's game; no game artwork/fonts/audio are distributed. The launcher passes the original Steam command, arguments, environment, working directory and exit behavior through. Actual Steam expansion, CA launcher/DRM/overlay/fullscreen and application recording still require live verification.

Tests use a separately compiled fixture helper restricted to a marked synthetic Steam root and a toy client that only changes fixture markers. Production rejects synthetic-root override arguments. The actual Inno engine exercises install/repair/uninstall, accounts without MostRecent, automatic toy-client close/reopen, resource-preparation failure, post-configuration rollback and a client that refuses normal exit. No test closes real Steam, launches the game or changes a real Steam setting.

The published private release remains v0.31. No new release or visibility change is part of this repair.

References: [Inno original-user execution](https://jrsoftware.org/ishelp/topic_isxfunc_execasoriginaluser.htm), [Microsoft CreateProcessW](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw), [WinVerifyTrust](https://learn.microsoft.com/en-us/windows/win32/api/wintrust/nf-wintrust-winverifytrust).
