# Test108 / v0.18 development: simplified HUD layout

This unpublished candidate implements the player's drawn layout. The soldier card stays at the bottom left with its existing dimensions, native artwork, fonts and information. Q/E/Z/C/R form one narrow vertical command legend at the bottom right. Its labels and feedback reflect the existing orders; input handling and native unit orders are unchanged.

The bottom-centre strip appears for relevant weapon activity, aiming, reloads, ammunition problems, equipment changes, kneeling or wall traversal. Idle melee and lowered idle ranged weapons leave that area clear. The strip shows the selected native ammunition and unit ammo, current weapon state and Tab where multiple ammunition types exist. Wall prompts use that same area instead of another floating panel. The permanent equipment tutorial, single-ammunition label and mouse instruction text have been removed from the presentation; their controls still work.

The QTE moves to the top centre, with its timing track anchored at 19% of viewport height. Routine action/instruction/result prose is removed. A larger highlighted mouse graphic below the track identifies the required button, and explicit INTERRUPTED feedback remains. Both the production QTE renderer and full HUD previews use the same meter definition. The QTE frame uses the existing HUD's corrected native trim sampling.

The accepted compact scale, firearm accuracy reticle and descending projectile marks are preserved. The native DirectX pixel transport, Windows fallback, cached surfaces, current-clock publication, presented-image input association, response deadline, full-cycle minimum and combat authority are unchanged. The exact accepted adapter binary is retained. No new graphics/engine hook, numeric shared layout or gameplay mechanic is introduced.

## Offline validation and state

Build explicitly with `python tools/build.py hud --candidate 108 --tests`, then the same command for `qte`, `loader` and `installer`. Four HUD fixtures include eight viewport geometries and contextual visibility. All 36 QTE fixtures, two loader fixtures and the full transactional installer regression are rerun for this candidate. Actual production previews cover 720p, 1080p, 1440p, ultrawide and 2160p. Pixel comparisons verify the unchanged soldier card, idle-context hiding and distinct left/right and interruption presentations. See [Test108-validation.json](Test108-validation.json) for results and component hashes.

Test108 is built but uninstalled, unloaded and not live accepted. VERSION remains 0.18; published v0.18 assets and installed accepted Test107 remain separate and unchanged. No game was launched, graphics setting changed, watcher restarted or duplicate recorder created for this work. Recordings continue to use the configured D: location.

Installation and formal play require the existing closed-game, fresh user Steam launch, READY, exact loaded-component and actual-recording checks. Offline rendering does not certify live fullscreen visibility, physical input latency, every device or the unresolved native crashes. This layout change is not a crash correction.
