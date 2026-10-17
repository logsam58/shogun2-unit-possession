# Local build dependencies

A source clone contains mod code and fixtures, not a complete game/resource distribution.

| Dependency | Needed for | How it is supplied |
| --- | --- | --- |
| Python 3.10+ | Build/configuration, diagnostics and format tests | Local Python installation |
| MSVC x86 and Windows SDK | Native DLLs, HUD, loader and setup | Visual Studio / Build Tools; configure vcvars32.bat or automatic discovery |
| Supported Empire.Retail.dll | Exact native fixture execution and installation compatibility | Licensed local SHOGUN 2 installation; fingerprint in README |
| 24 UI input files | Styled QTE/HUD resources | Separately prepared resource tree matching tools/local-assets.json |
| Prior loader/marker/runtime files | Full installer upgrade/rollback regression suite | Separately retained historical fixture inputs; the immediate predecessor pair and later development predecessors |
| Indexed native decoding | Research of native functions and structures | Optional read-only local database/exports; not required for ordinary compilation |
| Diagnostic runtime/configuration | Automatic crash capture | Separate local setup of recorder source and x86 debugger |

`tools/configure.py` writes ignored tools/local.json and verifies resource hashes before copying inputs into ignored native paths. The asset source must have the relative structure described by `source_relative` in tools/local-assets.json. It is a prepared UI resource closure, not the plain Steam game folder. The helper does not extract arbitrary packs or download proprietary assets. A fresh contributor currently needs that prepared input closure to build the full UI; a standalone extraction pipeline is not included.

The adapter target can be built without the UI inputs. Exact-native fixtures still need the supported engine. The full installer suite additionally needs historical fixture binaries; normal source browsing and building the adapter do not require those archived installer inputs.

Keep absolute paths and machine settings in ignored tools/local.json. Do not commit game binaries, fonts/artwork, complete research exports, local runtimes, raw recordings or dumps. See [BUILD_AND_TEST.md](../BUILD_AND_TEST.md) for commands and current path assumptions.
