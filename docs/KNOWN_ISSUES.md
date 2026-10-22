# Known problems and troubleshooting

The mod is still being worked on. **Visual and gameplay bugs, and some crashes, are expected.**

**Naval battles do not support possession.** Naval support is planned for a later update. **Multiplayer is not implemented:** Versus and campaign drop-in are under consideration.

## Crashes and slowdowns

Crashes can still happen during battles, including with blood turned off. The accepted v0.28 playtest also ended in a recorded crash; its cause remains unresolved. v0.24 corrects one captured fault where a detached soldier's native advance task expected a squad. Other captured failures remain unresolved, and this does not establish a shared cause for every crash.

v0.22 was reported smooth with Large units, one massive army per side and maximum graphics in DX11. Ultra units with substantial army presence can still become very slow. The audio guard covers one reproduced fault; it does not resolve all crashes. It can happen without a QTE showing, and sometimes the QTE stays smooth while the battle slows down. Some fights recover on their own. Leaving and re-entering possession isn't a confirmed fix.

If you hit either problem, include the unit, map and what was happening nearby. The [technical notes](https://github.com/logsam58/shogun2-first-person/blob/v0.28/docs/releases/v0.24-technical.md#remaining-limits) contain the crash details and investigation limits.

## Visual problems

v0.24 gameplay was accepted, with first-person weapon/arm poses and clipping left for later work. Melee combat presentation was reported good; ordinary walking and running keep their original movement states.

- **Mounted bows:** the weapon can look wrong while aiming. This is still an open issue.
- **Arms and weapons clipping:** first-person visibility has improved, but some weapons and animations still let you see through parts of the model. Looking sharply up/down can expose it.
- **Missing HUD or odd timing:** include resolution, fullscreen/windowed mode, Windows scaling, monitor refresh rate, battle speed and whether the whole game slowed down. Not every display/overlay combination has been tested.

## Combat and unit orders

- **Bow draw and sound:** bow strength depends on holding LMB, but the native draw animation and sound begin with RMB. Separating sound timing from that animation is deferred.

- **Charging is intermittent:** it sometimes works and sometimes does not. This remains an open bug in the current build.
- **Enemies targeting a separated player:** autonomous enemy shooting and charge acquisition when you leave your formation still need investigation. Being present in native targeting data does not prove that an enemy will choose you.

- **Interrupted melee prompts:** another hit, death or a change in the fight can cut a prompt short. INTERRUPTED is there to show that it ended early. Crowded fights still need more testing.
- **Shots or volleys not releasing:** include the exact unit, weapon and ammo. Bow, gun and other weapon support is broader than the combinations tested so far. Say whether it was your soldier, the rest of the unit, or both.
- **Unexpected return to the RTS view:** some battle situations still need investigation. Say whether your soldier was alive, wavering, climbing, falling or landing when it happened.
- **Formations:** orders use Shogun 2's own formations and pathfinding. Terrain, casualties and morale can change how a unit responds. Report soldiers staying stuck or failing to reform.
- **Castle wall climbing is intermittent:** it sometimes works and sometimes does not. This remains an open bug in the current build. Include the castle and wall location when reporting it.
- **Friendly-fire voice:** it covers direct hits from your own tracked shots. Splash damage, secondary effects and every weapon combination haven't been tested.
- **Aiming:** the descending dashes help with drop; they aren't a rangefinder. Shot spread follows unit accuracy, but isn't an exact copy of RTS shot scattering.

## Compatibility

The mod is meant for single-player land battles and sieges. Weapons from Shogun 2, Rise of the Samurai and Fall of the Samurai are recognised, but not every unit and ammo combination has been tested.

Multiplayer is not implemented; Versus and campaign drop-in support are under consideration. Naval possession and ship commands do not work and are planned for later. Overhaul compatibility needs separate testing. Artillery keeps the game's existing first-person mode.

## Setup and updates

**Setup rejects the game:** check your game files and game version. The installer checks for the supported game build. Developers can find its file fingerprint in the [build guide](../BUILD_AND_TEST.md).

**Setup finds another DINPUT8.dll:** another mod may use the same file. The installer won't overwrite an unknown one. Those mods need a compatible setup before they can be used together.

**F9 does nothing:** select a living unit in a land battle and check that the mod is enabled. Include the unit and what it was doing if entry keeps failing.

**An old build still seems active:** close Shogun 2 completely before updating, then launch it again through Steam.

**Turning it off or removing it:** F12 turns the mod off for the current session. The new standard setup provides a Windows uninstall entry; close the game, then use Settings → Apps → Installed apps → Shogun 2 First-Person Mod → Uninstall. It preserves unknown replacement files and retains rollback snapshots. Older custom installers require careful manual removal or restoration. See [standard setup](public-release/STANDARD_SETUP.md).

## Reporting a problem

Please include:

- Mod version, game/expansion, battle type and map.
- Unit, weapon and ammo.
- What you did, what you expected and what actually happened.
- Other mods and graphics settings, especially for visual or timing problems.
- Army count and unit size for slowdowns, plus whether the same nearby fight is smooth in RTS mode.
- For a crash, any error message or exception/module information you have.

The optional [crash recorder](../tools/diagnostics/README.md) can collect more detail. Full dumps and input recordings can contain private data, so review them before sharing. The [contributing guide](../CONTRIBUTING.md) has more reporting detail.
