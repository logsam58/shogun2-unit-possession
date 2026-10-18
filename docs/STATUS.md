# Support and development status

Current source product version: **0.16**, local development Test104. Latest published product version: **0.15**. The release contains the native possession, weapon, melee QTE, HUD, unit-order, camera-return and siege systems described in the [README](../README.md).

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


The `unit-commands-and-first-person-visibility` branch contains **v0.16 / Test104**. Test101 received "BIG improvement" feedback after exact loaded/capture verification; that is scoped user feedback rather than a complete roster certification. Test102 adds native contextual Q single/double orders, Z walk/run and Follow Me pace/catch-up. The Test101 Q/camera/pose corrections are retained. [Test102 notes](development/Test102.md) distinguish offline checks from pending live behavior.

Test102 received mixed feedback: following/gait mostly worked, Q attack failed, and multiple unintended possession exits occurred. **Test103** corrects current-target refresh/own-formation occlusion, isolates native movement/camera inputs, records detailed release failures, and locks manual movement throughout native knockdown/get-up. Its [development notes](development/Test103.md) separate reproduced native paths from unproven live causes.

Test104 investigates formation recovery across possession, restores ordinary native roster-query behavior outside the camera scope, protects a reproduced selected/wavering UI followed-actor write, and adds bounded horizontal head-look during native combat. [Test104 notes](development/Test104.md) distinguish these corrections from still-pending live formation/ownership/animation acceptance.
