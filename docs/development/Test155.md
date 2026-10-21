# Test155 — dismount and wall crash corrections

Test154 orbit and scrolling are accepted by the player. Dismounting stalled, and a subsequent wall-climb attempt crashed. This development build corrects two concrete errors found in that recording. Published v0.27 is unchanged.

## Recorded failures

Session `20261021-133502-pid22472` retained the unhandled fault at native `3b2478`: the navigation surface argument was `0x2A9`, a bearing. The mod built a bearing pose with `351e40`, then passed it to a path function expecting a different 20-byte navigation location. The earlier wall fixture replaced the planner before this validation and missed it.

Two preceding first-chance faults at adapter `2f0db` read address `1` during dismount. The mod treated unit `ba8` as a pointer; original `14aca0` establishes it is the count of inline task slots `ba0/ba4`. A 22.719-second log gap follows the mount order. This supports a concrete fault correction; it does not establish that every possible stall shares that cause.

## Change

F now obtains the complete native navigation location from the actual ground/surface portal getter `397e40/397e80`. Navigation surface and coordinate frame stay distinct. Endpoint exit also passes the native navigation surface. Native path planning, alignment, mounting, W/S rates, crest and landing continue through the existing native functions. No forced heading, position or animation.

Mount continuity now reads the bounded native task slots, validates task type/environment/unit, and tracks the actual task identity through the same-soldier lease transition. Original M order, native restrictions and animations remain. Accepted P, wheel and MMB orbit are byte-exact.

## Verification and limits

See `Test155-validation.json` and local `artifacts/Test155`. The strengthened wall fixture runs the original portal getter, location constructor, request builder and cached-route lookup that faulted. The mount fixture executes original task registration and both transition directions. Reintroducing either defect makes its regression fail at the corresponding native fault.

The preserved on-demand decoder ran for 19 mount/wall assets across 20 declarations. Twenty fresh read-only Ghidra instruction exports were checked against the supported retail engine. Native terrain, asynchronous route solving and rendered animation outcomes require a fresh live test. Missing routes, other crashes, isolated enemy acquisition, POV issues and Ultra crowd performance remain open.

Before play: fresh READY or IN BATTLE, exact Test155 loaded hashes/hooks and actual crash/input/memory/CPU/GPU recording. No automatic game launch or extra profiling. Exact Test154 rollback and independent installed readback are required for deployment. No publication is requested.
