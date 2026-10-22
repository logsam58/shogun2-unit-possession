# Local game resources

Test159 moves extraction to the player's installation. The setup download contains no Shogun images, unit cards, ability icons, CUF fonts or advisor audio. No placeholders replace them.

`prepare-local-assets159.exe` runs outside the game during setup. It checks the supported engine, reads `data.pack` and `local_en.pack` without changing them, and verifies all 615 original entries against metadata-only hashes. Its bounded PFH3/TGA reader builds the accepted HUD and QTE atlas layout, copies 20 fonts, prepares native ability/status icons and crops the English advisor voice to 3.25–4.80 seconds. Windows Media Foundation decodes the MP3; no Python, FFmpeg, ImageMagick, Node or compiler is needed by players. Windows N requires the Media Feature Pack.

Two data files are prepared in temporary staging before deployment: `hud-assets159.s2fp` and `qte-assets159.s2fp`. They are installed under `data/shogun2_possession_local_assets159`. The loader validates them before loading gameplay hooks, then copies them atomically to its distinct runtime cache. HUD/QTE use read-only mapped files with the existing rendering and font code. There is no embedded asset fallback or new per-frame extraction.

Each file binds its format/kind, extraction recipe, IDs, dimensions, sizes and payload hashes. Missing/corrupt/foreign files fail before mod initialization. Existing changed cache files are preserved; an operator must inspect them rather than deleting unfamiliar caches. Setup/uninstall includes the two local files in its identified ownership/rollback checks. Generated resources stay on the player's computer and must not be uploaded as release assets.

The independent developer preparation pipeline remains available in `tools/prepare_resources.py`: ImageMagick TGA/trim, Free Texture Packer PNG+JSON and FFmpeg crop. Its 594 sprites are 592 originals plus two approved native aliases. It provides the comparison/reference and metadata regeneration path. Historical AI-restored icons are not inputs. Fonts compare byte for byte; visible atlas/icon pixels compare exactly. Windows and FFmpeg audio decoding differ slightly in sample alignment/rounding, so the audio comparison verifies duration and PCM correlation rather than claiming identical WAV bytes. Bow animation audio is untouched.

Read [build commands](../../BUILD_AND_TEST.md), [setup behavior](STANDARD_SETUP.md), and [license scope](PROVENANCE.md). Code licensing does not grant ownership of the game resources prepared locally.
