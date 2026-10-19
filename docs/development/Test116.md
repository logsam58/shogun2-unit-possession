# Test116: QTEs after loading another battle

The QTE could stop appearing after loading another battle without restarting the game. The game can reuse its environment and phase addresses while replacing the battle context and resetting the simulation clock. The old two-pointer key identified both battles as one, so the QTE timeline rejected the lower ticks. Prompts could remain unavailable until the new counter caught up, or for the entire shorter battle.

The runtime now gives each observed environment/common-context/phase identity a local generation. It uses the already validated clock snapshot under the existing coordinator. A backward tick alone still fails validation. This adds no native hooks, game-memory writes, allocations or extra clock reads.

The offline regression reproduces the observed pointer reuse through the production Session/NativeBridge. The old key fails to admit the next battle's prompt; the corrected identity admits attack and defence prompts and rejects clicks from the previous battle. Other cases retain pause/rate polling, invalid-context rejection and same-battle clock rollback checks.

This is a v0.20 development candidate. The possession adapter and HUD are the exact Test115 components. The QTE response rules, input/presentation timing, native selection and unit AI are retained. The 40-program QTE suite and final loader/installer results are recorded in [the validation receipt](Test116-validation.json). Native providers in the regression are fixtures; live validation across consecutive battles is still required.

The crowded-battle slowdown and native crashes remain unresolved. A Small-unit battle was reported much smoother, but it also encountered this QTE failure and did not exercise the normal QTE combat path. That observation is not a controlled performance-fix result.

The generation changes when a validated native context changes; an arbitrary tick rollback is deliberately not treated as a new battle. If a future transition recycles every observed context address without any intervening context change, additional native lifecycle evidence will be needed rather than relaxing the timeline guard.

This candidate does not publish a release or replace an installed DLL while the game is running. Installation requires the game closed, then a fresh process with loaded hashes and automatic capture verified before play.
