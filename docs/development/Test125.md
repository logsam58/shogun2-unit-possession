# Test125 â€” possession entry and Follow Me acknowledgements

This candidate builds on v0.22. Entering first person starts along the soldier's facing. A soldier already walking or running keeps moving at the native gait until the player presses a movement key. Combat, knockdown/get-up, climbing, crouching, focus loss and leaving possession cancel the carried movement.

A new QTE starts facing its opponent. Mouse-look then works within the existing horizontal limit, centered on that initial opponent direction. Native eye translation and roll remain intact, including during the animation after the prompt ends. The QTE's timing, input judgement and combat outcome are unchanged.

Follow Me acknowledges its first automatic movement order. Later updates still use the original native movement and formation handling, but their voice acknowledgement is suppressed until another player order or a new possession. Unrelated units, ordinary movement orders and other battle audio retain their original path.

## Implementation

`possession_entry125.h` reads numeric movement intent captured at the fully validated acquisition boundary. Retail actor bearing `+0x64` supplies yaw; mounted ownership supplies the exact horse. Native gait `+0x244`, ground state `+0x1f0` and current horizontal velocity determine whether movement should carry forward. The input worker receives only numeric intent and never reads native actors. Existing movement speed, fatigue, run permission and acceleration remain authoritative.

`combat_focus125.h` identifies a new live commitment by serial, lease and participant identities. It centers the view once, then bounds the requested yaw around that direction. The existing request freshness and ownership checks remain required. It changes camera composition only.

`follow_voice125.h` tracks at most 16 recent automatic follow destinations. The original move-orientation consumer at RVA `0x45e930` consumes an exact matching ticket. The event dispatcher at `0x18f2f0` records that order's numeric provenance before forwarding the original event unchanged. A fixed 64-entry FIFO distinguishes automatic follow events from ordinary orders through native deferred delivery. The voice subscriber at `0x2fc450` checks the same owned unit, possession generation and walk/run acknowledgement before suppressing a repeat.

Other subscribers and all native movement still execute. Tickets and voice provenance expire after two seconds. Order/possession resets rearm the acknowledgement but retain pending events from the old generation until their callbacks drain. Mismatches and overflow leave audio enabled; ambiguous delivery disables suppression for that adapter lifetime. There is no growing log or heap queue in this feature. All three hooks validate original signatures and follow the existing shutdown lifecycle.

The indexed retail decode and original instructions establish `0x45e930` as a five-argument thiscall/RET20 consumer, `0x2fc450` and `0x18f2f0` as one-argument thiscall/RET4 callbacks, and native acknowledgement types 3/4 for run/walk. Constructor `0x27d4a0` registers the voice subscriber with deferred delivery initially enabled, so synchronous-only suppression would miss normal acknowledgements. These addresses apply to the already supported engine hash. No mixer, audio device or game preference is changed.

## Validation limits

`verify_possession125.cpp` exercises production acquisition, movement takeover, mounted ownership, opponent centering, free-look and release. It runs the original retail move wrapper and voice subscriber in a private fixture image, substituting external planning/audio/event-queue providers. It checks stack balance, unchanged movement/event dispatch across 1,000 immediate and 1,000 deferred follow updates, one acknowledgement per Follow Me order, interleaved ordinary/foreign orders, queued callbacks across resets, expiry, mismatch/overflow fallbacks and exact hook restoration. Geometry cases preserve eye origin, pitch and roll across facing seams. The mounted-camera regression now expects opponent-first centering instead of the old animated-head limit.

The complete adapter suite and loader/installer checks are required before deployment. QTE and HUD binaries are retained exactly from v0.22/Test124. Offline checks do not establish the visual feel or audible result in every live battle. Recording stays off for the user's current configuration. Published v0.22 remains the rollback release; this candidate is not a new publication.
