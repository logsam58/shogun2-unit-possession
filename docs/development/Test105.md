# Test105 / v0.17 development: shared runtime audit

This is an unpublished correction candidate, not a replacement for the published v0.17 assets. Installation and loaded state require separate receipts. The running Test104 game was not patched, relaunched or stopped during this audit.

The reported sequence was loss of HUD/QTE/death-overlay visibility after selecting 1080p fullscreen and maximum graphics, restored HUD visibility after returning to 720p windowed, continued failed personal shots, and less smooth QTE marker motion. The existing HUD/QTE already render 1080p layouts. Resolution artwork is not the missing feature. A display-mode association is observed; the exact exclusive-fullscreen presentation cause is not established.

The audit covers installed/loaded identity, current recording, possession/input boundaries, native animation reset and completion, equipment/aim lifetime, independent ranged tasks, movement/recovery, camera/death presentation, QTE timing and registry/service behavior, window selection/display changes, resource samples and every offline component suite. These are finite checks, not certification of every game lifecycle.

## Native personal weapon ownership

The failing recording contains 31 missing-animation observations and no spawned projectile from 32 queued requests at the preserved snapshot. Original reset-mode return site `0x3e93c8` clears the owned firing animation at frame zero, before its release marker. The preceding session successfully spawned six personal shots. Two matching failures were also present in Test103. These facts establish an intermittent pre-existing lifecycle conflict; they do not establish graphics settings as the firing cause.

The old protection covered one specific 35-frame sling asset. The same existing stop hook now protects the exact currently owned infantry ready/draw/fire animation from this reset caller. Current actor/unit/lease/camera proof, supported active projectile, ranged equipment, held RMB, ready mode, no paired combat/recovery, exact current animation-database binding and an unfinished native frame are mandatory. Native reset side effects still run. Original clock completion, explicit stops/lowering, recovery, ownership loss, mounted handling and all other actors forward. No restart-on-error, timed projectile or ammo bypass is introduced.

An exact native reset/predicate/stop/clock fixture reproduces the original failure before the correction. Production ownership and the corrected hook then exercise 136 catalog/standing/kneeling/draw/fired combinations, native completion, identity failures and 1,000 peer resets. Projectile lookup, OS button state, reset side-effect providers and root-event provider are explicit substitutions. It does not simulate the projectile pool or real asset animation meshes. Existing projectile/ammunition, independent-AI and recovery fixtures remain required.

## QTE motion and time

Recorded submissions were approximately 8–10 ms apart even during the reported visual slowdown. Submission cadence is not unique marker motion or physical scanout. The live registry stayed bounded, with single-probe lookups; resource samples do not indicate memory/VRAM exhaustion in the compared interval.

The timeline retained a wall-time prediction anchor from the start of the battle. Small native scheduling delays could accumulate until predictions repeatedly hit the one-tick horizon, producing repeated marker positions despite frequent rendering. A two-minute synthetic drift regression reproduces this: 0.5 ms lateness per native step makes the old code clamp about 99% of sampled frames. Reconciliation at each actual native tick reduces the worst tested fraction to about 0.5%, across three speeds and four refresh rates. Monotonic time, the same native prediction horizon, pause, skipped ticks, speed transitions, stale-clock rejection and authoritative response deadlines remain enforced.

Private diagnostics now distinguish submitted frames from changing/repeated marker frames, report maximum marker hold and publish numeric simulation/horizon anchors. Intentional click-frame freezing and pause are excluded from the marker-hold counters. These counters are software diagnostics, not hardware latency or presentation measurements. Live attribution and smoothness still need comparison with the candidate loaded.

## Display and aim lifetime

HUD and QTE share exact native game-window identification, reject large game-process overlay windows, wait through startup minimization/zero-size windows, reacquire a replaced game window, validate client coordinates and recreate cached drawing surfaces on display/DPI changes. The private Win32 fixture covers missing/hidden/replaced windows, overlay/foreign-process rejection and actual 720p, 1080p and 2160p DIB lifecycle. No artwork, approved layout or graphics-quality setting changes. This is window-lifecycle hardening, not a proven exclusive-fullscreen fix or an instruction to use 720p.

All aim-frame consumers now require the same current lease/actor serial/unit identity already checked for presentation; gameplay could previously accept a recent same-address sample without its recorded lease fields. The audit found this in code; it is not attributed as the cause of the captured error91 failures.

No additional native hooks or numeric protocol changes. Native movement, independent AI/fire, accuracy, ammo, Q/E/Z/C/R, recovery, QTE deadlines, head-look, RTS return and siege behavior remain under the full regression suite. Published assets, installed Test104 and the known unresolved effects crash are separate and preserved.

See `Test105-validation.json` for final fresh checks, exact binaries and limits. A live check requires closed-game installation, a fresh user Steam launch, READY and verification of loaded hashes plus actual recording. Do not claim the display or gameplay failures fixed from offline evidence alone.
