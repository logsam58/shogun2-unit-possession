# Naval movement correction, private Revision 129

Revision128 entry/return works in the owner's battle, but movement was reported
as stuttering and sideways while pressing W with the camera facing across the
sailor. The bounded, read-only 30-second capture observed 594 owned samples,
36 submitted requests, 37 rejected projections, and about 10 position changes
per second against about 110 camera callbacks per second. This confirms the
installed128 identity; it does not validate129 gameplay.

## Verified native paths

Ghidra exports and original bytes are preserved privately under
D:/Shogun2Deck/promotion/naval127-research-1/movement129-*.

- 3c1d90 -> 366a10 -> 367600: sailor physics uses the same movement integrator
  as land actors. 366a10 computes locomotion from actual velocity afterwards.
- 3936f0 (ECX actor, XZ pointer, heading; RET8) sets displacement, turn delta,
  magnitude and animation-drive flag without writing actor position.
- 3e3d20 (ECX walking context, gait; RET4) updates actor244 walk/run variant.
- 3e3f50 retains native state/rig transitions and returns early for an equal
  state. Generic locomotion state0 is valid for the eligible deck sailors.
- 3d9320 destroys/replaces the lower walking context.128 called it repeatedly
  for directions/yaw/time.129 submits one stationary native context and then
  supplies direct intent before the existing integrator; no recurring paths.
- 367600 retains its native collision/surface processing and 3a8900 moving-ship
  transport. Our code does not teleport the actor or change ship orders.
- 25b360/2a02a0: scene68's ship view array is capacity38/count3c/rows40,
  indexed ship serial minus one. 26c1f0 verifies view70's owner ship.
- 49d240: holder08 is actor; 49ce80 uses15e9e00 with4cd620 at vtable04.
  4cda80 (ECX holder, matrix pointer; RET4) requests native renderer interpolation
  through holder14/ec and copies its rendered matrix. Translation is03/07/11.
 129 camera uses this pose each frame, with current view/actor ownership checks.
 A temporarily absent render holder falls back to the last verified position.

## Scope and checks

WASD and facing share land's steering_view/face_view. Diagonals are normalized;
walk/run and maximum turning rate come from this sailor's native configuration.
Native priority work, death, traversal and lost station ownership end the lease.
Only the original current duty restarts; no old task/roster/station bytes restore.

Stock and reviewed Bugfix fixtures execute production code and native functions:
40 heading/direction/pace combinations,200 uninterrupted input changes,stop,
ship transport/simple-body integration,120 sub-tick rendered poses,stale-holder
rejection,peer isolation,death/traversal/focus/retirement,100 same-sailor duty
returns,100 acquisitions,plus native camera-vtable dispatch and exact unhook.
Collision broadphase, actual ship deck pathfinding and visual gameplay are NOT
proven by these synthetic worlds. No additional live capture is running.

PC single-player normal sailor movement/camera/duty return only. Personal naval
weapons, boarding, crew switching, naval HUD and Steam Deck naval controls remain
unimplemented. Land gameplay is retained.The owner subsequently accepted the scoped movement test described below.

## Owner acceptance and branch placement (2026-11-10)

The owner reported that the stutter was gone, directions were correct, movement
remained clean while the ship moved, and possession -> release -> possession
worked. This is scoped manual gameplay acceptance, not proof of all ship types,
naval weapons or long-term stability. No new loaded-hash inspection was performed
for that report. The locally staged Revision129 pack was independently verified.

Revision129 is preserved on `testing`; stable `main` remains the exact Revision126
source at897bc56411df42135c982cc4fa1cab04c3031964. Steam's installed download for
item3825625050 is byte-identical to the Revision126 pack. The Testing branch
launcher package reuses the accepted129 bytes under its existing separate name.
Only one possession entry should be enabled. No Workshop publication is performed
by saving this source branch. The experiment is parked pending further owner work.
