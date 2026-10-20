# Current status: v0.26

Whole-unit T/G abilities, original icons, friendly-target Inspire, centered UI and native per-ability sound selection are included. The user confirmed friendly targeting. The shared native mounted-to-foot continuity correction is retained. This does not certify every ability/unit/effect combination. Historical candidate notes follow.

Test142 live feedback was partial/rejected. Test143 adds original targeted Inspire, shared mounted dismount continuity, native per-ability audio and centered stock UI. See Test143.md; final battle effects and audible/visual output remain pending.

# Checkpoint3: native unit abilities

Test141 adds T to cycle the possessed unit's unlocked special abilities and G to use/end the selected ability. Orders go to the complete native unit. The player retains the same unit and its native stats/effects while remaining excluded from formation-arrival waits. Native ability definitions, use restrictions, cooldowns, charges and effect targeting remain authoritative. The selector does not invent new ability bonuses or per-unit lists.

Special ammunition is included here. Tab no longer switches ammo. Activate an unlocked style with G; end it with G to return to ordinary ammo. Changing styles serializes the old-style disable and new-style enable as one capacity-checked transaction, outside a committed personal draw/shot/reload.

The preserved on-demand decoder verified71 unit definitions,43 special-ability definitions and83 effect constants against the installed pack. Native enum-name IDs were read from the supported engine, not inferred from DB row order.42 presentation entries reuse40 existing atlas icons and2 lossless conversions of original stock button images. The non-player tower reload entry has no player button and is omitted; towers are not possessable.

The live list is bounded to128 native manager entries, refreshed at most10Hz for the one possessed unit. Individual read failures are skipped. Separate seqlock telemetry checks PID, lease, actor, unit and freshness. T/G are suppressed in the native keyboard buffer only during possession and held-key release; no recorder or game settings change.

Checkpoint3 offline adapter81/HUD7 passed before checkpoint4. Native lookup, unlock, cooldown, constructor and serializer execute in fixtures with explicit primitive/vector/phase/movement providers. The first fixture missed unit+14's environment relationship; it was corrected rather than replacing native ability checks. Actual battle effect scheduling and all-unit visual behavior await live play.
