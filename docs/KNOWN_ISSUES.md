# Known issues and troubleshooting

## Unresolved crashes

Private captures establish three distinct immediate faults: a native effects-table read at Empire.Retail.dll RVA0x14A4011 with a null table; a Miles mss32.dll sample-loop read with an invalid negative remaining count; and a DirectSound audio-thread call through an invalid virtual-method pointer. The latest DirectSound capture has `gfx_blood_effects false` in its recorded preferences. Blood being disabled did not prevent that occurrence; this does not establish a common cause with the effects fault.

The DirectSound caller bytes match the relocated system backing file in both first-chance and unhandled dumps. This establishes the failing call, not the earlier corrupting write or object lifetime. Native module/code identity does not exclude indirect mod involvement. Recorded resource headroom does not exclude every hardware or allocation fault. v0.19 preserves accepted gameplay and includes no speculative crash patch.

When reporting a crash, include the version, unit/map, actions before the exit, blood/audio settings, other mods and exception/module/offset if available. Optional [diagnostics](../tools/diagnostics/README.md) preserve more evidence. Full process dumps and raw input logs should be reviewed privately before sharing.

## Gameplay limitations

- **HUD and friendly-fire callout:** v0.19 retains the accepted Test109 layout and short local-game voice clip. Direct personal projectile contact is implemented; area-only/secondary effects and every weapon/device are not certified. Player feedback is general build acceptance, not individual-case certification.

- **Display/QTE coverage:** v0.19 includes native DirectX presentation and the current-clock QTE correction accepted in recorded 1920Ã—1080 fullscreen DirectX11 play. The earlier stale publication horizon and repeated marker holds were reproduced offline. This acceptance does not certify every graphics device/overlay, physical scanout, hardware latency or the death overlay across all modes. Existing artwork includes 1080p layouts; reducing resolution is not the fix. See [v0.18 evidence](releases/v0.18-validation.json).

- **Self clipping:** Test100 was rejected with visible cuts through a rifle stock and arms, looking up and level. Test101 reduces only the current living first-person camera's near plane, rotates infantry weapon-look around the current eye, and preserves the neck/collar. Native death clipping and exact exit restoration are retained. The user reported a big improvement in Test101. Complete visual coverage across weapons, fighting and traversal remains pending; geometry fixtures do not prove every mesh is clear.
- **Unit controls and recovery:** v0.19 retains the Test104-accepted command, roster and camera foundations, including Q contextual targeting, native formation placement, Z guard, follow catch-up and full get-up movement locking. Earlier Test100/102/103 feedback led to these corrections. Unit path completion, every destructible target, cannon/recovery asset and castle context still need broader coverage; no custom replacement formation loop is used.

- **Mounted bows:** targeting and presentation have separate paths. Rider weapon/camera presentation remains problematic; complete mounted-bow support is not claimed.
- **Ranged roster:** the native catalog includes many personal projectile families, but specialty animation/ammo combinations and every unit's firing lifecycle are not fully verified. Report both the possessed soldier's shot and the rest of the unit's volley behavior.
- **Crowded melee:** valid native deferred pair preparation is supported, but death, lost ownership and native combat changes can still cancel an event. INTERRUPTED explains an admitted event that was canceled; it is not itself a diagnosis.
- **Sieges:** native-speed climbing, crest and landing are implemented. Different wall resources, castles, falling/casualty transitions and simultaneous combat need further live coverage.
- **Accuracy:** effective native accuracy drives a circular cone with fixed calibration. This is not exact stock rectangular scatter or a certified stock hit-probability match. Descending non-firearm dashes are not calibrated ballistic range marks.
- **Input timing:** QTE input matches recent software-submitted geometry and simulation time. Physical scanout, mouse hardware and end-to-end latency are not measured by this implementation.
- **Coverage:** multiplayer, overhaul-mod compatibility, every campaign lifecycle, naval/ship control and crew possession are not certified. Artillery retains stock first-person behavior.

- **Formation recovery / RTS drops:** Test103 play reported rigid casualty/displacement reformation and returns to RTS under pressure or wavering. Test104 confines roster overrides to native FPS camera work and protects a reproduced UI followed-actor overwrite path. The two captured living drops do not establish their writer or morale cause. The user accepted Test104 after recorded play. This does not establish every native path/formation lifecycle or eliminate all possible possession exits. Z still uses native guard, without a replacement reformation loop.
- **Committed animation head-look:** Test104 adds horizontal mouse-look up to 100 degrees either side of the animated head while retaining vertical control and native eye translation/roll. The Test104 playtest received positive acceptance; authored kill/recovery assets and clipping across the full roster still need broader coverage.

## Installation and startup

**Setup rejects the engine:** verify the installed game build and compare Empire.Retail.dll with the supported SHA256 in the README. Do not bypass fingerprint checks; hook addresses are build-specific.

**Setup finds another DINPUT8.dll:** another injector/mod occupies the same filename. Keep its files and use a tested compatibility arrangement before installing. The possession installer deliberately refuses unknown files.

**F9 does not enter possession:** select a living eligible unit in a land battle, ensure the possession mod is selected, and check that the installed build matches the current engine. Routing, death, unsupported actor state or invalid native camera ownership can prevent entry. Include unit/map and state in a report rather than repeatedly reinstalling.

**An old version still appears loaded:** fully exit the game before updating and start a fresh process afterward. On-disk replacement does not replace a DLL already loaded by a running game. Preserve installation receipts and loaded backing hashes during development.

**Reverting/removing:** no standalone uninstall wizard is supplied. F12 disables the running session. Permanent removal or rollback requires a closed game and careful restoration/removal of only the installer-managed loader, marker and launcher selection. Upgrades retain `.possession-previous` snapshots for managed files; these are backups, not a one-click rollback command. Do not delete unrelated mod selections, modified caches or Windows system DInput files. Ask for help with the installation receipt if the managed-file state is unclear.

**Visual/input issue:** report resolution, Windows display scaling, monitor refresh rate, battle speed, pause state, weapon and whether gameplay FPS also dropped. Software render timings alone are not measured game FPS or monitor scanout.
