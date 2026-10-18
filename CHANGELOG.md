# Changelog

## v0.20 - 18 October 2026

Packages the current Test115 gameplay checkpoint. Changes since the last published v0.18 include:

- Uncluttered native-style HUD, vertical Q/E/Z/C/R legend, contextual bottom information, compact top-centre QTE and larger mouse-button cues.
- Cropped original SHAMEFUL DISPLAY callout for direct personal projectile hits on allies, without the preceding routing sentence.
- QTE clicks read the latest native acknowledged image directly, retaining current identity, expiry, pause and native outcome checks.
- Unrelated readable infantry encounters use one bounded 80-byte snapshot; failed snapshots retain the original individual-read fallback and possible matches retain full identity proof. Offline cases verify the work reduction; this does not establish a whole-game FPS fix.
- Exact Test115 adapter, HUD and QTE binaries retained. Release loader/installer metadata and exact predecessor upgrade/rollback coverage updated for v0.20.

A five-minute trace captured 19,021 frames and 44 sampled QTE serials with no significant lag reported. Earlier intermittent crowded-fight slowdowns, native effects/audio crashes and mounted-bow presentation remain unresolved. [Release notes](docs/releases/v0.20.md) and [validation](docs/releases/v0.20-validation.json).

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
