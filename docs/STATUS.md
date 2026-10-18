# Support and development status

Current source product version: **0.19**, retaining the exact accepted Test109 adapter/HUD/QTE. Local release packages are prepared; the [latest published download remains v0.18](https://github.com/logsam58/shogun2-first-person/releases/tag/v0.18). Installation, publication and loaded state are separate. See [v0.19 notes](releases/v0.19.md) and [validation](releases/v0.19-validation.json).

v0.19 adds the uncluttered HUD, compact top-centre QTE with larger button cues, and original cropped SHAMEFUL DISPLAY feedback for direct personal projectile hits on allies. The player said “I loved it” after Test109 play; automatic recording verifies loaded build and capture retrospectively. No formal pre-play READY verification occurred. Native effects/audio crashes remain unresolved, including a new DirectSound fault with blood disabled.

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
| Stability | Crash capture and diagnostic tools available | Native effects and audio-thread crashes unresolved |

Offline fixtures cover important native forwarding, identity, cleanup, geometry, timing and installation cases. They do not simulate every battle. [VALIDATION.md](../VALIDATION.md) and the release receipt distinguish freshly run checks from retained component evidence.

Current development priorities are the unresolved native effects/audio crashes, mounted-bow presentation, broader ranged-unit/animation coverage, crowded-combat interruptions and siege-map lifecycle testing. Known limitations and reporting guidance are in [KNOWN_ISSUES.md](KNOWN_ISSUES.md) and [CONTRIBUTING.md](../CONTRIBUTING.md).
