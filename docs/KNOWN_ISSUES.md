# Known issues and troubleshooting

## Extended-play crash

A crash remains unresolved after extended siege play. Full dumps show C0000005 reading address 0x2C at Empire.Retail.dll RVA 0x14A4011 in native effects processing. An effects object has a null table and zero count while an active record still references an element. The immediate dereference is established; the originating state change, lifetime error and possible mod involvement are not.

v0.15 and the v0.16 development build contain no speculative fix for this fault. Matching native code bytes or a native module in a crash report does not exclude a mod-induced earlier state change. Resource samples around the captured case did not show exhaustion, but they do not rule out every allocation or hardware problem. Reports from other crashes may have a different cause.

When reporting a crash, include the version, unit/map, actions before the exit, other mods and exception/module/offset if available. Optional [diagnostics](../tools/diagnostics/README.md) can preserve more evidence. Full process dumps and raw input logs should be reviewed privately before sharing.

## Gameplay limitations

- **Self clipping:** Test100 was rejected with visible cuts through a rifle stock and arms, looking up and level. Test101 reduces only the current living first-person camera's near plane, rotates infantry weapon-look around the current eye, and preserves the neck/collar. Native death clipping and exact exit restoration are retained. The user reported a big improvement in Test101. Complete visual coverage across weapons, fighting and traversal remains pending; geometry fixtures do not prove every mesh is clear.
- **Development unit controls:** Test100 Q and Follow Me were live rejected. Test101 keeps Q at the crosshair while mouse-look remains active and prevents stationary mouse-facing from restarting Follow Me. v0.16 adds contextual move/attack and native double-tap run, Z walk/run and pace-matched Follow Me with catch-up. Native game-window target selection/attack, unit path completion and castle contexts still require live checks; offline fixtures are not gameplay acceptance.
- **Mounted bows:** targeting and presentation have separate paths. Rider weapon/camera presentation remains problematic; complete mounted-bow support is not claimed.
- **Ranged roster:** the native catalog includes many personal projectile families, but specialty animation/ammo combinations and every unit's firing lifecycle are not fully verified. Report both the possessed soldier's shot and the rest of the unit's volley behavior.
- **Crowded melee:** valid native deferred pair preparation is supported, but death, lost ownership and native combat changes can still cancel an event. INTERRUPTED explains an admitted event that was canceled; it is not itself a diagnosis.
- **Sieges:** native-speed climbing, crest and landing are implemented. Different wall resources, castles, falling/casualty transitions and simultaneous combat need further live coverage.
- **Accuracy:** effective native accuracy drives a circular cone with fixed calibration. This is not exact stock rectangular scatter or a certified stock hit-probability match. Descending non-firearm dashes are not calibrated ballistic range marks.
- **Input timing:** QTE input matches recent software-submitted geometry and simulation time. Physical scanout, mouse hardware and end-to-end latency are not measured by this implementation.
- **Coverage:** multiplayer, overhaul-mod compatibility, every campaign lifecycle, naval/ship control and crew possession are not certified. Artillery retains stock first-person behavior.

## Installation and startup

**Setup rejects the engine:** verify the installed game build and compare Empire.Retail.dll with the supported SHA256 in the README. Do not bypass fingerprint checks; hook addresses are build-specific.

**Setup finds another DINPUT8.dll:** another injector/mod occupies the same filename. Keep its files and use a tested compatibility arrangement before installing. The possession installer deliberately refuses unknown files.

**F9 does not enter possession:** select a living eligible unit in a land battle, ensure the possession mod is selected, and check that the installed build matches the current engine. Routing, death, unsupported actor state or invalid native camera ownership can prevent entry. Include unit/map and state in a report rather than repeatedly reinstalling.

**An old version still appears loaded:** fully exit the game before updating and start a fresh process afterward. On-disk replacement does not replace a DLL already loaded by a running game. Preserve installation receipts and loaded backing hashes during development.

**Reverting/removing:** no standalone uninstall wizard is supplied. F12 disables the running session. Permanent removal or rollback requires a closed game and careful restoration/removal of only the installer-managed loader, marker and launcher selection. Upgrades retain `.possession-previous` snapshots for managed files; these are backups, not a one-click rollback command. Do not delete unrelated mod selections, modified caches or Windows system DInput files. Ask for help with the installation receipt if the managed-file state is unclear.

**Visual/input issue:** report resolution, Windows display scaling, monitor refresh rate, battle speed, pause state, weapon and whether gameplay FPS also dropped. Software render timings alone are not measured game FPS or monitor scanout.
