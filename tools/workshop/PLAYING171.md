# Shogun 2 Unit Possession — Test171 compatibility correction

Test170 missed a separate stock-only engine check in the active playable QTE
worker. Test171 corrects that check. The exact stock and reviewed ModDB bugfix
profiles, occupied-hook refusal, PFH2/PFH3 timestamp support, custom-unit HUD,
native resource preparation and existing gameplay are retained.

The compiled QTE DLL is checked in an isolated helper with no movement adapter:
both reviewed engine files must pass the real worker's engine gate and stop at
the missing-adapter condition before any hooks, renderer or engine function.
These startup checks are not combined gameplay acceptance.

The local trial retains the launcher filename shogun2_unit_possession_test170
to preserve the owner's selection; its embedded runtime is now Test171.
Keep older possession/checkpoint entries off. Restart after this replacement.
F9 possesses, P changes perspective and F10 returns to RTS. Other controls and
known charging/climbing, clipping, crowd-performance and crash limitations are
unchanged. Naval possession and multiplayer remain unavailable.

Required resources come from the player's owned game. The third-party engine
is not bundled in this pack. No recorder, debugger or camera chooser is started.
Unknown engine builds, occupied hooks and unknown pack layouts still refuse.
Report the exact startup error if initialization fails.
