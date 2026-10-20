# Test135: Rapid Advance pace

Test134's native melee-mode toggle was accepted by the player. During Rapid Advance, however, both W alone and Shift fell behind the running unit. The previous restriction selected the boosted walking rate for both inputs. Saved controlled-movement samples show the ability multiplier reaching the player; they do not measure an AI soldier's actual velocity.

During the exact active Rapid Advance effect, possession now asks the original native unit gait query (RVA18c120) whether the unit is walking or running. Shift adds no independent sprint. The existing movement reader uses the corresponding native entity walk/run coefficient and applies the unit's live speed multiplier once, as native18b310 does. It excludes formation-arrival/catch-up multipliers. Normal manual Shift running returns after expiry. Follow Me and movement fatigue use the same gait decision. Unit stats, orders and squad membership are not changed by this query.

This deliberately corrects the earlier interpretation of disabling Shift: it no longer forces the player to walk behind a running unit. During Rapid, the native unit order chooses the gait. Outside Rapid, controls are unchanged.

The accepted Test134 native1 melee toggle, Q targeting, independent Q/Z ranged fire, Test130 camera and squad detachment remain. HUD and QTE source and binaries are retained exactly. No charging, ability-selection or bow-draw work is included.

The expanded fixture executes original native18c120 and18b310 with isolated task-predicate, run-permission and routing providers. It checks both native command stores, permissions, both Shift states, forced run, invalid reads, original stat addition/hash lookup and expiry. The complete adapter suite and final installer upgrade/rollback suite are required, including the exact installed Test134 predecessor through all seven interruptions and repeated installation. See Test135-validation.json for actual completed checks; fixture coverage does not establish live animation or AI velocity matching.

The existing automatic recorder stays armed. Installation requires a closed game and an exact rollback. A fresh Steam launch, user READY cue, loaded hashes and actual crash/input/resource capture checks must precede formal testing. Native crashes, Ultra crowd slowdown and visual limitations remain unresolved. This is an unpublished product0.23 development candidate.
