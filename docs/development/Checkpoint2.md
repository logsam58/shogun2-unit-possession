# Checkpoint 2: native infantry and cavalry charges

Checkpoint 1 is complete by the player's decision, with first-person weapon/arm presentation left for later. v0.24/Test138 removes the rejected locomotion override while retaining the accepted combat behavior. Finish its offline checks and closed-game installation before starting charge changes.

## What should happen

Infantry should join the charge their formation is already performing. A possessed cavalry rider should use the game's charge behavior when closing on an enemy, alongside the formation. Keep the game's charge eligibility, momentum, attacks and animations. Walking into an enemy is not enough evidence that a native charge happened.

Use existing movement and attack controls. Do not add a charge minigame, arbitrary speed bonus, new key or replacement collision system. Player control remains separate from formation-arrival/firing requirements; preserve the working detachment and rejoin behavior.

## Investigation before implementation

1. Use the existing decoding index to trace the native unit attack/charge order into the foot-soldier and horse/rider task paths. Separate order intent, target validity, gait, approach, charge commitment and contact.
2. Compare those paths with personal movement, Q attack dispatch and detached actor tasks. Establish which native prerequisites are missing during possession before changing them. The existing scoped advance-task compatibility is crash protection, not proof that charging works.
3. Choose the smallest owned-actor intervention that lets the original native charge execute. Prove unit/actor/mount identity and lifetime. Do not restore the player to formation-arrival duties or alter unrelated AI.
4. Verify transitions out of a charge: stop, turn away, target change, routing, knockdown/get-up, matched melee, wall traversal, death and exit possession. Retain original native restrictions and cooldowns.

Start with `native_movement.h`, `native_commands100.h`, `native_context103.h`, `unit_detach130.h`, `unit_detach_combat130.h`, `native_recovery103.h` and `weapon_animation.h` in the active adapter source. These are entry points for investigation, not a claim that the charge function has already been identified.

## Validation

Use original native code in focused fixtures for charge eligibility/task dispatch and cancellation, with providers stated explicitly. Check foot, mounted and irrelevant-actor cases; repeated charges must not accumulate state or broaden crowd scans. Run the required adapter, loader, installer upgrade/rollback and Python checks with the game closed. Preserve exact accepted Rapid Advance, native1 toggle, Q targeting, formation firing, HUD/QTE and recorder behavior.

Live testing needs a fresh READY/IN BATTLE cue and exact loaded-build plus recording verification. Compare an ordinary native formation charge and its possessed soldier, for infantry and cavalry, including entry while the unit is already charging. A visual sprint or successful Q unit order alone does not count as charge acceptance.

T/G abilities and variable bow draw remain later checkpoints. Ability selection must use native definitions, restrictions/cooldowns and original Shogun icons/button artwork; no placeholders.
