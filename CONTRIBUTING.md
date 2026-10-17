# Contributing

This project modifies SHOGUN 2's native battle behavior. Start with the [README](README.md), [source map](SOURCE_MAP.md), [architecture](docs/ARCHITECTURE.md) and [build guide](BUILD_AND_TEST.md). The game remains responsible for the battle simulation; changes should preserve that relationship and the independent behavior of unpossessed soldiers.

## Development workflow

1. Create a focused branch from main and configure local build dependencies.
2. Find the active component in SOURCE_MAP.md. Versioned directory names do not mean a component is obsolete.
3. Make the smallest coherent change. Preserve ownership/lifetime validation, native forwarding for unrelated actors and compatible numeric protocols.
4. Build and run the affected offline fixtures. For hook changes, include original-native forwarding, invalid identity, cleanup and restoration cases where relevant.
5. Review the diff, then run `py tools/audit_repository.py` against the staged files before pushing.
6. Open a pull request explaining the observable behavior, why it changes, checks actually run and unresolved limitations.

Keep generated binaries, extracted game resources, local configuration and raw recordings/dumps out of Git. Do not update docs/source-import.json to make intentional edits appear byte-identical to the original import. No project-wide license has been declared; preserve existing third-party notices.

## Useful bug reports

Include the mod version, game/expansion, battle type and map, unit and weapon/ammunition, other mods, reproduction steps, expected behavior and actual behavior. For possession or combat problems, describe whether the soldier was mounted, climbing, reloading, routing, dying or entering a paired animation. For timing issues, include battle speed, pause state, refresh rate and whether the game itself slowed down.

For a crash, provide the exception code/module/offset if available and distinguish recovered exceptions from the final unhandled crash. Native module attribution alone does not prove a game-only cause. Start with a small sanitized report; a full dump can contain private data and should not be attached publicly without review. See the [diagnostics guide](tools/diagnostics/README.md).

## Live testing

A new native DLL needs a closed-game installation and a fresh launch. Keep an exact rollback of managed files. Record the built and loaded component hashes, engine hash, unit/map and tested transitions. When coordinating an instrumented session, confirm capture and loaded state before directing gameplay. Do not gate automatic capture on pause, READY or mod availability.

Record scoped results such as 'climbed and landed on this map with this infantry unit.' Avoid treating one successful session or synthetic fixture as proof of universal compatibility. Existing known issues remain open until evidence addresses them.
