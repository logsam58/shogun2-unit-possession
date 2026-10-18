# Test115: one bounded read for unrelated infantry encounters

The player reports that Test114 improves crowded QTE fights but remains laggy. This v0.19 development candidate removes the remaining eager reservation proofs from foreign encounter traversal. Participant fields are read into an 80-byte stack snapshot once. Unrelated human addresses return without reading either reserved actor. Possible matches still perform the full current identity proof in `member`; mounted aliases retain their reciprocal rider checks. Nothing is cached across callbacks.

If the bounded read fails or is short, traversal uses the original individual-field reader. This preserves readable matches near an inaccessible page boundary. The snapshot only supplies candidate participant fields; it never serves as identity or lifetime proof. Native contact objects use their original group and target rules.

The Test114 header fails the new read-budget regression at 23 reads/86 bytes for a five-participant unrelated encounter. Test115 requires one read/80 bytes for each of the three tested groups. The focused test passes 5,152 differential, identity and fallback cases, including serial/unit reuse, death, side changes, failed current proof, mutation after snapshot, actual inaccessible pages, malformed counts, contact layouts and mounted aliases. A 100,000-encounter flood uses 100,000 reads. Fixture timing is not a gameplay FPS result.

All 39 QTE fixtures, two loader checks and the full installer upgrade/interruption/reinstall suite pass. The exact installed Test114 predecessor is included. Test113's acknowledged-image input correction and the exact accepted Test109 adapter/HUD remain. No graphics, audio, recording or gameplay-order changes are included.

Installation and loaded state are recorded privately; public validation describes package preparation. Test115 has no live acceptance, full slowdown fix or native crash-fix claim. A fresh user Steam process and exact loaded/capture checks are required before formal testing. Recording runs independently of READY and chat state. Published v0.18 and frozen local v0.19 remain unchanged.
