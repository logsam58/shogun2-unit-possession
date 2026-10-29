# Shogun 2 Unit Possession — Test168 candidate

This is a startup-compatibility correction under test, not an accepted new
product release. The supported platform remains the original Windows Steam
game build. Restart the game after changing the enabled pack.

Enable the mod and launch normally. Select a friendly unit in a land battle or
siege and press F9. Third person is the default; P switches perspective and F10
returns to RTS. Existing movement, weapons, QTEs, abilities and unit orders are
retained. This pack includes no separate installer or startup chooser.

On first launch the mod prepares its artwork, native fonts and optional advisor
voice from your owned game. English fonts retain their exact existing checks.
When the English pack is absent, compatible native fonts are sought in installed
localization packs. HUD labels remain English. Not every real language pack has
been tested; this is not a promise of universal language support. If compatible
fonts are unavailable, the startup message explains the restriction. The English
"Shameful display!" callout is omitted when its source or decoder is unavailable;
it no longer blocks possession. No game fonts, artwork or audio are redistributed.

Startup attempts to retain the game's original menu callbacks even when native
mod initialization fails. Native startup failures show a single error message.
If the bridge cannot load at all, an exclusive first-error note may be written
to %LOCALAPPDATA%/S2UP-Workshop-startup-error.txt. There is at most one such file,
up to 4 KiB; an existing file is preserved. Normal successful launches create no
session/input/QTE logs. No debugger, recorder or dump collector is started.
Necessary local code and resource caches still use storage.

Controls: WASD moves, Shift runs, RMB readies ranged weapons, LMB fires (hold and
release for bows). Q moves/attacks or drags formation width; E stops, Z holds
here, C follows, R cycles fire orders. Double-tap Q/Z for a running order. 1
toggles native melee/ranged mode; T/G selects/uses an ability or ammunition.
M mounts/dismounts, F starts climbing, W/S climbs up/down. Mouse wheel changes
third-person distance; hold middle mouse to orbit. K hides ordinary HUD while
combat timing prompts remain visible. F12 disables the mod for the session.

Naval possession and multiplayer are unavailable. Charging, climbing, large-army
performance, clipping and crashes retain their known limitations. Enabled database/unit packs retain their normal loading. One other enabled UI
pack replacing the front-end root can be preserved through the launcher's
used_mods list when this bootstrap is the winning entry. Multiple different
root replacements, ambiguous pack paths, another mod winning that entry, movie
packs outside that list, unknown engine builds and overlapping native hooks
remain compatibility limits. Do not bypass an unsupported-engine warning. Offline fixtures are not a
substitute for the reporting player's actual game/language test.

F9 waits for coordinated component startup. A later initialization failure
requests a stop only for components started by this coordinator, and the warning
includes the component's bounded error reason. Temporary menu handoff files
are deleted when their handles close, including a forced process exit.

The author source for Shogun2 Loader 0.5.1 has passed an isolated patch-location
and signature check. This is not a combined in-game acceptance test, a test of
its optional crash handler, or evidence about a different modified engine DLL.
