# Test144: bow audio and melee aiming dot

User confirmed Test143 bow strength and friendly-target ability aiming. Bow audio was rejected; a small melee/mounted-melee command aiming dot was requested. This development candidate preserves all bow input/power/projectile code and ability targeting unchanged. Published private v0.25 remains exact accepted Test140.

## Bow audio correction

Test143 intercepted general sound bus2fb2a0, while authored bow notes use original292620 directly. The native enum builder also stores ID before name; Test143 mistakenly used the following ID. Verified stock BOW_DRAW isd3 and BOW_RELEASE isd4. Originalcd56e0 maps animation notes20/21 through166f558 to those IDs. These are now exercised by the original executable blocks in the audio fixture.

The new292620 hook reads the physical actor at the original holder+8 and requires the current possession lease, bow family, aiming state and the current environment's bounded audio subscription/controller/device proof. Only owned bow draw and claimed release events are filtered. Other actors and other events forward to native code. There is no shared asset edit, position heuristic, actor-list scan or per-frame sound allocation.

RMB still drives native aiming animation. LMB press plays the correct stock draw cue once. LMB release stops only the saved generation-bearing handle before the correct release cue. Quick tap, focus loss, cancellation, stale lease and changed environment behavior remain tested. The new hook is removed at shutdown; input logging now records animation suppression, draw stops and releases. Actual audible output still requires user feedback.

## Melee aiming dot

A two-pixel ivory center with a dark outline identifies the same center used for Q/G targeting, for known equipped melee mode on foot or mounted soldiers. It is hidden during QTEs, climbing and native formation placement, and does not replace ranged reticles or appear for unknown/pending equipment. Original HUD artwork/layout remain.

## Research and validation

The preserved on-demand decoder read72 original sound.pack animation sound sidecars without rebuilding archives or tools. Nativebd7970 verifies their versioned8/12-byte headers and8/12-byte records. Nine indexed native functions were instruction-byte checked against the supported retail DLL. A misleading bone-interface lead was discarded; it is not patched.

The expanded audio fixture executes original292620 andcd56e0 against explicit unit/position/voice/device providers; it checks owned suppression, actual enum translation, peer audio, unrelated sounds, stale release and hook removal. Existing lifetime tests cover release stop ordering and quick taps. Full adapter86/HUD7/loader2/installer and Python7 checks, exact143 rollback and independent deployment readback are required; see Test144-validation.json for completed results. Exact QTE140 is retained. No live audible success is inferred from fixtures.

Validation also exposed a runner defect: CMD's `if errorlevel 1` ignored negative native exception exits. The runner now rejects every nonzero exit and unexpected fixture-fault output; a synthetic access-violation exit verifies this. The new audio fixture's initial false pass was withdrawn and its explicit providers corrected. The stricter run then caught an older Hold Here fixture still asserting the removed crosshair ray. That assertion now verifies the original current-position projection's serialized terrain height and absence of another ray query. Production Hold Here code is unchanged. Failed runs and their sources remain in private Test144 evidence; the final full run is authoritative.
