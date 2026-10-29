# Test169: custom-unit HUD and pack identification

This is an offline-tested candidate. It has not been installed, uploaded or
accepted in gameplay. The published release and existing subscription are
unchanged. Test167 and Test168 remain preserved separately.

## Custom-unit HUD

An owner-supplied Workshop comment reported a missing HUD on a custom unit
while possession controls continued working. Source review found that the
HUD rejected the entire frame when a valid native unit key had no entry in
our stock artwork/name table. An isolated test running Test168's actual HUD
measurement reproduced that rejection with an otherwise valid custom unit.

Test169 makes stock presentation metadata optional. Live identity, ownership,
morale, fatigue, men, weapon state and commands retain their existing reads
and validation. Known units keep their existing names/cards. An unknown unit
shows its actual native key and the renderer's existing no-portrait/type
fallbacks while the HUD remains available. This does not create artwork, guess
statistics, load a custom portrait automatically or translate another mod's
display names. The native key-format and identity/lifetime guards remain.

The production measurement fixture passed 138 checks across ten soldier roles,
including custom keys, unknown faction artwork, stale/dead/foreign actors and
restoring a stock unit. Actual resource-renderer checks at 1280x720 and
1920x1080 verified that the populated right-side command region remains
pixel-identical between stock and custom-unit views. Other HUD fields retain
their existing stock rendering; the tests do not simulate exclusive fullscreen.

## Renamed packs and duplicate filenames

Test168's menu-cooperation path skipped its own archive by a fixed filename.
A renamed copy was treated as another UI archive. The actual Test168 pack was
too large for its menu-root bound and was rejected in the baseline test; no
recursive startup was observed. Delegating a self bootstrap would also be
incorrect, so the replacement must identify it before delegation.

Test169 writes its bridge SHA-256 in the pack metadata. Selection checks the
bounded metadata and the matching bridge-cache filename in the compiled root
against the actually loaded bridge file. This identifies the current bootstrap
even when the archive is renamed. Current duplicate copies can be skipped as
self entries; an older/different/incomplete possession package is refused with
a specific warning. A self root without its metadata is not delegated.
This is self-identification, not a publisher signature or a trust guarantee for
other downloaded mods.

Pack roots are read one at a time. The reader permits up to 8 MiB for inspecting
the embedded bootstrap; external menu scripts and temporary Lua handoffs remain
limited to 256 KiB. It does not retain a list of whole bootstrap payloads across
search folders. Metadata is bounded to 16 KiB. Cache-write/reparse restrictions,
engine fingerprints and exact native hook guards remain.

Different database-only packs sharing a filename do not by themselves block
menu startup. Identical UI roots at duplicate paths can be preserved. A
filename resolving to different UI roots or both UI/non-UI content is reported
as ambiguous. Files are not deleted, relocated or copied into the game by
these checks. Unknown/mixed copies should be reviewed rather than overwritten.

An actual matching float-Lua/native-bridge test passed with two renamed copies
of the compiled bootstrap and a separate menu addon: all 37 stock callbacks and
the addon's callback were retained, in the original environment, with one
initialization. Native fixtures also cover older/incomplete copies, database
name collisions and conflicting UI sources. These checks run outside the game.

## The manual Data-copy report remains unresolved

The screenshot also reported that copying the downloaded Workshop pack into
Data enabled the mod. This does not prove which part of that player's setup
failed. Earlier Test165 evidence already established real subscribed loading
on the owner's PC via the Workshop search directory, with no local Data pack.

The normal bridge obtains the game folder from the host executable and uses
absolute paths. Its float-Lua startup fixtures run with a working directory
outside the game. The pack builder retains canonical backslash virtual paths
and the owned stock pack's nonzero timestamp, including the previously verified
timestamp correction. These facts do not reproduce the other player's native
archive selection, active mod list, resource availability or game version.

No new speculative archive-priority change, automatic Data copy, movie-pack
autoload, CPU setting or launcher preference is applied. The new copy checks
protect the candidate; they are not presented as a proven cause or fix for the
reported v0.32 delivery failure. If another mod wins the bootstrap entry,
our code may never run. Existing active-list, local regular-input, engine and
hook restrictions still apply; universal mod compatibility is not claimed.

Officer selection difficulty, the one-off square-formation control failure
that recovered after restart, and intermittent wall climbing are separate
reports. This candidate does not claim fixes for them or infer a persistent
square-formation incompatibility from that single comment.

## Retained behavior and verification

Test168's local-font/optional-English-audio corrections, single-menu-addon
cooperation, component readiness barrier, bounded startup error reporting,
Miles guard and no-recorder/no-routine-log behavior are retained. No changes
are made to possession movement, combat, charge, wall climbing or unit orders.
The accepted source for those components is retained and the normal regression
suite is rerun; this is not new gameplay acceptance.

See TEST169_VALIDATION.json for authoritative hashes and test receipts. The
player package contains code/notices only; game resources are prepared locally.
Private fixtures contain owned-game material and must not be distributed.
Installation requires separate rollback-backed staging approval and fresh
loaded-build/readiness checks. Workshop/GitHub publication is not authorized
by this investigation/build task.
