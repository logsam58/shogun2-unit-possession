# Contributing

Start with the [build guide](BUILD_AND_TEST.md), [source map](SOURCE_MAP.md) and [native contracts](docs/public-release/NATIVE_CONTRACTS.md). Work on a branch; describe the trigger, change, meaningful checks, untested boundaries and gameplay risks.

First-party contributions use the root MIT license. Submit only work you may license that way. Identify borrowed material and preserve notices. Do not add game binaries, extracted art/fonts/audio, full decompilations, private dumps, credentials or personal configuration. MIT does not cover game material or replace dependency licenses.

Preserve native mechanics and ownership guards. Record image hash, RVA, ABI, bytes and evidence for native changes; never infer a structure merely from a similar class. Forward unrelated native calls.

Protocol changes require coordinated producers/consumers and negative identity tests. Role changes require adapter/HUD/diagnostic coverage. Installer changes require killed-process recovery tests. Run relevant current checks; unavailable optional history is a skip.

Imports, help, configuration and builds must not install, launch or activate recording. Real gameplay, installation and publication remain explicit operator steps. See [releasing](docs/public-release/RELEASING.md). Contributors need no private archives or original operator machine.
