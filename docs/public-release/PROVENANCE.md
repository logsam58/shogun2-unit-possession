# License scope and provenance

The owner explicitly selected MIT for first-party mod code on 2026-10-21. The root LICENSE applies to that code and its first-party documentation. It does not grant rights over the game or change a dependency's license. Contributors must have permission to submit their work under the same first-party terms and identify any third-party content.

| Material | Origin and transformation | Terms / distribution status |
| --- | --- | --- |
| Mod C++, Python, tests and contributor documentation | First-party project implementation; native addresses and recovered observations are documented as evidence, not original engine source | MIT, except material separately identified below |
| MinHook and HDE sources under vendor directories | Vendored upstream source, with existing copyright/license files | Retain the upstream BSD-style notices, including binary-distribution notices; not relicensed |
| Original engine, Miles library, packs and decoded game code | User-supplied legitimate supported game installation; used read-only for exact-native fixtures and resource preparation | Not included in public source/payloads; no redistribution permission asserted |
| CUF fonts, UI sprites, ability icons and cropped advisor voice | Original local game packs; decoded/trimmed/packed or cropped for local use | Game-derived; outside MIT; public binary redistribution remains unresolved |
| Sprite placement, unit-art and native palette/animation tables | Generated observations/data from original game material and prior preparation | Outside the first-party license grant to the extent they contain third-party material; regeneration and distribution must be documented individually |
| Two historical restored status icons | Prior AI-assisted reconstructions used by the accepted HUD, named status_firing-hd-v1 and status_melee-hd-v1 | Originals/manifests recovered; owner approved original native icon aliases for this public kit only. Historical restored pixels remain private and unchanged; excluded from public resources |
| Offline fixture providers and test-generated data | First-party synthetic layouts; original instructions are loaded from the user's fingerprint-checked engine where required | Synthetic first-party code MIT; do not bundle the original engine or private dumps |
| Public build tools | Python, MSVC/Windows SDK, ImageMagick, Node/npm, texture packer and FFmpeg obtained separately | Tool licenses remain their own; record exact tested versions and keep notices if ever redistributing tools |
| Private session logs, dumps, profiles, recorder baselines and historical local payload archives | Operator evidence | Excluded from public source/payloads; not prerequisites for mandatory current tests |

MIT was selected for straightforward community reuse, including permissive reuse with notices. See the [MIT text](https://opensource.org/license/mit) and [GitHub's license summary](https://choosealicense.com/licenses/mit/). No project-wide relicensing of third-party/game material is intended.

Public packaging must exclude unresolved derived resources, even when they are embedded in an executable rather than present as loose files. Local reproduction and authorization to redistribute are separate questions. A code release is not a grant from Creative Assembly/SEGA.

Owner approved local-resource source kit and native status icon profile on 2026-10-21. No public prebuilt resource redistribution is approved or inferred. The prepared native aliases deliberately differ from the private restored status artwork; all other native sprite geometry is checked.

Test159 implements the code-only player route: setup prepares the original resources locally from each player’s copy, using a native read-only extractor. Runtime binaries and setup contain metadata and code rather than game pixels/fonts/audio. Older embedded-resource installers and the ZIPs containing them were subsequently deleted from GitHub by explicit owner request; verified private local copies are retained. Their source archives and notices remain. The owner authorized v0.31 using this code-only route inside the private repository; a repository visibility change remains a separate decision.

Test165/166 extend the same local-resource method to Workshop. Its first-party Lua/native bootstrap reads the original UI script and prepares artwork, unit cards, ability icons, fonts and the cropped voice locally. Those game bytes are absent from the pack and GitHub player/source archives. v0.32 used that accepted Workshop build; v0.33 packages Test173 with the same local-resource method. The old setup route is historical.

Removing bundled resources does not establish legal clearance for native DLL hooks or reverse-engineering work. CA documents [Shogun 2 mod support](https://wiki.totalwar.com/w/Official_CA_modding_tips_and_tutorials), while the [game EULA, section 5(e)](https://store.steampowered.com/eula/eula_shogun2) restricts reverse engineering/modification subject to applicable-law exceptions. No specific SEGA/CA permission for this implementation has been obtained. The owner has since made the repository public; this release does not change visibility.
