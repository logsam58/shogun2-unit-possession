# Unit Possession — Test172 candidate

This build improves startup compatibility with modified original-game engine
files. It checks a shared set of native dependencies instead of requiring a new
checksum entry for every file. A genuine overlap or changed engine layout can
still prevent startup; the error identifies the affected region or hook.

It also fixes an independent stock-only check in fresh local-resource creation.
Artwork, fonts, ability icons and the optional voice clip continue to come from
the player's own game. Gameplay controls and mechanics are unchanged.

Use only one Unit Possession entry. The publisher filename remains
shogun2_unit_possession_workshop165.pack for the existing Workshop item.
This candidate has not been uploaded or accepted in live gameplay yet.

The original Windows x86 Steam game is the target. This is not support for the
64-bit Complete Edition, arbitrary replacement engines, naval possession or
multiplayer. Existing charge/climb, visual and crash limitations still apply.

No recorder, debugger, growing gameplay logs or automatic crash dumps are
included. Ordinary local resource/code caches are still required.
