# Workshop candidate: Test165

This is a private Workshop test build. The owner accepted both the local pack
and the actual subscribed build. Steam downloaded the prepared pack exactly,
and the game loaded it directly from the Workshop folder without the older
DInput mod loader or a local copy in the game data folder. Fresh loaded hashes,
both status channels and17 core hook destinations passed their checks.

On the first local test, the pack prepared artwork, unit cards, ability icons,
fonts and cropped voice from the owned game; all615 source entries passed
hash checks. The subscribed launch validated that cache. Recording and the
debugger stayed off at the owner's request. Second-machine installation and
extended stability have not been tested.

[Private Workshop item](https://steamcommunity.com/sharedfiles/filedetails/?id=3825625050).
The owner reports its visibility is Hidden. This page records the earlier
Test165 build; v0.32 now publishes the accepted Test166 update on GitHub without
changing visibility. The uploaded archive adds the license notices and controls
inside the pack; its two original entries, including all compiled Lua/native
code, are byte-exact from the accepted local Test165.

The player steps are subscribe, enable the mod, and launch through
Steam. There is no setup program, launch-option edit, external script or camera
chooser. Possession starts in third person; P switches between first and third
person. F9 possesses a selected friendly soldier and F10 returns to RTS.

On first launch, a hidden copy of the existing resource preparer reads the
player's own game. It prepares the HUD artwork, unit cards, ability icons,
fonts and cropped advisor voice in the user's cache. The original front-end UI
script is also read from that game. None of those game resources are in the
download. Later launches reuse validated local files. Steam configuration and
the game installation are not changed by the runtime.

The gameplay DLLs and HUD are the exact tested local-resource components from
Test163. Movement, combat, orders and camera code have not been rebuilt for
this candidate. Charging, climbing, clipping, crashes and very large battles
retain their known limitations. Naval possession and multiplayer are unsupported.

## Build

Use Windows, Python3.12, MSVC x86 with the Windows SDK, and a matching x86
Lua5.1 interpreter/compiler configured with float numbers. Its compiled chunk
header must be `1b4c75615100010404040400`. Lua's ordinary double-number compiler
does not produce compatible bytecode. These are developer dependencies only.

First build the normal local-resource native components using BUILD_AND_TEST.md.
Then run from this source folder, using new output folders:

    py -3.12 tools/workshop/build_workshop165.py --game "D:\Steam Library\steamapps\common\Total War SHOGUN 2" --components "D:\Shogun Mod Build\native\bin" --output "D:\Shogun Mod Build\workshop" --vcvars "C:\BuildTools\VC\Auxiliary\Build\vcvars32.bat" --lua "C:\Tools\Lua51Float\lua51.exe" --luac "C:\Tools\Lua51Float\luac51.exe"

The distributable folder is `package`. Keep the build's other folders private:
they contain locally recovered scripts and generated game resources for tests.
The pack uses canonical backslash paths and a nonzero timestamp inherited from
the owned data.pack. A zero timestamp caused the native archive reader to return
an empty stream in the controlled startup comparison.

## Native boundary and checks

The first-party Lua wrapper extracts only its own DLL through exclusive staging,
verifies every byte and commits without replacing an existing target. The
supported game's CRT has an explicit exclusive `x` mode handler. The native
bridge reads the original script using bounded uint64 pack offsets: game Lua
uses float numbers, so ordinary Lua arithmetic cannot represent large offsets
exactly. Script cache writes are staged and existing differing files are kept.

The bridge runs on the calling Lua thread. Its only additional engine function
is the pinned, read-only `lua_gettop` at RVAea2d60: cdecl int(lua_State*), with
all14 original bytes checked after the engine hash gate. Both entries require
one argument; the wrapper passes true and a successful entry returns that same
existing Lua stack value. There are no new actor/world calls or gameplay hooks.
The native module is pinned before the original script runs. Gameplay starts
once in the existing worker, after original UI initialization.

Fresh checks exercise actual loadlib under matching float Lua, all37 original
callbacks and their environments, repeated initialization, invalid arity,
production non-game rejection, sparse offsets above4GiB, malformed input,
foreign-cache preservation, actual automatic resource preparation and exact
resource comparison, repeat cache use, restricted child handles, output capture
and termination of an owned timed-out toy child. No game process is terminated.
PE resources are checked for the four exact first-party component binaries only.

The adapter97, QTE50, HUD8 and resource-renderer results are retained byte-exact
evidence from the component build, not newly rerun gameplay suites. Test164's
successful inert entry is separate from Test165 gameplay acceptance. Live tests
must verify this pack's hashes and actual loaded components. Verify ordinary
recording when enabled; this test's owner explicitly chose to leave it off.

The operator's reversible staging tools are private test helpers, not player
installation steps. Disabling or updating an active native mod requires a fresh
game process; cached files alone do not start it.

This resource audit is not legal clearance. Existing game/vendor terms and
the project's notices still apply.

References: [Lua5.1 C function results](https://www.lua.org/manual/5.1/manual.html#lua_CFunction),
[Microsoft fopen modes](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-wfopen),
[CA's Workshop guide](https://wiki.totalwar.com/w/Steam_Workshop_General_Usage_Guide_(TWS2).html).
