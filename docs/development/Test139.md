# Test139: native charge bridge

Development candidate for checkpoint 2, based on accepted v0.24/Test138. Gameplay acceptance is pending a fresh loaded-build check and infantry/cavalry tests.

While the player runs toward the enemy, a current native charge from the formation can also reach the detached player. The bridge passes the stock descriptor to the original personal charge constructor. The original task selects charge gait, handles its target approach and requests native contact. An already charging soldier keeps the current task. Direction-only formation charges remain direction-only.

The bridge accepts only current human/horse actors, proves reciprocal mounted relationships, and validates targets through their current unit roster, environment, serial, configuration and side. It does not choose an enemy by proximity. It reads at most32 formation entries per100ms, with a total2000-entry target search budget; active targets use their current roster slot. Completed charges are not repeatedly recreated while the same run request remains held. Walking, turning away or a stopped native order cancels only the still-owned task through the native task factory. Combat, recovery, traversal and exit retain their existing authority.

Charge speed comes from the original constructed task and the live unit multiplier, with no walk/run global coefficient. Existing terrain, fatigue, acceleration and accepted Rapid Advance ceiling still apply. The native unit run-intent getter returns a boolean in AL; it is not a charge enum. Actual native charge tasks provide charge evidence.

The preserved on-demand asset decoder and animation companion were located and used through an explicit archive relocation adapter.18 stock charge declarations/assets were checked against the existing pinned references and current packs. Those results establish asset storage/declarations, not runtime blending or combat behavior. Selected indexed retail C/ASM exports and exact instruction-byte checks supply the runtime evidence.

`verify_charge139.cpp` executes original retail construction, charge entry, moving-pose selection, charge speed and the complete charge-update function's contact/completion branch. Geometry/pathfinding, property values, transition lookup and contact eligibility/queue results are explicit fixture providers. Cases include infantry, horse/rider ownership, mounted enemies, direction-only charges, native special pace, entry during an existing charge,1000 stable updates, cancellation, identity reuse, friendly/dead targets, ranged-mode rejection and bounded work. This is not proof of live charge bonuses, outcomes, renderer appearance or every roster asset.

Retain exact Test138 HUD/QTE and all accepted Rapid136, native1 toggle, Q targeting, detachment, formation firing, camera and siege behavior. T/G abilities and variable bow draw are later checkpoints. POV quality, Ultra crowds and other crashes remain open.
