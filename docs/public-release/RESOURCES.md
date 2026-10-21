# Local resources

The owner approved a source/local-resource kit. It prepares each builder's owned supported game resources, without asserting redistribution permission.

resource-recipe.json pins 592 original UI entries, 20 CUF fonts and two artillery images by pack path/hash and sprite geometry. audio109.json pins the English advisor entry/crop. The bounded PFH3 reader rejects unsafe/duplicate entries, malformed indexes and changed contents; it never writes packs.

ImageMagick decodes TGA, records transparent trim coordinates and adds two pixels of border. Free Texture Packer CLI exports PNG+JSON atlases. The importer combines offsets and validates bounds, no rotation, complete coverage and original logical dimensions. Fully transparent sprites retain the accepted padded representation. Raw and trimmed PNGs are preserved.

594 PNGs comprise 592 originals plus two explicitly approved native aliases for status_firing/status_melee. Historical AI-restored variants stay in the private accepted build; they are neither public kit inputs nor silently claimed to be originals. No placeholders are introduced.

Fonts are byte-exact; artillery pixels use RGBA hash checks. FFmpeg crops 3.25–4.80 seconds to the accepted 1.55-second PCM “Shameful display!” WAV, excluding the preceding sentence. Bow animation audio is untouched.

The manifest records tools, inputs, outputs, atlas metadata and hashes. Prepared headers/RC files go under the chosen output and into an isolated source snapshot. No private prepared tree or old installer is used. Commands are in [BUILD_AND_TEST.md](../../BUILD_AND_TEST.md). Keep failed output for diagnosis and retry in a new directory. Never commit prepared assets or local binaries.
