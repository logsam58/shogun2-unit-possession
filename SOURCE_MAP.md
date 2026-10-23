# Source map

[Current status](docs/current-release.json) is authoritative. Directory suffixes are historical implementation names.

| Component | Active source | Responsibility |
| --- | --- | --- |
| Loader | native/work/autoload99-preview | DInput forwarding, payload checks, inherited camera default, versioned cache, delayed startup and game-process callout; launch161.cpp shows the chooser before starting the Steam game command |
| Adapter | native/outputs/shogun2-accuracy90-preview/src | Actor lease, native movement/weapons/orders/camera/charge/climbing |
| Shared | native/outputs/shogun2-possession-cleanup-v46/src and native/common | Identity, protocols, visibility and final-human roles |
| QTE | native/outputs/shogun2-authentic-hud80-preview/qte-src | Native pairs, response/input timing, animation camera |
| HUD | native/outputs/shogun2-ammunition83-preview/hud-src | Read-only measurement, artwork and controls; friendly-fire playback belongs to the game loader worker |
| Installer | native/work/installer-siege99 | Fingerprints, durable journals, repair, conservative retention |
| Player setup/local assets | tools/installer, native/common/local_assets159.h | Inno wizard, read-only native extraction, six-target rollback, external resources; steam_launch162/steam_session163 manage the owned Steam connection and normal install-time client lifecycle |
| Diagnostics | tools/diagnostics/source | Optional external capture, telemetry and dump inspection |
| Build | tools/build.py, build_frontend.py, native_build.py | Isolated snapshot, explicit inputs, native checks |
| Resources | tools/prepare_resources.py, game_packs.py, resource-recipe.json | Owned-game extraction/trim/packing/fonts/icons/voice |
| Derived records | tools/data, generate_tables.py, generate_roles.py | Canonical observations and repeatable headers |

Build-generated hashes belong in the build snapshot. See [native contracts](docs/public-release/NATIVE_CONTRACTS.md) and [derived data](docs/public-release/DERIVED_DATA.md).

Versioned docs/development and older docs/releases are historical evidence, excluded from the curated source kit; current v0.31 release notes are included. They can describe earlier machines/limits and do not override current instructions. The original implementation/research remains in baseline commit 24d31a2f5e5c2cf84a42c459a69bdcd9bd86c600.
