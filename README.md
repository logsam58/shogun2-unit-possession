# Shogun 2 First-Person Possession

**Fight as a soldier inside your own Total War battle.** This native C++ mod for **Total War: SHOGUN 2** lets you take first-person control of an existing member of a selected unit, move across the battlefield, aim and fire their weapon, fight through timed melee encounters, and climb siege walls. Return to command at any time, with your previous RTS camera restored.

The battle continues around you. The game still runs the armies, formations, pathfinding, morale, fatigue, ammunition, projectile physics, casualties and victory conditions. Your other soldiers keep their own AI and can keep fighting and firing while you control one member of the unit.

**[Download v0.18](https://github.com/logsam58/shogun2-first-person/releases/tag/v0.18)** | [Known issues](docs/KNOWN_ISSUES.md) | [Build instructions](BUILD_AND_TEST.md) | [Contributing](CONTRIBUTING.md)

**v0.18** adds native fullscreen HUD/QTE presentation, smoother QTE marker timing and an infantry firing-animation correction. It keeps Q/E/Z/C/R unit commands, native move/attack and formation dragging, pace-matched following, first-person model improvements, recovery locking and combat head-look. See the [unit controls](#unit-orders-and-independent-ai) and [v0.18 release notes](docs/releases/v0.18.md).

## What the mod does

### First-person possession and camera

Select a living unit during a land battle and press **F9** to possess an eligible soldier. The view follows the soldier's native animated body and head, including weapon handling and traversal. Movement and actions apply to that soldier; the rest of the unit remains part of the RTS simulation.

**F10** or **Escape** ends possession and returns to the RTS camera position, direction and field of view saved before entry. The mod validates the current battle, camera and soldier identity before using them. If the soldier dies, a model-following death view and closing-eyelid sequence precede the return to command.

### Movement, terrain and cavalry

Move with **WASD**, hold **Shift** to run, and use the mouse to look. Manual movement stays locked through knockdown and the native get-up animation. Movement uses the game's walking/running parameters, acceleration, turn limits, terrain interaction and fatigue. Possession does not give every soldier a common movement speed.

Mounted possession keeps rider and horse identities together and separates looking from steering. Mounted free-look is constrained around the rider's forward direction. Cavalry support exists, but mounted-bow presentation remains a known problem; see the [coverage table](docs/STATUS.md).

### Weapons, aiming and accuracy

Hold **right mouse** to ready or draw a supported ranged weapon, then press **left mouse** to fire or release it. The soldier uses native weapon animations, ammunition, projectile records and reload behavior. Bows, firearms and recognized personal thrown/projectile families have handling paths; the catalog includes base-game, Rise of the Samurai and Fall of the Samurai weapon records. Catalog recognition does not guarantee every unit, animation or ammunition combination has been tested.

Shots spread in a circular cone from the muzzle. Cone width comes from the selected unit's **native effective accuracy**, including the game providers responsible for unit type, experience and current modifiers. Better effective accuracy produces a narrower cone. Projectile speed, gravity, collision and native effects remain controlled by the game.

Firearm crosshair bars move farther apart as spread increases and closer together as accuracy improves. Non-firearm projectile reticles use descending dashes to help aim with drop. Those dashes are an aiming reference, not a calibrated rangefinder. The circular cone is a first-person conversion of native accuracy; it does not exactly reproduce the game's rectangular scatter or guarantee the same stock hit probability.

Press **1 / 2** to select melee or ranged equipment and **Tab** to cycle currently available native ammunition. Availability and unlocks are respected. **Hold Ctrl** for crouched firing on supported, unlocked firearm animation sets; movement is locked while crouched. Equipment changes wait through unsafe shot, reload and committed-combat states. Vertical aim is retained during supported infantry weapon lowering and reload transitions.

### Melee and timed combat prompts

Close combat uses the game's paired combat preparation and animations, with **quick-time events (QTEs)** for the possessed soldier. Follow the displayed **LEFT CLICK** or **RIGHT CLICK** instruction and click when the moving indicator reaches the success region. Native effective combat state informs difficulty, and the outcome feeds back into native combat rather than a separate health simulation.

An unanswered prompt allows **at least three active-simulation seconds and one complete outward-and-return indicator cycle**. Actual pause freezes its response clock; battle-speed changes follow simulation timing. A completed answer or a genuine interruption can end a prompt earlier. Large mouse-button graphics identify the required input. **INTERRUPTED** appears when an admitted event is canceled so a lost opportunity is visible.

Crowded engagements can defer native pair preparation, and a prompt cannot outlive its valid participants or ownership. Death, lost identity, projectiles and other native lifecycle changes can still interrupt combat. Timestamped clicks are matched to recent submitted visual geometry; this is software timing alignment, not a measurement of monitor scanout or total hardware latency. During QTEs and the continuing native combat animation, the mouse controls vertical and horizontal head-look, limited to 100 degrees either side of the animated head. Native body motion and the combat outcome continue.

### Siege wall climbing

Possession supports a soldier approaching a climbable wall, already climbing it, crossing the crest and landing inside the fortification. Near an eligible wall endpoint, **F** starts climbing. **W** ascends, **S** descends, and releasing both holds position. At the top, the native climb-over animation and contact/landing behavior take over before ground movement resumes.

Climbing uses native wall geometry, the unit's climb-speed parameters and animation progression. A soldier already on the wall does not need a second climb command. The camera remains attached through the crest-to-landing transition when ownership stays valid. Coverage across all castle layouts and interruption scenarios is still incomplete.

### Unit orders and independent AI

The development build uses the controls below while possessed:

- **Q - Move / Attack:** aim at ground, an enemy unit or a destructible target. Tap Q for the native walk order; double-tap Q for the native run/charge order. The game chooses move or attack from the target under the crosshair. Hold Q and move your aim from left to right to lay out a formation, then release Q to commit. First-person mouse-look stays active and the native context cursor stays at the crosshair. Q remaps the stock right-button gesture at that aim point; the game owns placement graphics, frontage, constraints and order submission. There is no replacement formation overlay.
- **E - Stop:** issue the native halt command.
- **Z - Hold Here:** tap once to walk into formation at your captured position and facing, with native guard behavior. Double-tap to run to that same captured position/facing. The first tap acts immediately.
- **C - Follow Me:** keep a normal native formation beside/around the possessed soldier, including forming up when the player is stationary. While behind, the formation runs to catch up. Once nearby, it matches your native walk/run pace, including Shift running. Actual movement updates its native goal and travel direction. Looking around while stationary does not issue new orders; the existing formation is allowed to finish.
- **R - Firing policy:** cycle **Hold Fire -> Fire at Will -> Fire on Command**. Command mode allows a same-unit native volley after a successful personal shot. Failed clicks/dry draws do not authorize one; native aiming, reload, ammunition and weapon release sequences remain in charge.

These commands retain native pathfinding, formation movement, morale and casualties. E/Z/C replace following; Q replaces it when its gesture is released. Double-tap recognition uses the Windows double-click interval; held-key autorepeat does not count. Commands are unavailable during owned paired combat, climbing or airborne landing. Ordinary firing outside the explicitly selected command-fire mode retains independent unit AI. The published v0.15 release uses F5/F6/F7 for halt/advance/follow.


### Contextual HUD

The compact HUD uses SHOGUN 2 artwork, trim and fonts to fit the game's presentation. It shows unit strength, morale and fatigue alongside personal equipment, ammunition and relevant action state. Aiming reticles, climb instructions, crouch availability and melee prompts appear in context. Unit-level information describes the unit; weapon and QTE information describes the possessed soldier.

## Installation and first use

Requirements: a Windows Steam installation of SHOGUN 2 using the supported retail engine build, and a single-player land battle. The mod runs as x86 native code inside the game. Players do not need a compiler, Python, a game SDK or a separate companion launcher.

1. Close SHOGUN 2 completely.
2. Download **Shogun2-First-Person-v0.15-Setup.exe** from the [release page](https://github.com/logsam58/shogun2-first-person/releases/tag/v0.15), or extract the Windows ZIP and run the setup inside it.
3. Select the SHOGUN 2 game folder in the wizard. Setup checks the engine and existing mod files, installs the runtime and selects the possession mod in the launcher.
4. Launch normally through Steam, enter a land battle, select a living unit and press **F9**.
5. Press **F10 / Escape** to return to command. **F12** disables the mod for the running session.

The installer recognizes supported previous possession builds, keeps one previous managed-file backup, restores changes if installation fails, and leaves unrecognized or modified cache files alone. It refuses an unrelated `DINPUT8.dll` in the game folder. Other injection mods using that filename require a compatibility solution; do not overwrite them manually. Native DLL updates require a closed game and a fresh launch.

The supported `Empire.Retail.dll` SHA256 is `022fae2e2da92b59b8b1b8ee52887003c624e3c8eca060b209f5d792a7f874f5`. Release assets include checksums and a component manifest. See [known issues](docs/KNOWN_ISSUES.md) for removal, compatibility and troubleshooting details.

## Controls

| Input | Action |
| --- | --- |
| F9 | Possess an eligible soldier in the selected living unit |
| F10 / Escape | End possession and restore the saved RTS view |
| WASD | Move; W/S become ascent/descent while climbing |
| Shift | Run when the native unit state permits |
| Mouse | Look and aim |
| Q | Native move/attack at crosshair: tap to walk, double-tap to run/charge; hold and drag aim left/right for formation placement |
| E | Stop the possessed unit |
| Z | Hold at captured position/facing: tap to walk, double-tap to run |
| C | Follow beside/around you; run to catch up, then match your native pace |
| R | Cycle Hold Fire / Fire at Will / Fire on Command |
| Hold RMB, then LMB | Ready/draw, then fire/release a personal ranged weapon |
| 1 / 2 | Select melee / ranged equipment |
| Tab | Cycle available, unlocked native ammunition |
| Hold Ctrl | Supported crouched firing; movement is locked |
| Displayed LMB / RMB prompt | Answer a melee QTE in its success region |
| F near a valid wall | Start climbing |
| W / S / release both on a wall | Ascend / descend / hold |
| F12 | Disable the mod for the session |

## Compatibility and current limits

The mod targets single-player land combat, including sieges. Weapon catalogs cover content from SHOGUN 2, Rise of the Samurai and Fall of the Samurai, but that is not a claim of complete campaign or roster validation. Naval possession and ship commands are not implemented; artillery retains its stock first-person behavior. Multiplayer compatibility is unverified. Overhaul mods, altered engine builds and other camera/input injectors need separate testing.

**An unresolved crash has occurred during extended siege play.** Captured dumps show a native effects-table dereference; the preceding state change and possible mod involvement are unresolved. v0.15 does not include a fix for that crash. Mounted-bow visuals, specialty-unit coverage and some crowded-combat/ownership transitions also require further testing. Read the [full issue list](docs/KNOWN_ISSUES.md) and [support status](docs/STATUS.md) before reporting a problem.

## Source, architecture and development

This repository contains the C++ possession adapter, melee QTE runtime, HUD, integrated DInput loader, transactional setup wizard, offline regression fixtures and Python crash-diagnostic tools. The loader verifies and starts the embedded mod components. Native hooks adapt the controlled soldier's behavior; strict actor/unit/camera identities and ownership guards constrain those hooks. The HUD receives numeric state, and a separate QTE worker handles presentation and timestamped input.

Start with [SOURCE_MAP.md](SOURCE_MAP.md) for component entry points and [architecture](docs/ARCHITECTURE.md) for ownership, timing and data flow. Versioned internal filenames and directories identify stable component interfaces; the product version is in [VERSION](VERSION).

Build on Windows with Python 3.10+, MSVC x86 C++ tools and the Windows SDK:

```powershell
py tools/build.py adapter --tests
py tools/build.py all --tests
py -m unittest discover -s tests -v
```

Read [BUILD_AND_TEST.md](BUILD_AND_TEST.md) first: full HUD/QTE builds need separately prepared game UI resources, and native/installer fixtures need exact local binary inputs. Those dependencies are **not supplied by a fresh clone**. Outputs go to ignored `artifacts/`; building does not install or launch the game. [VALIDATION.md](VALIDATION.md) explains fixture coverage and its limits. [CONTRIBUTING.md](CONTRIBUTING.md) covers change review and useful bug reports.

## Diagnostics and licensing

Optional [diagnostic source](tools/diagnostics/README.md) watches game launches, captures crash dumps and records process/resource, GPU/VRAM, input and native telemetry for investigation. It needs separate local setup and is not installed by the player setup. Full dumps and recordings can contain private process and input data; keep them out of commits and review them before sharing.

MinHook source and its license are included. The main project has **no declared project-wide license** yet; see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md). Original game binaries, extracted artwork/fonts, complete decoding databases, recordings, crash dumps and local configuration are excluded from source control. This is an unofficial mod for Total War: SHOGUN 2.
