# Shogun 2 Unit Possession

Command the army. Then join the fight.

Select one of your units in **Total War: SHOGUN 2**, press **F9**, and take control of a soldier on the battlefield. Fight with the weapon they're carrying, ride with the cavalry, or lead your men from the front. Your army keeps fighting around you. **F10** takes you back to your RTS camera whenever you want to command from above.

**Naval battles do not support possession.** Naval support is planned for a later update. **Multiplayer is not implemented**; Versus and campaign drop-in support are under consideration. Expect visual bugs, gameplay bugs and occasional crashes. Current support is for single-player land battles and sieges.

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

**v0.32 is the Workshop release.** [Subscribe on Steam Workshop](https://steamcommunity.com/sharedfiles/filedetails/?id=3825625050), enable **Shogun 2 Unit Possession** in the launcher, and start the game through Steam. In a single-player land battle or siege, select a friendly unit and press **F9**.

Possession starts in **third person**. Press **P** to switch to first person. There is no setup program, external script, camera popup or Steam launch-option edit. Restart the game after enabling, disabling or updating the mod.

You need an owned copy of the **original Windows Steam version of Total War: SHOGUN 2**, using the supported game build. The English `local_en.pack` supplies the advisor voice; Windows N editions need the Media Feature Pack. No compiler or Python installation is needed to play.

On first launch the mod prepares its artwork, unit cards, ability icons, fonts and cropped “Shameful display!” voice from your installed game, then reuses them. Those game resources are not bundled in the download. The mod loads native code inside the game and runs a separate HUD process. It does **not** install or start a recorder, debugger, hardware monitor or crash-dump collector. Routine gameplay, input and QTE disk logs are disabled; required code and resource caches still use disk space.

The [GitHub release](https://github.com/logsam58/shogun2-unit-possession/releases/tag/v0.32) also provides the matching source kit and an archive of the exact Workshop pack. Existing Workshop users need no separate GitHub download. Repository and Workshop access remain controlled by the owner.

To stop using this build, close the game and disable or unsubscribe from the Workshop item. If upgrading from an older setup.exe version, uninstall that version and remove its old camera-helper launch option before enabling Workshop; running both loaders together is unsupported. See [known issues](docs/KNOWN_ISSUES.md).

## Controls

Third person is the starting view. **P** switches perspectives during possession.

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

This mod is intended for **single-player land battles and sieges**. It recognises weapons from Shogun 2, Rise of the Samurai and Fall of the Samurai, but not every unit, weapon, ability or castle layout has been tested. Naval possession is not available and is planned for later. Multiplayer, including Versus and campaign drop-in, is under consideration and is not implemented. Overhaul compatibility is unconfirmed. Artillery keeps the game's own first-person mode.

Attack charging is still partly broken and unreliable: it may work in one attempt and fail in another. Castle wall climbing is also intermittent. Both remain known bugs.

Battle crashes remain unresolved. **Ultra unit size with several armies or dense fighting can still cause serious slowdowns**, even on low graphics settings. Large units with one full army per side have been reported smooth, but that isn't a performance guarantee. Some first-person weapons and arms can clip, and mounted bows still have presentation issues. Bow draw audio follows the native animation rather than the variable draw input.

See [known issues and troubleshooting](docs/KNOWN_ISSUES.md) for the remaining combat, targeting, camera and display limitations. The Workshop bootstrap replaces the front-end root script entry; compatibility with other mods replacing that entry is untested.

## For developers

This is a native C++ mod running inside the original game, with a separate HUD and Python build/diagnostic tools. The source includes the possession adapter, camera and movement handling, combat/QTE integration, unit commands, abilities, Workshop bootstrap and resource preparation. Older loader/installer tools are retained for reference and compatibility work.

Start with the [source map](SOURCE_MAP.md) and [build guide](BUILD_AND_TEST.md), then use the documentation that fits what you're changing:

- [Architecture and subsystem relationships](docs/ARCHITECTURE.md)
- [Native interfaces, ownership, lifetime and threading contracts](docs/public-release/NATIVE_CONTRACTS.md)
- [Local build dependencies](docs/LOCAL_DEPENDENCIES.md) and [owned-game resource preparation](docs/public-release/RESOURCES.md)
- [Validation and test coverage](VALIDATION.md), including [v0.32 release checks](docs/public-release/V0_32_VALIDATION.json)
- [Workshop bootstrap](docs/public-release/WORKSHOP165.md) and [storage handling](docs/public-release/WORKSHOP166.md)
- [Release packaging](docs/public-release/RELEASING.md)
- [Optional crash diagnostics](tools/diagnostics/README.md) and [contributing](CONTRIBUTING.md)

The source kit prepares game fonts, artwork and audio locally from your owned installation. It doesn't include the engine, private crash recordings or the complete decoding databases. The Workshop runtime prepares those resources from each player’s game, including ability icons and the advisor voice. They are not included in the download. See [resource provenance and distribution status](docs/public-release/PROVENANCE.md).

## Make it your own

Explore it, fork it, improve it, or take it in your own direction. First-party code is available under the **[MIT license](LICENSE)**. Community development is welcome; ongoing maintenance, updates and individual support aren't promised.

Total War: SHOGUN 2 and its game material belong to Creative Assembly, SEGA and their respective rights holders. MinHook and its disassembler code retain their own notices. See [third-party notices](THIRD_PARTY_NOTICES.md). This is an unofficial mod, with no affiliation or endorsement implied.
