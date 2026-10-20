# Test131: native movement, head-look and unit controls

This is the first checkpoint of the requested possession improvements, based on accepted v0.23/Test130. It is a development candidate, not a new published release. In-game acceptance is pending.

- Possessed movement now follows the native soldier calculation's global walk/run coefficients as well as the current configuration and live unit modifier. The modifier is applied once. This corrects a source-level difference; it does not yet establish why Rapid Advance appeared doubled in the reported battle.
- Head-look uses an 80-degree limit to either side, 70 degrees up and 60 down, with a shared 360-degree-per-second mouse rotation budget. The body still turns at its native rate. Native QTE entry orientation remains in control until the established animation lock releases.
- During Q commands, proven enemy hits take priority over foreground allied soldiers, including other allied armies. Native banner metadata and world hits remain intact. Ordinary RTS selection and friendly-only hits keep their original behavior.
- Keys1/2 request the native whole-unit melee/ranged order. Personal weapon switching waits for native acceptance. Rapid reversals wait for the outstanding order before submitting the latest choice; requests are not repeated every frame.

The accepted squad detachment/rejoin, formation firing, Follow Me voice handling, camera entry, audio guard, HUD and QTE remain in place. No recorder/debugger, CPU, affinity or graphics-setting change is part of this candidate.

## Offline boundaries

New fixtures execute the production mouse handler, command filter and mode request/acknowledgment code. They compare movement rates against original native18b310 and execute the original native mode constructor, boolean serializer and unit1861e0 mode setter. Native scene/eligibility/primitive writer providers and synthetic records are explicitly isolated. These checks do not reproduce a full battle, native scheduling, collision or authored animations.

The first focused run exposed an invalid signed heading in the new fixture; a later targeting fixture lacked its required heartbeat. The first speed comparison deliberately substituted a different movement configuration, which correctly failed full ownership proof. Those attempts are retained privately. The final fixture includes a separate mismatch-rejection case and uses valid ownership for speed comparisons.

## In-game checks

After a closed-game installation and fresh loaded-build check, compare walking/running with Rapid Advance off, active and expired; look through large mouse turns while stationary, moving, mounted and in a QTE; target an enemy banner through friendly foreground soldiers; and switch the entire ranged unit using1/2. Repeat Q/Z placement with the player outside its slot and confirm independent formation fire, then exit possession and check native unit control.

## Following checkpoints

Native charges, T/G special abilities and variable bow draw follow after this checkpoint's live result. The T/G display will reuse original Shogun ability artwork and native button states. A private readback verified1,371 original icon/button entries against the existing decoding index; no replacement or placeholder icons were generated. This checkpoint does not add the ability selector or change bow input.

The earlier crash and Ultra/crowd performance limits remain unresolved.
