# Test127 / v0.22: Follow Me acknowledgements

Follow Me should acknowledge once, then stay quiet while the unit keeps following. A new player order or leaving possession clears that gate.

Test126 tried to match queued movement packets to delayed voice callbacks. A mismatch or a full attribution queue could leave repeat acknowledgements enabled for the remainder of the session. That behavior exists in the old code and fixtures; the particular condition reached in the user's battle was not captured.

Test127 filters the existing native order-voice subscriber directly. It requires current full possession proof, active Follow Me mode, one exact owned unit and the native walk/run event type. The first acknowledgement attempt retains native audio selection and eligibility; repeated walk/run callbacks stay quiet until the existing reset boundaries. Other units, other voice categories, RTS and invalid ownership forward normally. No movement-event provenance queue or timeout can disable the gate. Native movement and event dispatch functions are no longer hooked by this feature.

The original order subscriber selects positional unit audio, so an acknowledgement may sound as if it comes from the soldiers. That observation alone does not establish a separate audio route. Existing indexed call sites were reviewed; ordinary soldier chatter remains native.

The native fixture executes the original retail movement wrapper and order subscriber with explicit planning/audio/event-queue substitutions. It covers 4,000 immediate/deferred updates, changing gaits/destinations, large callback queues, first acknowledgement/reset, unrelated voice categories and units, RTS, invalid ownership and exact hook restoration. It does not prove every in-game speaker or battle lifecycle.

Accepted Test126 native-animation QTE entry, bounded free-look, soldier-facing possession, inherited movement and weapon source are preserved. HUD and QTE binaries remain exact. The reported Ninja slingbomb failure worked on retry; no failure snapshot was taken and no projectile or weapon change is included. The earlier failure's cause remains unconfirmed.

Full build and packaging evidence is recorded in Test127-validation.json. Installation and loaded state have separate private receipts. Recorder and debugger stay off. No graphics, thread, affinity or game launch change; no new performance or crash-safety claim. Published v0.22 is unchanged.

The first complete installer run reached the new Test126 upgrade and reported `bad allocation`. The 32-bit fixture kept more than a gigabyte of historical loader buffers alive until the end of its entry function. Twenty-two later upgrade cases now release their loader/marker buffers on leaving each case. All prior test operations and assertions remain identical; production installer behavior did not change. The failed run is preserved privately, and the full final-payload suite is rerun after this test-only correction.
