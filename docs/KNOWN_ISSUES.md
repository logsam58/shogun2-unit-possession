# Known problems and troubleshooting

The mod is still being worked on. These are the main things to watch for.

## Crashes and slowdowns

Crashes can happen during battles, including with blood turned off. We've captured several different failures, but haven't pinned down what starts them or whether they share a cause.

v0.21 improves crowded-fight performance and the latest player test worked well, but large infantry and cavalry fights can still become slow. It can happen without a QTE showing, and sometimes the QTE stays smooth while the battle slows down. Some fights recover on their own. Leaving and re-entering possession isn't a confirmed fix.

If you hit either problem, include the unit, map and what was happening nearby. The [technical notes](releases/v0.21-technical.md#crash-evidence) contain the crash details and investigation limits.

## Visual problems

- **Mounted bows:** the weapon can look wrong while aiming. This is still an open issue.
- **Arms and weapons clipping:** first-person visibility has improved, but some weapons and animations still let you see through parts of the model. Looking sharply up/down can expose it.
- **Missing HUD or odd timing:** include resolution, fullscreen/windowed mode, Windows scaling, monitor refresh rate, battle speed and whether the whole game slowed down. Not every display/overlay combination has been tested.

## Combat and unit orders

- **Interrupted melee prompts:** another hit, death or a change in the fight can cut a prompt short. INTERRUPTED is there to show that it ended early. Crowded fights still need more testing.
- **Shots or volleys not releasing:** include the exact unit, weapon and ammo. Bow, gun and other weapon support is broader than the combinations tested so far. Say whether it was your soldier, the rest of the unit, or both.
- **Unexpected return to the RTS view:** some battle situations still need investigation. Say whether your soldier was alive, wavering, climbing, falling or landing when it happened.
- **Formations:** orders use Shogun 2's own formations and pathfinding. Terrain, casualties and morale can change how a unit responds. Report soldiers staying stuck or failing to reform.
- **Siege climbing:** speed, climbing over and landing have been tested, but not every castle layout or interruption. Include the castle and where on the wall it happened.
- **Friendly-fire voice:** it covers direct hits from your own tracked shots. Splash damage, secondary effects and every weapon combination haven't been tested.
- **Aiming:** the descending dashes help with drop; they aren't a rangefinder. Shot spread follows unit accuracy, but isn't an exact copy of RTS shot scattering.

## Compatibility

The mod is meant for single-player land battles and sieges. Weapons from Shogun 2, Rise of the Samurai and Fall of the Samurai are recognised, but not every unit and ammo combination has been tested.

Multiplayer and overhaul mods need separate testing. Ship possession and ship commands aren't included. Artillery keeps the game's existing first-person mode.

## Setup and updates

**Setup rejects the game:** check your game files and game version. The installer checks for the supported game build. Developers can find its file fingerprint in the [build guide](../BUILD_AND_TEST.md).

**Setup finds another DINPUT8.dll:** another mod may use the same file. The installer won't overwrite an unknown one. Those mods need a compatible setup before they can be used together.

**F9 does nothing:** select a living unit in a land battle and check that the mod is enabled. Include the unit and what it was doing if entry keeps failing.

**An old build still seems active:** close Shogun 2 completely before updating, then launch it again through Steam.

**Turning it off or removing it:** F12 turns the mod off for the current session. There isn't an uninstall wizard yet. Permanent removal or rollback needs the game closed and the installer-managed files restored or removed carefully. Keep the installation receipt and backups; don't remove unrelated mods or Windows files.

## Reporting a problem

Please include:

- Mod version, game/expansion, battle type and map.
- Unit, weapon and ammo.
- What you did, what you expected and what actually happened.
- Other mods and graphics settings, especially for visual or timing problems.
- For a crash, any error message or exception/module information you have.

The optional [crash recorder](../tools/diagnostics/README.md) can collect more detail. Full dumps and input recordings can contain private data, so review them before sharing. The [contributing guide](../CONTRIBUTING.md#useful-bug-reports) has more reporting detail.
