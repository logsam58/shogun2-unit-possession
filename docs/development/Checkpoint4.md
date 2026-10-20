# Current status: v0.26

The user confirmed variable bow strength after Test143. Test144 audio decoupling was functional but rejected because the native draw animation starts before the LMB-controlled sound. Test145 restores native animation-linked bow audio; sound decoupling is deferred. The confirmed input/strength/projectile behavior remains. Historical candidate notes below describe their own versions.

Test142 bow power was rejected. Test143 fixes the real window bypass and the misidentified release-event field, retaining native RMB animation and arrow-release marker. LMB starts the draw cue, release stops its handle and plays the release cue once. See Test143.md; native renderer/audio feel needs fresh live feedback.

# Checkpoint4: variable bow draw

Test142 adds bow-only LMB hold/release while RMB remains the native ready/draw control. A short LMB hold produces a weak shot; a full hold reaches the current projectile's native speed. Full draw time comes from the actor's native draw asset (foot126, mounted758). The explicit control curve rises from35% to100% of native speed. This is an added player control policy, not a claim about stock RTS bow mechanics.

The input uses a monotonic performance counter, not frames or input repeats. Focus loss, changed possession identity, weapon changes, melee commitment and release cancel a pending hold. Unknown weapon-family state cannot fire a bow as an immediate firearm click. A numeric strength sample binds to the current lease/actor/unit/projectile before the native shot is queued. No projectile definition, formation AI, damage, ammo, reload, gravity, accuracy or other unit is changed. Guns and thrown weapons retain immediate click-to-fire.

Only the per-shot speed argument to native40e9d0 changes; native420050 still solves launch direction. Existing drop, unit accuracy, native draw/hold/release clips and release markers remain in charge. The projectile spawns at the established animation event, not on the raw mouse edge. Existing POV quality remains deferred; this does not replace authored bow deformation.

The preserved on-demand decoder/animation companion checked19 existing bow draw/aim/fire declarations and decoded10 distinct original assets. Existing decoded projectile tables were byte-roundtripped. Nine selected indexed native functions were instruction-byte verified. The fixture executes the original launch constructor and full direct ballistic solve at five draw strengths, plus8004 monotonic/capped cases and focus/lease/family cancellation. Phase/primitive/scene providers are explicit. Final input feel, release appearance and live foot/mounted/ammo combinations await player testing.
