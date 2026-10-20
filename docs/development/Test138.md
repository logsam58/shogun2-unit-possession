# Test138 / v0.24: close checkpoint 1

The player accepted Test137's combat weapon presentation, rejected its combat-style walking animation and closed checkpoint 1 with a caveat: the first-person weapon/arm view still needs work. That visual work is deferred.

This correction leaves ordinary native states0/4 unchanged at both the animation hook and equipment synchronization boundary. Existing readiness8/9 and mounted29 continue through Test137's owned-actor melee handling; paired combat and traversal retain native control. Native mode1 toggle, Rapid136 movement, Q targeting, detached control/formation firing and exact HUD/QTE remain unchanged. The Test137 advance-task crash compatibility is retained.

The presentation fixture compares hooked ordinary state calls against the unhooked native result from identical actor bytes, allowing native refusal rather than assuming availability. It retains supported-catalog readiness, ammo, identity, paired-combat and mounted checks. Final authored POV appearance remains a live limitation.

Required: adapter79, retained exact HUD6/QTE46, loader2, complete installer suite including exact Test137/all predecessors and all seven interruption stages/repeated backup retention, Python7, source/payload/version checks and independent deployment readback. v0.24 metadata does not imply publication or live acceptance of the rollback. Actual results are recorded separately in this candidate's validation receipt.

The next work is [checkpoint 2: native charging](Checkpoint2.md). The current checkpoint does not implement charging, T/G abilities or variable bow draw.
