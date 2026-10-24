> **Historical record:** This page describes the Test162 Steam-launch integration.
> The current release is v0.32 / Test166 Workshop. Use the
> [developer guide](../README.md), [release identity](../current-release.json),
> and [release procedure](RELEASING.md) for current work. Commands, pending
> tasks and approvals below apply only to the recorded session.

# Automatic launcher and game-process callout — Test162

Development candidate for v0.32; not a published or live-accepted release.

Setup now connects the compiled quick-start/camera chooser to Steam automatically. Exit Shogun 2 and Steam before installing or uninstalling. The native setup action selects the most recent account in Steam's loginusers.vdf, backs up its existing launch setting, and edits only app 34330's LaunchOptions value. Existing ordinary game arguments are retained. No script, copied command or game executable replacement is required. Another wrapper or an ambiguous/edited configuration is preserved and stops setup.

The helper supports both the pinned original game executable and the native Creative Assembly launcher under the supported game's launcher directory. The latter must have a valid cached Windows Authenticode chain and Creative Assembly signer. This corrects v0.31's direct-game-only guard: local Steam app metadata actually selects launcher/launcher.exe on Windows. Its original command, arguments, working directory and Steam environment are retained. The guide still appears before the child starts; cancellation starts nothing. Real Steam/CA launcher/DRM/overlay behavior requires a separate live test.

The Steam editor preserves unrelated VDF bytes. It uses bounded parsing, duplicate-key rejection, UTF-8 Windows paths and native file APIs. Persistent ownership records retain the original launch option only; unrelated Steam account data is not copied into the mod directory. Per-setup transaction records support a failed installation's rollback without disabling an existing installation. Uninstall restores the original value while retaining other configuration changes. Unknown edits stop removal before the helper can be deleted. The exact v0.31 wrapper at the same app path is migrated without retaining a dangling helper on uninstall.

Only the most recent Steam account is configured. Sign into the intended account once, then exit Steam before setup. Records belonging to another account or installation are refused. The production helper discovers Steam through the current Windows account's registry; it accepts no synthetic-root override. The separately compiled fixture helper requires a marked synthetic directory and cannot operate on an ordinary Steam root. Setup never terminates Steam or changes CPU, graphics, saves, recorder or debugger settings.

The friendly-fire clip is still prepared from each owner's game. Playback moves from possession-hud.exe into the existing game-process loader worker. The worker reads the established possession/event mappings, checks current identity, focus and heartbeat, and opens a bounded Windows waveform stream. It never calls Miles or adds a game hook. Only the cropped 134 KiB voice is retained in the game, not the full artwork/font bundle; that buffer stays alive until the stream closes. Overlapping events are dropped rather than queued; stale, foreign and inactive events are consumed without playback. HUD visibility does not control audio anymore. The separate HUD no longer plays the clip, preventing duplicates.

Offline tests include exact VDF edits/recovery, the real guide plus toy child, read-only signature checks against the installed original game/CA launcher, PCM/event guards and Windows process-only audio capture of a quiet synthetic tone through the production playback class. The capture fixture records its own process only, never the game or other applications, and needs Windows process-loopback support (Windows 11 or Windows build 20348+). That test is separate from live game/recording acceptance.

References: [Microsoft process-only capture](https://learn.microsoft.com/en-us/samples/microsoft/windows-classic-samples/applicationloopbackaudio-sample/), [waveOutOpen](https://learn.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-waveoutopen), [CreateProcessW](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw) and [WinVerifyTrust](https://learn.microsoft.com/en-us/windows/win32/api/wintrust/nf-wintrust-winverifytrust). No third-party wrapper code is incorporated.
