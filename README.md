# Shogun 2 First-Person Possession

Take control of a soldier in your army and fight your **Total War: SHOGUN 2** battles from the front line. Select a unit and press **F9** to jump in. Press **F10** when you want to return to commanding the army.

Fight on foot or horseback, fire bows and guns, and face enemy soldiers in melee. Your unit fights alongside you, and you can give them orders without leaving first person.

**[Download v0.24](https://github.com/logsam58/shogun2-first-person/releases/tag/v0.24)** · [v0.24 notes](docs/releases/v0.24.md) · [Known problems](docs/KNOWN_ISSUES.md)

**v0.24** improves Rapid Advance and targeting enemies through friendly troops. Press **1** to toggle your unit's native melee mode. Your unit can still form up and fire while you move freely away from its formation. The first-person weapon view still needs some work; see the [known problems](docs/KNOWN_ISSUES.md).

## Playing the mod

- **Ranged combat:** Aim and fire your soldier's weapon, switch ammunition and manage reloads. Shot spread depends on your unit's accuracy.
- **Melee combat:** Time your mouse clicks with the on-screen prompts to fight at close quarters.
- **Unit commands:** Order moves and attacks, drag out formations, hold a position, follow you and choose when to fire.
- **Siege battles:** Climb castle walls and fight inside the fortifications.
- **HUD:** See your unit's condition, weapon, ammo and controls in a style that fits the game.

And if you shoot one of your own men, you'll hear **“SHAMEFUL DISPLAY!”**

## Getting started

You'll need the Windows Steam version of **Total War: SHOGUN 2**.

1. Open the [release page](https://github.com/logsam58/shogun2-first-person/releases/tag/v0.24) and download **Shogun2-First-Person-v0.24-Setup.exe**.
2. Close the game completely, run setup and choose your Shogun 2 folder.
3. Launch through Steam. In a land battle, select one of your units and press **F9**.

Close the game before installing an update, then start it again afterward.

## Controls

| Key | What it does |
| --- | --- |
| **F9** | Possess a soldier in the selected unit |
| **F10 / Escape** | Return to your previous RTS camera view |
| **WASD** | Move |
| **Shift** | Run |
| **Mouse** | Look around |
| **Hold RMB** | Ready or draw your ranged weapon |
| **LMB** | Fire or release your shot |
| **1** | Toggle the unit between native melee and ranged mode, when supported |
| **Tab** | Cycle available ammunition |
| **Hold Ctrl** | Crouch to fire, where supported |
| **LMB / RMB prompt** | Click the shown button when the marker reaches the highlighted zone |
| **F** near a wall | Start climbing |
| **W / S** while climbing | Climb up / down; release both to hold position |
| **F12** | Turn the mod off for the rest of the session |

### Unit orders

| Key | Order |
| --- | --- |
| **Q — Move / Attack** | Aim at the ground to move, or at an enemy or supported breakable target to attack. Tap to walk; double-tap to run or charge. Hold Q and drag left to right to set the formation, then release. |
| **E — Stop** | Stop the unit. |
| **Z — Hold Here** | Form up where you're standing, facing the direction you're looking when you press Z. Tap to walk there; double-tap to run. |
| **C — Follow Me** | Form up beside you. Run to catch up if left behind, then match your pace. When you stop, the unit finishes forming up around you. |
| **R — Fire orders** | Cycle **Hold Fire → Fire at Will → Fire on Command**. Fire on Command tells the unit to fire when you fire. |

You can't issue these orders while locked in a melee animation, climbing or landing.

## Known problems and compatibility

**Large units, one full army per side and maximum graphics in DX11** have been reported smooth. Ultra units with several armies or heavy fighting nearby can still slow down badly. v0.24 retains those optimizations, the audio guard and a correction for one captured detached-soldier crash. Other crashes remain unresolved. Mounted bows can look wrong, and some weapon or arm clipping remains. See the [known problems and reporting guide](docs/KNOWN_ISSUES.md).

The mod is intended for single-player land battles and sieges. It supports weapons from the base game, Rise of the Samurai and Fall of the Samurai, though not every unit, ammo type or castle layout has been tested. Multiplayer and overhaul mods haven't been confirmed to work. Ship possession isn't included; artillery uses the game's existing first-person mode.

## For contributors

The mod is written in C++, with Python build and debugging tools. Start with the [source map](SOURCE_MAP.md), [build guide](BUILD_AND_TEST.md) and [contributing guide](CONTRIBUTING.md). The [architecture](docs/ARCHITECTURE.md), [v0.24 technical notes](docs/releases/v0.24-technical.md) and [validation records](VALIDATION.md) cover implementation, tests and ongoing investigations.

Building the full HUD and QTE needs separately prepared game resources; a fresh clone doesn't include them. The optional [crash recorder](tools/diagnostics/README.md) has its own setup and isn't part of the player installer. The source download is for contributors; players should use the setup download above.

There is no project-wide license yet. MinHook's license is included; see [third-party notices](THIRD_PARTY_NOTICES.md). This is an unofficial Shogun 2 mod.
