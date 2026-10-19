# Changelog

## v0.23 — 2026-10-19

- Let the rest of your unit form up and fire while your possessed soldier moves independently. Leaving possession restores normal squad control.
- Face the soldier's direction on entry and carry walking/running movement into possession.
- Follow the opening combat animation before allowing limited head-look during melee prompts.
- Stop repeated Follow Me voice acknowledgements as the unit moves and catches up.
- Retain the HUD, QTE, weapon behavior and crowd optimizations.

The player accepted Test130 gameplay and reported a crash in the same session. Its cause remains unresolved. [Player notes](docs/releases/v0.23.md) · [Technical notes](docs/releases/v0.23-technical.md).

## v0.22 — 2026-10-19

- Keep the latest crowd-performance improvements, cleaner HUD and friendly-fire callout.
- Skip more work for unrelated melee encounters while preserving full checks for the player's combat.
- Guard a reproduced zero-sample audio fault. Other crash causes remain under investigation.
- Preserve the player-confirmed smooth Large-unit, one-army-per-side, maximum-graphics DX11 configuration. Ultra units with heavy army presence can still cause severe slowdown.

See [release notes](docs/releases/v0.22.md) for controls and installation, and [technical notes](docs/releases/v0.22-technical.md) for implementation and evidence.

## v0.21 - 19 October 2026

- Improved first-person performance in large infantry and cavalry fights by reducing repeated checks for unrelated soldiers.
- Keeps the updated HUD, melee prompts, unit controls and cropped "SHAMEFUL DISPLAY!" friendly-fire callout.
- Packages the exact Test122 gameplay selected after a successful playtest. That session ended in a crash; crash safety remains unresolved.
- Updated installer with verified upgrades and interrupted-install rollback from v0.20 and Test122.

[Release notes](docs/releases/v0.21.md) · [Technical details](docs/releases/v0.21-technical.md).

## v0.20 - 18 October 2026

- Cleaner HUD, with commands down the right side and less clutter around the weapon information.
- Smaller top-centre melee meter and larger mouse-button prompts.
- Tighter QTE click handling and fewer repeated checks in crowded melee. The occasional severe battle slowdown is still being investigated.
- Cropped original SHAMEFUL DISPLAY callout when you shoot an ally.
- Updated installer, with the existing possession, weapons, unit orders, cavalry and siege features.

Crashes, mounted-bow visuals and some clipping remain open problems. [Player release notes](docs/releases/v0.20.md) · [Technical notes and test details](docs/releases/v0.20-technical.md).

## v0.19 - 18 October 2026

Promotes the exact accepted Test109 gameplay with the existing possession, combat, command and siege systems.

- Less cluttered native-style HUD: unchanged bottom-left soldier card, vertical right-edge Q/E/Z/C/R legend and relevant-only bottom-centre context.
- Compact top-centre QTE with a larger mouse-button cue, retained INTERRUPTED feedback and unchanged accepted timing/input authority.
- Original cropped SHAMEFUL DISPLAY callout for verified direct personal projectile hits on allies; the routing lead-in is excluded. Playback runs asynchronously in the HUD process.
- Exact gameplay binaries/source retained. Fresh release loader and full installer fixtures cover Test109 and published-v0.18 upgrades, seven interrupted rollbacks each and repeat-install retention.

The player accepted Test109 without sending READY; automatic recording verifies the build and capture retrospectively. That session crashed with blood disabled through a distinct invalid DirectSound virtual call. Originating corruption, possible mod involvement and the relationship to earlier effects/Miles crashes remain unresolved. No crash fix is included. [Release notes](docs/releases/v0.19.md) and [validation](docs/releases/v0.19-validation.json).

## v0.18 - 18 October 2026

Promotes the accepted Test107 gameplay with the full v0.17 possession and unit-control feature set.

- Existing HUD, QTE and death artwork now has native DirectX9/11 presentation, retaining the Windows fallback and 1080p layouts. HUD visibility was accepted in1920×1080 fullscreen DirectX11 play.
- Current proven simulation-clock anchors keep the QTE marker moving between combat callbacks. Pause, native authority, response deadlines and submitted-image click judgement are preserved.
- A shared owned-infantry animation reset correction and consistent current aim identity address reproduced personal firing lifecycle failures while retaining independent unit firing.
- Exact accepted gameplay binaries/source are retained. Fresh release loader/installer checks include exact Test107 and published-v0.17 upgrades, interruption rollback and repeat-install backup retention.

The user reported Test107 "worked great" after verified loaded hashes and actual recording. Full roster/device/death-overlay coverage, hardware latency and the effects crash remain open. [Release notes](docs/releases/v0.18.md) and [validation](docs/releases/v0.18-validation.json).

## v0.17 - 17 October 2026

Promotes the accepted Test104 gameplay into a versioned release package.

- Native Q/E/Z/C/R unit commands, contextual walk/run/charge orders and stock formation dragging.
- Follow Me catches up at a run, then matches walking/running pace without stationary mouse-look restarting orders; Z captures one position/facing for walk or double-tap run.
- Improved owned first-person near clipping and eye-relative infantry weapon/arm look, with preserved native pose caches and exact restoration.
- Current native Q targeting excludes the possessed formation from the aim ray. Manual movement stays locked through knockdown and the complete get-up animation.
- FPS roster overrides stay inside camera work, protecting ordinary native unit consumers; exact current holder lookup and a guarded UI followed-actor write reduce reproduced possession-drop paths.
- Horizontal and vertical head-look continue through QTE and native kill-animation handoff.
- The exact accepted adapter, HUD and QTE binaries are retained. Release metadata is 0.17; loader/installer integrity and exact Test104 upgrade/rollback checks are freshly run.

The user accepted Test104 after verified loaded hashes and automatic crash/input/memory/hardware capture. This does not certify all maps/units or fix the unresolved effects crash and mounted-bow presentation. [Release notes](docs/releases/v0.17.md) and [validation](docs/releases/v0.17-validation.json). The GitHub release includes the installer, Windows package, source archive and checksums.

## v0.16 - unpublished development series, 17 October 2026

- Q remaps native contextual move/attack: single-tap walk, double-tap run/charge, and hold/drag native formation placement with first-person aim active.
- E stops; Z captures Hold Here with guard, single-tap walk or double-tap run; C follows beside/around the player and runs to catch up before matching the player's native pace.
- R cycles Hold Fire, Fire at Will and Fire on Command after an actual successful personal shot. Independent native AI/firing remains in charge outside that policy.
- Stationary mouse-look no longer restarts follow goals. Near clipping and infantry arm/weapon look use the owned first-person camera/current eye, with exact restoration and preserved native head/shoulder caches.
- Native unit pathfinding, formation geometry, run eligibility, fatigue, equipment, accuracy, QTE, RTS return and siege foundations are retained.

Historical development builds Test102 through Test104; superseded by v0.17. Test104 received user playtest acceptance before promotion. [Release notes](docs/releases/v0.16.md).

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
