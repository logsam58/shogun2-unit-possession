# Dependencies

Windows x86 native code is built on Windows x64. The current Workshop target needs Python, MSVC/SDK and an x86 Lua 5.1 interpreter/compiler using float numbers (chunk header `1b4c75615100010404040400`). The additional image/atlas/audio tools below are for the independent resource-reference pipeline, not routine Workshop extraction. Inno Setup is needed only for the older installer targets. See [build commands](../BUILD_AND_TEST.md).

Tested resource-reference prerequisites:

| Tool | Version | Obtain separately |
| --- | --- | --- |
| CPython | 3.12.10 x64, standard library | https://www.python.org/downloads/release/python-31210/ |
| MSVC / SDK | VS 2022 17.14, x86 toolset 14.44, Windows SDK | https://visualstudio.microsoft.com/downloads/ |
| ImageMagick | 7.1.2-24 Q16-HDRI x64 | https://imagemagick.org/script/download.php |
| Node / npm | 24.16.0 / 11.13.0 | https://nodejs.org/en/download |
| Free Texture Packer CLI / core | 0.3.0 / 0.3.5 | tools/assets/package-lock.json; npm ci --ignore-scripts |
| FFmpeg | 9.0.2 essentials | https://ffmpeg.org/download.html |

No tools/runtimes are bundled. Python requires 3.11+ for file_digest; 3.12.10 is tested. ImageMagick's exact version is checked; npm transitive versions are locked and scripts disabled. Compiler drift can change binary bytes.

Supply legitimate supported Empire.Retail.dll, mss32.dll, installscript.vdf and data/data_fots plus compatible language packs. English voice is optional
for Workshop; the independent resource-reference pipeline has stricter stock inputs. DLC/language differences can fail exact resource checks and need review before support. See [resources](public-release/RESOURCES.md).

Recorder providers include Windows performance counters and PowerShell; NVIDIA GPU detail additionally uses nvidia-smi on PATH/System32. Missing providers are explicit, not zero usage. Optional history/decompiler research is not required for current build/tests. No private runtime, prepared resource archive or real dump is required.
