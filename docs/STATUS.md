# Support and development status

Current source product version: **0.18**, promoting accepted Test107 gameplay. [Download v0.18](https://github.com/logsam58/shogun2-first-person/releases/tag/v0.18), including the integrated installer, Windows package, source archive and checksums. Publication and local installation are separate. The release contains the native possession, weapon, melee QTE, HUD, unit-order, camera-return and siege systems described in the [README](../README.md).

v0.18 adds native DirectX presentation for the existing HUD/QTE/death artwork, current-clock QTE publication and the shared infantry firing-animation correction. The user reported Test107 worked great after verified loaded components and actual recording in 1920Ã—1080 fullscreen DirectX11. This is scoped acceptance, without a universal roster/device or effects-crash claim. See [v0.18 notes](releases/v0.18.md) and [validation](releases/v0.18-validation.json).

| Area | Implemented behavior | Coverage / limitation |
| --- | --- | --- |
| Foot soldiers | Native movement, camera, equipment and personal combat | Exact engine build required; roster-wide validation incomplete |
| Ranged weapons | Recognized bow, firearm and thrown/projectile records; native reload/ammo; accuracy cone | Specialty animations/ammo and every unit firing path need further testing |
| Independent AI | Other soldiers continue normal orders, weapon tasks and combat | Report stalled volleys with the exact unit and ammunition |
| Melee QTE | Native pair preparation, timed input, full-cycle minimum, interruption feedback | Native combat/ownership changes can interrupt; crowds need continued coverage |
| Cavalry | Rider/horse possession, steering and bounded free-look | Mounted-bow presentation remains problematic |
| Sieges | F entry, W/S/hold climbing, native speed, crest and landing camera continuity | Traversal exercised in live play; every castle/layout/lifecycle not verified |
| Camera / death | Saved RTS view restored; native model-following death sequence | Battle/camera identity changes intentionally invalidate stale saved state |
| HUD | Native-style unit, personal weapon/ammo, reticle and action prompts | Check alternate display sizes/scaling and nonstandard unit resources |
| Base game / expansions | Native content catalog includes SHOGUN 2, Rise and Fall weapon records | Catalog coverage is broader than confirmed live roster coverage |
| Naval / artillery | Stock artillery first-person behavior retained | Naval possession, ship commands and crew possession not implemented |
| Multiplayer / overhauls | No compatibility certification | Require separate investigation and testing |
| Stability | Crash capture and diagnostic tools available | Extended-play native effects crash unresolved |

Offline fixtures cover important native forwarding, identity, cleanup, geometry, timing and installation cases. They do not simulate every battle. [VALIDATION.md](../VALIDATION.md) and the release receipt distinguish freshly run checks from retained component evidence.

Current development priorities are the unresolved effects crash, mounted-bow presentation, broader ranged-unit/animation coverage, crowded-combat interruptions and siege-map lifecycle testing. Known limitations and reporting guidance are in [KNOWN_ISSUES.md](KNOWN_ISSUES.md) and [CONTRIBUTING.md](../CONTRIBUTING.md).

## Current unpublished candidate

[Test109](development/Test109.md) includes the simplified Test108 HUD and a short original-game SHAMEFUL DISPLAY notification for a verified personal projectile hitting an ally. The routing lead-in is cropped out. The passive impact observer preserves native damage and existing gameplay; asynchronous playback runs in the HUD process. See its validation receipt for the actual installation state and offline/live limits. Published v0.18 remains unchanged.

## Development history

Test100 unit commands and model presentation were rejected. Test101 improved crosshair placement, stationary following and owned camera/weapon presentation. Test102 added native contextual Q double-click orders and follow/Z pace; failed targeting and possession drops prompted Test103 current-context/input isolation and knockdown/get-up locking. Test104 confined roster overrides to FPS camera work, guarded a reproduced UI followed-actor overwrite and added horizontal combat head-look. The user accepted Test104 for v0.17.

Test105 corrected a reproduced owned-infantry animation reset and QTE timeline drift. Test106 added actual native graphics presentation after window-only recovery did not restore fullscreen UI. The user confirmed HUD visibility but rejected QTE smoothness. Test107 reproduced and corrected stale combat-callback publication while retaining the exact Test106 adapter/HUD. Its accepted gameplay is promoted to v0.18. Numbered [development receipts](development/Test107-validation.json) preserve the fresh/retained test distinctions and limits; earlier published downloads remain unchanged.
