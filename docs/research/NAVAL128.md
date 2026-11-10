# Private naval Revision 128: camera integration correction

The owner's Revision127 PC attempt failed. The loaded pack and adapter hashes
matched the staged candidate. A bounded read-only capture found all 100 sailors
on the selected ship passed the entry predicates, while retained control state
showed seven navigation requests, one native duty return, and a pending camera
restoration. The shown pose was zero: the camera helper never ran.

## Verified cause and correction

Revision127 attached naval view work to the land/context matrix setter134fad0.
Normal overhead naval rendering instead runs manager3268e0 through the current
camera's vtable+13c, which resolves to3290a0 for overhead type15d1a74. That
thiscall frame accepts two floats and returns a boolean in AL, RET8. The native
render accessors324f20 and325010 read interpolated eye184 and target19c, while
1374c60 interpolates each three-float current vector toward the next three
floats. Updating only raw eye8/target14 would not publish the correct view.

Revision128 adds an exact-guarded3290a0 hook. Unowned cameras pass straight to
the original. The owned overhead numeric pose and interpolation vectors are
restored before the original update, so its terrain/altitude constraints do not
operate on the possessed view. Afterward, the sailor view updates both current
and destination vectors and the native134fe50 basis. No camera object, manager
pointer, actor position, ship order, roster or station ownership is replaced.
Release restores the captured RTS pose and smoothing state. An unapplied or
retired camera cannot leave the entry barrier permanently armed. A genuine
pending return is reported separately from unavailable crew.

## Evidence and validation scope

Private Ghidra evidence is under naval127-research-1/camera128*, including
original instructions, call relationships and both camera tables. All exported
bytes were compared with the hash-pinned stock and reviewed Bugfix images.
The new fixture enters through the native vtable and actual hooked3290a0 body,
executes native interpolation and render getters, and checks return/reentry.
Logging, terrain/environment services and the clock use explicit fixture
providers. Existing task fixtures still exercise real dispatch, request
construction and retained-duty return with synthetic navigation providers.

These checks do not prove live camera rendering, pathfinding, moving-ship
behavior, combat or gameplay acceptance. The next owner trial is still needed.
The first test remains PC single-player sailor movement/camera/native duty
return only; personal naval weapons, boarding, Deck controls and HUD are not
implemented. Revision126 rollback and the failed127 artifacts are preserved.

## Relocated-engine startup regression found before staging

The first full package failed the existing ASLR contract test at3290a0.
Its first instructions contain HIGHLOW operands at+7 and+14, the latter
crossing the old16-byte boundary. Windows relocation changes these address
bytes legitimately. Both owned engine relocation tables agree exactly.

The contract generator now records the precise HIGHLOW offsets, extends the
prefix to include complete operands (18 bytes for this hook), and hashes every
byte after subtracting only the load-address delta from those recorded words.
No opcode or operand is ignored. Other prefixes retain their16-byte checks.
Microsoft's PE specification documents these32-bit fixups:
https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#base-relocation-types

The isolated contract test passes69 cases on each relocated stock/reviewed
engine, including mutation of every byte of the new prefix and exact restore.
The initial failed package and original verifier are retained privately. Full
regeneration and a fresh final package are required before staging.

## Final candidate status

Authoritative package: D:/Shogun2Deck/promotion/naval-crew128-final-aslr.
Pack SHA256601f6ea87e11e9f7365d261926be82164be9e4d52a6b3e7118071bc934c2f94b,
4676068 bytes. Full Ghidra-based contract regeneration matched exactly. Fresh
100 adapter,51 QTE,8 HUD test programs, package startup/resource checks,
8 compiled campaign and8 owned-reader cases passed. Native sailor16 cases and
camera7 cases (including100 reacquisitions) passed on both supported engines;
relocated-engine contract69 cases passed on each. The old127 camera header
fails the new camera-dispatch regression as expected.

Exact127 rollback was rehearsed, including refusal of changed targets, occupied
temporary files and altered backups. The accepted126 pack is also preserved.
At the final readiness check, PC Shogun2 PID20016 and HUD15660 still ran, and
the installed pack remained exact127. Revision128 is therefore READY BUT NOT
INSTALLED. No live128 acceptance, Deck transfer, Workshop upload or GitHub push.
Only install after the receiving game and launcher are closed. The earlier
naval-crew128-final folder contains the failed ASLR candidate and must not be staged.
