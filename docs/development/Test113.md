# Test113: read the current native QTE image when sampling input

This is an uninstalled QTE correction candidate. Installed accepted Test109 and the frozen local v0.19 release packages remain unchanged. No gameplay adapter, HUD, recorder, game setting or native hook changes are included. This candidate does not claim to resolve the separately reported possession slowdown or native combat crashes.

The player reported intermittent slowdown relieved by F10/F9 and a small mismatch between the QTE click and the marker's final position. Recorded native Present counts confirm variable rendering cadence. Memory/VRAM headroom, a maximum of243 live QTE registry entries and single-probe lookups do not support an exhausted-memory or growing-registry explanation in that session. Before/after averages do not show consistent FPS recovery after re-entry; several resets also change soldiers or end death views. These aggregates cannot exclude individual frame hitches, a native resource lifetime issue or expensive work inside a hook. There is no per-function timing trace for this run.

## Reproduced input scheduling gap

Input previously copied the render worker's last acknowledged image. A newer native Present could complete between render-worker polls while input continued using that older copy. Polling mouse buttons quickly did not remove the additional dependency on renderer scheduling. The existing presented-image fixture proved that unsubmitted CPU geometry was rejected; it did not cover an acknowledgement advancing after the render worker last polled it.

A deterministic regression supplies an older, still eligible render-cache image outside the zone, a newer acknowledged native image inside it, and a still newer unsubmitted CPU image outside. The old input path judges the older image early in all36 tested combinations; the correction judges the actual acknowledged image successfully. This establishes a software scheduling defect, not the player's exact physical scanout at a historical click.

The input worker now reads native acknowledgement metadata through its own existing-channel reader. A fixed32-entry image history, protected by a lock around numeric copies only, resolves the acknowledgement to its original prompt. Current lease/actor/unit, battle and prompt serial must match. A newer acknowledged image can replace the renderer's cached copy; an older acknowledgement cannot replace a newer fallback frame. The existing50ms click freshness limit, authoritative response clock, native eligibility, pause, expiry and displayed-image judgement remain. No extra prediction horizon, timing bonus, altered success zone or input grace is added. Windows fallback presentation is retained.

Each queued input also records the exact selected image number/source, age, marker position, zone and inside/outside state. The existing mouse_sample reticle fields describe current projection and must not be mistaken for the image actually judged. This diagnostic distinction is necessary for the next live comparison.

## Validation and limits

All37 QTE fixture executables passed after the production change, including the retained timing, pause, expiry, native commitment, publication and presentation suites. The final input fixture adds a real shared-channel acknowledgement round trip and40000 concurrent image publications; accepted reads must match the exact image number without torn numeric fields. Invalid identities, battle, serial, ring reuse, future timestamps, missing backend, expired frames and pause are covered across DX9/DX11 metadata and30/60/75/120/144/240Hz schedules. These are offline tests, not real renderer/device or scanout acceptance.

The candidate DLL is built separately. No loader or installer package is produced and no installation or game launch is performed. Private Test113 receipts identify the executable, source hashes, initial full-suite and final focused checks. A future installed test requires closed-game replacement, a fresh user Steam launch, READY and exact loaded/capture verification. Automatic recording continues independently.
