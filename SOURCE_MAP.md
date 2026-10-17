# Maintained source map

Legacy directory names identify component origins, not the latest product version. All paths below contain the active Test99 source closure unless described otherwise.

| Component | Repository path |
| --- | --- |
| Possession adapter, movement, accuracy, RTS return, ranged/look, siege/landing and50 fixtures | native/outputs/shogun2-accuracy90-preview/src |
| Active styled QTE runtime and its fixtures | native/outputs/shogun2-authentic-hud80-preview/qte-src |
| Active QTE renderer and resource definitions | native/outputs/shogun2-authentic-hud80-preview/src |
| Active HUD and state policy fixture | native/outputs/shogun2-ammunition83-preview/hud-src |
| Equipment/ammunition protocol headers | native/outputs/shogun2-ammunition83-preview/src |
| Shared identity/status and MinHook | native/outputs/shogun2-possession-cleanup-v46 |
| Adapter's retained MinHook closure | native/outputs/shogun2-accuracy90-preview/vendor |
| Legacy protocol dependencies and HUD core | native/outputs/shogun2-possession-qte55-preview, shogun2-melee-qte63-preview, shogun2-melee-meter79-preview |
| Integrated loader/startup and payload/hash preparation | native/work/autoload99-preview |
| Wizard, reversible install/rollback, retention and HKCU fixtures | native/work/installer-siege99 |
| Predecessor cache catalog used to generate current build catalog | native/work/installer-siege98/cache_catalog.h |
| Automatic crash recorder and corrected full-memory dump reader | tools/diagnostics/source |
| Portable x86 build wrapper / local dependency importer | tools/build.py, tools/configure.py |
| Byte-preserved import provenance | docs/source-import.json |

The redundant historical adapter mirror under melee-meter79 was not imported as a build target. Its one HUD core dependency is retained. This avoids presenting that incomplete historical mirror as the maintained adapter. The active adapter is the accuracy90-preview directory above.

Game UI pixels/fonts, game binaries, complete shared decoding/decompilation and raw evidence remain external. Machine-specific pointers are in ignored tools/local.json or the local project state. docs/LOCAL_DEPENDENCIES.md explains the original machine's retained locations without copying their contents into Git.
