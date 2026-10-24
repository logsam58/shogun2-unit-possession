# Shogun 2 Unit Possession

v0.32 uses the accepted Test166 Workshop runtime. The owner reported a
successful in-game trial; known limitations are listed below.

Enable the mod and launch normally. The first launch prepares its UI and voice
from your own game in the background. There is no camera chooser or installer.
This build supports the original Windows Steam game and its English resource
pack. Windows N needs the Media Feature Pack.

In a land battle or siege, select a friendly unit and press F9. Zoom in and point
at a friendly soldier to choose that soldier. F10 returns to the RTS camera.
Third-person is the default; press P for first-person whenever you like.

| Key | Action |
| --- | --- |
| WASD / Shift | Move / run |
| RMB / LMB | Ready or aim / attack or fire |
| Hold and release LMB with a bow | Choose shot strength |
| Q | Move or attack at your aim; hold and drag to place the formation |
| E | Stop the unit |
| Z | Hold here, facing your direction |
| C | Follow you |
| R | Cycle hold fire, fire at will and fire on command |
| 1 | Toggle the unit's native melee/ranged mode where available |
| T / G | Select / use a native ability or ammunition option |
| M | Native unit mount/dismount |
| F near a wall / W / S | Start climbing / ascend / descend |
| P / wheel / hold middle mouse | Switch view / third-person distance / orbit |
| K | Hide ordinary UI; combat timing prompts stay visible |

Double-tap Q or Z for a running order; a single tap walks. During melee, follow
the mouse-button prompt and time the moving indicator into the green zone.
While running toward an enemy, W+Shift can start a native charge.

Naval possession does not work. Multiplayer is not implemented. Charging and
climbing can be unreliable; visual and gameplay bugs and some crashes are
expected. Huge battles on Ultra unit size can slow down substantially.

The Workshop build replaces the front-end root script entry. Compatibility
with other mods that replace that same entry has not been tested. Restart the
game after enabling, disabling or updating this native mod.

The mod does not install a debugger, crash recorder or hardware monitor. Normal
play does not write gameplay, input or QTE logs. The local artwork/audio cache
is needed by the HUD and is reused across launches; compatible updates share
those resource bytes on filesystems that support hard links. A tiny live-check
receipt is removed automatically when the game process exits, including after
a crash. Your saves and Steam's own files are managed by the game and Steam.
