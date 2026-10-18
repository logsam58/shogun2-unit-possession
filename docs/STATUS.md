# Support and development status

Current product version: **0.20**, packaging the Test115 possession adapter, HUD and QTE gameplay. [Download v0.20](https://github.com/logsam58/shogun2-first-person/releases/tag/v0.20). See [release notes](releases/v0.20.md) and [validation](releases/v0.20-validation.json). Release loader/cache identity is distinct from the development build; packaging and publication do not replace an already running installation.

v0.20 includes the uncluttered HUD, compact top-centre QTE, cropped friendly-fire callout, latest native acknowledged-image click handling and bounded crowd-encounter reads. A five-minute Test115 session produced no significant lag reported by the player and included 44 sampled QTE serials. Earlier crowded-fight slowdowns remain unresolved; this session is a smooth-play reference, not reproduction or proof of a fix. Native effects/audio crashes remain open, including captures with blood disabled.

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

Current development priorities are intermittent crowded-battle slowdown, the unresolved native effects/audio crashes, mounted-bow presentation, broader ranged-unit/animation coverage, crowded-combat interruptions and siege-map lifecycle testing. Known limitations and reporting guidance are in [KNOWN_ISSUES.md](KNOWN_ISSUES.md) and [CONTRIBUTING.md](../CONTRIBUTING.md).
