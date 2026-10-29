# Shogun 2 Unit Possession — Test170 compatibility candidate

This is a local test build. It retains Test169's gameplay and custom-unit HUD,
local font fallback, optional English advisor voice and startup error recovery.
No recorder, debugger, camera chooser or installer is started by this pack.
Required game artwork, fonts and voice are prepared from the player's own game.

The original Windows Steam engine is supported, plus the exact reviewed engine
from ModDB's Total War SHOGUN 2 Bugfix Mod archive dated 2 October 2026:

    c4d91d74ce9ff8338cf61d5b021b03a2776830177a590fcac205509a42239545

Other engine edits remain unsupported. Native hook checks still refuse occupied
or changed locations. This does not establish compatibility with every version
of the bugfix or another loader, Linux/Proton, or a different game edition.
The replacement engine itself is not included in this pack.

Enabled PFH2/PFH3 mods can use the normal per-entry timestamp layout. Bounds,
path and duplicate checks remain. Unknown/compressed/encrypted layouts are
refused; a startup error identifies the enabled pack that could not be read.
One other enabled front-end root can be retained when this bootstrap wins.
Different competing UI roots and mixed possession builds remain incompatible.

For the local trial, enable only the Test170 possession entry; leave older
possession/checkpoint entries off. Other ordinary mods may remain enabled.
Restart the game after changing engine DLLs or enabled possession packs.

Select a friendly unit in a land battle and press F9. P changes first/third
person; F10 returns to RTS. WASD moves, Shift runs. RMB readies ranged weapons;
LMB fires (hold and release for bows). Q moves/attacks or drags the formation,
E stops, Z holds here, C follows and R cycles fire orders. Double-tap Q/Z to run.
1 toggles melee/ranged mode. T/G selects/uses abilities or ammunition. M mounts
or dismounts, F starts climbing and W/S climbs. Scroll changes third-person
distance; hold middle mouse to orbit. K hides ordinary HUD but keeps combat
timing prompts. F12 disables possession for the session.

Naval possession and multiplayer are unavailable. Charging/climbing, clipping,
large-army slowdown and crashes retain their known limitations. Offline checks
are not joint gameplay acceptance; report the exact startup message if F9 fails.
