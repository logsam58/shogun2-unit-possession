# Shogun 2 First-Person Mod

Command the army. Then join the fight.

Select one of your units in **Total War: SHOGUN 2**, press **F9**, and take control of a soldier on the battlefield. Fight with the weapon they're carrying, ride with the cavalry, or lead your men from the front. Your army keeps fighting around you. **F10** takes you back to your RTS camera whenever you want to command from above.

**[Install and play](#install-and-play)** · **[Controls](#controls)** · **[Known issues](docs/KNOWN_ISSUES.md)** · **[Build and source](#for-developers)**

## On the battlefield

- **First or third person.** Play through your soldier's eyes, or pull the camera back and orbit around them. Switch views without giving up control.
- **Choose your soldier.** At the closest RTS zoom, point the cursor at someone in your selected unit and press F9. Generals, officers, flag bearers and musicians are included, using their own equipment. From further above, a soldier is picked for you.
- **Aim, draw and fire.** Use bows, guns and thrown weapons. Hold and release to vary a bow shot's strength. Unit accuracy affects your spread; firearm crosshairs open and close to show it, while projectile sights help you allow for drop.
- **Fight up close.** Time the indicated mouse click during melee, with Shogun 2's own combat animations playing out around you. Running at an enemy can lead into a native charge encounter; the game decides the attack, defense and outcome.
- **Keep your unit with you.** Point out a move or attack, drag a formation, hold a position, call your men alongside you, and choose when they fire. Their formation, pathfinding, morale and fatigue still matter.
- **Use the unit's abilities.** Cycle the abilities and special ammunition the unit actually has. Target friendlies for abilities such as Inspire, with the game's restrictions and cooldowns.
- **Fight through sieges.** Climb castle walls, move through the fortifications, and mount or dismount where the game supports it.

The HUD puts your unit's condition, weapon, ammo, abilities and orders within reach. Press **K** for a clearer view; melee timing prompts stay visible. And if one of your own shots hits an ally, expect **“SHAMEFUL DISPLAY!”**

## Install and play

**v1.0.** Download **`Shogun2-First-Person-v1.0-Setup.exe`** from the [latest release](https://github.com/logsam58/shogun2-first-person/releases/tag/v1.0). The repository remains private until its owner opens access.

You need an installed, owned copy of the **original Windows Steam version of Total War: SHOGUN 2**. The installer checks the supported game build and runs on Windows 10 or later. No compiler, Python installation or separate mod launcher is needed to play.

1. Open the [v1.0 release](https://github.com/logsam58/shogun2-first-person/releases/tag/v1.0) and download **`Shogun2-First-Person-v1.0-Setup.exe`** from **Assets**. You need to be signed into GitHub with repository access.
2. Close Shogun 2 completely. Run setup and choose the folder containing **Shogun2.exe** and **Empire.Retail.dll**.
3. Launch the game normally **through Steam**. In a single-player land battle, select one of your units and press **F9**.

Close the game before updating or removing the mod, and start a fresh game process afterward. To remove this setup, open **Windows Settings → Apps → Installed apps → Shogun 2 First-Person Mod → Uninstall**. Setup reads the artwork, unit cards, ability icons, fonts and cropped “Shameful display!” voice from your installed game. Those resources are not bundled in this download. The supported English `local_en.pack` is needed for the voice; Windows N editions need the Media Feature Pack. Setup preserves unknown replacement files and retains rollback snapshots. The optional crash recorder is separate and isn't installed or removed with the mod.

## Controls

### Your soldier and camera

| Key | Action |
| --- | --- |
| **F9** | Possess a soldier in the selected unit; at closest RTS zoom, point the mouse cursor at who you want |
| **F10 / Escape** | Return to your previous RTS camera |
| **WASD** | Move |
| **Shift** | Run; hold **W + Shift** toward an enemy to attempt a charge |
| **Mouse** | Look around |
| **P** | Switch first/third person |
| **Mouse wheel** in third person | Move the camera closer or further away |
| **Hold middle mouse** in third person | Orbit your soldier while keeping your movement direction |
| **M** | Order the unit to mount/dismount, where supported |
| **Hold RMB** | Ready your ranged weapon |
| **LMB** | Fire guns or thrown weapons; hold and release for bow draw strength |
| **1** | Toggle the whole unit's native melee/ranged mode, where supported |
| **T / G** | Select an ability or special ammunition / use or end it; aim at friendlies for targeted abilities |
| **Hold Ctrl** | Crouch to fire, where supported |
| **Shown LMB / RMB prompt** | Click the indicated button as the marker reaches the highlighted zone |
| **F** near a wall | Begin climbing |
| **W / S** while climbing | Climb up / down; release both to hold position |
| **K** | Hide/show the RTS and possession HUD; melee prompts remain visible |
| **F12** | Disable the mod until the game restarts |

### Your unit

| Key | Order |
| --- | --- |
| **Q — Move / Attack** | Point at the ground to move, or an enemy or supported breakable target to attack. Tap to walk; double-tap to run or charge. Hold Q and drag left to right to set the formation, then release. |
| **E — Stop** | Stop the unit. |
| **Z — Hold Here** | Form up at your current position, facing the direction you're looking when you press Z. Tap to walk there; double-tap to run. |
| **C — Follow Me** | Form up beside you. Run to catch up when needed, then match your pace. When you stop, your men finish forming up around you. |
| **R — Fire orders** | Cycle **Hold Fire → Fire at Will → Fire on Command**. Fire on Command tells the unit to fire when you fire. |

Unit orders are unavailable while you're locked in a melee animation, climbing or landing.

## Compatibility and limits

This mod is intended for **single-player land battles and sieges**. It recognises weapons from Shogun 2, Rise of the Samurai and Fall of the Samurai, but not every unit, weapon, ability or castle layout has been tested. Multiplayer and overhaul compatibility are unconfirmed. Ship possession isn't included; artillery keeps the game's own first-person mode.

Charging and castle wall climbing are intermittent: sometimes they work, sometimes they do not. Both remain known bugs.

Battle crashes remain unresolved. **Ultra unit size with several armies or dense fighting can still cause serious slowdowns**, even on low graphics settings. Large units with one full army per side have been reported smooth, but that isn't a performance guarantee. Some first-person weapons and arms can clip, and mounted bows still have presentation issues. Bow draw audio follows the native animation rather than the variable draw input.

See [known issues and troubleshooting](docs/KNOWN_ISSUES.md) for the remaining combat, targeting, camera and display limitations. Setup also refuses an unrelated **DINPUT8.dll** rather than overwriting another mod.

## For developers

This is a native C++ mod running inside the original game, with a separate HUD and Python build/diagnostic tools. The source includes the possession adapter, camera and movement handling, combat/QTE integration, unit commands, abilities, loader and installer components.

Start with the [source map](SOURCE_MAP.md) and [build guide](BUILD_AND_TEST.md), then use the documentation that fits what you're changing:

- [Architecture and subsystem relationships](docs/ARCHITECTURE.md)
- [Native interfaces, ownership, lifetime and threading contracts](docs/public-release/NATIVE_CONTRACTS.md)
- [Local build dependencies](docs/LOCAL_DEPENDENCIES.md) and [owned-game resource preparation](docs/public-release/RESOURCES.md)
- [Validation and test coverage](VALIDATION.md), including [v1.0 release checks](docs/public-release/V1_0_VALIDATION.json)
- [Standard Windows setup packaging](docs/public-release/STANDARD_SETUP.md)
- [Optional crash diagnostics](tools/diagnostics/README.md) and [contributing](CONTRIBUTING.md)

The source kit prepares game fonts, artwork and audio locally from your owned installation. It doesn't include the engine, private crash recordings or the complete decoding databases. The installer prepares those resources from each player’s game, including ability icons and the advisor voice. They are not included in the download. See [resource provenance and distribution status](docs/public-release/PROVENANCE.md).

## Make it your own

Explore it, fork it, improve it, or take it in your own direction. First-party code is available under the **[MIT license](LICENSE)**. Community development is welcome; ongoing maintenance, updates and individual support aren't promised.

Total War: SHOGUN 2 and its game material belong to Creative Assembly, SEGA and their respective rights holders. MinHook and its disassembler code retain their own notices. See [third-party notices](THIRD_PARTY_NOTICES.md). This is an unofficial mod, with no affiliation or endorsement implied.
