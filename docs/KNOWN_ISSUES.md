# Known issues and troubleshooting

## Extended-play crash

A crash remains unresolved after extended siege play. Full dumps show C0000005 reading address 0x2C at Empire.Retail.dll RVA 0x14A4011 in native effects processing. An effects object has a null table and zero count while an active record still references an element. The immediate dereference is established; the originating state change, lifetime error and possible mod involvement are not.

v0.17 retains the accepted gameplay without a speculative fix for this fault. Matching native code bytes or a native module in a crash report does not exclude a mod-induced earlier state change. Resource samples around the captured case did not show exhaustion, but they do not rule out every allocation or hardware problem. Reports from other crashes may have a different cause.

When reporting a crash, include the version, unit/map, actions before the exit, other mods and exception/module/offset if available. Optional [diagnostics](../tools/diagnostics/README.md) can preserve more evidence. Full process dumps and raw input logs should be reviewed privately before sharing.

## Gameplay limitations

- **Display changes and current firing/QTE investigation:** after selecting maximum graphics and 1080p fullscreen, a player reported missing HUD/QTE/death overlays; returning to 720p windowed restored HUD visibility, while personal firing remained broken and QTE motion felt less smooth. The 1080p layouts already exist. Test105 is an unpublished candidate addressing a reproduced shared infantry animation-reset conflict, accumulated QTE prediction drift and window/display recovery. An exclusive-fullscreen presentation fix and live symptom attribution remain unproven. See the [audit and limits](development/Test105.md); published v0.17 assets have not been replaced.

- **Self clipping:** Test100 was rejected with visible cuts through a rifle stock and arms, looking up and level. Test101 reduces only the current living first-person camera's near plane, rotates infantry weapon-look around the current eye, and preserves the neck/collar. Native death clipping and exact exit restoration are retained. The user reported a big improvement in Test101. Complete visual coverage across weapons, fighting and traversal remains pending; geometry fixtures do not prove every mesh is clear.
- **Development unit controls:** Test100 Q and Follow Me were live rejected. Test101 keeps Q at the crosshair while mouse-look remains active and prevents stationary mouse-facing from restarting Follow Me. v0.16 adds contextual move/attack and native double-tap run, Z walk/run and pace-matched Follow Me with catch-up. Test102 live play reported failed Q attacks and repeated possession drops. Test103 adds stock target refresh/own-unit ray filtering, stronger input isolation and detailed release evidence. Native attack execution, remaining ownership transitions, unit path completion and castle contexts still require live checks; offline fixtures are not gameplay acceptance.
- **Knockdown recovery:** Test103 blocks manual movement in prone postures and through the active native get-up animation. Cannon knockback, collision and native animation still run. Offline native-clock fixtures are covered; recorded gameplay with actual cannon impacts and different recovery assets is pending.
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
