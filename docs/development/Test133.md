# Test133 — look restoration and weapon mode display

This unpublished v0.23 candidate restores the accepted Test130 camera feel, including mounted look and combat animation free-look. The Test131/132 head speed and body-follow changes are removed. Q targeting and whole-unit weapon-mode orders remain.

Shift-to-sprint is disabled while the exact native Rapid Advance effect is active, as requested. The existing boosted walk pace remains. Follow Me uses the same leader gait decision; formation catch-up can still run when necessary. Native ability statistics and AI movement are unchanged. Normal sprint resumes when the ability expires; the shared fatigue flag alone is not used to identify Rapid Advance.

The soldier panel now continuously shows the unit's accepted Melee mode or Ranged mode, with the original stock Shogun status artwork and its1/2 key hint. The display reads the native unit flag after identity validation, rather than assuming a keypress or the personal weapon animation means the unit accepted the order. It stays visible when the contextual ammunition panel is hidden.

## Checks and limits

Build adapter, HUD, loader and installer with explicit `--candidate 133`; run all their fixtures and Python checks. Test132 is an exact installer predecessor, including seven interrupted transactions and repeat-install backup retention. QTE remains the exact previously tested binary. Preserve an exact Test132 rollback, source snapshot and recorder configuration. The ordinary capture-only recorder remains armed on D: throughout closed-game work; no extra profiling is started.

The saved Test132 movement record confirms the player used the boosted native walk pace for either Shift state. It does not measure peer AI speed. The native speed effect adds1.4 to its live accumulator (1 becomes2.4); this is not evidence that possession applied it twice. No AI speed or crash fix is claimed. Head feel, mode display and movement need a fresh loaded-build gate and user testing. Prior Q targeting and independent Q/Z ranged fire are accepted; whole-unit1/2 is still not explicitly live accepted. Crash, Ultra crowd performance and visual limitations remain.

Charging, T/G abilities with original native artwork, and variable bow draw remain separate later checkpoints. No release publication is part of this candidate.
