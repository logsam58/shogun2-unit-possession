# Test109 / v0.18 development: HUD layout and friendly-fire callout

Test109 includes the Test108 HUD layout and adds the requested original-game SHAMEFUL DISPLAY voice notification when the possessed player's personal projectile contacts an allied living soldier or horse. The preceding routing sentence is excluded: the local English advisor line is cropped from 3.25 to 4.80 seconds, leaving a 1.55-second PCM clip. Source path, hashes and exact crop are in tools/audio109.json. No internet audio or generated imitation is used.

## Native attribution and playback

Only projectiles appended during the validated personal-shot Register41c760 call are recorded. A bounded 128-entry registry stores native projectile address/id, environment and possession actor/unit/generation; it does not poll retained native addresses. A single passive hook on retail impact wrapper473f80 observes actual target contact. It forwards the original receiver, four stack arguments and return value once. Damage, armour, collision, RNG, effects and native sound remain native.

The observer requires the exact current living player, lease, battle, camera, holder, selected unit and native serials. The impact projectile must match both its saved native id and shooter; reused addresses, AI projectiles, old possessions, dead targets and RTS mode do not qualify. Battle-side team ids admit allies in separate armies. Each tracked projectile can notify once. Invalid observation fails silently for the callout without releasing possession or suppressing the original impact.

An optional 64-byte FriendlyFire109 notification carries numeric identities to the existing separate HUD process. The HUD consumes each event once, requires current possession identity/focus and a recent timestamp, and plays the embedded WAV asynchronously through Windows audio. Overlapping notifications are dropped instead of queued. No Miles/DirectSound game-state access or audio decoding runs on the game thread. The existing gameplay/QTE/pixel protocols remain unchanged; this addition introduces one passive engine hook and the optional notification mapping. It is not a new graphics hook or crash correction.

The direct native projectile-contact path covers the personal weapons using that wrapper. Area-explosion-only damage, reflected/secondary projectiles and every specialty effect have not been certified by live play. No casualty-count or aiming-ray heuristic substitutes for a proven personal hit. The fixed registry deliberately drops very old/excess tracked shots instead of guessing ownership.

## Build dependency

Prepare the private audio dependency from the local game with:

```powershell
py tools/prepare_audio109.py --game 'C:/path/to/Total War SHOGUN 2' --ffmpeg 'C:/path/to/ffmpeg.exe'
```

The helper reads only the exact pack entry, verifies its source hash, crops through FFmpeg, verifies the expected PCM hash and writes the ignored HUD private resource. FFmpeg is a build-time dependency only. The adapter and QTE do not link Windows audio libraries; the HUD links winmm. The source archive does not include the proprietary voice recording.

Build adapter and HUD with `py tools/build.py <component> --candidate 109 --tests`; retain the exact Test108 QTE and its input hashes. Then build loader and installer with the same candidate number. Use Test109-validation.json for the pinned final results. The new adapter fixture executes original retail473f80/40f130 bytes and the actual hook, with a deliberately substituted damage provider. It checks native forwarding/ABI plus personal attribution, team filtering, identity invalidation and deduplication. The HUD fixture checks event policy and the exact cropped PCM resource without playing audio. Offline fixtures do not certify every live impact path or audio device.

Test108's original candidate binaries and before-source files are retained privately. VERSION and published v0.18 remain unchanged. Native installation must preserve the exact installed Test107 rollback and recorder baseline, require a closed/finalized game, and use a fresh user Steam launch. Formal play still requires READY followed by exact loaded hashes and actual recording verification. Automatic capture remains independent of READY; recordings stay on D:.
