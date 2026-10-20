# Test143: checkpoint3/4 corrections

Development candidate for v0.25, following the user's rejected/partial Test142 feedback. Published v0.25 remains exact Test140. No release or Git publication is part of this correction.

## Input and bow audio

The native window handler previously bypassed the bow release sampler with an unconditional mouse-down fire request. Window and worker observations now use one locked edge sampler. A bow shot requires its matching release observation; unknown families and missing metadata cannot silently produce a full-power bow shot. Firearms and thrown weapons retain immediate click input.

The native asset's +90 field is a release event, not duration. Foot raise126 has no such event. Full draw uses original frame count+64 and native simulation step; mounted758 uses its valid authored release marker. The existing35%-100% speed curve is a player-input policy. Native projectile constructor, original direct ballistic solver, gravity, unit accuracy/damage/ammo/reload and authored arrow-release marker remain.

RMB retains the original drawing/aim animation. Holding LMB starts the stock BOW_DRAW cue once. Releasing LMB stops only that generation-bearing draw handle, then plays stock BOW_RELEASE once; the later owned animation cue is suppressed to avoid duplication. A tap completed before the next native update skips a stale draw cue. Focus loss, recovery, weapon change and exit retire the owned draw handle without firing. AI audio is forwarded. Playback uses the same native positional parameters and device Play/Playing/Stop ABI as2fb2a0/331010; no actor audio slots or sound assets are replaced.

## Unit abilities, dismount and Z

Inspire uses the native targeted ability packet, including source unit, target ally, enum35 and native MovePoint fields. G refreshes the original crosshair hover without clicking or changing selection. The scoped ability query retains allied foreground models; accepted Q enemy priority remains. Native range, cooldown and effect scheduling remain authoritative. Other abilities continue through the existing native whole-unit command path.

The normal dismount1214f0 and Stand and Fight1cf5d0 share a same-human transition guarded by original task/environment/unit backlinks. Completion requires a current task or a same-generation task witness observed at the camera boundary; unrelated mount loss is rejected. The camera keeps possession while native dismount motion runs. A fully proved foot subject receives a fresh lease generation in the camera callback, clearing obsolete mounted weapon/charge state. No nearest soldier swap or routing/death exception is introduced. This shared path covers mounted unit categories, not just generals.

Ability activation reuses retail2fb480's per-ability sound selection. The native audio controller's bounded current subscription list is resolved on input edges. The ability title, status and T/G row use the stock artwork and measured CUF text widths, checked at720p/1080p.

Z projects the captured player position through the same original37c010 surface resolver already used by Follow Me. The previous down-ray and30cm height rejection are removed; walk/double-tap run, facing, native formation/guard orders, detachment and independent firing remain. Failed projection now has a diagnostic error.

## Evidence and limits

Preserved on-demand animation tools decoded242 existing bow declarations into98 distinct original assets. These slots reuse foot AIM/FIRE and mounted directional assets; this change does not fabricate power-specific animations. Thirteen indexed functions were byte-verified against the supported retail engine. Existing decoded ability/projectile tables were reused.

Full validation runs adapter86, HUD7, loader2, complete installer regressions including exact Test142, Test140 and published v0.25 upgrades, seven interrupted transactions and repeat-install backup retention; Python7. Exact Test140 QTE binary and its47-test evidence are retained. See Test143-validation.json for actual completed results rather than treating this plan as a pass.

New fixtures exercise the production window path, native Z projection/serialization, original targeted consumer/unit-reference writers, full same-rider dismount ownership, original sound callbacks/selectors and explicit sound handle providers. Native final effect scheduling, full dismount/renderer output and physical audible playback await a fresh loaded-build gate and player feedback. Charging remains buggy; POV, Ultra crowd slowdowns and other crashes remain open.
