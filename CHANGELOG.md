# Changelog

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
