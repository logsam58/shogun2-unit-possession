# Shogun 2 First-Person Possession

Select one of your units, press **F9**, and join the fight as one of its soldiers. Run across the battlefield, shoot, fight up close, give your unit orders and climb castle walls. Press **F10** to return to the camera view you left behind.

The battle keeps going around you. The rest of your unit can carry on fighting and firing while you control one soldier. Your soldier keeps their usual movement speed and fatigue.

**[Download v0.20](https://github.com/logsam58/shogun2-first-person/releases/tag/v0.20)** · [What's new](docs/releases/v0.20.md) · [Known problems](docs/KNOWN_ISSUES.md)

## What you can do

- **Fight with your soldier's weapons.** Use bows, guns and other supported ranged weapons, change ammunition, or switch to melee. Reloading and ammo still matter.
- **Aim with your unit's accuracy.** Unit type, experience, buffs and debuffs affect your shot spread. Gun crosshairs open up as accuracy drops. Bows and other projectile weapons use descending aiming dashes to help with drop.
- **Fight through melee quick-time events.** Click the shown mouse button when the marker reaches the highlighted zone. You can look around during these fights and the kill animations that follow.
- **Command your unit from the front.** Send them forward, attack a target, spread out their formation, stop, hold a position or have them follow you. The controls are below.
- **Ride and climb.** Possess cavalry, or take an infantry soldier up a siege wall and over the top. You can also possess someone who's already climbing.
- **Keep track of the fight.** The HUD shows your unit's condition, weapon and ammo, with prompts for fighting and climbing. It uses Shogun 2's artwork and fonts.
- **Get called out for friendly fire.** Accidentally shoot one of your own men and you'll hear **“SHAMEFUL DISPLAY!”**

If your soldier gets knocked down, you'll have to wait for them to get back up before moving. If they're killed, a death view plays before you return to commanding the army.

## Getting started

You'll need the Windows Steam version of **Total War: SHOGUN 2**.

1. Open the [release page](https://github.com/logsam58/shogun2-first-person/releases/tag/v0.20) and download **Shogun2-First-Person-v0.20-Setup.exe**. The source download is for people working on the code.
2. Close the game completely, run setup and choose your Shogun 2 folder. Setup enables the mod.
3. Start the game through Steam as usual. In a land battle, select one of your units and press **F9**.

The HUD and combat prompts start with the mod. You don't need to launch them separately. Close the game before installing an update, then start it again afterward.

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
| **1 / 2** | Choose melee / ranged equipment |
| **Tab** | Cycle available ammunition |
| **Hold Ctrl** | Crouch to fire, where supported; you can't move while crouched |
| **Shown LMB / RMB prompt** | Answer a melee quick-time event |
| **F** near a wall | Start climbing |
| **W / S** while climbing | Climb up / down; release both to hold position |
| **F12** | Turn the mod off for the rest of the session |

### Giving your unit orders

| Key | Order |
| --- | --- |
| **Q — Move / Attack** | Point at the ground to move, or an enemy or supported breakable target to attack. Tap to walk; double-tap to run or charge. Hold Q and drag left to right to set the formation, then release—like the usual RTS order. |
| **E — Stop** | Stop the unit. |
| **Z — Hold Here** | Form up where you were standing, facing the direction you were looking when you pressed Z. Tap to walk there; double-tap to run. |
| **C — Follow Me** | Form up beside you. Run to catch up if left behind, then match your walking or running pace. When you stop, the unit finishes forming up around you. |
| **R — Fire orders** | Cycle **Hold Fire → Fire at Will → Fire on Command**. Fire on Command tells the unit to fire when you fire. |

These orders use the game's normal formations and pathfinding. They aren't available while you're locked in a melee animation, climbing or landing.

### Melee and climbing

A melee prompt gives you at least three in-game seconds and one full indicator cycle to answer, unless the fight interrupts it. Pausing the game pauses the meter. **INTERRUPTED** means that prompt was cut short.

At a climbable wall, follow the **F** prompt, then use **W / S** to go up or down. Release both to stay put. At the top, your soldier climbs over and returns to ground movement. Climbing speed follows the unit's own speed.

## Known problems

The mod is still being worked on. Crashes and heavy slowdowns can happen, especially in crowded fights. Mounted bows can look wrong, and some weapon or arm clipping remains. More weapons, ammo types and castle layouts still need testing. See the [known problems and reporting guide](docs/KNOWN_ISSUES.md).

This is for single-player land battles and sieges. Weapon support includes the base game, Rise of the Samurai and Fall of the Samurai, but every unit and ammo combination hasn't been tested. Multiplayer and overhaul mods need separate testing. Ship possession isn't included; artillery keeps the game's existing first-person mode.

## For contributors

The mod is written in C++, with Python build and debugging tools. Start with the [source map](SOURCE_MAP.md), [build guide](BUILD_AND_TEST.md) and [contributing guide](CONTRIBUTING.md). The [architecture](docs/ARCHITECTURE.md), [v0.20 technical notes](docs/releases/v0.20-technical.md) and [validation records](VALIDATION.md) cover how it works and what has been tested.

Building the full HUD and QTE needs separately prepared game resources; a fresh clone doesn't include them. The optional [crash recorder](tools/diagnostics/README.md) also needs its own setup and isn't part of the player installer.

There is no project-wide license yet. MinHook's license is included; see [third-party notices](THIRD_PARTY_NOTICES.md). This is an unofficial Shogun 2 mod.
