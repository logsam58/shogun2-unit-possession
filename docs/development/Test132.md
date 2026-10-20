# Test132: body-led turning and Rapid Advance sprint correction

This revises the first checkpoint after Test131 gameplay feedback. Q targeting and independent ranged firing after Q/Z were confirmed working. Whole-unit1/2 remains untested. Test131's artificial mouse slowdown was rejected, and Shift during Rapid Advance remained too fast.

## Changes

Normal infantry view now uses the model's actual bearing. Mouse movement requests a turn through a separate bounded numeric goal; the existing native body-turn step determines how quickly the visible horizontal view turns. Excess mouse input is discarded at the goal limit, rather than letting the head rotate ahead of the body or building a backlog. Vertical mouse look and animation/QTE free-look no longer have the360-degree-per-second input budget. Anatomical limits, native QTE entry facing and mounted steering remain.

While the exact native Rapid Advance effect is active, Shift keeps the current boosted walking pace rather than selecting the faster base run gait as well. On expiry ordinary walk/run selection returns. This is the requested manual-control policy, not a claim that native Rapid Advance prohibits all running. The native ability's stat bonus remains untouched, as do other units and non-possessed control.

The effect check follows the existing unit ability manager, enum27, native hash bucket/node layout, exact effect vtable, owner/environment and positive remaining time. The shared fatigue-immunity flag alone is insufficient: Second Wind must retain normal sprinting. No new native hook or actor/roster/stat write is added.

## Evidence and limits

The preserved stock ability value is1.4; native278080 passes it to the additive modifier routinec1a580. The exact-native fixture verifies that addition, the2d5560 hash lookup, and1000 active/expiry cycles with production speed selection. It rejects non-Rapid, foreign, expired and cyclic records. This establishes the isolated policy, not the user's actual post-install speed.

Production mouse/body-follow tests exercise30/60/120/240Hz, visible model-bearing alignment, responsive pitch/QTE look, reversal, anatomical stops, wraparound and the native entry lock. Mounted animation and command regression remain required. The first focused run passed the new head and Rapid tests, then exposed one old mounted fixture expectation that still assumed the removed360-degree budget; it is preserved privately, and that expected handoff delta was updated to the restored mouse sensitivity.

The first installer compilation rejected an extra brace in the new Test131 predecessor catalog row. That packaging-only typo was corrected before installation; adapter and loader inputs were unchanged, and the full installer suite was rerun. The failed compile log is retained privately.

Accepted Q targeting, squad detachment, formation firing, Follow Me voice and exact HUD/QTE binaries are retained. Charges, T/G abilities with original Shogun artwork and variable bow draw remain separate later checkpoints.

The user authorized re-enabling the existing automatic recorder. Build and deployment run while it is stopped; afterward restore the existing watcher and capture-only crash recorder, with outputs onD:. Do not start extra profiling, experimental breakpoints, change CPU/settings or launch the game automatically. Fresh READY and loaded hashes/capture checks are required before formal live play.

Prior crash, Ultra crowd and visual limitations remain unresolved.
