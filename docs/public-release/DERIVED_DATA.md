# Derived data and evidence

There are three different operations. Do not describe retained observations or old release recovery as new game extraction.

| Output | Recreate / validate | Origin and limit |
| --- | --- | --- |
| UI atlas PNG/JSON, sprite headers, fonts, artillery icons, voice WAV and RC files | tools/prepare_resources.py; recipe hashes, geometry, RGBA and exact font/audio checks | Recreated from the supported owned game using declared public tools; no old prepared tree |
| Python role constants | tools/generate_roles.py, then --check | Generated from native/common/soldier_roles.h; deliberate roster changes require cross-component negatives |
| Unit/faction/ability/weapon catalogs, rig definitions and saved palette header | tools/generate_tables.py, then --check | Six headers rendered from tools/data/derived-records.json, which preserves the baseline factual records and exact expected text hashes |
| Hook/ABI/offset/lock index | tools/generate_native_inventory.py, then --check | Re-indexes current C++ source; an index is not proof of inferred semantics |
| Embedded hashes, marker pack and cache catalog | tools/build.py all | Generated only in the isolated snapshot from freshly built payloads and retained predecessor fingerprints |

The canonical tables retain observations from baseline 24d31a2: catalog keys/labels/art references; human/alternate/horse rig names/parents; and finite saved original x86 sampler outputs used as golden regression vectors. Formatting those records into a header is reproducible. It does not rerun the original decoder or resample animations. The original sampler capture inputs/backend are not fully present in this public kit; no claim of fresh game-to-palette sampling is made.

The golden palettes are test observations, not runtime animation assets. Current runtime applies verified rig matching and refuses changed layouts. New engine/rig/clip support requires a new documented capture procedure, legitimate inputs, native byte evidence and reviewed golden vectors. Do not change expected values merely to make a regression pass.

Factual compatibility records and any game-derived content are excluded from the first-party MIT grant to the extent third-party rights apply. No game executable bytes, decompiled implementation, image pixels, font programs or audio samples are in these JSON records. Assets are prepared locally. See provenance.

Historical sprite headers remain as baseline evidence; the current local-resource build overlays regenerated ones. The former prepared-asset import manifest is legacy migration evidence and is not a mandatory input.
