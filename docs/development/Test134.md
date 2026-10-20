# Test134 — native melee-mode toggle

Press1 to toggle the possessed unit's native melee mode.2 no longer selects a weapon mode. Melee-only units ignore the toggle; unavailable requests cannot leave a "Switching weapon" message pending.

The command now calls the game's original battle UI action handler. That handler owns construction, serialization and queuing of the ordinary melee-mode command. Native execution owns eligibility and the unit flag. The adapter no longer constructs a separate mode packet. This action is allowed while the possessed soldier draws or reloads, as a unit command in RTS is; personal animation transitions still wait for the existing safe simulation boundary. After native melee acceptance, held RMB cannot keep the player's ranged readiness active. Combat/QTE, knockdown and reload protection remain.

Personal weapon state follows the actual native unit mode even without a private request, including native changes made while paused. The HUD retains its original Shogun mode icons and shows the actual unit mode with1 as the toggle hint for units with an alternate mode. Original assets, layout and exact QTE remain. Rapid Advance is user accepted and unchanged, as are Test130 looking/detachment, Q targeting and independent ranged fire.

## Verification and limits

Run adapter76/HUD6/loader2 and complete installer regression with `--candidate 134`, including exact Test133 predecessor upgrade, seven interruptions and repeat backup retention; run Python7. Preserve exact Test133 rollback and the armed automatic recorder on D: with no active game/session during installation.

The focused native fixture executes the original UI action wrapper, controller, selection predicates, original queue producer and boolean serializer. Descriptor lookup and primitive writer providers remain isolated. It reproduces a personal-draw order blocked by the previous gate and verifies dispatch now reaches the native packet, native mode acceptance, ranged-readiness suppression, Katana-style no-op, invalid cleanup, reversals, queue/identity/controller guards. Native unit scheduler and real authored animations still require a fresh loaded-build check and user testing; offline tests are not live acceptance.

The full checkpoint is not yet accepted. Rapid is confirmed; looking is not explicitly confirmed. No crash or Ultra performance fix is claimed. Native charge participation, original-icon T/G abilities and variable bow draw remain later checkpoints.
