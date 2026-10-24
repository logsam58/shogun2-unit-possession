# Resource extraction and historical Windows setup

The current v0.32 player route is Workshop. This directory also contains shared
code used by that route: `prepare_local_assets159.cpp`, its recipe generator,
asset-format/renderer checks, and the data-only PE resource auditor. See the
[current build guide](../../BUILD_AND_TEST.md).

The Inno wizard, Steam launch-option editor and Steam-client lifecycle fixtures
belong to older setup builds. They are retained for compatibility and recovery
tests, not required to play or build the Workshop pack.

The historical Inno Setup wrapper packages an existing, hash-verified mod build.
It does not compile gameplay code. Its [packaging contract and validation](../../docs/public-release/STANDARD_SETUP.md)
describe the earlier installer route.
