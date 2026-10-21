# Checkpoint 2 complete: v0.27 / Test149

The user reported Test149 working in battle and requested completion and release on 2026-10-21. This is user-reported acceptance; a formal Test149 loaded-build/capture gate was not completed before the feedback.

## Accepted behavior

Hold W+Shift while running or galloping toward a valid forward enemy. Once committed, the original native task owns approach, target engagement and the authored attack/defense outcome. Whole-unit orders continue normally for other soldiers. There is no charge minigame or forced kill. The ordinary QTE yields to the charge's native encounter.

The final design replaces the earlier unit-coupled charge policy at the user's request. Native initiation, alignment and contact gates were traced with the preserved on-demand decoder, animation companion and read-only Ghidra exports. Test149 corrected the reproduced early-return alignment bypass. K Hide UI also carries seamlessly between RTS and possession.

The release preserves exact Test149 adapter, Test148 HUD and Test146 QTE binaries and source. See [release details](../releases/v0.27-technical.md), [validation](../releases/v0.27-validation.json) and [Test149 investigation](Test149.md). Earlier Tests146-148 were rejected or incomplete; their documents describe historical candidates, not the current accepted state.

Enemy acquisition while separated, first-person presentation, Ultra crowd slowdown and other crashes remain open. A charge kill, cavalry leap or a particular animation is not guaranteed against every enemy or approach.
