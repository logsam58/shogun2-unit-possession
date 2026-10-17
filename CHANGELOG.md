# Changelog

## v0.16 - prepared 17 October 2026

- Q remaps native contextual move/attack: single-tap walk, double-tap run/charge, and hold/drag native formation placement with first-person aim active.
- E stops; Z captures Hold Here with guard, single-tap walk or double-tap run; C follows beside/around the player and runs to catch up before matching the player's native pace.
- R cycles Hold Fire, Fire at Will and Fire on Command after an actual successful personal shot. Independent native AI/firing remains in charge outside that policy.
- Stationary mouse-look no longer restarts follow goals. Near clipping and infantry arm/weapon look use the owned first-person camera/current eye, with exact restoration and preserved native head/shoulder caches.
- Native unit pathfinding, formation geometry, run eligibility, fatigue, equipment, accuracy, QTE, RTS return and siege foundations are retained.

Local development build Test102; not yet published. New pace and contextual attack behavior still needs recorded live validation. [Release notes](docs/releases/v0.16.md).

## v0.15 - 17 October 2026

First packaged GitHub release of Shogun 2 First-Person Possession. This release brings the complete native possession experience together:

- First-person control of a selected living soldier, native movement/running/fatigue and animated camera/death behavior.
- Personal bow, firearm and recognized projectile handling, native ammunition/reload, melee/ranged equipment selection and supported crouched firing.
- Native effective-accuracy muzzle cone, dynamic firearm spread reticles and descending non-firearm aiming dashes.
- Native paired melee QTEs with readable left/right prompts, at least three active seconds plus one full cycle, timestamped input and interruption feedback.
- Cavalry possession with separate steering/look, independent unit AI/firing and halt/advance/follow orders.
- Native-speed siege wall ascent/descent/hold, crest animation and possession continuity into landing.
- Saved RTS camera restoration and a contextual HUD using the game's visual language.
- Integrated loader/setup with exact predecessor recognition, transactional interruption rollback, reinstall handling and guarded cache cleanup.
- Maintained C++ source, build/regression tooling, diagnostic source, component manifests and player checksums.

The release retains the verified gameplay component build; release-specific code changes are product metadata and exact predecessor installer support. See [release notes](docs/releases/v0.15.md), [validation](docs/releases/v0.15-validation.json) and [known issues](docs/KNOWN_ISSUES.md). Extended-play crashes and mounted-bow presentation remain unresolved.
