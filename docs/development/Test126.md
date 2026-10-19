# Test126 - native QTE engagement view

The game establishes the initial QTE view through its own facing and engagement animation. The camera no longer snaps to a calculated opponent direction. Limited mouse-look becomes available once both participants have established their facing, or their matched native animation has started. It begins from the exact view already on screen.

This follows the user's clarification: use the animation's own turn and release limited head-look after the initial lock. There is no separate camera turn duration or interpolation clock. Test125 possession entry, continued native walk/run and Follow Me acknowledgement handling are retained. QTE timing, click judgement, movement, reservations and combat outcomes are unchanged.

## Implementation

`combat_turn126.h` keeps two numeric pose samples captured only after the existing full native pair identity checks. It checks the same 15-degree mutual-facing tolerance used by the QTE's `facing_ready`, or two fresh participants playing the same native witness. Serial, lease, participant identity/kind and 500ms freshness remain required. No enemy roster search or native actor pointer is passed to the input worker.

While waiting, `adjust_eye` retains the original native eye direction, pitch, roll and position. It captures that exact yaw as the bounded free-look anchor when the native lock is established. New pairs reset the gate; phase changes in the same pair retain free-look. The existing matched-animation handoff remains available after the prompt. Mouse deltas during the initial lock are discarded, including at unlock, so queued input cannot create a jump.

The fixed two-sample state uses the existing native motion/camera boundaries. Read failures in this optional observation are isolated so they cannot alter the existing native combat control path. It adds no growing log, native movement writes, animation retiming or new hook. The original Test125 entry and voice hooks stay exact in source. The installer recognizes the exact Test125 loader, marker and cache and tests its upgrade, all seven interruption stages and repeated-install backup retention.

## Validation

`verify_combat_turn126.cpp` covers 1,024 facing/playback cases across foot/mounted participants and bearing seams, with identity, freshness, unilateral playback and wrong-animation rejection. Production infantry and mounted camera fixtures check exact native-eye continuity, native turn frames, locked mouse input, no-jump unlock, later authored-head turns and bounded free-look. The complete adapter, loader and installer suites are required for the final payload. QTE and HUD binaries are retained exactly from Test125.

Test125 was user-accepted except for its jumpy QTE centering. The separate timed-turn draft was preserved privately after the user clarified the desired native behavior; that draft was never built or installed. Offline checks do not establish the visual feel in every authored combat animation. Recording and the debugger remain off. This is an unpublished v0.22 development candidate.
